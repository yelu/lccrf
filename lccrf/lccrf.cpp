#include <fstream>
#include <boost/algorithm/string.hpp>
#include <spdlog/spdlog.h>
#include "lccrf.h"
#include "perceptron.h"
#include "decoder.h"
using std::ofstream;
using std::ifstream;

LCCRF::LCCRF()
{
}

LCCRF::~LCCRF(void)
{
}

double LCCRF::Fit(vector<Query>& qs, 
                int max_iter,  double learning_rate, double l1)
{
	if (qs.size() == 0)
	{
		spdlog::error("no query data found");
		throw std::runtime_error("no query data found");
	}

	// iterate through all queries to construct lccrf features.
	_features.Clear();
	_GenerateLCCRFFeatures(qs);

	spdlog::info("#queries {0}, max iterations {1}, step {2}, l1 {3}", qs.size(), max_iter, learning_rate, l1);

	std::vector<double> zero_weights(_features.FeatureCount(), 0.0);
	_weights.swap(zero_weights);
    PerceptronTrainer trainer(qs, *this);
	trainer.Run(learning_rate, max_iter);
	return trainer.GetBestLoss();
}

vector<Query> LCCRF::LoadData(std::string& dataFile)
{
	std::ifstream infile(dataFile);

	std::vector<Query> qs;
	int length = 0;
	string line;
	Query q;
	while (std::getline(infile, line))
	{
		boost::algorithm::trim(line);
		if (line.size() == 0)
		{
			if (q.Length() != 0)
			{
				qs.push_back(std::move(q));
			}
			q.Clear();
			continue;
		}

		Token t;

		size_t first_scroll = line.find_first_of('|');
		if (first_scroll == std::string::npos)
		{
			throw std::runtime_error("invalid line");
		}

		// [label] ([importance]) ([text])
		string label_seg = line.substr(0, first_scroll);
		boost::algorithm::trim(label_seg);
		vector<string> label_seg_fields;
		boost::split(label_seg_fields, label_seg, boost::is_any_of(" "), boost::token_compress_on);
		int label = atoi(label_seg_fields[0].c_str());
		if (label < 0 || label > (int)UINT16_MAX)
		{
			throw std::runtime_error("query label must be of type UINT16_T");
		}
		t.SetLabel((uint16_t)label);

		// | [f1]:[val] [f2]:[val]
		string feat_seg = line.substr(first_scroll + 1);
		boost::algorithm::trim(feat_seg);
		vector<string> feat_seg_fields;
		boost::split(feat_seg_fields, feat_seg, boost::is_any_of(" "), boost::token_compress_on);
		for (auto ite = feat_seg_fields.begin(); ite != feat_seg_fields.end(); ite++)
		{
			vector<string> fields;
		    boost::split(fields, *ite, boost::is_any_of(":"));
			int64_t xid = strtoll(fields[0].c_str(), NULL, 10);
			if (xid < 0 || xid > (int64_t)UINT32_MAX)
			{
				throw std::runtime_error("feature id must be of type UINT32_T");
			}
			float val = 1.0;
			if (fields.size() > 1 && fields[1].size() != 0)
			{
				val = atof(fields[1].c_str());
			}
			t.AddFeature(xid, val);
		}
		q.AddToken(t);
	}
	if (q.Length() != 0)
	{
		qs.push_back(std::move(q));
	}

	return qs;
}

double LCCRF::Fit(std::string& dataFile, int max_iter, 
			    double learning_rate, double l1)
{
	std::vector<Query> qs = LoadData(dataFile);
	return Fit(qs, max_iter, learning_rate, l1);
}

double LCCRF::GetEdgeWeight(uint16_t from_label, uint16_t to_label)
{
	uint32_t id = 0.0;
	if (_features.GetFeatureID(from_label, to_label, id))
	{
		return _weights[id];
	}

	return 0.0;
}

double LCCRF::GetNodeWeight(uint32_t xid, uint16_t label)
{
	uint32_t id = 0.0;
	if (_features.GetFeatureID(xid, label, id))
	{
		return _weights[id];
	}

	return 0.0;
}

void LCCRF::_GenerateLCCRFFeatures(vector<Query>& qs)
{
	for (auto ite = qs.begin(); ite != qs.end(); ite++)
	{
		_features.CollectFromQuery(*ite);
	}
    spdlog::info("{0} features, {1} labels", _features.FeatureCount(), _features.LabelCount());
}

std::vector<uint16_t> LCCRF::Predict(const Query& q)
{
	Decoder decoder(*this);
	std::vector<uint16_t> res = decoder.Decode(q);
	return res;
}

std::vector<uint8_t> LCCRF::Predict(const std::vector<pair<int, int>>& x, int length)
{
	throw std::runtime_error("LCCRF::Predict is not implemented");
}

void LCCRF::Save(const string& path)
{
    spdlog::info("Save weights to {0}. {1} features, {2} labels.", 
              path.c_str(), 
              _features.FeatureCount(),
              _features.LabelCount());
    ofstream ofs(path.c_str());
    ofs << _features.FeatureCount() << "\t" << _features.LabelCount() << std::endl;
	for (auto xIte = _features.UnigramFeatures.begin(); 
	     xIte != _features.UnigramFeatures.end(); xIte++)
	{
		for (auto labelIte = xIte->second->begin(); labelIte != xIte->second->end(); labelIte++)
		{
			ofs << "U" << "\t"
				<< labelIte->second << "\t"
				<< xIte->first << "\t"
				<< labelIte->first << "\t"
				<< _weights[labelIte->second]
				<< std::endl;
		}
	}
	for (uint16_t i = 0; i < (uint16_t)_features.LabelCount(); i++)
	{
		for (uint16_t j = 0; j < (uint16_t)_features.LabelCount(); j++)
		{
			uint32_t id = 0;
			_features.GetFeatureID(i, j, id);
			ofs << "T" << "\t"
				<< id << "\t"
				<< i << "\t"
				<< j << "\t"
				<< _weights[id]
				<< std::endl;
		}
	}
    ofs.close();
}

void LCCRF::Load(const string& path)
{
    _features.Clear();
    ifstream ifs(path.c_str());
	int fcnt = 0;
	int lCount = 0;
    ifs >> fcnt >> lCount;
	std::vector<double> zero_weights(fcnt, 0.0);
	_weights.swap(zero_weights);
	string type;
	uint32_t fid = 0;
    while(ifs >> type >> fid)
    {
		if (type == "U")
		{
			int xid = 0;
			int label = 0;
			double weight = 0.0;
			ifs >> xid >> label >> weight;
			_features.AddUnigramFeature(xid, label, &fid);
			_weights[fid] = weight;
		}
		else if (type == "T")
		{
			int s1 = 0;
			int s2 = 0;
			double weight = 0.0;
			ifs >> s1 >> s2 >> weight;
			_features.AddTransitionFeature(s1, s2, &fid);
			_weights[fid] = weight;
		}
    }
    ifs.close();
    spdlog::info("Load from {0}. {1} features, {2} labels.", 
		path.c_str(), 
        _features.FeatureCount(), _features.LabelCount());
 
}

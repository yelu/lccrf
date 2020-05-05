#include <fstream>
#include <spdlog/spdlog.h>
#include "lccrf.h"
#include "perceptron.h"
#include "viterbi.h"
#include "sgdl1.h"
#include "fwbw.h"
#include "str.h"
using std::ofstream;
using std::ifstream;

LCCRF::LCCRF()
{

}

LCCRF::~LCCRF(void)
{
}

void LCCRF::Fit(const vector<Query>& qs, 
                int max_iter,  double learning_rate, double l1)
{
	// iterate through all queries to construct lccrf features.
	_features.Clear();
	_GenerateLCCRFFeatures(qs);

	spdlog::info("queries : {0}, max iterations : {1}, step : {2}, l1 : {3}", qs.size(), max_iter, learning_rate, l1);

	std::vector<double> tmpWeights(_features.FeatureCount(), 0.0);
	_weights.swap(tmpWeights);
    PerceptronTrainer trainer(qs, *this);
	trainer.Run(learning_rate, max_iter);
}

void LCCRF::Fit(const std::string& dataFile, int max_iter, 
			    double learning_rate, double l1)
{
	std::ifstream infile(dataFile);

	std::vector<Query> qs;
	int length = 0;
	string line;
	Query q;
	while (std::getline(infile, line))
	{
		trim(line);
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
		vector<string> xids = split(line, ' ');
		int label = atoi(xids[0].c_str());
		if (label > UINT16_MAX)
		{
			throw std::runtime_error("query label can not be bigger than UINT16_MAX");
		}
		t.SetLabel((uint16_t)label);

		for (auto ite = xids.begin() + 1; ite != xids.end(); ite++)
		{
			vector<string> fields = split(ite->c_str(), ':');
			int xid = atoi(fields[0].c_str());
			if (xid < 0) 
			{ 
				throw std::runtime_error("feature id must ne positive");
			}
			float val = 1.0;
			if (fields.size() > 1)
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

	Fit(qs, max_iter, learning_rate, l1);
}

double LCCRF::GetWeight(uint16_t from_label, uint16_t to_label)
{
	auto ite = _features.TransitionFeatures.find(TransitionFeature(from_label, to_label));
	if (ite != _features.TransitionFeatures.end())
	{
		return _weights[ite->second];
	}
	return 0.0;
}

double LCCRF::GetWeight(uint32_t id, uint16_t label)
{
	auto label_ids = _features.UnigramFeatures.find(id);
	if (label_ids == _features.UnigramFeatures.end())
	{
		return 0.0;
	}

	auto ite = label_ids->second->find(label);
	if (ite == label_ids->second->end()) { return 0.0; }

	return _weights[ite->second];
}

void LCCRF::_GenerateLCCRFFeatures(vector<Query>& qs)
{
	for (auto ite = qs.begin(); ite != qs.end(); ite++)
	{
		_features.AddSample(*ite);
	}
    spdlog::info("{0} features, {1} labels", _features.FeatureCount(), _features.LabelCount());
}

std::vector<uint16_t> LCCRF::Predict(const Query& q)
{
	std::vector<uint16_t> res;
    MultiArray<double, 3> edges(q.Length(), _features.LabelCount(), _features.LabelCount(), 0.0);
    MultiArray<double, 2> nodes(q.Length(), _features.LabelCount(), 0.0);

	// 
    FWBW::MakeEdgesAndNodes(q, res, _features, _weights, edges, nodes);
    vector<int> path(q.Length(), -1);
    Viterbi::GetPath(edges, nodes, path);
    y.Clear();
    for(auto ite = path.begin(); ite != path.end(); ite++)
    {
        y.AppendTag(*ite);
    }
	return y;
}

std::vector<uint8_t> LCCRF::Predict(const std::vector<pair<int, int>>& x, int length)
{
	X xInner(x, length);
	auto y = Predict(xInner);
	return y.Tags;
}

/*
   j  : current position
   s1 : previous state
   s2 : current state
*/
double LCCRF::_Phi(int s1, int s2, int j,
                  const X& x, 
				  const Y& y,
				  const LCCRFFeatures& lccrfFeatures,
                  vector<double>& weights,
                  list<pair<int, double>>* hitFeatures)
{
    double ret = 0.0;

	// hitted unigram features.
    for(auto xIte = x.Features.begin(); xIte != x.Features.end(); xIte++)
    {
		if (xIte->second->count(j) > 0 && y.Tags[j] == s2)
		{
			LCCRFFeatures::UnigramFeature feature(xIte->first, s2);
			int fid = lccrfFeatures.GetFeatureID(feature);
			if (fid >= 0)
			{
				ret += weights[fid];
				hitFeatures->push_back(pair<int, double>(fid, weights[fid]));
			}
		}
    }

	// hitted transition features.
	LCCRFFeatures::TransitionFeature feature(s1, s2);
	int fid = lccrfFeatures.GetFeatureID(feature);
	if (fid >= 0 && j > 0 && y.Tags[j - 1] == s1 && y.Tags[j] == s2) 
	{ 
		ret += weights[fid];
		hitFeatures->push_back(pair<int, double>(fid, weights[fid]));
	}
	
    return ret;
}

pair<list<list<pair<int, double>>>, double> LCCRF::Debug(const X& x, 
                                                         const Y& y)
{
    int preState = -1;
    double score = 0.0;
    pair<list<list<pair<int, double>>>, double> res;
    for(int j = 0; j < x.Length(); j++)
    {
        if(y.Tags[j] >= _features.LabelCount())
        {
            return res;
        }
        res.first.push_back(list<pair<int, double>>());
        score += _Phi(preState, y.Tags[j], j, x, y, _features, _weights, &(res.first.back()));
        preState = y.Tags[j];
    }
    res.second = score;
    return res;
}

void LCCRF::Save(const string& path)
{
    LOG("Save weights to %s. %d features, %d labels.", 
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
	for (auto tranIte = _features.TransitionFeatures.begin();
		 tranIte != _features.TransitionFeatures.end(); tranIte++)
	{
		ofs << "T" << "\t"
			<< tranIte->second << "\t"
			<< tranIte->first.s1 << "\t"
			<< tranIte->first.s2 << "\t"
			<< _weights[tranIte->second]
			<< std::endl;
	}
    ofs.close();
}

void LCCRF::Load(const string& path)
{
    _features.Clear();
    ifstream ifs(path.c_str());
	int fCount = 0;
	int lCount = 0;
    ifs >> fCount >> lCount;
	std::vector<double> newWeights(fCount, 0.0);
	_weights.swap(newWeights);
	string type;
	int fID = 0;
    while(ifs >> type >> fID)
    {
		if (type == "U")
		{
			int xid = 0;
			int label = 0;
			double weight = 0.0;
			ifs >> xid >> label >> weight;
			_features.AddUnigramFeature(xid, label, fID);
			_weights[fID] = weight;
		}
		else if (type == "T")
		{
			int s1 = 0;
			int s2 = 0;
			double weight = 0.0;
			ifs >> s1 >> s2 >> weight;
			_features.AddTransitionFeature(s1, s2, fID);
			_weights[fID] = weight;
		}
    }
    ifs.close();
    LOG("Load from %s. %d features, %d labels.", path.c_str(), 
        _features.FeatureCount(), _features.LabelCount());
 
}

#include "LCCRF.h"
#include "Viterbi.h"
#include "SGDL1.h"
#include "FWBW.h"
#include <fstream>
#include "../lib/Str.h"
using std::ofstream;
using std::ifstream;

LCCRF::LCCRF()
{

}

LCCRF::~LCCRF(void)
{
}

void LCCRF::Fit(const vector<X>& xs, const vector<Y>& ys, 
                int maxIteration,  double learningRate, double l1)
{
    _xs = &xs;
    _ys = &ys;

	// iterate through all xs and ys to construct lccrf features.
	_lccrfFeatures.Clear();
	_GenerateLCCRFFeatures(xs, ys);

	LOG("queries : %lu, max iterations : %d, step : %f, l1 : %f", xs.size(), maxIteration, learningRate, l1);

	std::vector<double> tmpWeights(_lccrfFeatures.FeatureCount(), 0.0);
	_weights.swap(tmpWeights);
    SGDL1 sgd(*_xs, *_ys, _lccrfFeatures, _weights);
    sgd.Run(learningRate, l1, maxIteration);
}

void LCCRF::Fit(const std::string& dataFile, int maxIteration, 
			    double learningRate, double l1)
{
	std::ifstream infile(dataFile);

	std::vector<X> xs;
	std::vector<Y> ys;
	int length = 0;
	while (infile >> length)
	{
		X x(length);
		Y y(length);
		for (int i = 0; i < length; i++)
		{
			int pos = 0;
			int label = 0;
			string xidStr;
			infile >> pos >> label >> xidStr;
			y.SetTag(pos, label);
			vector<string> xids = split(xidStr, ',');
			for (auto ite = xids.begin(); ite != xids.end(); ite++)
			{
				int xid = atoi(ite->c_str());
				if (xid < 0) { continue; }
				x.AddFeature(xid, pos);
			}
		}
		xs.push_back(x);
		ys.push_back(y);
	}

	Fit(xs, ys, maxIteration, learningRate, l1);
}

void LCCRF::_GenerateLCCRFFeatures(const vector<X>& xs, const vector<Y>& ys)
{
	vector<X>::const_iterator iteX = xs.begin();
	vector<Y>::const_iterator iteY = ys.begin();
	for (; iteX != xs.end() && iteY != ys.end(); iteX++, iteY++)
	{
		_lccrfFeatures.AddSample(*iteX, *iteY);
	}
    LOG("%d features, %d labels.", _lccrfFeatures.FeatureCount(), _lccrfFeatures.LabelCount());
}

Y LCCRF::Predict(const X& x)
{
	Y y(x.Length());
    MultiArray<double, 3> edges(x.Length(), _lccrfFeatures.LabelCount(), _lccrfFeatures.LabelCount(), 0.0);
    MultiArray<double, 2> nodes(x.Length(), _lccrfFeatures.LabelCount(), 0.0);
    //SGD::MakePhiMatrix(x, _weights, 1.0, graph);
    FWBW::MakeEdgesAndNodes(x, y, _lccrfFeatures, _weights, edges, nodes);
    vector<int> path(x.Length(), -1);
    Viterbi::GetPath(edges, nodes, path);
    y.Clear();
    for(auto ite = path.begin(); ite != path.end(); ite++)
    {
        y.AppendTag(*ite);
    }
	return y;
}

std::vector<int> LCCRF::Predict(const std::vector<pair<int, int>>& x, int length)
{
	X xInner(x, length);
	auto y = Predict(xInner);
	return y.Tags;
}

const vector<double>& LCCRF::GetWeights() const
{
    return _weights;
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
        if(y.Tags[j] >= _lccrfFeatures.LabelCount())
        {
            return res;
        }
        res.first.push_back(list<pair<int, double>>());
        score += _Phi(preState, y.Tags[j], j, x, y, _lccrfFeatures, _weights, &(res.first.back()));
        preState = y.Tags[j];
    }
    res.second = score;
    return res;
}

void LCCRF::Save(const string& path)
{
    LOG("Save weights to %s. %d features, %d labels.", 
              path.c_str(), 
              _lccrfFeatures.FeatureCount(),
              _lccrfFeatures.LabelCount());
    ofstream ofs(path.c_str());
    ofs << _lccrfFeatures.FeatureCount() << "\t" << _lccrfFeatures.LabelCount() << std::endl;
	for (auto xIte = _lccrfFeatures.UnigramFeatures.begin(); 
	     xIte != _lccrfFeatures.UnigramFeatures.end(); xIte++)
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
	for (auto tranIte = _lccrfFeatures.TransitionFeatures.begin();
		 tranIte != _lccrfFeatures.TransitionFeatures.end(); tranIte++)
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
    _lccrfFeatures.Clear();
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
			_lccrfFeatures.AddUnigramFeature(xid, label, fID);
			_weights[fID] = weight;
		}
		else if (type == "T")
		{
			int s1 = 0;
			int s2 = 0;
			double weight = 0.0;
			ifs >> s1 >> s2 >> weight;
			_lccrfFeatures.AddTransitionFeature(s1, s2, fID);
			_weights[fID] = weight;
		}
    }
    ifs.close();
    LOG("Loaded from %s. %d features, %d labels.", 
              path.c_str(), 
              _lccrfFeatures.FeatureCount(),
              _lccrfFeatures.LabelCount());
 
}

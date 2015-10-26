#include "LCCRF.h"
#include "Viterbi.h"
#include "SGDL1.h"
#include "FWBW.h"
#include <fstream>
using std::wofstream;
using std::wifstream;

LCCRF::LCCRF(int featureCount, int labelCount):_weights(featureCount, 0.0)
{
    _featureCount = featureCount;
    _tagCount = labelCount;
}

LCCRF::LCCRF()
{
    _featureCount = -1;
    _tagCount = -1;
}

LCCRF::~LCCRF(void)
{
}

void LCCRF::Fit(const vector<X>& xs, 
                const vector<Y>& ys, 
                int maxIteration, 
                double learningRate, 
                double l1)
{
    _xs = &xs;
    _ys = &ys;

    // if no feature/tag count provided, iterate through training set to get it.
    if(_featureCount <= 0 || _featureCount <= 0)
    {
        _featureCount = XList::GetFeatureCount(*_xs);
        _tagCount = YList::GetTagCount(*_ys);
        std::vector<double> tmpWeights(_featureCount, 0.0);
        _weights.swap(tmpWeights);
    }

    SGDL1 sgd(*_xs, *_ys, _weights, _tagCount);
    sgd.Run(learningRate, l1, maxIteration);
}

void LCCRF::Fit(XList& xs, YList& ys, int maxIteration, double learningRate, double l1)
{
	LOG_DEBUG("queries : %d, iter : %d step : %f l1 : %f", xs.Raw().size(), maxIteration, learningRate, l1);
    Fit(xs.Raw(), ys.Raw(), maxIteration, learningRate, l1);
}

void LCCRF::Predict(const X& x, Y& y)
{
    MultiArray<double, 3> edges(x.Length(), _tagCount, _tagCount, 0.0);
    MultiArray<double, 2> nodes(x.Length(), _tagCount, 0.0);
    //SGD::MakePhiMatrix(x, _weights, 1.0, graph);
    FWBW::MakeEdgesAndNodes(x, _weights, edges, nodes);
    vector<int> path(x.Length(), -1);
    Viterbi::GetPath(edges, nodes, path);
    y.Clear();
    for(auto ite = path.begin(); ite != path.end(); ite++)
    {
        y.AppendTag(*ite);
    }
}

void LCCRF::Predict(XList& xs, YList& ys)
{
    const vector<X>& rawXs = xs.Raw();
    for(auto ite = rawXs.begin(); ite != rawXs.end(); ite++)
    {
        Y y;
        Predict(*ite, y);
        ys.Append(y);
    }
}

vector<double>& LCCRF::GetWeights()
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
                  vector<double>& weights,
                  list<pair<int, double>>* hitFeatures)
{
    double ret = 0.0;
    for(auto ite = x.Raw().begin(); ite != x.Raw().end(); ite++)
    {
        auto featureID = ite->first;
        auto positions = ite->second;
        for(auto position = positions->begin(); position != positions->end(); position++)
        {
            if(j == position->j && s1 == position->s1 && s2 == position->s2)
            {
                ret += (weights[featureID]);
                hitFeatures->push_back(pair<int, double>(featureID, weights[featureID]));
            }
        }
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
        if(y.Tags()[j] >= _tagCount)
        {
            return res;
        }
        res.first.push_back(list<pair<int, double>>());
        score += _Phi(preState, y.Tags()[j], j, x, _weights, &(res.first.back()));
        preState = y.Tags()[j];
    }
    res.second = score;
    return res;
}

void LCCRF::Save(string& path)
{
    wofstream ofs(path.c_str());
    ofs << _featureCount << L"\t" << _tagCount << std::endl;
    for(size_t i = 0; i < _weights.size(); i++)
    {
        ofs << i << "\t" << _weights[i] << std::endl;
    }
}

void LCCRF::Load(string& path)
{
    wifstream ifs(path.c_str());
    ifs >> _featureCount >> _tagCount;
    int featureID = 0;
    double featureWeight = 0.0;
    _weights.clear();
    while(ifs >> featureID >> featureWeight)
    {
        _weights.push_back(featureWeight);
    }
}

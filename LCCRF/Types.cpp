#include "Types.h"

double XSampleType::GetFeatureValue(int j, int s1, int s2, int featureID) const 
{
	Position pos(j, s1, s2);
	auto ite = _features.find(featureID);
	if(ite == _features.end() || ite->second->count(pos) == 0)
	{
		return 0;
	}
	return 1;
}

void XSampleType::SetFeature(int j, int s1, int s2, int featureID)
{
	Position pos(j, s1, s2);
	if(_features.count(featureID) == 0)
	{
        _features[featureID] = shared_ptr<PositionSet>(new PositionSet());
	}
	_features[featureID]->insert(pos);
}

int YSampleType::Length() const 
{
	return (int)_tags.size();
}

const std::vector<int>& YSampleType::Tags() const
{
	return _tags;
}

void YSampleType::Clear()
{
	_tags.clear();
}

void YSampleType::SetTag(int i, int tag)
{
    if(i >= (int)_tags.size())
    {
        _tags.resize(i + 1);
    }
	_tags[i] = tag;
}

void XType::SetFeature(int i, int j, int s1, int s2, int featureID)
{
	int ite = (int)(_xs.size());
    for(; ite < i + 1; ++ite)
    {
        XSampleType x;
        _xs.push_back(x);
    }
    _xs.back().SetFeature(j, s1, s2, featureID);
}

const vector<XSampleType>& XType::Raw()
{
	return _xs;
}

void XType::Append(XSampleType& x)
{
    _xs.push_back(x);
}

XSampleType& XType::At(int i)
{
	for(int ite = (int)_xs.size(); ite < i + 1; ite++)
    {
        XSampleType x;
        _xs.push_back(x);
    }
    if(-1 == i)
    {
        i = (int)_xs.size() - 1;
    }
    return _xs[i];
}

int XType::GetFeatureCount(const vector<XSampleType>& xs)
{
    std::unordered_set<int> features;
    for(auto x = xs.begin(); x != xs.end(); x++)
    {
        for(auto feature = x->Raw().begin(); feature != x->Raw().end(); feature++)
        {
            features.insert(feature->first);
        }
    }
    return features.size();
}

void YType::SetTag(int i, int j, int tag)
{
    for(int ite = (int)_ys.size(); ite < i + 1; ite++)
    {
        YSampleType y;
        _ys.push_back(y);
    }
    // -1 is the last.
    if(-1 == i) {i = (int)_ys.size() - 1;}
	_ys[i].SetTag(j, tag);
}

void YType::Append(YSampleType& y)
{
    _ys.push_back(y);
}

YSampleType& YType::At(int i)
{
	for(int ite = (int)_ys.size(); ite < i + 1; ite++)
    {
        YSampleType y;
        _ys.push_back(y);
    }
    if(-1 == i)
    {
        i = (int)_ys.size() - 1;
    }
    return _ys[i];
}

const vector<YSampleType>& YType::Raw() const
{
	return _ys;
}

int YType::GetTagCount(const vector<YSampleType>& ys)
{
    std::unordered_set<int> tags;
    for(auto y = ys.begin(); y != ys.end(); y++)
    {
        for(auto tag = y->Raw().begin(); tag != y->Raw().end(); tag++)
        {
            tags.insert(*tag);
        }
    }
    return tags.size();
}

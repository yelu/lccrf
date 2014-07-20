#include "Types.h"

XSampleType::XSampleType(void)
{
	_length = 0;
}

shared_ptr<std::set<int>> XSampleType::GetFeatures(int j, int s1, int s2) const
{
	Key key(j, s1, s2);
	auto ite = _features.find(key);
	if( ite == _features.end())
	{
		shared_ptr<std::set<int>> ret;
		return ret;
	}
	return ite->second;
}

double XSampleType::GetFeatureValue(int j, int s1, int s2, int featureID) const 
{
	Key key(j, s1, s2);
	auto ite = _features.find(key);
	if(ite == _features.end() || ite->second->count(featureID) == 0)
	{
		return 0;
	}
	return 1;
}

void XSampleType::SetFeature(int j, int s1, int s2, int featureID)
{
	Key key(j, s1, s2);
	if(_features.count(key) == 0)
	{
		_features[key] = shared_ptr<std::set<int>>(new std::set<int>());
	}
	_features[key]->insert(featureID);
    _featureSet.insert(featureID);
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
    for(int ite = (int)_xs.size(); ite < i + 1; ite++)
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

const vector<YSampleType>& YType::Raw()
{
	return _ys;
}

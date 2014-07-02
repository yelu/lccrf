#include "Types.h"

XSampleType::XSampleType(void)
{
	_length = 0;
}

shared_ptr<std::set<int>> XSampleType::GetFeatures(int j, int s1, int s2) const
{
	Key key(j, s1, s2);
	if(_features.count(key) == 0)
	{
		shared_ptr<std::set<int>> ret(new std::set<int>());
		return ret;
	}
	return _features.at(key);
}

double XSampleType::GetFeatureValue(int j, int s1, int s2, int featureID) const 
{
	Key key(j, s1, s2);
	if(_features.count(key) == 0 || (_features.at(key))->count(featureID) == 0)
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
}

size_t YSampleType::Length() const 
{
	return _tags.size();
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
    for(int ite = _xs.size(); ite < i + 1; ite++)
    {
        XSampleType x;
        _xs.push_back(x);
        _index[ite] = &(_xs.back());
    }
    _index[i]->SetFeature(j, s1, s2, featureID);
}

const list<XSampleType>& XType::Raw()
{
	return _xs;
}

void XType::Append(XSampleType& x)
{
    _xs.push_back(x);
    _index[_xs.size() - 1] = &(_xs.back());
}

XSampleType& XType::At(int i)
{
	for(int ite = _xs.size(); ite < i + 1; ite++)
    {
        XSampleType x;
        _xs.push_back(x);
        _index[ite] = &(_xs.back());
    }
    if(-1 == i)
    {
        i = _xs.size() - 1;
    }
    return *(_index[i]);
}

void YType::SetTag(int i, int j, int tag)
{
    for(int ite = _ys.size(); ite < i + 1; ite++)
    {
        YSampleType y;
        _ys.push_back(y);
        _index[ite] = &(_ys.back());
    }
    // -1 is the last.
    if(-1 == i) {i = _ys.size() - 1;}
	_index[i]->SetTag(j, tag);
}

void YType::Append(YSampleType& y)
{
    _ys.push_back(y);
    _index[_ys.size() - 1] = &(_ys.back());
}

YSampleType& YType::At(int i)
{
	for(int ite = _ys.size(); ite < i + 1; ite++)
    {
        YSampleType y;
        _ys.push_back(y);
        _index[ite] = &(_ys.back());
    }
    if(-1 == i)
    {
        i = _ys.size() - 1;
    }
    return *(_index[i]);
}

const list<YSampleType>& YType::Raw()
{
	return _ys;
}

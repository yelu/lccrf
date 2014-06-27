#include "Types.h"

XType::XType(void)
{
}

shared_ptr<std::set<int>> XType::GetFeatures(int j, int s1, int s2) const
{
	Key key(j, s1, s2);
	if(_features.count(key) == 0)
	{
		shared_ptr<std::set<int>> ret(new std::set<int>());
		return ret;
	}
	return _features.at(key);
}

double XType::GetFeatureValue(int j, int s1, int s2, int featureID) const 
{
	Key key(j, s1, s2);
	if(_features.count(key) == 0 || (_features.at(key))->count(featureID) == 0)
	{
		return 0;
	}
	return 1;
}

void XType::AddFeature(int j, int s1, int s2, int featureID)
{
	Key key(j, s1, s2);
	if(_features.count(key) == 0)
	{
		_features[key] = shared_ptr<std::set<int>>(new std::set<int>());
	}
	_features[key]->insert(featureID);
}

size_t YType::Length() const 
{
	return _tags.size();
}

const std::vector<int>& YType::Tags() const
{
	return _tags;
}

void YType::Clear()
{
	_tags.clear();
}

void YType::AddTag(int i, int tag)
{
    if(i >= (int)_tags.size())
    {
        _tags.resize(i + 1);
    }
	_tags[i] = tag;
}

void XListType::AddFeature(int i, int j, int s1, int s2, int featureID)
{
    for(int ite = _xs.size(); ite < i + 1; ite++)
    {
        XType x;
        _xs.push_back(x);
        _index[ite] = &(_xs.back());
    }
    _index[i]->AddFeature(j, s1, s2, featureID);
}

const list<XType>& XListType::Raw()
{
	return _xs;
}

void XListType::Append(XType& x)
{
    _xs.push_back(x);
    _index[_xs.size() - 1] = &(_xs.back());
}

XType& XListType::At(int i)
{
    if(-1 == i)
    {
        i = _xs.size() - 1;
    }
    return *(_index[i]);
}

void YListType::AddTag(int i, int j, int tag)
{
    for(int ite = _ys.size(); ite < i + 1; ite++)
    {
        YType y;
        _ys.push_back(y);
        _index[ite] = &(_ys.back());
    }
    // -1 is the last.
    if(-1 == i) {i = _ys.size() - 1;}
	_index[i]->AddTag(j, tag);
}

void YListType::Append(YType& y)
{
    _ys.push_back(y);
    _index[_ys.size() - 1] = &(_ys.back());
}

YType& YListType::At(int i)
{
    if(-1 == i)
    {
        i = _ys.size() - 1;
    }
    return *(_index[i]);
}

const list<YType>& YListType::Raw()
{
	return _ys;
}

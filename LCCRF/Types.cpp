#include "Types.h"

XType::XType(int length)
{
	_length = length;
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

size_t XType::Length() const
{
	return _length;
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

void YType::AddTag(int j)
{
	_tags.push_back(j);
}

void XListType::AddFeature(int j, int s1, int s2, int featureID)
{
	_xs.back().AddFeature(j, s1, s2, featureID);
}

void XListType::PushBack(int length)
{
	XType x(length);
	_xs.push_back(x);
}

const list<XType>& XListType::Raw()
{
	return _xs;
}

void YListType::AddTag(int tag)
{
	_ys.back().AddTag(tag);
}

void YListType::PushBack()
{
	YType y;
	_ys.push_back(y);
	_index[_ys.size() - 1] = &(_ys.back());
}

int YListType::Size()
{
	return _ys.size();
}

int YListType::LengthOf(int i)
{
	return _index[i]->Length();
}

int YListType::TagOf(int i, int j)
{
	return (_index[i]->Tags())[j];
}

const list<YType>& YListType::Raw()
{
	return _ys;
}
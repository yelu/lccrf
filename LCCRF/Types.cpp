#include "Types.h"

X::X(int length)
{
	_length = length;
}

shared_ptr<std::set<int>> X::GetFeatures(int j, int s1, int s2) const
{
	Key key(j, s1, s2);
	if(_features.count(key) == 0)
	{
		shared_ptr<std::set<int>> ret(new std::set<int>());
		return ret;
	}
	return _features.at(key);
}

double X::GetFeatureValue(int j, int s1, int s2, int featureID) const 
{
	Key key(j, s1, s2);
	if(_features.count(key) == 0 || (_features.at(key))->count(featureID) == 0)
	{
		return 0;
	}
	return 1;
}

size_t X::Length() const
{
	return _length;
}

//export to cython.
void X::SetFeature(int j, int s1, int s2, int featureID)
{
	Key key(j, s1, s2);
	if(_features.count(key) == 0)
	{
		_features[key] = shared_ptr<std::set<int>>(new std::set<int>());
	}
	_features[key]->insert(featureID);
}

size_t Y::Length() const 
{
	return _tags.size();
}

const std::vector<int>& Y::Tags() const
{
	return _tags;
}

void Y::Clear()
{
	_tags.clear();
}

// export to python.
void Y::AppendTag(int j)
{
	_tags.push_back(j);
}


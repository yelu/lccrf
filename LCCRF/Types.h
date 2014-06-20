#pragma once

#include <functional>
#include <list>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <set>
#include "SGD.h"
using std::function;
using std::list;
using std::wstring;
using std::vector;
using std::set;
using std::map;
using std::shared_ptr;

struct X
{
public:
	struct Key
	{
		Key(int _j, int _s1, int _s2)
		{
			j = _j;
			s1 = _s1;
			s2 = _s2;
		};
		int j;
		int s1;
		int s2;
	};

	class KeyCompare 
	{ // simple comparison function
	   public:
		  bool operator()(const X::Key x1,const X::Key x2) 
		  { 
			  if(x1.j < x2.j)
			  {
				  return true;
			  }
			  else if(x1.j > x2.j)
			  {
				  return false;
			  }

			  if(x1.s1 < x2.s1)
			  {
				  return true;
			  }
			  else if(x1.s1 > x2.s1)
			  {
				  return false;
			  }

			  return x1.s2 < x2.s2;
		  }
	};

	X()
	{
		_length = 0;
	}

	shared_ptr<std::set<int>> GetFeatures(int j, int s1, int s2) const
	{
		Key key(j, s1, s2);
		if(_features.count(key) == 0)
		{
			shared_ptr<std::set<int>> ret(new std::set<int>());
			return ret;
		}
		return _features.at(key);
	}

	double GetFeatureValue(int j, int s1, int s2, int featureID) const 
	{
		Key key(j, s1, s2);
		if(_features.count(key) == 0 || (_features.at(key))->count(featureID) == 0)
		{
			return 0;
		}
		return 1;
	}

	size_t Length() const
	{
		return _length;
	}

	void SetLength(int length)
	{
		_length = length;
	}

	//export to python.
	void SetFeature(int j, int s1, int s2, int featureID)
	{
		Key key(j, s1, s2);
		if(_features.count(key) == 0)
		{
			_features[key] = shared_ptr<std::set<int>>(new std::set<int>());
		}
		_features[key]->insert(featureID);
	}

private:
	std::map<Key, shared_ptr<std::set<int>>, KeyCompare> _features;
	size_t _length;
};

struct Y
{
	size_t Length() const 
	{
		return _tags.size();
	}

	const std::vector<int>& Tags() const
	{
		return _tags;
	}

	void Clear()
	{
		_tags.clear();
	}

	// export to python.
	void AppendTag(int j)
	{
		_tags.push_back(j);
	}

private:
	std::vector<int> _tags;
};

struct ListX
{
	void Append(const X& x)
	{
		_xs.push_back(x);
	}

	const list<X>& GetX() const
	{
		return _xs;
	}

private:
	list<X> _xs;
};

struct ListY
{
	void Append(const Y& y)
	{
		_ys.push_back(y);
	}

	const list<Y>& GetY() const
	{
		return _ys;
	}

private:
	list<Y> _ys;
};

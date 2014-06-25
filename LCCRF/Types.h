#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <set>
#include <list>
#include "Log.h"
using std::wstring;
using std::vector;
using std::set;
using std::map;
using std::list;
using std::shared_ptr;

class XType
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
		  bool operator()(const XType::Key& x1,const XType::Key& x2) const
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


	// export to cython.
	XType(int length);

	shared_ptr<std::set<int>> GetFeatures(int j, int s1, int s2) const;

	double GetFeatureValue(int j, int s1, int s2, int featureID) const ;

	size_t Length() const;

	//export to cython.
	void AddFeature(int j, int s1, int s2, int featureID);

private:
	std::map<Key, shared_ptr<std::set<int>>, KeyCompare> _features;
	size_t _length;
};

class YType
{
public:

	YType()
	{
		LOG_DEBUG("Y constructed.");
	}

	size_t Length() const;

	const std::vector<int>& Tags() const;

	void Clear();

	// export to python.
	void AddTag(int j);

private:
	std::vector<int> _tags;
};

class XListType
{
public:
	XListType()
	{
	}

	// export to python.
	void AddFeature(int j, int s1, int s2, int featureID);

	// export to python.
	void PushBack(int length);

	const list<XType>& Raw();

private:
	list<XType> _xs;
};

class YListType
{
public:
	YListType()
	{
		LOG_DEBUG("YList constructed.");
	}

	// export to python.
	void PushBack();

	// export to python.
	void AddTag(int tag);

	// export to python.
	int Size();

	// export to python.
	int LengthOf(int i);

	// export to python.
	int TagOf(int i, int j);

	const list<YType>& Raw();

private:
	list<YType> _ys;
	map<int, const YType*> _index;
};

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

    const std::map<Key, shared_ptr<std::set<int>>, KeyCompare>& Raw()
    {
        return _features;
    }

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

    const std::vector<int>& Raw()
    {
        return _tags;
    }

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

    void GetAllFeatures(list<list<std::pair<list<int>, list<int>>>>& res)
    {
        for(auto i = _xs.begin(); i != _xs.end(); i++)
        {
            auto x = i->Raw();
            list<std::pair<list<int>, list<int>>> xOut;
            for(auto j = x.begin(); j != x.end(); j++)
            {
                std::pair<list<int>, list<int>> fs;
                XType::Key k = j->first;
                fs.first.push_back(k.j);
                fs.first.push_back(k.s1);
                fs.first.push_back(k.s2);
                for(auto f = j->second->begin(); f != j->second->end(); f++)
                {
                    fs.second.push_back(*f);
                }
                xOut.push_back(fs);
            }
            res.push_back(xOut);
        } 
    }

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

    void GetAllTags(list<list<int>>& res)
    {
        for(auto i = _ys.begin(); i != _ys.end(); i++)
        {
            std::list<int> tags;
            auto y = i->Raw();
            for(auto tag = y.begin(); tag != y.end(); tag++)
            {
                tags.push_back(*tag);
            }
            res.push_back(tags);
        }
    }

	const list<YType>& Raw();

private:
	list<YType> _ys;
	map<int, const YType*> _index;
};

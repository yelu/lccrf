#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <set>
#include <list>
#include <tuple>
#include <unordered_map>
#include "Log.h"
#include "MurmurHash3.h"
using std::wstring;
using std::vector;
using std::set;
using std::map;
using std::list;
using std::tuple;
using std::pair;
using std::unordered_map;
using std::shared_ptr;

class XSampleType
{
public:
	struct Position
	{
		Position(int _j, int _s1, int _s2)
		{
			j = _j;
			s1 = _s1;
			s2 = _s2;
		}

		int j;
		int s1;
		int s2;

		struct Compare
		{
			const bool operator()(const Position& pos1, const Position& pos2) const
			{
				if(pos1.j != pos2.j) {return pos1.j < pos2.j;}
				if(pos1.s1 != pos2.s1) {return pos1.s1 < pos2.s2;}
				return pos1.s2 < pos2.s2;
			}
		};

		struct Hash
		{
			size_t operator()(const Position& k) const
			{
				uint32_t res  = 0;
				MurmurHash3_x86_32(&k, sizeof(Position), 47, &res);
				return (size_t)res;
			}
		};
	};

	typedef std::unordered_map<int, shared_ptr<std::set<Position, Position::Compare>>> FeaturesContainer;

	// export to cython.
	XSampleType(void)
	{
		_length = 0;
	}

    XSampleType(int length)
    {
        _length = length;
    }

	double GetFeatureValue(int j, int s1, int s2, int featureID) const;

	//export to cython.
	void SetFeature(int j, int s1, int s2, int featureID);

    int Length() const
    {
        return _length;
    }

	void SetLength(int length)
	{
		_length = length;
	}

	const FeaturesContainer& Raw() const
    {
        return _features;
    }

private:
	FeaturesContainer _features;
    int _length;
};

class YSampleType
{
public:

	YSampleType()
	{
	}

	int Length() const;

	const std::vector<int>& Tags() const;

	void Clear();

	// export to python.
    // set ith tag to be j
	void SetTag(int i, int j);

	void AppendTag(int tag)
    {
        _tags.push_back(tag);
    }

    const std::vector<int>& Raw()
    {
        return _tags;
    }

private:
	std::vector<int> _tags;
};

class XType
{
public:
	XType()
	{
	}

	// export to cython.
	void SetFeature(int i, int j, int s1, int s2, int featureID);

	const vector<XSampleType>& Raw();

    // export to cython.
    void ToArray(list<list<std::pair<list<int>, list<int>>>>& res)
    {
        for(auto i = _xs.begin(); i != _xs.end(); i++)
        {
            auto x = i->Raw();
            list<pair<list<int>, list<int>>> xOut;
			map<list<int>, list<int>> fs;
            for(auto j = x.begin(); j != x.end(); j++)
            {
				auto position = j->second->begin();
				for(; position != j->second->end(); position++)
				{
					list<int> pos;
					pos.push_back(position->j);
					pos.push_back(position->s1);
					pos.push_back(position->s2);
					fs[pos].push_back(j->first);
				}
            }
			for(auto j = fs.begin(); j != fs.end(); j++)
			{
				list<int> position;
				xOut.push_back(pair<list<int>, list<int>>(j->first, j->second));
			}
            res.push_back(xOut);
        } 
    }

    // export to cython.
    XSampleType& At(int i);

    // export to cython.
    void Append(XSampleType& x);

private:
	vector<XSampleType> _xs;
};

class YType
{
public:
	YType()
	{
	}

	// export to cython.
	void SetTag(int i, int j, int tag);

    // export to cython.
    void ToArray(list<list<int>>& res)
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

    // export to cython.
    void Append(YSampleType& y);

    // export to cython.
    YSampleType& At(int i);

    const vector<YSampleType>& Raw();

private:
	vector<YSampleType> _ys;
};

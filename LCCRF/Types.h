#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <set>
#include <list>
#include <unordered_map>
#include "Log.h"
using std::wstring;
using std::vector;
using std::set;
using std::map;
using std::list;
using std::unordered_map;
using std::shared_ptr;

class XSampleType
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

		struct EqualTo
		{
			const bool operator()(const Key& key1, const Key& key2) const
			{ 
				return (key1.j == key2.j && key1.s1 == key2.s1 && key1.s2 == key2.s2);
			}
		};

		struct Hash
		{
			std::size_t operator()(const Key& k) const
			{
				using std::size_t;
				using std::hash;
				using std::string;

				// Compute individual hash values for first,
				// second and third and combine them using XOR
				// and bit shifting:

				return ((hash<int>()(k.j)
						^ (hash<int>()(k.s1) << 1)) >> 1)
						^ (hash<int>()(k.s2) << 1);
			}
		};
	};

	typedef std::unordered_map<Key, shared_ptr<std::set<int>>, Key::Hash, Key::EqualTo> FeaturesContainer;

	// export to cython.
	XSampleType(void);

    XSampleType(int length)
    {
        _length = length;
    }

	shared_ptr<std::set<int>> GetFeatures(int j, int s1, int s2) const;

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

	const std::unordered_map<Key, shared_ptr<std::set<int>>, Key::Hash, Key::EqualTo>& Raw()
    {
        return _features;
    }

    const std::set<int>& GetFeatureSet() const
    {
        return _featureSet;
    }

private:
	std::unordered_map<Key, shared_ptr<std::set<int>>, Key::Hash, Key::EqualTo> _features;
    std::set<int> _featureSet;

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
            list<std::pair<list<int>, list<int>>> xOut;
            for(auto j = x.begin(); j != x.end(); j++)
            {
                std::pair<list<int>, list<int>> fs;
                XSampleType::Key k = j->first;
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

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <set>
#include <list>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
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
using std::unordered_set;
using std::shared_ptr;

class X
{
public:
    struct Feature
    {
		Feature(int _id, int _s1, int _s2)
        {
            id = _id;
            s1 = _s1;
            s2 = _s2;
        }

        int id;
        int s1;
        int s2;

        struct Equal
        {
			const bool operator()(const Feature& feat1, const Feature& feat2) const
            {
				return feat1.id == feat2.id;
            }
        };

        struct Hash
        {
            size_t operator()(const Feature& k) const
            {
                uint32_t res  = 0;
                MurmurHash3_x86_32(&k.id, sizeof(int), 47, &res);
                return (size_t)res;
            }
        };
    };

    typedef std::unordered_set<int> PositionSet;
    typedef std::unordered_map<Feature, shared_ptr<PositionSet>, Feature::Hash, Feature::Equal> FeaturesContainer;

    // export to cython.
    X(void)
    {
        _length = 0;
    }

    X(int length)
    {
        _length = length;
		for (int i = 0; i < _length; i++)
		{
			allUnigramPositions.insert(i);
		}
		for (int i = 1; i < _length; i++)
		{
			allBigramPositions.insert(i);
		}
    }

    double GetFeatureValue(int j, int s1, int s2, int featureID) const;

    //export to cython.
    void SetFeature(int j, int s1, int s2, int featureID);

	//export to cython.
	static void SetCommonFeature(int j, int s1, int s2, int featureID);

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
public:
	PositionSet allBigramPositions;
	PositionSet allUnigramPositions;

	static FeaturesContainer commonFeatures;
};

class Y
{
public:

    Y()
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

    const std::vector<int>& Raw() const
    {
        return _tags;
    }

private:
    std::vector<int> _tags;
};

class XList
{
public:
    XList()
    {
    }

    // export to cython.
    void SetFeature(int i, int j, int s1, int s2, int featureID);

    const vector<X>& Raw();

    // export to cython.
    X& At(int i);

    // export to cython.
    void Append(X& x);

    static int GetFeatureCount(const vector<X>& xs);

private:
    vector<X> _xs;
};

class YList
{
public:
    YList()
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
    void Append(Y& y);

    // export to cython.
    Y& At(int i);

    const vector<Y>& Raw() const;

    static int GetTagCount(const vector<Y>& ys);

private:
    vector<Y> _ys;
};

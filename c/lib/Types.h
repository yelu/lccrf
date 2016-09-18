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
		Feature(int _id, int _pos)
        {
            id = _id;
            pos = _pos;
        }

        int id;
        int pos;
    };
    
    // export to cython.
    X(size_t i)
    {
        _length = i;
    }

	X(const std::vector<pair<int, int>>& x, size_t i)
	{
		_length = i;
		for (auto f = x.begin(); f != x.end(); f++)
		{
			AddFeature(f->first, f->second);
		}
	}

	void AddFeature(int xFeatureID, int position)
	{
		if (xFeatureID < 0) { return; }
		if (Features.count(xFeatureID) == 0)
		{
			Features[xFeatureID] = shared_ptr<std::unordered_set<int>>(new std::unordered_set<int>());
		}
		Features[xFeatureID]->insert(position);
	}

    size_t Length() const
    {
        return _length;
    }

    void SetLength(int length)
    {
        _length = length;
    }

	std::unordered_map<int, std::shared_ptr<std::unordered_set<int>>> Features;

private:

    size_t _length;
};

class Y
{
public:

    Y(size_t length)
    {
		std::vector<int> tags(length, 0);
		Tags.swap(tags);
    }

	int Length() const { return Tags.size(); }

	void Clear() { Tags.clear(); }

    // export to python.
    // set ith tag to be j
	void SetTag(int i, int tag)
	{
		if (i >= Tags.size())
		{
			Tags.resize(i + 1);
		}
		Tags[i] = tag;
	}

    void AppendTag(int tag)
    {
		Tags.push_back(tag);
    }

	std::vector<int> Tags;

private:
};


class LCCRFFeatures
{
public:
	LCCRFFeatures()
	{
		_labelCount = 0;
		_featureCount = 0;
	}
	~LCCRFFeatures() {}

	struct UnigramFeature
	{
		UnigramFeature(int _id, int _s1)
		{
			id = _id;
			s1 = _s1;
		}

		int id;
		int s1;

		struct Equal
		{
			const bool operator()(const UnigramFeature& feat1, const UnigramFeature& feat2) const
			{
				return feat1.id == feat2.id && feat1.s1 == feat2.s1;
			}
		};

		struct Hash
		{
			size_t operator()(const UnigramFeature& k) const
			{
				uint32_t res = 0;
				MurmurHash3_x86_32(&k, sizeof(UnigramFeature), 47, &res);
				return (size_t)res;
			}
		};
	};

	struct TransitionFeature
	{
		TransitionFeature(int _s1, int _s2)
		{
			s1 = _s1;
			s2 = _s2;
		}

		int s1;
		int s2;

		struct Equal
		{
			const bool operator()(const TransitionFeature& feat1, const TransitionFeature& feat2) const
			{
				return feat1.s1 == feat2.s1 && feat1.s2 == feat2.s2;
			}
		};

		struct Hash
		{
			size_t operator()(const TransitionFeature& k) const
			{
				uint32_t res = 0;
				MurmurHash3_x86_32(&k, sizeof(TransitionFeature), 47, &res);
				return (size_t)res;
			}
		};
	};

	int GetFeatureID(const UnigramFeature& feature) const
	{
		auto labelAndId = UnigramFeatures.find(feature.id);
		if (labelAndId == UnigramFeatures.end()) { return -1; }

		auto id = labelAndId->second->find(feature.s1);
		if (id == labelAndId->second->end()) { return -1; }
		return id->second;
	}
	
	int GetFeatureID(const TransitionFeature& feature) const
	{
		if (TransitionFeatures.count(feature))
		{
			return TransitionFeatures.at(feature);
		}
		return -1;
	}

	void AddSample(const X& x, const Y& y)
	{
		// unigram
		for (auto fIte = x.Features.begin(); fIte != x.Features.end(); fIte++)
		{
			for (auto posIte = fIte->second->begin(); posIte != fIte->second->end(); posIte++)
			{
				int label = y.Tags[*posIte];
				AddUnigramFeature(fIte->first, label);
			}
		}

        // transition
		for (size_t i = 1; i < y.Tags.size(); i++)
		{
			AddTransitionFeature(y.Tags[i - 1], y.Tags[i]);
		}

	}

	size_t LabelCount() const
	{
		return _labelCount;
	}

	size_t FeatureCount() const
	{
		return _featureCount;
	}

	void AddUnigramFeature(int xid, int label, int id = -1)
	{
		if (UnigramFeatures.count(xid) == 0)
		{
			shared_ptr<std::map<int, int>> newMap(new std::map<int, int>);
			UnigramFeatures[xid] = newMap;
		}
		auto labelAndId = UnigramFeatures.at(xid);
		if (labelAndId->count(label) == 0)
		{
			(*labelAndId)[label] = (id >= 0) ? id : _featureCount;
			_featureCount += 1;
		}
        if(label >= _labelCount) {_labelCount = label + 1;}
	}

	void AddTransitionFeature(int s1, int s2, int id = -1)
	{
		LCCRFFeatures::TransitionFeature trans(s1, s2);
		if (TransitionFeatures.count(trans) == 0)
		{
			TransitionFeatures[trans] = (id >= 0) ? id : _featureCount;
			_featureCount += 1;
		}
        if(s1 >= _labelCount) {_labelCount = s1 + 1;}
        if(s2 >= _labelCount) {_labelCount = s2 + 1;}
	}

	void Clear()
	{
		UnigramFeatures.clear();
		TransitionFeatures.clear();
		_labelCount = 0;
		_featureCount = 0;
	}

	typedef std::unordered_map<int, std::shared_ptr<std::map<int, int>>> UnigramFeaturesType;
	typedef std::unordered_map<TransitionFeature, int, TransitionFeature::Hash, TransitionFeature::Equal> TransitionFeaturesType;
	UnigramFeaturesType UnigramFeatures;
	TransitionFeaturesType TransitionFeatures;

private:
	size_t _labelCount;
	size_t _featureCount;
};

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <set>
#include <list>
#include <tuple>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>
#include "hash.h"
#include "query.h"
using std::wstring;
using std::vector;
using std::set;
using std::map;
using std::list;
using std::tuple;
using std::pair;
using std::unordered_map;
using std::unordered_set;

struct UnigramFeature
{
	UnigramFeature(int id, int label)
	{
		this->id = id;
		this->label = label;
	}

	int id;
	int label;

	struct Equal
	{
		const bool operator()(const UnigramFeature& feat1, const UnigramFeature& feat2) const
		{
			return feat1.id == feat2.id && feat1.label == feat2.label;
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
	TransitionFeature(int label1, int label2)
	{
		this->label1 = label1;
		this->label2 = label2;
	}

	uint8_t label1;
	uint8_t label2;

	struct Equal
	{
		const bool operator()(const TransitionFeature& feat1, const TransitionFeature& feat2) const
		{
			return feat1.label1 == feat2.label1 && feat1.label2 == feat2.label2;
		}
	};

	struct Hash
	{
		size_t operator()(const TransitionFeature& k) const
		{
			uint32_t res = (uint32_t)k.label1 + (((uint32_t)k.label2) << 8);
			return res;
		}
	};
};

class LCCRFFeatures
{
public:
	LCCRFFeatures()
	{
		_label_count = 0;
		_feature_count = 0;
	}

	~LCCRFFeatures() 
	{
		for (auto ite = UnigramFeatures.begin(); ite != UnigramFeatures.end(); ite++)
		{
			delete ite->second;
		}
	}

	bool GetFeatureID(uint32_t fid, uint16_t label, uint32_t& res)
	{
		auto labelAndId = UnigramFeatures.find(fid);
		if (labelAndId == UnigramFeatures.end()) 
		{ 
			return false;
		}

		auto ite = labelAndId->second->find(label);
		if (ite == labelAndId->second->end())
		{ 
			return false;
		}

		res = ite->second;
		return true;
	}
	
	bool GetFeatureID(uint16_t from_label, uint16_t to_label, uint32_t& res)
	{
		TransitionFeature trans(from_label, to_label);
		auto ite = TransitionFeatures.find(trans);
		if (ite == TransitionFeatures.end())
		{
			return false;
		}

		res = ite->second;
		return true;
	}

	void AddQuery(const Query& q)
	{	
		for (auto ite = 0; ite < q.Length(); ite++)
		{
			// unigram
			const Token& token = q.GetToken(ite);
			for (auto f = token.Features().begin(); f != token.Features().end(); f++)
			{
				uint16_t label = token.GetLabel();
				AddUnigramFeature(f->first, label);
			}

			// transition
			if (ite > 0)
			{
				const Token& pre_token = q.GetToken(ite - 1);
				AddTransitionFeature(pre_token.GetLabel(), token.GetLabel());
			}
		}
	}

	uint16_t LabelCount() const
	{
		return _label_count;
	}

	int FeatureCount() const
	{
		return _feature_count;
	}

	void AddUnigramFeature(uint32_t xid, uint16_t label)
	{
		if (UnigramFeatures.count(xid) == 0)
		{
			UnigramFeatures[xid] = new std::unordered_map<uint16_t, uint32_t>();
		}
		auto labelAndId = UnigramFeatures.at(xid);
		if (labelAndId->count(label) == 0)
		{
			(*labelAndId)[label] = _feature_count;
			_feature_count += 1;
		}
        if(label >= _label_count) {_label_count = label + 1;}
	}

	void AddTransitionFeature(int from_label, int to_label)
	{
		TransitionFeature trans(from_label, to_label);
		if (TransitionFeatures.count(trans) == 0)
		{
			TransitionFeatures[trans] = _feature_count;
			_feature_count += 1;
		}
        if(from_label >= _label_count) {_label_count = from_label + 1;}
        if(to_label >= _label_count) {_label_count = to_label + 1;}
	}

	void Clear()
	{
		for (auto ite = UnigramFeatures.begin(); ite != UnigramFeatures.end(); ite++)
		{
			delete ite->second;
		}
		UnigramFeatures.clear();
		TransitionFeatures.clear();
		_label_count = 0;
		_feature_count = 0;
	}

	// fId on x -> {labels->global feature ids}
	typedef std::unordered_map<uint32_t, std::unordered_map<uint16_t, uint32_t>*> UnigramFeaturesType;
	typedef std::unordered_map<TransitionFeature, uint32_t, TransitionFeature::Hash, TransitionFeature::Equal> TransitionFeaturesType;
	UnigramFeaturesType UnigramFeatures;
	TransitionFeaturesType TransitionFeatures;

private:
	uint16_t _label_count;
	uint32_t _feature_count;
};

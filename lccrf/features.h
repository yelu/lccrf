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

class LCCRFFeatures
{
public:
	LCCRFFeatures()
	{
		_label_count = 0;
		_unigram_count = 0;
	}

	~LCCRFFeatures() 
	{
		for (auto ite = UnigramFeatures.begin(); ite != UnigramFeatures.end(); ite++)
		{
			delete ite->second;
		}
	}

	bool GetFeatureID(uint32_t xid, uint16_t label, uint32_t& res)
	{
		auto labelAndId = UnigramFeatures.find(xid);
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
		if (from_label >= _label_count || to_label >= _label_count)
		{
			return false;
		}

		res = _unigram_count + from_label * _label_count + to_label;
		return res;
	}

	void CollectFromQuery(const Query& q)
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
		return _unigram_count + _label_count * _label_count;
	}

	void AddUnigramFeature(uint32_t xid, uint16_t label, uint32_t* predefined_id = nullptr)
	{
		auto ite1 = UnigramFeatures.find(xid);
		if (ite1 == UnigramFeatures.end())
		{
			UnigramFeatures[xid] = new std::unordered_map<uint16_t, uint32_t>();
			ite1 = UnigramFeatures.find(xid);
		}
		
		auto label_ids = ite1->second;
		auto ite2 = label_ids->find(label);
		if (ite2 != label_ids->end()) { return; }

		(*label_ids)[label] = predefined_id == nullptr ? _unigram_count : *predefined_id;
		_unigram_count += 1;

		if (label >= _label_count) { _label_count = label + 1; }
	}

	void AddTransitionFeature(int from_label, int to_label, uint32_t* predefined_id = nullptr)
	{
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
		_label_count = 0;
		_unigram_count = 0;
	}

	// fId on x -> {labels->global feature ids}
	typedef std::unordered_map<uint32_t, std::unordered_map<uint16_t, uint32_t>*> UnigramFeaturesType;
	UnigramFeaturesType UnigramFeatures;

private:
	uint16_t _label_count;
	uint32_t _unigram_count;
};

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
	}

	bool GetFeatureID(uint32_t xid, uint16_t label, uint32_t& res)
	{
		uint64_t uid = ((uint64_t)label << 32) + (uint64_t)xid;
		if (_uids.count(uid) == 0)
		{
			return false;
		}

		res = _uids[uid];
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
		uint64_t uid = ((uint64_t)label << 32) + (uint64_t)xid;
		if (_uids.count(uid) != 0)
		{
			return;
		}

		_uids[uid] = (predefined_id == nullptr ? _unigram_count : *predefined_id);
		
		auto ite1 = _ug_feats.find(xid);
		if (ite1 == _ug_feats.end())
		{
			_ug_feats[xid] = std::vector<std::pair<uint16_t, uint32_t>>();
			ite1 = _ug_feats.find(xid);
		}
		
		auto& label_ids = ite1->second;
		label_ids.push_back(std::pair<uint16_t, uint32_t>(label, _unigram_count));
		_unigram_count += 1;

		if (label >= _label_count) { _label_count = label + 1; }
	}

	void AddTransitionFeature(int from_label, int to_label, uint32_t* predefined_id = nullptr)
	{
		if(from_label >= _label_count) {_label_count = from_label + 1;}
		if(to_label >= _label_count) {_label_count = to_label + 1;}
	}

	typedef std::unordered_map<uint32_t, std::vector<std::pair<uint16_t, uint32_t>>> UGFeatType;

	inline const UGFeatType& GetUnigramFeatures() const
	{
		return _ug_feats;
	}

	void Clear()
	{
		_uids.clear();
		_ug_feats.clear();
		_label_count = 0;
		_unigram_count = 0;
	}

private:
	uint16_t _label_count;
	uint32_t _unigram_count;

	// (label << 32 + xid) -> uid
	std::unordered_map<uint64_t, uint32_t> _uids;

	// fId on x -> {labels->global feature ids}
	UGFeatType _ug_feats;
};

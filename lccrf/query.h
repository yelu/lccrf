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
using std::wstring;
using std::vector;
using std::set;
using std::map;
using std::list;
using std::tuple;
using std::pair;
using std::unordered_map;
using std::unordered_set;

class Token
{
public:
	Token() :_label(std::numeric_limits<uint16_t>::max()) {}

	Token(const std::vector<std::pair<uint32_t, float>>& features, uint16_t label):Token()
	{
		_features = features;
		_label = label;
	}

	const std::vector<std::pair<uint32_t, float>>& Features() const
	{
		return _features;
	}

	void AddFeature(uint32_t fid, float fval = 1.0)
	{
		_features.push_back(std::pair<uint32_t, float>(fid, fval));
	}

	void SetLabel(uint16_t label)
	{
		_label = label;
	}

	uint16_t GetLabel() const
	{
		return _label;
	}

	void SetText(const char* text)
	{
		_text = text;
	}

	const std::string& GetText() const
	{
		return _text;
	}

private:
	// feature_id on x -> numeric importance
	std::vector<std::pair<uint32_t, float>> _features;
	uint16_t _label;
	std::string _text;
};

class Query
{
public:
	Query()
	{
	};

	Query(size_t i)
    {
		_tokens.resize(i);
    }

	Query(Query&& q) noexcept
	{
		this->_tokens.swap(q._tokens);
	}

	virtual ~Query() = default;

	void AddToken(Token& t)
	{
		_tokens.push_back(t);
	}

	void AddFeature(uint16_t idx, uint32_t fid, float fval = 1.0)
	{
		if (idx >= _tokens.size())
		{
			_tokens.resize(idx);
		}

		Token& token = _tokens[idx];
		token.AddFeature(fid, fval);
	}

	const Token& GetToken(uint16_t idx) const
	{
		return _tokens[idx];
	}

    size_t Length() const
    {
        return _tokens.size();
    }

	void Clear()
	{
		_tokens.clear();
	}

private:
	std::vector<Token> _tokens;
};

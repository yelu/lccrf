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
#include <regex>
#include <stdexcept>
#include "Str.h"
#include "Log.h"
#include "MurmurHash3.h"
#include "RapidXml/rapidxml.hpp"
using std::wstring;
using std::vector;
using std::set;
using std::map;
using std::list;
using std::tuple;
using std::pair;
using std::string;
using std::unordered_map;
using std::unordered_set;
using std::shared_ptr;
using std::regex;
using namespace rapidxml;

class MatcherBase
{
public:
	virtual set<int> Match(std::vector<string>& tokenizedQuery, int start) = 0;
	virtual void Aggregate(shared_ptr<MatcherBase> childMatcher) {};
};

class StringMatcher : public MatcherBase
{
public:
	StringMatcher(const char* str)
	{
		string content(str);
		_tokenizedItem = split(content);
		if (_tokenizedItem.size() == 0)
		{
			string msg("Empty string is not allowed");
			throw std::runtime_error(msg.c_str());
		}
	}

	set<int> Match(std::vector<string>& tokenizedQuery, int start);
private:
	vector<string> _tokenizedItem;
};

class RegexMatcher : public MatcherBase
{
public:
	RegexMatcher(const char* str) :_regex(str)
	{
	}

	set<int> Match(std::vector<string>& tokenizedQuery, int start);
private:
	regex _regex;
};

class SequenceMatcher : public MatcherBase
{
public:
	SequenceMatcher()
	{
		
	}

	void Aggregate(shared_ptr<MatcherBase> childMatcher)
	{
		_matchers.push_back(childMatcher);
	}

	set<int> Match(std::vector<string>& tokenizedQuery, int start);
private:
	vector<shared_ptr<MatcherBase>> _matchers;

	set<int> _Match(std::vector<string>& tokenizedQuery,
		int qStart,
		int mStart);
};

class OneOfMatcher : public MatcherBase
{
public:
	OneOfMatcher()
	{

	}

	void Aggregate(shared_ptr<MatcherBase> childMatcher)
	{
		_matchers.push_back(childMatcher);
	}

	set<int> Match(std::vector<string>& tokenizedQuery, int start);
private:
	vector<shared_ptr<MatcherBase>> _matchers;
};

class OptionalMatcher : public MatcherBase
{
public:
	OptionalMatcher()
	{
	}

	void Aggregate(shared_ptr<MatcherBase> childMatcher)
	{
		_matcher = childMatcher;
	}

	set<int> Match(vector<string>& tokenizedQuery, int start);
private:
	shared_ptr<MatcherBase> _matcher;
};

class Rule
{
public:
	Rule()
	{
		IsPublic = true;
	}
	shared_ptr<MatcherBase> Matcher;
	bool IsPublic;
	string Name;
};

class RefMatcher : public MatcherBase
{
public:
	RefMatcher(map<string, Rule>& rules, const char* name):_rules(rules), _name(name)
	{
	}

	set<int> Match(vector<string>& tokenizedQuery, int start);
private:
	shared_ptr<MatcherBase> _matcher;
	map<string, Rule>& _rules;
	string _name;
};

class Match
{
public:
	struct Comparator
	{
		bool operator()(const pair<int, int>& m1, const pair<int, int>& m2) const
		{
			if (m1.first != m2.first)
				return m1.first < m2.first;
			else
				return m1.second < m2.second;
		}
	};
};

class CFGParser
{
public:
	CFGParser() {}

	void LoadXml(const string& path);
	map<string, vector<pair<int, int>>> Parse(string& query);
	shared_ptr<MatcherBase> _ParseNode(const xml_node<>* node);

private:
	map<string, Rule> _rules;
	map<string, Rule> _publicRules;
};


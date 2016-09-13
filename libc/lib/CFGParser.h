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
#include "RapidXml\rapidxml.hpp"
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

class Matcher
{
public:
	virtual set<int> Match(std::vector<string>& tokenizedQuery, int start) = 0;
	virtual void Aggregate(shared_ptr<Matcher> childMatcher) {};
};

class StringMatcher : public Matcher
{
public:
	StringMatcher(const char* str)
	{
		string content(str);
		_tokenizedItem = split(content);
		if (_tokenizedItem.size() == 0)
		{
			throw std::exception("Empty string is not allowed");
		}
	}

	set<int> Match(std::vector<string>& tokenizedQuery, int start);
private:
	vector<string> _tokenizedItem;
};

class RegexMatcher : public Matcher
{
public:
	RegexMatcher(const char* str) :_regex(str)
	{
	}

	set<int> Match(std::vector<string>& tokenizedQuery, int start);
private:
	regex _regex;
};

class SequenceMatcher : public Matcher
{
public:
	SequenceMatcher()
	{
		
	}

	void Aggregate(shared_ptr<Matcher> childMatcher)
	{
		_matchers.push_back(childMatcher);
	}

	set<int> Match(std::vector<string>& tokenizedQuery, int start);
private:
	vector<shared_ptr<Matcher>> _matchers;

	set<int> _Match(std::vector<string>& tokenizedQuery,
		int qStart,
		int mStart);
};

class OneOfMatcher : public Matcher
{
public:
	OneOfMatcher()
	{

	}

	void Aggregate(shared_ptr<Matcher> childMatcher)
	{
		_matchers.push_back(childMatcher);
	}

	set<int> Match(std::vector<string>& tokenizedQuery, int start);
private:
	vector<shared_ptr<Matcher>> _matchers;
};

class OptionalMatcher : public Matcher
{
public:
	OptionalMatcher()
	{
	}

	void Aggregate(shared_ptr<Matcher> childMatcher)
	{
		_matcher = childMatcher;
	}

	set<int> Match(vector<string>& tokenizedQuery, int start);
private:
	shared_ptr<Matcher> _matcher;
};

class Rule
{
public:
	Rule()
	{
		IsPublic = true;
	}
	shared_ptr<Matcher> Matcher;
	bool IsPublic;
	string Name;
};

class RefMatcher : public Matcher
{
public:
	RefMatcher(map<string, Rule>& rules, const char* name):_rules(rules), _name(name)
	{
	}

	set<int> Match(vector<string>& tokenizedQuery, int start);
private:
	shared_ptr<Matcher> _matcher;
	map<string, Rule>& _rules;
	string _name;
};

class Match
{
public:
	int Start;
	int End;
	string RuleName;

	Match(int s, int e, const string& name) :Start(s), End(e), RuleName(name) {}

	struct Comparator
	{
		bool operator()(const Match& m1, const Match& m2) const
		{
			if (m1.Start != m2.Start)
				return m1.Start < m2.Start;
			else
				return m1.End < m2.End;
		}
	};
};





class CFGParser
{
public:
	CFGParser() {}

	void LoadXml(const string& path);

	vector<Match> Parse(string& query);

	shared_ptr<Matcher> _ParseNode(const xml_node<>* node);

private:
	map<string, Rule> _rules;
	map<string, Rule> _publicRules;
};


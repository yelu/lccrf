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
using namespace rapidxml;

class Matcher
{
public:
	virtual list<int> Match(std::vector<string>& tokenizedQuery, int start) = 0;
	virtual void Aggregate(shared_ptr<Matcher> childMatcher) {};
};

class StringMatcher : public Matcher
{
public:
	StringMatcher(const char* str):_content(str)
	{
	}

	list<int> Match(std::vector<string>& tokenizedQuery, int start);
private:
	string _content;
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

	list<int> Match(std::vector<string>& tokenizedQuery, int start);
private:
	vector<shared_ptr<Matcher>> _matchers;

	std::list<int> _Match(std::vector<string>& tokenizedQuery,
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

	list<int> Match(std::vector<string>& tokenizedQuery, int start);
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

	list<int> Match(vector<string>& tokenizedQuery, int start);
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

	list<int> Match(vector<string>& tokenizedQuery, int start);
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
};

class CFGParser
{
public:
	CFGParser() {}

	void LoadXml(const string& path);

	vector<Match> Parse(vector<string>& tokenizedQuery, int start);

	shared_ptr<Matcher> _ParseNode(const xml_node<>* node);

private:
	map<string, Rule> _rules;
};


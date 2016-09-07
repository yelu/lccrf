#include "CFGParser.h"
#include "Str.h"
#include <iostream>
#include <fstream>

list<int> StringMatcher::Match(std::vector<string>& tokenizedQuery, int start)
{
	vector<string> tokenizedItem = split(_content);
	list<int> ret;
	if ((int)(tokenizedQuery.size()) - start < (int)(tokenizedItem.size())) { return ret; }

	for (size_t i = 0; i < tokenizedItem.size(); i++)
	{
		if (tokenizedItem[i] != tokenizedQuery[i + start])
		{
			return ret;
		}
	}
	ret.push_back(start + (int)tokenizedItem.size());
	return ret;
}

list<int> SequenceMatcher::Match(std::vector<string>& tokenizedQuery, int start)
{
	auto ret = _Match(tokenizedQuery, start, 0);
	return ret;
}

std::list<int> SequenceMatcher::_Match(std::vector<string>& tokenizedQuery, 
														int qStart, 
														int mStart)
{
	list<int> ret;
	if (mStart >= (int)_matchers.size())
	{
		ret.push_back(qStart);
		return ret;
	}
	list<int> ends = _matchers[mStart]->Match(tokenizedQuery, qStart);
	for each (int end in ends)
	{
		auto finalEnds = _Match(tokenizedQuery, end, mStart + 1);
		if (finalEnds.size() == 0) { continue; }
		else { ret.merge(finalEnds); }
	}
	return ret;
}

list<int> OneOfMatcher::Match(std::vector<string>& tokenizedQuery, int start)
{
	list<int> ret;
	for each (auto matcher in _matchers)
	{
		auto ends = matcher->Match(tokenizedQuery, start);
		ret.merge(ends);
	}
	return ret;
}

list<int> OptionalMatcher::Match(std::vector<string>& tokenizedQuery, int start)
{
	list<int> ret;
	ret.push_back(start);
	auto ends = _matcher->Match(tokenizedQuery, start);
	ret.merge(ends);
	return ret;
}

list<int> RefMatcher::Match(std::vector<string>& tokenizedQuery, int start)
{
	if (!_matcher) { _matcher = _rules[_name].Matcher; }
	auto ret = _matcher->Match(tokenizedQuery, start);
	return ret;
}

void CFGParser::LoadXml(const string& path)
{
	xml_document<> doc;
	xml_node<> * root_node;
	// Read the xml file into a vector
	std::ifstream theFile(path);
	vector<char> buffer((std::istreambuf_iterator<char>(theFile)), std::istreambuf_iterator<char>());
	buffer.push_back('\0');
	// Parse the buffer using the xml file parsing library into doc 
	doc.parse<0>(&buffer[0]);
	// Find our root node
	root_node = doc.first_node("root");

	// Iterate over the children
	for (xml_node<>* ruleNode = root_node->first_node("rule"); ruleNode; ruleNode = ruleNode->next_sibling("rule"))
	{
		Rule rule;
		string name(ruleNode->first_attribute("name")->value());
		rule.Name = name;
		auto scopeAttr = ruleNode->first_attribute("scope");
		if (scopeAttr != 0 && strcmp(scopeAttr->value(), "private") == 0)
		{
			rule.IsPublic = false;
		}
		rule.Matcher = _ParseNode(ruleNode->first_node());
		_rules[rule.Name] = rule;
	}
}

shared_ptr<Matcher> CFGParser::_ParseNode(const xml_node<>* node)
{
	shared_ptr<Matcher> ret;

	if (strcmp(node->name(), "seq") == 0)
	{
		ret.reset(new SequenceMatcher());
		for (xml_node<>* child = node->first_node();child; child = child->next_sibling())
		{
			ret->Aggregate(_ParseNode(child));
		}
	}
	else if (strcmp(node->name(), "one-of") == 0)
	{
		ret.reset(new OneOfMatcher());
		for (xml_node<>* child = node->first_node(); child; child = child->next_sibling())
		{
			ret->Aggregate(_ParseNode(child));
		}
	}
	else if (strcmp(node->name(), "string") == 0)
	{
		ret.reset(new StringMatcher(node->value()));
	}
	else if (strcmp(node->name(), "ref") == 0)
	{
		ret.reset(new RefMatcher(_rules, node->first_attribute("rule")->value()));
	}

	// if it is optioanl
	auto optianalAttr = node->first_attribute("optional");
	if (optianalAttr != 0 && strcmp(optianalAttr->value(), "true") == 0)
	{
		shared_ptr<OptionalMatcher> optionalMatcher(new OptionalMatcher());
		optionalMatcher->Aggregate(ret);
		ret = optionalMatcher;
	}
	return ret;
}

vector<Match> CFGParser::Parse(vector<string>& tokenizedQuery, int start)
{
	vector<Match> ret;
	for each (auto rule in _rules)
	{
		if (!rule.second.IsPublic) { continue; }
		auto ends = rule.second.Matcher->Match(tokenizedQuery, start);
		for each (auto end in ends)
		{
			Match match;
			match.Start = start;
			match.End = end;
			match.RuleName = rule.second.Name;
			ret.push_back(match);
		}
	}
	return ret;
}



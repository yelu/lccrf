#include "CFGParser.h"
#include "Str.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <stdexcept>

set<int> StringMatcher::Match(std::vector<string>& tokenizedQuery, int start)
{
	set<int> ret;
	if ((int)(tokenizedQuery.size()) - start < (int)(_tokenizedItem.size())) { return ret; }

	for (size_t i = 0; i < _tokenizedItem.size(); i++)
	{
		if (_tokenizedItem[i] != tokenizedQuery[i + start])
		{
			return ret;
		}
	}
	ret.insert(start + (int)_tokenizedItem.size());
	return ret;
}

set<int> RegexMatcher::Match(std::vector<string>& tokenizedQuery, int start)
{
	set<int> ret;
	string query;
	for (size_t i = start; i < tokenizedQuery.size(); i++)
	{
		query += tokenizedQuery[i];
		query += " ";
	}
	rtrim(query);	
	if (query.size() == 0) { return ret; }

	std::smatch match;
	std::regex_search(query, match, _regex, std::regex_constants::match_continuous);	
 	if (match.size() == 0) { return ret; }
	string matchedPrefix = match[0].str();
	if (matchedPrefix.size() == 0) { return ret; }

	// check if the matched prefix are complete words.	
	size_t idx = query.find(matchedPrefix) + matchedPrefix.size();
	if (idx >= query.size()) { idx -= 1; }
	else if (query[idx] == ' ') { idx -= 1; }
	else if (query[idx - 1] == ' ') { idx -= 2; }
	else { return ret; }
	
	int end = start + 1;
	int i = idx;
	while (i >= 0)
	{
		if (query[i] == ' ') end += 1;
		i--;	
	}
	ret.insert(end);

	return ret;
}

set<int> SequenceMatcher::Match(std::vector<string>& tokenizedQuery, int start)
{
	auto ret = _Match(tokenizedQuery, start, 0);
	return ret;
}

set<int> SequenceMatcher::_Match(std::vector<string>& tokenizedQuery,
														int qStart, 
														int mStart)
{
	set<int> ret;
	if (mStart >= (int)_matchers.size())
	{
		ret.insert(qStart);
		return ret;
	}
	set<int> ends = _matchers[mStart]->Match(tokenizedQuery, qStart);
	for(int end:ends)
	{
		auto finalEnds = _Match(tokenizedQuery, end, mStart + 1);
		if (finalEnds.size() == 0) { continue; }
		else { ret.insert(finalEnds.begin(), finalEnds.end()); }
	}
	return ret;
}

set<int> OneOfMatcher::Match(std::vector<string>& tokenizedQuery, int start)
{
	set<int> ret;
	for(auto matcher = _matchers.begin(); matcher != _matchers.end(); matcher++)
	{
		auto ends = (*matcher)->Match(tokenizedQuery, start);
		ret.insert(ends.begin(), ends.end());
	}
	return ret;
}

set<int> OptionalMatcher::Match(std::vector<string>& tokenizedQuery, int start)
{
	set<int> ret;
	ret.insert(start);
	auto ends = _matcher->Match(tokenizedQuery, start);
	ret.insert(ends.begin(), ends.end());
	return ret;
}

set<int> RefMatcher::Match(std::vector<string>& tokenizedQuery, int start)
{
	if (!_matcher) { _matcher = _rules[_name].Matcher; }
	auto ret = _matcher->Match(tokenizedQuery, start);
	return ret;
}

void CFGParser::LoadXml(const string& path)
{
	xml_document<> doc;
	xml_node<> * rootNode;
	// Read the xml file into a vector
	std::ifstream theFile(path);
	vector<char> buffer((std::istreambuf_iterator<char>(theFile)), std::istreambuf_iterator<char>());
	buffer.push_back('\0');
	// Parse the buffer using the xml file parsing library into doc 
	doc.parse<0>(&buffer[0]);
	// Find our root node
	rootNode = doc.first_node("root");

	// Iterate over the children
	for (xml_node<>* ruleNode = rootNode->first_node("rule"); ruleNode != NULL; 
		ruleNode = ruleNode->next_sibling("rule"))
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
		// if it is empty rule, ignore it.
		if (!rule.Matcher) { continue; }
		_rules[rule.Name] = rule;
		if (rule.IsPublic)
		{
			_publicRules[name] = rule;
		}
	}
}

shared_ptr<MatcherBase> CFGParser::_ParseNode(const xml_node<>* node)
{
	shared_ptr<MatcherBase> ret;

	if (strcmp(node->name(), "sequence") == 0)
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
	else if (strcmp(node->name(), "reference") == 0)
	{
		auto ruleAttr = node->first_attribute("rule");
		if (ruleAttr == NULL)
		{
			string msg("attribute \"rule\" is missing in reference node");
			throw std::runtime_error(msg.c_str());
		}
		ret.reset(new RefMatcher(_rules, node->first_attribute("rule")->value()));
	}
	else if (strcmp(node->name(), "regex") == 0)
	{
		if (node->value() == NULL)
		{
			string msg("regex can't be empty");
			throw std::runtime_error(msg.c_str());
		}
		string pattern(node->value());
		trim(pattern);
		if(pattern.size() == 0)
		{
			throw std::runtime_error("regex can't be empty");
		}
		
		ret.reset(new RegexMatcher(pattern.c_str()));
	}
	else
	{
		char msg[100];
		sprintf(msg, "unknown node \"%s\"", node->name());
		throw std::runtime_error(msg);
	}

	// if it is optioanl
	auto optianalAttr = node->first_attribute("optional");
	if (optianalAttr != NULL && strcmp(optianalAttr->value(), "true") == 0)
	{
		shared_ptr<OptionalMatcher> optionalMatcher(new OptionalMatcher());
		optionalMatcher->Aggregate(ret);
		ret = optionalMatcher;
	}
	return ret;
}

map<string, vector<pair<int, int>>> CFGParser::Parse(string& query)
{
	vector<string> tokenizedQuery = split(query);
	map<string, vector<pair<int, int>>> matches;
	for (size_t start = 0; start < tokenizedQuery.size(); start++)
	{
		for(auto rule = _publicRules.begin(); rule != _publicRules.end(); rule++)
		{
			if (!rule->second.IsPublic) { continue; }
			auto ends = rule->second.Matcher->Match(tokenizedQuery, start);
			for (auto end = ends.begin(); end != ends.end(); end++)
			{
				if (matches.count(rule->second.Name) == 0)
				{
					matches[rule->second.Name] = vector<pair<int, int>>();
				}
				matches[rule->second.Name].push_back(pair<int, int>(start, *end));
			}
		}
	}
	// merge intervals of the same rule.
	vector<Match> ret;
	for (auto ite  = matches.begin(); ite != matches.end(); ite++)
	{
		sort(ite->second.begin(), ite->second.end(), Match::Comparator());
		int preStart = 0;
		int preEnd = 0;
		auto last = ite->second.begin();
		for (auto match = ite->second.begin(); match != ite->second.end(); match++)
		{
			if (match->first > preEnd)
			{
				if (preEnd != 0)
				{
					last->first = preStart;
					last->second = preEnd;
					last++;
				}
				preStart = match->first;
				preEnd = match->second;
			}
			else
			{
				preEnd = match->second < preEnd ? preEnd : match->second;
			}
		}
		if (preEnd != 0)
		{
			last->first = preStart;
			last->second = preEnd;
			last++;
		}
		ite->second.erase(last, ite->second.end());
	}
	return matches;
}




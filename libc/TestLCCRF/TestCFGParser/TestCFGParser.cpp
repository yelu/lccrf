#include "gtest/gtest.h"
#include <memory>
#define private public
#include "../../lib/CFGParser.h"
#include "../../lib/Str.h"

class CFGParserTestSuite : public ::testing::Test
{
protected:

	static void SetUpTestCase()
	{
	}

	static void TearDownTestCase()
	{
	}

	static CFGParser parser;
};

CFGParser CFGParserTestSuite::parser;

TEST_F(CFGParserTestSuite, TestMatch)
{
	parser.LoadXml("../TestLCCRF/TestCFGParser/en-us.datetime.grammar.xml");
	string query = "thursday september 8 2016";
	auto ret = parser.Parse(query);
	EXPECT_EQ(ret.size(), 2);

	map<string, vector<Match>> matchesByRuleName;
	for (auto match : ret)
	{
		if (matchesByRuleName.count(match.RuleName) == 0)
		{
			matchesByRuleName[match.RuleName] = vector<Match>();
		}
		matchesByRuleName[match.RuleName].push_back(match);
	}	
	EXPECT_EQ(matchesByRuleName["MonthDay"][0].Start, 1);
	EXPECT_EQ(matchesByRuleName["MonthDay"][0].End, 3);
	EXPECT_EQ(matchesByRuleName["YearMonthDay"][0].Start, 0);
	EXPECT_EQ(matchesByRuleName["YearMonthDay"][0].End, 4);
}


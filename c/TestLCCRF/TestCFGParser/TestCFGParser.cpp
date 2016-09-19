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
	parser.LoadXml("../TestLCCRF/TestCFGParser/cfg.xml");
	string query = "thursday september 8 2016";
	vector<string> tokenizeQuery = split(query);
	auto ret = parser.Parse(tokenizeQuery);
	EXPECT_EQ(ret.size(), 2);
	
	EXPECT_EQ(ret["MonthDay"][0].first, 1);
	EXPECT_EQ(ret["MonthDay"][0].second, 3);
	EXPECT_EQ(ret["YearMonthDay"][0].first, 0);
	EXPECT_EQ(ret["YearMonthDay"][0].second, 4);
}


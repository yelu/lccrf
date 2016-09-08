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
	EXPECT_EQ(ret[0].Start, 1);
	EXPECT_EQ(ret[0].End, 3);
	EXPECT_STREQ(ret[1].RuleName.c_str(), "MonthDay");
	EXPECT_EQ(ret[1].Start, 0);
	EXPECT_EQ(ret[1].End, 4);
	EXPECT_STREQ(ret[0].RuleName.c_str(), "YearMonthDay");
}


/*
#include "../LCCRF/LCCRF.h"
#include "gtest/gtest.h"

class LCCRFTestSuite : public ::testing::Test
{
protected:
	static void SetUpTestCase() 
	{
	}

	static void TearDownTestCase() 
	{
	}
};

void MakeDocument(Document& doc, wstring xs[], int ys[], int len)
{
	for(int i = 0; i < len; i++)
	{
		X x;
		x.push_back(xs[i]);
		Y y = ys[i];
		Token t(x, y);
		doc.push_back(t);
	}
}

TEST_F(LCCRFTestSuite, Test1)
{
	list<Document> trainingSet;
	Document doc;
	wstring xs[] = {L"I", L"love", L"you"};
	int ys[] = {0,1,2};
	MakeDocument(doc, xs, ys, 3);
	trainingSet.push_back(doc);
	
	vector<LCCRF::FeatureType> fs;

	LCCRF::FeatureType f1 = [](const Document& doc, int s1, int s2, int j)
	{
		if(j < 0 || j >= doc.size()) {return 0;}
		if(doc[j].x[0] == L"I" && doc[j].y == 0) {return 1;}
		return 0;
	};
	LCCRF::FeatureType f2 = [](const Document& doc, int s1, int s2, int j)
	{
		if(j < 0 || j >= doc.size()) {return 0;}
		if(doc[j].x[0] == L"love" && doc[j].y == 1) {return 1;}
		return 0;
	};
	LCCRF::FeatureType f3 = [](const Document& doc, int s1, int s2, int j)
	{
		if(j < 0 || j >= doc.size()) {return 0;}
		if(doc[j].x[0] == L"you" && doc[j].y == 2) {return 1;}
		return 0;
	};

	fs.push_back(f1);
	fs.push_back(f2);
	fs.push_back(f3);

	LCCRF lccrf(fs, 0.01);
	lccrf.Learn(trainingSet, 1);
}

int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	int ret = RUN_ALL_TESTS();
	return ret;
}*/

#include "../LCCRF/LCCRF.h"

void MakeDocument(Document& doc, wstring xs[], wstring ys[], int len)
{
	for(int i = 0; i < len; i++)
	{
		X x;
		x.push_back(xs[i]);
		Y y = ys[i];
		Token t(x, y);
		doc.push_back(t);
	}
}

void Test()
{
	list<Document> trainingSet;
	Document doc;
	wstring xs[] = {L"I", L"love", L"you"};
	wstring ys[] = {L"0",L"1",L"2"};
	MakeDocument(doc, xs, ys, 3);
	trainingSet.push_back(doc);
	
	vector<LCCRF::FeatureType> fs;

	LCCRF::FeatureType f1 = [](const Document& doc, wstring s1, wstring s2, int j)
	{
		if(j < 0 || j >= doc.size()) {return 0;}
		if(doc[j].x[0] == L"I" && s2 == L"0") {return 1;}
		return 0;
	};
	LCCRF::FeatureType f2 = [](const Document& doc, wstring s1, wstring s2, int j)
	{
		if(j < 0 || j >= doc.size()) {return 0;}
		if(doc[j].x[0] == L"love" && s2 == L"1") {return 1;}
		return 0;
	};
	LCCRF::FeatureType f3 = [](const Document& doc, wstring s1, wstring s2, int j)
	{
		if(j < 0 || j >= doc.size()) {return 0;}
		if(doc[j].x[0] == L"you" && s2 == L"2") {return 1;}
		return 0;
	};

	fs.push_back(f1);
	fs.push_back(f2);
	fs.push_back(f3);

	LCCRF lccrf(fs, 0.01);
	lccrf.Learn(trainingSet, 0.01, 1, 100);
}


int main()
{
	Test();
	return 0;
}

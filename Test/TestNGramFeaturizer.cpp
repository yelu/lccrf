#include "gtest/gtest.h"
#define private public
#include "../LCCRF/LCCRF.h"
#include "../LCCRF/NGramFeaturizer.h"
#include <memory>

void MakeDocument(Document& doc, wstring xs[], wstring ys[], int len)
{
	for(int i = 0; i < len; i++)
	{
		X x;
		x.push_back(xs[i]);
		wstring attr = xs[i];
		attr.append(L".attr");
		x.push_back(attr);
		Y y = ys[i];
		Token t(x, y);
		doc.push_back(t);
	}
}

class NGramFeaturizerTestSuite : public ::testing::Test
{
protected:
	static void SetUpTestCase() 
	{
		Document doc;
		wstring xs[] = {L"I ", L"love\n"};
		wstring ys[] = {L"tag1", L"tag2"};
		int n = sizeof(xs) / sizeof(wstring);
		MakeDocument(doc, xs, ys, n);
		trainingSet.push_back(doc);
	
	}

	static void TearDownTestCase() 
	{
	}

	static list<Document> trainingSet;
};

list<Document> NGramFeaturizerTestSuite::trainingSet;

TEST_F(NGramFeaturizerTestSuite, TestUnigram)
{
	std::shared_ptr<NGramFeaturizer> f1(new NGramFeaturizer(1));
	f1->SetStartID(0);
	f1->Fit(trainingSet);
	EXPECT_STREQ(L"tag1 I   ", f1->_idAllocator._idToTextMapping[0].c_str());
    EXPECT_STREQ(L"tag2 love   ", f1->_idAllocator._idToTextMapping[1].c_str());

	vector<int> fields(2, 0);
	fields[0] = 0; fields[1] = 1;
	std::shared_ptr<NGramFeaturizer> f2(new NGramFeaturizer(1, fields));
	f2->SetStartID(0);
	f2->Fit(trainingSet);
	EXPECT_STREQ(L"tag1 I   I  .attr ", f2->_idAllocator._idToTextMapping[0].c_str());
    EXPECT_STREQ(L"tag2 love   love  .attr ", f2->_idAllocator._idToTextMapping[1].c_str());
}

TEST_F(NGramFeaturizerTestSuite, TestBigram)
{
	std::shared_ptr<NGramFeaturizer> f1(new NGramFeaturizer(2));
	f1->SetStartID(0);
	f1->Fit(trainingSet);
	EXPECT_STREQ(L"tag2 I   love   ", f1->_idAllocator._idToTextMapping[0].c_str());

	vector<int> fields(2, 0);
	fields[0] = 0; fields[1] = 1;
	std::shared_ptr<NGramFeaturizer> f2(new NGramFeaturizer(2, fields));
	f2->SetStartID(0);
	f2->Fit(trainingSet);
	EXPECT_STREQ(L"tag2 I   I  .attr love   love  .attr ", f2->_idAllocator._idToTextMapping[0].c_str());
}

TEST_F(NGramFeaturizerTestSuite, TestSerilize)
{
	vector<int> fields(2, 0);
	fields[0] = 0; fields[1] = 1;
	std::shared_ptr<NGramFeaturizer> f1(new NGramFeaturizer(1, fields));
	f1->SetStartID(0);
	f1->Fit(trainingSet);
	f1->Serialize(L".\\unigram.features");

	std::shared_ptr<NGramFeaturizer> f2(new NGramFeaturizer());
	f2->Deserialize(L".\\unigram.features");
}

int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	int ret = RUN_ALL_TESTS();
	return ret;
}


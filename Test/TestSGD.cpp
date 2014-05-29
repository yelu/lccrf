#include "../LCCRF/SGD.h"
#include "gtest/gtest.h"

class SGDTestSuite : public ::testing::Test
{
protected:
	static void SetUpTestCase() 
	{
	}

	static void TearDownTestCase() 
	{
	}
};

	class Example
	{
	public:
		Example(double x1, double x2, double y)
		{
			this->x1 = x1;
			this->x2 = x2;
			this->y = y;
		}
		double x1;
		double x2;
		double y;
	};

TEST_F(SGDTestSuite, Test1)
{
	vector<double> weights(2, 0.0);

	function<double (vector<double>&, Example&)> object = [](vector<double>& weights, Example& example) -> double 
	{
		double tmp = 1 + weights[0] * example.x1 + weights[1] * example.x2 - example.y;
		double ret = tmp * tmp;
		return ret / 2.0;
	};
	auto d1 = [](vector<double>& weights, Example& example)
	{
		double ret = (1 + weights[0] * example.x1 + weights[1] * example.x2 - example.y) * example.x1;
		return ret;
	};
	auto d2 = [](vector<double>& weights, Example& example)
	{
		double ret = (1 + weights[0] * example.x1 + weights[1] * example.x2 - example.y) * example.x2;
		return ret;
	};
	vector<function<double (vector<double>& weights, Example& example)>> ds;
	ds.push_back(d1);
	ds.push_back(d2);
	list<Example> tSet;
	tSet.push_back(Example(1,2,15.4597809));
	tSet.push_back(Example(2,3,21.36083423));
	tSet.push_back(Example(3,4,30.450696));
	tSet.push_back(Example(4,5,39.35539018));
	tSet.push_back(Example(5,6,46.52048191));
	tSet.push_back(Example(6,7,55.13645011));
	tSet.push_back(Example(7,8,63.62777841));
	tSet.push_back(Example(8,9,71.18711109));
	tSet.push_back(Example(9,10,78.61837528));
	SGD<Example> sgd(tSet, weights, ds, object);
	const vector<double>& res = sgd.Run(0.001, 10, 1000);
	std::cout << res[0] << "\t" << res[1] << std::endl;
	EXPECT_EQ(1, 1);
}

int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	int ret = RUN_ALL_TESTS();
	return ret;
}
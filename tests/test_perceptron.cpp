#include "gtest/gtest.h"
#include "lccrf.h"
#include "evaluator.h"

TEST(TestLCCRF, TestPerf)
{
    LCCRF lccrf;
    string data_file = "./lccrf.vw.dat";
    string model = "./model.bin";

    auto queries = lccrf.LoadData(data_file);
    double best_loss = lccrf.Fit(queries, 50, 0.1);

    ASSERT_TRUE(best_loss < 26);

    vector<vector<uint16_t>> predicted_results;
    for (auto i = 0; i < queries.size(); i++)
    {
        vector<uint16_t> predicted = lccrf.Predict(queries[i]);
        predicted_results.push_back(predicted);
    }

    Evaluator evaluator(lccrf.GetFeatures().LabelCount());
    evaluator.Eval(queries, predicted_results);
    evaluator.PrintReport();
    
    lccrf.Save(model);
}

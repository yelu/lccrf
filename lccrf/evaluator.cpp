#include "evaluator.h"

Evaluator::Evaluator(uint16_t label_cnt):
    _report(label_cnt, tuple<int,int,int>(0,0,0)),
    _tp(0),
    _fn(0),
    _fp(0)
{
}

void Evaluator::Eval(const vector<Query>& references,
    const vector<vector<uint16_t>> predicted_results)
{
    for (auto i = 0; i < references.size(); i++)
    {
        const Query& ref = references[i];
        const vector<uint16_t>& predicted = predicted_results[i];

        for (auto t = 0; t < ref.Length(); t++)
        {
            const Token& token = ref.GetToken(t);
            if (token.GetLabel() == predicted[t])
            {
                std::get<0>(_report[token.GetLabel()]) += 1;
                _tp += 1;
            }
            else
            {
                std::get<1>(_report[token.GetLabel()]) += 1;
                std::get<2>(_report[predicted[t]]) += 1;
                _fp += 1;
                _fn += 1;
            }
        }
    }
}

Evaluator::Report& Evaluator::GetReport()
{
    return _report;
}

float Evaluator::Precision()
{
    return _tp / (float)(_tp + _fp);
}

float Evaluator::Recall()
{
    return _tp / (float)(_tp + _fn);
}

float Evaluator::F1()
{
    return 2 * Precision() * Recall() / (Precision() + Recall());
}

void Evaluator::PrintReport()
{
    printf("label\ttp\tfp\tfn\tp\tr\tf1\n");
    for (auto i = 0; i < _report.size(); i++)
    {
        uint32_t tp = std::get<0>(_report[i]);
        uint32_t fp = std::get<1>(_report[i]);
        uint32_t fn = std::get<2>(_report[i]);
        float p = tp / (float)(tp + fp);
        float r = tp / (float)(tp + fn);
        float f1 = 2 * p * r / (p + r);
        printf("%d\t%d\t%d\t%d\t%.2f\t%.2f\t%.2f\n", i, tp, fp, fn, p, r, f1);
    }
    printf("\n");
    printf("overall:\n");
    printf("precision:%.2f, recall:%.2f, f1:%.2f\n", Precision(), Recall(), F1());
}


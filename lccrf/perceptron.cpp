#include <random>
#include "spdlog/spdlog.h"
#include "perceptron.h"
#include "features.h"
#include "lccrf.h"
#include "query.h"

PerceptronTrainer::PerceptronTrainer(
    const vector<Query>& qs,
    LCCRF& model) :
    _qs(qs),
    _model(model),
    _weights(model.GetWeights()),
    _learning_rate(0.001),
    _best_iter(-1),
    _best_loss(1e20)
{
}

double PerceptronTrainer::GetBestLoss()
{
    return _best_loss;
}

uint32_t PerceptronTrainer::GetErrors(const Query& q, const vector<uint16_t> predicted)
{
    if (q.Length() != predicted.size())
    {
        spdlog::error("token count is {0}, expected token count is {1}", q.Length(), predicted.size());
        throw std::runtime_error("token count of q and expected mismatch");
    }

    uint32_t error = 0;
    for (auto ite = 0; ite < predicted.size(); ite++)
    {
        if (predicted[ite] >= _model.GetFeatures().LabelCount())
        {
            spdlog::error("invalid label predict[{0}]={1}", ite, predicted[ite]);
            throw std::runtime_error("invalid label");
        }
        if (q.GetToken(ite).GetLabel() != predicted[ite]) { error++; }
    }

    return error;
}

float LearningRateDecay(float rate, float inital_value, uint32_t iter_idx, uint32_t num_of_data)
{
    // exponential learning rate decay.
    float new_rate = rate * pow(inital_value, 1.0f * static_cast<float>(iter_idx) / num_of_data);
    return new_rate;
}

const vector<double>& PerceptronTrainer::Run(double learning_rate, int max_iter)
{
    const double early_stop_threshold = 1e-3;
    const float decay_init_value = 0.7;
    double pre_loss = 1e20;
    bool shuffle_data = true;
    _best_loss = pre_loss;
    _best_iter = 0;
    _learning_rate = learning_rate;

    // initialize weights
    for (auto& item : _model.GetWeights())
    {
        item = 0.0;
    }

    std::vector<uint32_t> iter_order(_qs.size(), 0);
    for (auto i = 0; i < iter_order.size(); i++)
    {
        iter_order[i] = i;
    }

    for (int i = 0; i < max_iter; i++)
    {
        if (shuffle_data)
        {
            std::shuffle(iter_order.begin(), iter_order.end(), std::default_random_engine(/*RANDOM_SEED = */ 0));
        }

        _learning_rate = LearningRateDecay(_learning_rate, decay_init_value, i, _qs.size());
        double loss = 0.0;
        int cnt = 0;
        // calculate loss for every training sample(aka, mini-batch=1).
        for (auto ite = iter_order.begin(); ite != iter_order.end(); ite++)
        {
            loss += UpdateWeights(_qs[*ite]);
            cnt += 1;
            if(cnt % 10000 == 0)
            {
                //spdlog::info("{0}/{1} training samples processed", cnt, _qs.size());
            }
        }
        //spdlog::info("{0}/{1} training samples processed", cnt, _qs.size());

        if (loss < _best_loss)
        {
            _best_loss = loss;
            _best_iter = i + 1;
        }

        // one iteration(epoch) finished, check the convergency.
        double loss_delta = pre_loss - loss;
        double loss_improve = loss_delta / pre_loss;       
        spdlog::info("#iteration {0}/{1}, loss {2}, delta {3}, learning rate {4}", i + 1, max_iter,
            loss, loss_improve, _learning_rate);
        if ((std::isnan(std::abs(loss_improve)) && std::abs(pre_loss - loss) < early_stop_threshold) ||
            std::abs(loss_improve) < early_stop_threshold)
        {
            spdlog::info("converged, early stop.");
            break;
        }
        pre_loss = loss;
    }

    return _model.GetWeights();
}

double PerceptronTrainer::UpdateWeights(const Query& q)
{
    double loss = 0.0;
    if (q.Length() == 0) { return loss; }

    vector<uint16_t> predicted_labels = _model.Predict(q);
    uint32_t error = GetErrors(q, predicted_labels);
    if (error <= 0) { return loss; }

    for (auto i = 0; i < q.Length(); i++)
    {
        const auto& token = q.GetToken(i);
        uint16_t label = token.GetLabel();

        if (predicted_labels[i] != label)
        {
            // boost correct node feature weights
            UpdateNodeWeights(token, label, _learning_rate);
            // suppress wrong node feature weights
            UpdateNodeWeights(token, predicted_labels[i], 0.0 - _learning_rate);
        }

        if (i > 0)
        {
            uint16_t pre_label = q.GetToken(i - 1).GetLabel();
            if (pre_label != predicted_labels[i - 1] || label != predicted_labels[i])
            {
                // boost correct transition feature weights
                UpdateEdgeWeights(pre_label, label, _learning_rate);
                // suppress wrong transition feature weights
                UpdateEdgeWeights(predicted_labels[i - 1], predicted_labels[i], 0.0 - _learning_rate);
            }
        }
    }

    loss += (double)error / (double)q.Length();
    return loss;
}

void PerceptronTrainer::UpdateNodeWeights(const Token& t, uint16_t label, double delta)
{
    auto& weights = _model.GetWeights();
    for (const auto& item : t.Features())
    {
        uint32_t id = item.first;
        float val = item.second;
        uint32_t fid = 0;
        if (_model.GetFeatures().GetFeatureID(id, label, fid))
        {
            _model.GetWeights()[fid] += (delta * val);
        }
    }
}

void PerceptronTrainer::UpdateEdgeWeights(uint16_t from_label, uint16_t to_label, double delta)
{
    uint32_t fid = 0;
    if (_model.GetFeatures().GetFeatureID(from_label, to_label, fid))
    {
        _model.GetWeights()[fid] += delta;
    }
}

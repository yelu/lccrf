#include <cassert>
#include <limits>
#include <algorithm>
#include "decoder.h"
#include "lccrf.h"

Decoder::Decoder(LCCRF& model):
    _model(model)
{
}

Decoder::~Decoder(void)
{
}

std::vector<uint16_t> Decoder::Decode(const Query& q)
{
    size_t n_step = q.Length();
    uint16_t n_state = _model.GetFeatures().LabelCount();

    // for feature doesn't exists, should assume it's like a feature with weight 0.0
    vector<double> pi(n_state, 0.0);
    vector<vector<uint16_t>> backtrace(n_step, vector<uint16_t>(n_state, 0));
    for (size_t j = 0; j < n_step; j++)
    {
        vector<double> new_pi(n_state, -1e10);
        if (j != 0)
        {
            for (int s1 = 0; s1 < n_state; s1++)
            {
                for (int s2 = 0; s2 < n_state; s2++)
                {
                    double d = pi[s2] + _model.GetEdgeWeight(s2, s1);
                    if (d > new_pi[s1])
                    {
                        backtrace[j][s1] = s2;
                        new_pi[s1] = d;
                    }
                }
            }
        }
        else
        {
            new_pi.swap(pi);
        }

        for (auto& item : q.GetToken(j).Features())
        {
            uint32_t id = item.first;
            float val = item.second;
            auto ite = _model.GetFeatures().UnigramFeatures.find(id);
            if (ite == _model.GetFeatures().UnigramFeatures.end()) { continue; }
            for (auto label_id = ite->second->begin(); label_id != ite->second->end(); label_id++)
            {
                double w = _model.GetWeights()[label_id->second];
                new_pi[label_id->first] += (val * w);
            }
        }

        pi.swap(new_pi);
    }

    // find the optimal path.
    std::vector<uint16_t> res(n_step, 0);
    uint16_t last = (uint16_t)std::distance(pi.begin(), std::max_element(pi.begin(), pi.end()));
    res[n_step - 1] = last;
    for (int j = (int)n_step - 1; j > 0; j--)
    {
        res[j-1] = backtrace[j][res[j]];
    }

    return res;
}

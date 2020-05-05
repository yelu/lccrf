#pragma once

#include <functional>
#include <memory>
#include <vector>
using std::function;
using std::shared_ptr;
using std::vector;

class LCCRF;

class Decoder
{
public:

    Decoder(LCCRF& model);
    virtual ~Decoder(void);

    std::vector<uint16_t> Decode(const Query& q);

private:
    LCCRF& _model;
};


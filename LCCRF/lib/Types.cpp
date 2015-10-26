#include "Types.h"

double X::GetFeatureValue(int j, int s1, int s2, int featureID) const 
{
    Position pos(j, s1, s2);
    auto ite = _features.find(featureID);
    if(ite == _features.end() || ite->second->count(pos) == 0)
    {
        return 0;
    }
    return 1;
}

void X::SetFeature(int j, int s1, int s2, int featureID)
{
    Position pos(j, s1, s2);
    if(_features.count(featureID) == 0)
    {
        _features[featureID] = shared_ptr<PositionSet>(new PositionSet());
    }
    _features[featureID]->insert(pos);
}

int Y::Length() const 
{
    return (int)_tags.size();
}

const std::vector<int>& Y::Tags() const
{
    return _tags;
}

void Y::Clear()
{
    _tags.clear();
}

void Y::SetTag(int i, int tag)
{
    if(i >= (int)_tags.size())
    {
        _tags.resize(i + 1);
    }
    _tags[i] = tag;
}

void XList::SetFeature(int i, int j, int s1, int s2, int featureID)
{
    int ite = (int)(_xs.size());
    for(; ite < i + 1; ++ite)
    {
        X x;
        _xs.push_back(x);
    }
    _xs.back().SetFeature(j, s1, s2, featureID);
}

const vector<X>& XList::Raw()
{
    return _xs;
}

void XList::Append(X& x)
{
    _xs.push_back(x);
}

X& XList::At(int i)
{
    for(int ite = (int)_xs.size(); ite < i + 1; ite++)
    {
        X x;
        _xs.push_back(x);
    }
    if(-1 == i)
    {
        i = (int)_xs.size() - 1;
    }
    return _xs[i];
}

int XList::GetFeatureCount(const vector<X>& xs)
{
    std::unordered_set<int> features;
    for(auto x = xs.begin(); x != xs.end(); x++)
    {
        for(auto feature = x->Raw().begin(); feature != x->Raw().end(); feature++)
        {
            features.insert(feature->first);
        }
    }
    return features.size();
}

void YList::SetTag(int i, int j, int tag)
{
    for(int ite = (int)_ys.size(); ite < i + 1; ite++)
    {
        Y y;
        _ys.push_back(y);
    }
    // -1 is the last.
    if(-1 == i) {i = (int)_ys.size() - 1;}
    _ys[i].SetTag(j, tag);
}

void YList::Append(Y& y)
{
    _ys.push_back(y);
}

Y& YList::At(int i)
{
    for(int ite = (int)_ys.size(); ite < i + 1; ite++)
    {
        Y y;
        _ys.push_back(y);
    }
    if(-1 == i)
    {
        i = (int)_ys.size() - 1;
    }
    return _ys[i];
}

const vector<Y>& YList::Raw() const
{
    return _ys;
}

int YList::GetTagCount(const vector<Y>& ys)
{
    std::unordered_set<int> tags;
    for(auto y = ys.begin(); y != ys.end(); y++)
    {
        for(auto tag = y->Raw().begin(); tag != y->Raw().end(); tag++)
        {
            tags.insert(*tag);
        }
    }
    return tags.size();
}

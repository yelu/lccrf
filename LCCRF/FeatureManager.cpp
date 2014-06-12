#include "FeatureManager.h"


FeatureManager::FeatureManager(void)
{
}


FeatureManager::~FeatureManager(void)
{
}

void FeatureManager::Fit(const Document& doc)
{
	for(auto ite = _featurizers.begin(); ite != _featurizers.end(); ite++)
	{
		(*ite)->Fit(doc);
	}
}

void FeatureManager::Transform(const Document& doc, const wstring& s1, const wstring& s2, int j, set<int>& res)
{
	for(auto ite = _featurizers.begin(); ite != _featurizers.end(); ite++)
	{
		(*ite)->Transform(doc, s1, s2, j, res);
	}
}

bool FeatureManager::IsHit(const Document& doc, const wstring& s1, const wstring& s2, int j, int featureID)
{
	for(auto ite = _featurizers.begin(); ite != _featurizers.end(); ite++)
	{
		if((*ite)->IsHit(doc, s1, s2, j, featureID))
		{
			return true;
		}
	}
	return false;
}

size_t FeatureManager::Size()
{
	if(0 == _featurizers.size())
	{
		return 0;
	}
	return (*(_featurizers.begin()))->Size();
}

void FeatureManager::AddFeaturizer(std::shared_ptr<Featurizer>& featurizer)
{
	_featurizers.push_back(featurizer);
}

void FeatureManager::Clear()
{
	for(auto ite = _featurizers.begin(); ite != _featurizers.end(); ite++)
	{
		(*ite)->Clear();
	}
	Featurizer::StaticClear();
}
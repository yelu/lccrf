#include "FeatureManager.h"


FeatureManager::FeatureManager(void)
{
}


FeatureManager::~FeatureManager(void)
{
}

void FeatureManager::Fit(const list<Document>& docs)
{
	_nextIDs.clear();
	int startID = 0;
	for(auto ite = _featurizers.begin(); ite != _featurizers.end(); ite++)
	{
		(*ite)->SetStartID(startID);
		(*ite)->Fit(docs);
		startID = (*ite)->GetNextID();
		_nextIDs.push_back(startID);
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
		if(featureID >= (*ite)->GetNextID())
		{
			continue;
		}
		if((*ite)->IsHit(doc, s1, s2, j, featureID))
		{
			return true;
		}
		else
		{
			return false;
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
}

void FeatureManager::Serialize(wstring path)
{
	for(auto ite = _featurizers.begin(); ite != _featurizers.end(); ite++)
	{
		wstring p = path;
		p.append(L"\\");
		p.append((*ite)->Name());
		p.append(L".features");
		(*ite)->Serialize(p);
	}
}
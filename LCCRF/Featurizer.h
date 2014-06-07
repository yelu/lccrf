#pragma once

#include "Document.h"
#include <set>
using std::set;

class Featurizer
{
public:
	Featurizer(void)
	{
		_maxID = -1;
	};
	virtual ~Featurizer(void){};
	virtual void Fit(const Document& doc) = 0;
	virtual void Transform(const Document& doc, wstring s1, wstring s2, int j, set<int>& res) = 0;
	virtual bool IsHit(const Document& doc, wstring s1, wstring s2, int j, int featureID) = 0;
	virtual const wstring& Name() = 0;

	int GetFeatureCount()
	{
		if(_maxID < 0)
		{
			return 0;
		}
		return _maxID + 1;
	}

protected:
	int AllocateID()
	{
		_maxID++;
		return _maxID;
	}

private:

	int _maxID;
};


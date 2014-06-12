#pragma once

#include "Document.h"
#include <set>
using std::set;

class Featurizer
{
public:
	Featurizer(void)
	{
	};
	virtual ~Featurizer(void){};
	virtual void Fit(const Document& doc) = 0;
	virtual void Transform(const Document& doc, const wstring& s1, const wstring& s2, int j, set<int>& res) = 0;
	virtual bool IsHit(const Document& doc, const wstring& s1, const wstring& s2, int j, int featureID) = 0;
	virtual const wstring& Name() = 0;
	virtual void Serialize(const wstring& filePath) = 0;
	virtual void Clear() = 0;
	virtual wstring FeatureToString(int featureID){return L"";}

	static void StaticClear()
	{
		_maxID = -1;
	}

	static size_t Size()
	{
		if(_maxID < 0)
		{
			return 0;
		}
		return _maxID + 1;
	}

protected:
	static int AllocateID()
	{
		_maxID++;
		return _maxID;
	}

private:

	static int _maxID;
};

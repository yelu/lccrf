#pragma once

#include "Document.h"
#include <set>
#include <list>
using std::set;
using std::list;

class Featurizer
{
public:
	Featurizer()
	{
	};
	virtual ~Featurizer(void){};
	virtual void Fit(const list<Document>& doc) = 0;
	virtual void Transform(const Document& doc, const wstring& s1, const wstring& s2, int j, set<int>& res) = 0;
	virtual bool IsHit(const Document& doc, const wstring& s1, const wstring& s2, int j, int featureID) = 0;
	virtual const wstring& Name() = 0;
	virtual void Serialize(const wstring& filePath) = 0;
	virtual void Clear() = 0;
	virtual wstring FeatureToString(int featureID){return L"";}

	void SetStartID(int startID)
	{
		_minID = startID;
		_nextID = _minID;
	}

	size_t Size()
	{
		return _nextID - _minID;
	}

	int GetNextID()
	{
		return _nextID;
	}
protected:
	int AllocateID()
	{
		_nextID++;
		return _nextID - 1;
	}

private:

	int _minID;
	int _nextID;
};

#pragma once

#include <set>
#include "Featurizer.h"
#include "IDAllocator.h"
using std::set;

class TransitionFeaturizer:public Featurizer
{
public:
	TransitionFeaturizer(void);
	virtual ~TransitionFeaturizer(void);
	void Fit(const list<Document>& docs);
	void Transform(const Document& doc, const wstring& s1, const wstring& s2, int j, set<int>& res);
	bool IsHit(const Document& doc, const wstring& s1, const wstring& s2, int j, int featureID);
	const wstring& Name();
	void Serialize(const wstring& filePath);
	void Deserialize(const wstring& filePath);
	void Clear();

private:

	wstring _MakeTransition(const wstring& s1, const wstring& s2);

	IDAllocator _idAllocator;
	wstring _name;
};


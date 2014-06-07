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
	void Fit(const Document& doc);
	void Transform(const Document& doc, wstring s1, wstring s2, int j, set<int>& res);
	bool IsHit(const Document& doc, wstring s1, wstring s2, int j, int featureID);
	const wstring& Name();

private:

	wstring _MakeTransition(wstring s1, wstring s2);

	IDAllocator _idAllocator;
	wstring _name;
};


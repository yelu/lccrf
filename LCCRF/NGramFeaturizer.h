#pragma once

#include <set>
#include "Featurizer.h"
#include "IDAllocator.h"
using std::set;

class NGramFeaturizer : public Featurizer
{
public:
	NGramFeaturizer(int n);
	virtual ~NGramFeaturizer(void);
	void Fit(const Document& doc);
	void Transform(const Document& doc, wstring s1, wstring s2, int j, set<int>& res);
	bool IsHit(const Document& doc, wstring s1, wstring s2, int j, int featureID);
	const wstring& Name();

private:

	wstring _MakeGram(const Document& doc, wstring s, int startPos);

	int _n;
	IDAllocator _idAllocator;
	wstring _name;
};


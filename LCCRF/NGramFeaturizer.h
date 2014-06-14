#pragma once

#include <set>
#include "Featurizer.h"
#include "IDAllocator.h"
using std::set;

class NGramFeaturizer : public Featurizer
{
public:
	NGramFeaturizer(int n, const vector<int>& fields = vector<int>(1, 0));
	virtual ~NGramFeaturizer(void);
	void Fit(const list<Document>& docs);
	void Transform(const Document& doc, const wstring& s1, const wstring& s2, int j, set<int>& res);
	bool IsHit(const Document& doc, const wstring& s1, const wstring& s2, int j, int featureID);
	const wstring& Name();
	void Clear();
	void Serialize(const wstring& filePath);

private:

	wstring _MakeGram(const Document& doc, const wstring& s, int startPos);

	int _n;
	IDAllocator _idAllocator;
	wstring _name;
	vector<int> _fields;
};


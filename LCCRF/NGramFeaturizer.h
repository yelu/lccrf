#pragma once

#include <set>
#include "Featurizer.h"
#include "IDAllocator.h"
using std::set;

class NGramFeaturizer : public Featurizer
{
public:
	NGramFeaturizer(void){}
	NGramFeaturizer(int n, const vector<int>& fields = vector<int>(1, 0));
	virtual ~NGramFeaturizer(void);
	void Fit(const list<Document>& docs);
	void Transform(const Document& doc, const wstring& s1, const wstring& s2, int j, set<int>& res);
	bool IsHit(const Document& doc, const wstring& s1, const wstring& s2, int j, int featureID);
	const wstring& Name();
	void Clear();
	void Serialize(const wstring& filePath);
	void Deserialize(const wstring& filePath);

private:

	int _MakeGram(const Document& doc, const wstring& s, int startPos, wstring& res);

	int _n;
	IDAllocator _idAllocator;
	wstring _name;
	vector<int> _fields;
};


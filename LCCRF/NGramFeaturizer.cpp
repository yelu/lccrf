#include "NGramFeaturizer.h"
#include <boost/lexical_cast.hpp>

NGramFeaturizer::NGramFeaturizer(int n):_n(n)
{
	_name = boost::lexical_cast<wstring>(_n);
	_name.append(L"gram");
}


NGramFeaturizer::~NGramFeaturizer(void)
{
}

const wstring& NGramFeaturizer::Name()
{
	return _name;
}

wstring NGramFeaturizer::_MakeGram(const Document& doc, wstring s, int endPos)
{
	// blank as seperator to seperator words. \t as seperotor to seperate tag.
	wstring gram = L"";
	for(int j = endPos - _n  + 1; j <= endPos; j++)
	{
		gram.append(doc[j].x[0]);
		gram.append(L" ");
	}
	gram.append(L"\t");
	gram.append(s);
	return gram;
}

void NGramFeaturizer::Fit(const Document& doc)
{
	for(int i = _n - 1; i < (int)(doc.size()); i++)
	{
		wstring gram = _MakeGram(doc, doc[i].y, i);
		if(_idAllocator.Contains(gram))
		{
			continue;
		}
		int id = AllocateID();
		_idAllocator.Insert(gram, id);
	}
}

void NGramFeaturizer::Transform(const Document& doc, wstring s1, wstring s2, int j, set<int>& res)
{
	if(j + 1 < _n || j >= (int)(doc.size()))
	{
		return;
	}
	wstring gram = _MakeGram(doc, s2, j);
	if(_idAllocator.Contains(gram))
	{
		res.insert(_idAllocator.GetOrAllocateID(gram));
	}
}

bool NGramFeaturizer::IsHit(const Document& doc, wstring s1, wstring s2, int j, int featureID)
{
	wstring gram = _MakeGram(doc, s2, j);
	if(gram == _idAllocator.GetText(featureID))
	{
		return true;
	}
	return false;
}
#include "NGramFeaturizer.h"
#include <boost/lexical_cast.hpp>
#include <fstream>
using std::wofstream;
using std::wifstream;

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

wstring NGramFeaturizer::_MakeGram(const Document& doc, const wstring& s, int endPos)
{
	// blank as seperator to seperator words. \t as seperotor to seperate tag.
	wstring gram = s;
	gram.append(L" ");
	for(int j = endPos - _n  + 1; j <= endPos; j++)
	{
		gram.append(doc[j].x[0]);
		gram.append(L" ");
	}
	return gram;
}

void NGramFeaturizer::Fit(const list<Document>& docs)
{
	for(auto doc = docs.begin(); doc != docs.end(); doc++)
	{
		for(int i = _n - 1; i < (int)(doc->size()); i++)
		{
			wstring gram = _MakeGram(*doc, (*doc)[i].y, i);
			if(_idAllocator.Contains(gram))
			{
				continue;
			}
			int id = AllocateID();
			_idAllocator.Insert(gram, id);
		}
	}
}

void NGramFeaturizer::Transform(const Document& doc, const wstring& s1, const wstring& s2, int j, set<int>& res)
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

bool NGramFeaturizer::IsHit(const Document& doc, const wstring& s1, const wstring& s2, int j, int featureID)
{
	wstring gram = _MakeGram(doc, s2, j);
	if(gram == _idAllocator.GetText(featureID))
	{
		return true;
	}
	return false;
}

size_t NGramFeaturizer::Size()
{
	return _idAllocator.Size();
}

void NGramFeaturizer::Clear()
{
	_idAllocator.Clear();
}

void NGramFeaturizer::Serialize(const wstring& filePath)
{
	std::wofstream ofs(filePath);
	ofs << _n;
	ofs << L"\n";
	for(auto ite = _idAllocator.Begin(); ite != _idAllocator.End(); ite++)
	{
		ofs << (*ite).first << L"\t" << (*ite).second;
		ofs << L"\n";
	}
	ofs.close();
}

wstring NGramFeaturizer::FeatureToString(int featureID)
{
	wstring res = _name;
	res.append(L".");
	res.append(_idAllocator.GetText(featureID));
	return res;
}
#include "NGramFeaturizer.h"
#include <boost/lexical_cast.hpp>
#include <fstream>
#include "Utils.h"
using std::wofstream;
using std::wifstream;

NGramFeaturizer::NGramFeaturizer(int n, const vector<int>& fields):_n(n), _fields(fields)
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
	// use blank as seperator to seperator words. blank character will be escaped to double blank.
	// '\r\n' will be replaced to blank to avoid deserilization problem.
	wstring gram = StringUtils::Escape(s, L' ');
	StringUtils::Replace(gram, L'\n', L' ');
	StringUtils::Replace(gram, L'\r', L' ');
	gram.append(L" ");
	for(int j = endPos - _n  + 1; j <= endPos; j++)
	{
		for(auto ite = doc[j].x.begin(); ite != doc[j].x.end(); ite++)
		{
			wstring t = StringUtils::Escape(*ite, L' ');
			StringUtils::Replace(gram, L'\n', L' ');
			StringUtils::Replace(gram, L'\r', L' ');
			gram.append(t);
			gram.append(L" ");
		}
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
	if(j - _n + 1 < 0)
	{
		return false;
	}
	wstring gram = _MakeGram(doc, s2, j);
	if(gram == _idAllocator.GetText(featureID))
	{
		return true;
	}
	return false;
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
	for(auto ite = _fields.begin(); ite != _fields.end(); ite++)
	{
		ofs << *ite;
		ofs << "\t";
	}
	ofs << "\n";
	for(auto ite = _idAllocator.Begin(); ite != _idAllocator.End(); ite++)
	{
		ofs << (*ite).first << L"\t" << (*ite).second;
		ofs << L"\n";
	}
	ofs.close();
}

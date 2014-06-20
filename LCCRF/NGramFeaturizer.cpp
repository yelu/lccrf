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

int NGramFeaturizer::_MakeGram(const Document& doc, const wstring& s, int endPos, wstring& res)
{
	// use blank as seperator to seperator words. blank character will be escaped to double blank.
	// '\r\n' will be replaced to blank to avoid deserilization problem.
	res = s;
	StringUtils::Replace(res, L'\n', L' ');
	StringUtils::Replace(res, L'\r', L' ');
	res = StringUtils::Escape(res, L' ');
	res.append(L" ");
	for(int j = endPos - _n  + 1; j <= endPos; j++)
	{
		for(auto ite = _fields.begin(); ite != _fields.end(); ite++)
		{
			if(*ite < 0 || *ite >= (int)doc.size())
			{
				return -1;
			}
			wstring t = doc[j].x[*ite];
			StringUtils::Replace(t, L'\n', L' ');
			StringUtils::Replace(t, L'\r', L' ');
			t = StringUtils::Escape(t, L' ');		
			res.append(t);
			res.append(L" ");
		}
	}
	return 0;
}

void NGramFeaturizer::Fit(const list<Document>& docs)
{
	for(auto doc = docs.begin(); doc != docs.end(); doc++)
	{
		for(int i = _n - 1; i < (int)(doc->size()); i++)
		{
			wstring gram;
			if(0 != _MakeGram(*doc, (*doc)[i].y, i,gram))
			{
				continue;
			}
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
	wstring gram;
	if(0 != _MakeGram(doc, s2, j, gram))
	{
		return;
	}
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
	wstring gram;
	if(0 != _MakeGram(doc, s2, j, gram))
	{
		return false;
	}
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

void NGramFeaturizer::Deserialize(const wstring& filePath)
{
	std::wstring line;
	std::wifstream ifs(filePath);

	std::getline(ifs, line);
	_n = _wtoi(line.c_str());

	std::getline(ifs, line);
	std::vector<wstring> tokens = StringUtils::Split(line, L'\t');
	for(int i = 0; i < tokens.size(); i++)
	{
		_fields.push_back(_wtoi(tokens[i].c_str()));
	}

	while (std::getline(ifs, line))
	{
		std::vector<wstring> tokens = StringUtils::Split(line, L'\t');
		int id = _wtoi(tokens[0].c_str());
		_idAllocator.Insert(tokens[1], id);
	}
	ifs.close();
}
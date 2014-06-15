#include "TransitionFeaturizer.h"
#include <fstream>
#include "Utils.h"
using std::wofstream;

TransitionFeaturizer::TransitionFeaturizer()
{
	_name = L"Transition";
}


TransitionFeaturizer::~TransitionFeaturizer(void)
{
}

wstring TransitionFeaturizer::_MakeTransition(const wstring& s1, const wstring& s2)
{
	wstring s11 = s1;
	StringUtils::Replace(s11, L'\n', L' ');
	StringUtils::Replace(s11, L'\r', L' ');
	s11 = StringUtils::Escape(s11, L' ');

	wstring s22 = s2;
	StringUtils::Replace(s22, L'\n', L' ');
	StringUtils::Replace(s22, L'\r', L' ');
	s22 = StringUtils::Escape(s22, L' ');

	wstring tr = L"";
	tr.append(s11);	
	tr.append(L" ");
	tr.append(s22);
	return tr;
}

const wstring& TransitionFeaturizer::Name()
{
	return _name;
}

void TransitionFeaturizer::Fit(const list<Document>& docs)
{
	for(auto doc = docs.begin(); doc != docs.end(); doc++)
	{
		for(int i = 1; i < (int)(doc->size()); i++)
		{
			wstring tr = _MakeTransition((*doc)[i - 1].y, (*doc)[i].y);
			if(_idAllocator.Contains(tr))
			{
				continue;
			}
			int id = AllocateID();
			_idAllocator.Insert(tr, id);
		}
	}
}

void TransitionFeaturizer::Transform(const Document& doc, const wstring& s1, const wstring& s2, int j, set<int>& res)
{
	if(j < 1 || j >= (int)(doc.size()))
	{
		return;
	}
	wstring tr = _MakeTransition(s1, s2);
	if(_idAllocator.Contains(tr))
	{
		res.insert(_idAllocator.GetOrAllocateID(tr));
	}
}

bool TransitionFeaturizer::IsHit(const Document& doc, const wstring& s1, const wstring& s2, int j, int featureID)
{
	wstring tr = _MakeTransition(s1, s2);
	if(tr == _idAllocator.GetText(featureID))
	{
		return true;
	}
	return false;
}

void TransitionFeaturizer::Clear()
{
	_idAllocator.Clear();
}

void TransitionFeaturizer::Serialize(const wstring& filePath)
{
	std::wofstream ofs(filePath);
	for(auto ite = _idAllocator.Begin(); ite != _idAllocator.End(); ite++)
	{
		ofs << (*ite).first << L"\t" << (*ite).second;
		ofs << L"\n";
	}
	ofs.close();
}

void TransitionFeaturizer::Deserialize(const wstring& filePath)
{
	std::wifstream ifs(filePath);
	wstring line;
	while(std::getline(ifs, line))
	{
		std::vector<wstring> tokens = StringUtils::Split(line, L'\t');
		int id = _wtoi(tokens[0].c_str());
		_idAllocator.Insert(tokens[1], id);
	}

	ifs.close();
}

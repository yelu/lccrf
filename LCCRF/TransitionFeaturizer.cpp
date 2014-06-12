#include "TransitionFeaturizer.h"
#include <fstream>
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
	wstring tr = L"";
	tr.append(s1);
	tr.append(L" ");
	tr.append(s2);
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

size_t TransitionFeaturizer::Size()
{
	return _idAllocator.Size();
}

void TransitionFeaturizer::Clear()
{
	_idAllocator.Clear();
}

void TransitionFeaturizer::Serialize(const wstring& filePath)
{
	std::wofstream ofs(filePath);
	ofs << L"\n";
	for(auto ite = _idAllocator.Begin(); ite != _idAllocator.End(); ite++)
	{
		ofs << (*ite).first << L"\t" << (*ite).second;
		ofs << L"\n";
	}
	ofs.close();
}

wstring TransitionFeaturizer::FeatureToString(int featureID)
{
	wstring res = _name;
	res.append(L".");
	res.append(_idAllocator.GetText(featureID));
	return res;
}
#include "TransitionFeaturizer.h"


TransitionFeaturizer::TransitionFeaturizer(void)
{
	_name = L"Transition";
}


TransitionFeaturizer::~TransitionFeaturizer(void)
{
}

wstring TransitionFeaturizer::_MakeTransition(wstring s1, wstring s2)
{
	wstring tr = L"";
	tr.append(s1);
	tr.append(L"\t");
	tr.append(s2);
	return tr;
}

const wstring& TransitionFeaturizer::Name()
{
	return _name;
}

void TransitionFeaturizer::Fit(const Document& doc)
{
	for(int i = 1; i < (int)(doc.size()); i++)
	{
		wstring tr = _MakeTransition(doc[i - 1].y, doc[i].y);
		if(_idAllocator.Contains(tr))
		{
			continue;
		}
		int id = AllocateID();
		_idAllocator.Insert(tr, id);
	}
}

void TransitionFeaturizer::Transform(const Document& doc, wstring s1, wstring s2, int j, set<int>& res)
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

bool TransitionFeaturizer::IsHit(const Document& doc, wstring s1, wstring s2, int j, int featureID)
{
	wstring tr = _MakeTransition(s1, s2);
	if(tr == _idAllocator.GetText(featureID))
	{
		return true;
	}
	return false;
}

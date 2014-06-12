#pragma once

#include <map>
using std::map;
#include <string>
using std::wstring;

class IDAllocator
{
public:

	IDAllocator(void);
	virtual ~IDAllocator(void);

	int GetOrAllocateID(wstring text)
	{
		if(_textToIDMapping.count(text) != 0)
		{
			return _textToIDMapping[text];
		}
		_currentMaxID++;
		_textToIDMapping[text] = _currentMaxID;
		_idToTextMapping[_currentMaxID] = text;
		return _currentMaxID;
	}

	wstring GetText(int id)
	{
		if(_idToTextMapping.count(id) != 0)
		{
			return _idToTextMapping[id];
		}
		return L"";
	}

	int Insert(wstring text, int id)
	{
		if(_textToIDMapping.count(text) != 0)
		{
			return _textToIDMapping[text];
		}
		_textToIDMapping[text] = id;
		_idToTextMapping[id] = text;
		return id;
	}

	bool Contains(wstring text)
	{
		if(_textToIDMapping.count(text) != 0)
		{
			return true;
		}
		return false;
	}

	size_t Size()
	{
		return _textToIDMapping.size();
	}

	// for iterations.
	typedef map<int, wstring>::const_iterator const_iterator;

	map<int, wstring>::const_iterator Begin()
	{
		return _idToTextMapping.begin();
	}

	map<int, wstring>::const_iterator End()
	{
		return _idToTextMapping.end();
	}

	void Clear()
	{
		_currentMaxID = -1;
		_textToIDMapping.clear();
		_idToTextMapping.clear();
	}

private:
	int _currentMaxID;
	map<wstring, int> _textToIDMapping;
	map<int, wstring> _idToTextMapping;
};


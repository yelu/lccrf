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

	int Allocate(wstring text)
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

	size_t Size()
	{
		return _textToIDMapping.size();
	}

private:
	int _currentMaxID;
	map<wstring, int> _textToIDMapping;
	map<int, wstring> _idToTextMapping;
};


#pragma once

#include<string>
#include<vector>
#include<sstream>
using std::wstring;
using std::vector;

class StringUtils
{
public:
	StringUtils(void){}
	virtual ~StringUtils(void){}
	static wstring Escape(const wstring& in, wchar_t ch = L' ')
	{
		wstring out;
		for(size_t i = 0; i < in.size(); i++)
		{
			if(in[i] == ch)
			{
				out.push_back(ch);
			}
			out.push_back(in[i]);
		}
		return out;
	}

	static void Replace(wstring& in, wchar_t src, wchar_t dst)
	{
		for(size_t i = 0; i < in.size(); i++)
		{
			if(in[i] == src)
			{
				in[i] = dst;
			}
		}
	}

	static wstring Join(const vector<wstring>& in, wchar_t ch = L' ')
	{
		wstring out;
		for(auto ite = in.begin(); ite != in.end(); ite++)
		{
			out.append(Escape(*ite, ch));
			out.push_back(ch);
		}
		return out;
	}

	static std::vector<std::wstring> Split(const std::wstring& s, wchar_t delim) 
	{
		std::vector<std::wstring> elems;
		std::wstringstream ss(s);
		std::wstring item;
		while (std::getline(ss, item, delim)) 
		{
			elems.push_back(item);
		}
		return elems;
	}

private:
};


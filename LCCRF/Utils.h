#pragma once

#include<string>
#include<vector>
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

	static void Replace(wstring in, wchar_t src, wchar_t dst)
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

	static void Split(const wstring& in, vector<wstring>& res, wchar_t ch = L' ')
	{
		size_t s = 0;
		size_t e = -1;
		wstring cur;
		for(size_t i = 0; i < in.size(); i++)
		{
			if(in[i]  == ch)
			{
				if(i + 1 < in.size() && in[i + 1] == ch)
				{
					cur.push_back(ch);
					i++;
				}
				else
				{
					res.push_back(cur);
					cur = L"";
				}
				continue;
			}
			cur.push_back(in[i]);
		}
		res.push_back(cur);
	}

private:
};


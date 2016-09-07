#include <string>
#include <sstream>
#include <vector>

static std::vector<std::string>& split(const std::string &s, char delim, std::vector<std::string> &elems) {
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) 
	{
		if (!item.empty())
		{
			elems.push_back(item);
		}
	}
	return elems;
}

static std::vector<std::string> split(const std::string &s, char delim = ' ') 
{
	std::stringstream ss(s);
	std::string item;
	std::vector<std::string> elems;
	while (std::getline(ss, item, delim))
	{
		if (!item.empty())
		{
			elems.push_back(item);
		}
	}
	return elems;
}
#pragma once
#include <vector>
#include <string>
#include <sstream>
#include <unordered_set>

class Helper
{
public:
	static std::vector<std::string> splitString(std::string str, char delim);
	static bool findInVector(std::vector<std::string> vec, std::string elem);
	static bool isAllCaps(std::string s);
	static std::unordered_set<std::string> concatenationOne(std::unordered_set<std::string> set1, std::unordered_set<std::string> set2, bool& error);
	static std::unordered_set<std::string> union_custom(std::unordered_set<std::string> set1, std::unordered_set<std::string> set2);
	static std::unordered_set<std::string> intersect_custom(std::unordered_set<std::string> set1, std::unordered_set<std::string> set2);
};


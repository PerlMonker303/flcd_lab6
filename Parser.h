#pragma once
#include "Grammar.h"
#include <map>
#include <unordered_set>

// LL(1)
class Parser
{
private:
	Grammar* grammar;
	std::map<std::string, std::unordered_set<std::string>> first_tbl;
	std::map<std::string, std::unordered_set<std::string>> follow_tbl;
public:
	Parser(Grammar* g);
	void first();
	void follow();

	void displayFirstTable();
	void displayFollowTable();
};

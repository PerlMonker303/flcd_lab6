#pragma once
#include "Grammar.h"
#include <map>
#include <unordered_set>
#include <stack>

// using a tuple similar to this structure
struct Tuple {
	std::string column;
	std::vector<std::string> RHS;
	int nr;
	std::string action; // for pop, acc
};

struct Config {
	std::stack<std::string> alpha; // input stack
	std::stack<std::string> beta; // working stack
	std::vector<int> pi; // output (result)
};

// LL(1)
class Parser
{
private:
	Grammar* grammar;
	std::map<std::string, std::unordered_set<std::string>> first_tbl;
	std::map<std::string, std::unordered_set<std::string>> follow_tbl;
	std::map< std::string, std::vector< Tuple > > ll1_tbl; // used a vector of pairs instead of a set of pairs (needed hash impl) + ORDER MATTERS
public:
	Parser(Grammar* g);
	void first();
	void follow();
	void constructLL1Table();
	bool checkConflictsLL1Table();
	std::string checkSequence(std::string w);
	Tuple getEntryLL1Table(std::string row, std::string column);

	void displayFirstTable();
	void displayFollowTable();
	void displayLL1Table();
	void displayConfig(Config config);
};

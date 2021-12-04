#pragma once
#include "Grammar.h"
#include "ParserOutput.h"
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
	ParserOutput* parserOutput;
public:
	Parser(Grammar* g, ParserOutput* po);
	void first();
	void follow();
	void constructLL1Table();
	bool checkConflictsLL1Table();
	std::vector<int> checkSequence(std::vector<std::string> w);
	Tuple getEntryLL1Table(std::string row, std::string column);
	void constructParseTree(std::vector<int> sequence);
	void addChildren(std::vector<int>& sequence, int& rowIndex, int parentIndex);
	std::vector<std::string> parsePIF(std::string fileName);

	void displayFirstTable();
	void displayFollowTable();
	void displayLL1Table();
	void displayConfig(Config config);
	void displayAllConflicts();
};

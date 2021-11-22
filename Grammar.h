#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "Helper.h"

struct Production {
	std::vector<std::string> LHSVector; // multiple non-terminal
	std::string LHS; // single non-terminal
	std::vector<std::string> RHS; // non-terminals + terminals + epsilon
	int index;
};


class Grammar
{
private:
	std::string gPath;
	std::vector<std::string> nonTerminals;
	std::vector<std::string> terminals;
	std::vector<Production> productions;
	std::string startingSymbol;
	std::string encounteredError = "";
	int currentProductionIndex = 1;

public:
	Grammar(std::string gPath);
	void readGrammar();
	std::vector<std::string> getTerminals();
	std::vector<std::string> getNonTerminals();
	std::vector<Production> getProductionsForNonTerminal(std::string nonTerminal);
	std::vector<Production> getProductionsWithNonTerminalOnRHS(std::string nonTerminal);
	std::vector<Production> getProductions();
	std::string getStartingSymbol();
	bool checkCFG();
	std::pair < std::vector<std::string>, std::vector<std::string> > splitRHSOnNonTerminal(Production p, std::string nonTerminal);

	bool getIsTerminal(std::string symbol);
	bool getIsNonTerminal(std::string symbol);
	bool getIsEpsilon(std::string symbol);
	bool getIsStartingSymbol(std::string symbol);

	void displayNonTerminals();
	void displayTerminals();
	void displayProductions();
	void displayProductions(std::vector<Production> productions);
	void displayStartingSymbol();

	std::string getEncounteredError();
};

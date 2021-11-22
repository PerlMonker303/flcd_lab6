#include "Grammar.h"

Grammar::Grammar(std::string gPath) {
	this->gPath = gPath;
}

void Grammar::readGrammar() {
	std::ifstream file(this->gPath);
	std::string nonTerminalsString = "";
	std::string terminalsString = "";

	std::getline(file, nonTerminalsString);
	std::getline(file, terminalsString);

	this->nonTerminals = Helper::splitString(nonTerminalsString, ' ');
	this->terminals = Helper::splitString(terminalsString, ' ');

	int noProductions = 0;
	std::string productionString;
	std::vector<std::string> productionVector;
	file >> noProductions;
	std::getline(file, productionString); // called to go to \n

	for (int i = 0; i < noProductions; i++) {
		std::getline(file, productionString);
		productionVector = Helper::splitString(productionString, ' ');
		// read until ->
		int j = 0;
		bool isLHS = true;
		std::vector<std::string> LHSVector;
		std::string LHS = "";
		// go through LHS
		while (j < productionVector.size() && isLHS) {
			if (productionVector[j] == "->") {
				isLHS = false;
				j++;
				continue;
			}
			if (LHS == "") {
				LHS = productionVector[j];
			}
			LHSVector.push_back(productionVector[j]);
			j++;
		}
		// check if LHS contains at least one non-terminal and if all terms are in the terminal set
		bool foundNonTerm = false;
		for (int k = 0; k < LHSVector.size(); k++) {
			if (Helper::findInVector(this->nonTerminals, LHSVector[k])) {
				foundNonTerm = true;
			}
			else if (!Helper::findInVector(this->terminals, LHSVector[k])) {
				this->encounteredError = "[Error: LHS symbol '" + LHSVector[k] + "' is neither a terminal, nor a non-terminal symbol.]\n";
				return;
			}
		}
		if (!foundNonTerm) {
			this->encounteredError = "[Error: LHS does not contain any non-terminal symbol.]\n";
			return;
		}

		// go through RHS
		std::vector<std::string> RHS;
		while (j < productionVector.size()) {
			if (productionVector[j] == "|") {
				Production production;
				production.LHSVector = LHSVector;
				production.LHS = LHS;
				production.RHS = RHS;
				production.index = this->currentProductionIndex++;
				this->productions.push_back(production);
				RHS.clear();
			}
			else {
				// check if symbol is non-terminal or terminal
				if (!Helper::findInVector(this->nonTerminals, productionVector[j]) && !Helper::findInVector(this->terminals, productionVector[j])) {
					if (productionVector[j] != "Epsilon") {
						this->encounteredError = "[Error: RHS symbol '" + productionVector[j] + "' is neither a terminal, nor a non-terminal symbol.]\n";
						return;
					}
				}
				RHS.push_back(productionVector[j]);
			}
			j++;
		}

		if (RHS.size() > 0) {
			Production production;
			production.LHSVector = LHSVector;
			production.LHS = LHS;
			production.RHS = RHS;
			production.index = this->currentProductionIndex++;
			this->productions.push_back(production);
		}
	}

	std::getline(file, this->startingSymbol);
}

std::vector<std::string> Grammar::getTerminals() {
	return this->terminals;
}

std::vector<std::string> Grammar::getNonTerminals() {
	return this->nonTerminals;
}

std::vector<Production> Grammar::getProductionsForNonTerminal(std::string nonTerminal) {
	std::vector<Production> result;
	for (int i = 0; i < this->productions.size(); i++) {
		if (this->productions[i].LHS == nonTerminal) {
			result.push_back(this->productions[i]);
		}
	}
	return result;
}

std::vector<Production> Grammar::getProductionsWithNonTerminalOnRHS(std::string nonTerminal) {
	std::vector<Production> result;
	for (int i = 0; i < this->productions.size(); i++) {
		if(Helper::findInVector(this->productions[i].RHS, nonTerminal)){
			result.push_back(this->productions[i]);
		}
	}
	return result;
}

std::vector<Production> Grammar::getProductions() {
	return this->productions;
}

std::string Grammar::getStartingSymbol() {
	return this->startingSymbol;
}

bool Grammar::checkCFG() {
	for (int i = 0; i < this->productions.size(); i++) {
		if (this->productions[i].LHSVector.size() > 1) {
			return false;
		}
	}
	return true;
}

// assuming the nonTerminal is unique in the Production
std::pair < std::vector<std::string>, std::vector<std::string> > Grammar::splitRHSOnNonTerminal(Production p, std::string nonTerminal) {
	std::vector<std::string> alpha;
	std::vector<std::string> gamma;

	bool reachedNonTerminal = false;
	for (auto symbol : p.RHS) {
		if (reachedNonTerminal) {
			gamma.push_back(symbol);
		}
		else {
			if (symbol == nonTerminal) {
				reachedNonTerminal = true;
			}
			else {
				alpha.push_back(symbol);
			}
		}
	}

	return std::pair<std::vector<std::string>, std::vector<std::string>>(alpha, gamma);
}

bool Grammar::getIsTerminal(std::string symbol) {
	return Helper::findInVector(this->terminals, symbol);
}

bool Grammar::getIsNonTerminal(std::string symbol) {
	return Helper::findInVector(this->nonTerminals, symbol);
}

bool Grammar::getIsEpsilon(std::string symbol) {
	return symbol == "Epsilon";
}

bool Grammar::getIsStartingSymbol(std::string symbol) {
	return this->startingSymbol == symbol;
}

void Grammar::displayNonTerminals() {
	std::cout << "[Non terminals ...]\n";
	std::cout << "{ ";
	for (int i = 0; i < this->nonTerminals.size(); i++) {
		std::cout << this->nonTerminals[i] << ' ';
	}
	std::cout << "}\n";
	std::cout << "[... done]\n";
}

void Grammar::displayTerminals() {
	std::cout << "[Terminals ...]\n";
	std::cout << "{ ";
	for (int i = 0; i < this->terminals.size(); i++) {
		std::cout << this->terminals[i] << ' ';
	}
	std::cout << "}\n";
	std::cout << "[... done]\n";
}

void Grammar::displayProductions() {
	std::cout << "[Productions ...]\n";
	for (int i = 0; i < this->productions.size(); i++) {
		for (int j = 0; j < this->productions[i].LHSVector.size(); j++) {
			std::cout << this->productions[i].LHSVector[j] << ' ';
		}
		std::cout << " -> ";
		for (int j = 0; j < this->productions[i].RHS.size(); j++) {
			std::cout << this->productions[i].RHS[j] << ' ';
		}
		std::cout << "(idx: " << this->productions[i].index << ")";
		std::cout << '\n';
	}
	std::cout << "[... done]\n";
}

void Grammar::displayProductions(std::vector<Production> productions) {
	std::cout << "[Productions ...]\n";
	for (int i = 0; i < productions.size(); i++) {
		std::cout << productions[i].LHS << " -> ";
		for (int j = 0; j < productions[i].RHS.size(); j++) {
			std::cout << productions[i].RHS[j] << ' ';
		}
		std::cout << "(idx: " << productions[i].index << ")";
		std::cout << '\n';
	}
	std::cout << "[... done]\n";
}

void Grammar::displayStartingSymbol() {
	std::cout << "[Starting symbol ...]\n";
	std::cout << this->startingSymbol << '\n';
	std::cout << "[... done]\n";
}

std::string Grammar::getEncounteredError() {
	return this->encounteredError;
}
#include "Parser.h"

Parser::Parser(Grammar* g) {
	this->grammar = g;
}

void Parser::first() {
	this->first_tbl.clear();
	std::vector<std::string> terminals = this->grammar->getTerminals();
	for (auto a : terminals) {
		first_tbl[a] = std::unordered_set<std::string>{ a };
	}
	std::vector<std::string> nonTerminals = this->grammar->getNonTerminals();
	for(auto A : nonTerminals){
		first_tbl[A] = std::unordered_set<std::string>();
		std::vector<Production> productions = this->grammar->getProductionsForNonTerminal(A);
		for (Production p : productions) {
			if (this->grammar->getIsTerminal(p.RHS[0]) || this->grammar->getIsEpsilon(p.RHS[0])) {
				first_tbl[A].insert(p.RHS[0]);
			}
		}
	}
	this->displayFirstTable();
	bool different = false, error, firstSymbol;
	do {
		different = false;
		for (auto A : nonTerminals) {
			std::unordered_set<std::string> new_set{};
			std::vector<Production> productions = this->grammar->getProductionsForNonTerminal(A);
			for (Production p : productions) {
				std::unordered_set<std::string> right_set{};
				// parse RHS
				error = false;
				firstSymbol = true;
				for (auto symbol : p.RHS) {
					if (this->grammar->getIsTerminal(symbol) || this->grammar->getIsEpsilon(symbol)) {
						if (firstSymbol) {
							right_set.insert(symbol);
						}
						else {
							right_set = Helper::concatenationOne(right_set, std::unordered_set<std::string>{symbol}, error);
						}
					}
					else if (this->grammar->getIsNonTerminal(symbol)) {
						if (firstSymbol) {
							right_set = first_tbl[symbol];
						}
						else {
							right_set = Helper::concatenationOne(right_set, first_tbl[symbol], error);
							if (error) {
								break;
							}
						}
					}
					firstSymbol = false;
				}
				if (!error) {
					new_set = Helper::union_custom(new_set, right_set);
				}
			}
			if (first_tbl[A].size() != new_set.size()) {
				different = true;
				first_tbl[A] = Helper::union_custom(first_tbl[A], new_set);
			}
		}
		this->displayFirstTable();
	} while (different);

	// remove terminals from first_tbl?
	// if yes, don't do it here, follow won't work
	/*
	for (auto symbol : this->first_tbl) {
		if (this->grammar->getIsTerminal(symbol.first)) {
			//auto it = this->first_tbl.find(symbol.first);
			this->first_tbl.erase(symbol.first);
		}
	}
	*/
}

void Parser::follow() {
	this->follow_tbl.clear();
	std::vector<std::string> nonTerminals = this->grammar->getNonTerminals();
	for (auto A : nonTerminals) {
		follow_tbl[A] = std::unordered_set<std::string>();
		if (this->grammar->getIsStartingSymbol(A)) {
			follow_tbl[A].insert("Epsilon");
		}
	}
	this->displayFollowTable();
	int different;
	std::string A;
	do {
		different = false;
		std::map<std::string, std::unordered_set<std::string>> follow_tbl_previous = this->follow_tbl;
		for (auto B : nonTerminals) {
			this->follow_tbl[B] = follow_tbl_previous[B];
			for (Production p : this->grammar->getProductionsWithNonTerminalOnRHS(B)) {
				A = p.LHS;
				std::vector<std::string> gamma = this->grammar->splitRHSOnNonTerminal(p, B).second;
				if (gamma.size() > 0) {
					// compute first(gamma)
					std::string symbol = gamma[0]; // only first symbol is important
					for (auto a : this->first_tbl[symbol]) {
						if (this->grammar->getIsEpsilon(a)) {
							this->follow_tbl[B] = Helper::union_custom(this->follow_tbl[B], follow_tbl_previous[A]);
						}
						else {
							// if it contains epsilon, remove it
							//if (Helper::findInSet(this->follow_tbl[B], "Epsilon")) {
								//Helper::removeElementFromSet(this->follow_tbl[B], "Epsilon");
							//}
							this->follow_tbl[B] = Helper::union_custom(this->follow_tbl[B], this->first_tbl[symbol], true);
						}
					}
				}
				else {
					// nothing after B
					this->follow_tbl[B] = Helper::union_custom(this->follow_tbl[B], follow_tbl_previous[A]);
				}
			}
		}
		this->displayFollowTable();
		// check different
		for (auto A : nonTerminals) {
			if (follow_tbl[A].size() != follow_tbl_previous[A].size()) {
				different = true;
				break;
			}
		}
	} while (different);
}

void Parser::displayFirstTable() {
	std::cout << "[First table ...]\n";
	for (auto a : this->first_tbl) {
		std::cout << a.first << ": ";
		for (auto b : a.second) {
			std::cout << b << ' ';
		}
		std::cout << '\n';
	}
	std::cout << "[... done]\n";
}

void Parser::displayFollowTable() {
	std::cout << "[Follow table ...]\n";
	for (auto a : this->follow_tbl) {
		std::cout << a.first << ": ";
		for (auto b : a.second) {
			std::cout << b << ' ';
		}
		std::cout << '\n';
	}
	std::cout << "[... done]\n";
}
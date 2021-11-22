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
			for (Production p : this->grammar->getProductionsWithNonTerminalOnRHS(B)) {
				A = p.LHS;
				std::vector<std::string> gamma = this->grammar->splitRHSOnNonTerminal(p, B).second;
				if (gamma.size() > 0) {
					// compute first(gamma)
					std::string symbol = gamma[0]; // only first symbol is important
					for (auto a : this->first_tbl[symbol]) {
						if (this->grammar->getIsTerminal(a)) {
							this->follow_tbl[B] = Helper::union_custom(follow_tbl_previous[B], this->first_tbl[symbol]);
						}
						else if (this->grammar->getIsNonTerminal(a)) {
							this->follow_tbl[B] = Helper::union_custom(follow_tbl_previous[B], this->first_tbl[symbol]);
						}
						else if (this->grammar->getIsEpsilon(a)) {
							this->follow_tbl[B] = Helper::union_custom(this->follow_tbl[B], follow_tbl_previous[A]);
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
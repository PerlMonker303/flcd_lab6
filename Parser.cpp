#include "Parser.h"

Parser::Parser(Grammar* g, ParserOutput* po) {
	this->grammar = g;
	this->parserOutput = po;
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
			/*if (A == "expression") {
				std::cout << "dbg\n";
			}*/
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
				std::cout << new_set.size() << '\n';
				different = true;
				first_tbl[A] = Helper::union_custom(first_tbl[A], new_set);
			}
		}
		//this->displayFirstTable();
	} while (different);
	this->displayFirstTable();
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
	bool error;
	do {
		different = false;
		std::map<std::string, std::unordered_set<std::string>> follow_tbl_previous = this->follow_tbl;
		for (auto B : nonTerminals) {
			this->follow_tbl[B] = follow_tbl_previous[B];
			for (Production p : this->grammar->getProductionsWithNonTerminalOnRHS(B)) {
				A = p.LHS;
				std::vector<std::string> y = this->grammar->splitRHSOnNonTerminal(p, B).second;
				// compute first(y)
				std::unordered_set<std::string> first;
				if (y.size() > 0) {
					first = this->first_tbl[y[0]];	
					for (auto s : y) {
						first = Helper::concatenationOne(first, this->first_tbl[s], error);
					}
				}
				
				// if epsilon in first of y
				if (Helper::findInSet(first, "Epsilon") || y.size() == 0) {
					this->follow_tbl[B] = Helper::union_custom(this->follow_tbl[B], follow_tbl_previous[A]);
				}
				this->follow_tbl[B] = Helper::union_custom(this->follow_tbl[B], first, true);
			}
		}
		//this->displayFollowTable();
		// check different
		for (auto A : nonTerminals) {
			if (follow_tbl[A].size() != follow_tbl_previous[A].size()) {
				different = true;
				break;
			}
		}
	} while (different);
	this->displayFollowTable();
}

void Parser::constructLL1Table() {
	this->ll1_tbl.clear();
	std::vector<std::string> terminals = this->grammar->getTerminals();
	std::vector<std::string> nonTerminals = this->grammar->getNonTerminals();
	for (auto nt : nonTerminals) {
		ll1_tbl[nt] = std::vector<Tuple>{};
	}
	// for each production
	std::unordered_set<std::string> first;
	bool error = false;
	bool foundEpsilon;
	for (auto p : this->grammar->getProductions()) {
		first.clear();
		foundEpsilon = false;
		if (p.RHS.size() > 0) {
			if (p.RHS.size() == 1 && this->grammar->getIsEpsilon(p.RHS[0])) {
				foundEpsilon = true;
			}
			else {
				first = this->first_tbl[p.RHS[0]];
			}
		}
		if (!foundEpsilon) {
			for (auto symbol : p.RHS) {
				first = Helper::concatenationOne(first, this->first_tbl[symbol], error);
			}
		}
		else {
			first = this->follow_tbl[p.LHS];
		}

		// for each symbol in first, add to the table
		for (auto symbol : first) {
			Tuple tuple;
			tuple.RHS = p.RHS;
			tuple.nr = p.index;
			tuple.action = "push";
			tuple.column = symbol;
			this->ll1_tbl[p.LHS].push_back(tuple);
		}
	}

	// add pops
	for (auto terminal : terminals) {
		Tuple tuple;
		tuple.action = "pop";
		tuple.column = terminal;
		this->ll1_tbl[terminal] = std::vector<Tuple>{ tuple };
	}

	// add accept
	Tuple tuple;
	tuple.action = "acc";
	tuple.column = "$";
	this->ll1_tbl["$"] = std::vector<Tuple>{ tuple };
}

bool Parser::checkConflictsLL1Table() {
	for (auto r : this->ll1_tbl) {
		if (r.second.size() > 1) {
			std::unordered_set<std::string> seen_col;
			// store encountered columns, if encountered the same column return true (conflict)
			for (auto a : r.second) {
				if (seen_col.count(a.column) > 0) {
					return true;
				}
				seen_col.insert(a.column);
			}
		}
	}
	return false;
}

std::vector<int> Parser::checkSequence(std::vector<std::string> w) {
	std::vector<int> result;
	if (this->checkConflictsLL1Table()) {
		std::cout << "[Error: Conflict detected]\n";
		return result;
	}
	// construct the initial configuration
	Config config;
	config.alpha = std::stack<std::string>{ };
	config.alpha.push("$");
	for (int i = w.size() - 1; i >= 0; i--) {
		config.alpha.push(w[i]);
	}
	config.beta = std::stack<std::string>{ };
	config.beta.push("$");
	config.beta.push(this->grammar->getStartingSymbol());
	config.pi = std::vector<int>{ };

	while (true) {
		this->displayConfig(config);
		if (config.alpha.top() == "$" && config.beta.top() == "$") {
			std::cout << "[ACCEPT]\n";
			for (auto el : config.pi) {
				result.push_back(el);
				//result = result + std::string(1, '0' + el);
			}
			return result;
		}
		else if (this->grammar->getIsTerminal(config.beta.top())) {
			// try pop
			if (config.alpha.top() == config.beta.top()) {
				// pop
				std::cout << "[POP]\n";
				config.alpha.pop();
				config.beta.pop();
			}
			else {
				std::cout << "[ERROR POP]\n";
				//return "error_pop\n";
				return result;
			}
		}else if(this->grammar->getIsNonTerminal(config.beta.top())) {
			// try push
			Tuple entry = this->getEntryLL1Table(config.beta.top(), config.alpha.top());
			if (entry.action == "NA") {
				std::cout << "[ERROR PUSH]\n";
				return result;
				//return "error_push\n";
			}
			std::cout << "[PUSH]\n";
			config.beta.pop();
			if (!this->grammar->getIsEpsilon(entry.RHS[0])) {
				for (int i = entry.RHS.size() - 1; i >= 0; i--) {
					config.beta.push(entry.RHS[i]);
				}
			}
			config.pi.push_back(entry.nr);
		}
	}
	return result;
}

Tuple Parser::getEntryLL1Table(std::string row, std::string column) {
	for (auto r : this->ll1_tbl) {
		if (r.first == row) {
			for (auto s : r.second) {
				if (s.column == column) {
					return s;
				}
			}
			break;
		}
	}
	Tuple t;
	t.action = "NA";
	return t;
}

void Parser::constructParseTree(std::vector<int> sequence) {
	// add starting symbol and take its index
	int idx = this->parserOutput->addRow(this->grammar->getStartingSymbol(), 0, 0);
	int sequenceIndex = 0;
	// begin adding children
	this->addChildren(sequence, sequenceIndex, idx);

	this->parserOutput->displayTable();
	this->parserOutput->writeToFile();
}

void Parser::addChildren(std::vector<int>& sequence, int& sequenceIndex, int parentIndex) {
	int leftSibling = 0;
	// take the current production  using the sequence index
	Production production = this->grammar->getProductionByIndex(sequence[sequenceIndex]);
	sequenceIndex++;

	// store the indices of non-terminal children
	std::vector<int> childrenIndices;
	// parse RHS and add children
	for (auto child : production.RHS) {
		int rowIndex = this->parserOutput->addRow(child, parentIndex, leftSibling);
		leftSibling++;
		if (this->grammar->getIsNonTerminal(child)) {
			childrenIndices.push_back(rowIndex);
		}
	}

	// search for non-terminals and add children
	int childrenIdx = 0;
	for (auto child : production.RHS) {
		if (this->grammar->getIsNonTerminal(child)) {
			this->addChildren(sequence, sequenceIndex, childrenIndices[childrenIdx]);
			childrenIdx++;
		}
	}
}

std::vector<std::string> Parser::parsePIF(std::string fileName) {
	std::vector<std::string> result;
	std::ifstream file(fileName);
	std::string line;
	std::getline(file, line);
	while (!std::getline(file, line).eof()) {
		result.push_back(Helper::splitString(line, ' ')[0]);
	}
	file.close();
	return result;
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

void Parser::displayLL1Table() {
	std::cout << "[LL1 table ...]\n";
	std::vector<std::string> terminals = this->grammar->getTerminals();
	int idx;
	for (auto a : this->ll1_tbl) {
		std::cout << a.first << ": ";
		for (auto b : a.second) {
			std::cout << b.column << ": ";
			if (b.action == "push") {
				std::cout << '(';
				for (auto c : b.RHS) {
					std::cout << c;
				}
				std::cout << ',' << b.nr << ") ";
			}
			else {
				std::cout << b.action;
			}
		}
		std::cout << '\n';
	}
	std::cout << "[... done]\n";
}

void Parser::displayConfig(Config config) {
	std::stack<std::string> alpha = config.alpha;
	std::stack<std::string> beta = config.beta;

	std::cout << "(";
	while (!alpha.empty()) {
		std::cout << alpha.top();
		alpha.pop();
	}
	std::cout << " , ";
	while (!beta.empty()) {
		std::cout << beta.top();
		beta.pop();
	}
	std::cout << " , ";
	for (auto el : config.pi) {
		std::cout << el;
	}
	std::cout << ")\n";
	/*
	std::cout << "[\nalpha=";
	while (!alpha.empty()) {
		std::cout << alpha.top();
		alpha.pop();
	}
	std::cout << "\nbeta=";
	while (!beta.empty()) {
		std::cout << beta.top();
		beta.pop();
	}
	std::cout << "\npi=";
	for (auto el : config.pi) {
		std::cout << el;
	}
	std::cout << "\n]\n";
	*/
}

void Parser::displayAllConflicts() {
	std::cout << "[Displaying conflicts ...]\n";
	bool found = false;
	for (auto r : this->ll1_tbl) {
		if (r.second.size() > 1) {
			std::unordered_set<std::string> seen_col;
			Tuple tpl;
			for (auto a : r.second) {
				if (seen_col.count(a.column) > 0) {
					tpl = a;
					std::cout << "[CONFLICT: \n";
					std::cout << "(row,col) = (" << r.first << "," << tpl.column << ")\n";
					std::cout << "(alpha,i) = (";
					for (auto rhs : tpl.RHS) {
						std::cout << rhs;
					}
					std::cout << "," << tpl.nr << ")\n";
					std::cout << "]\n";
					found = true;
				}
				seen_col.insert(a.column);
			}
		}
	}
	if (!found) {
		std::cout << "No conflicts found\n";
	}
	std::cout << "[... done]\n";
}
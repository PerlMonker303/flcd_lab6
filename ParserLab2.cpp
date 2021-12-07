#include "ParserLab2.h"
#include "Helper.h"

ParserLab2::ParserLab2(std::string faPath) {
	this->faPath = faPath;
}

void ParserLab2::readFA() {
	std::ifstream file(this->faPath);

	std::string statesString = "";
	std::string alphabetString = "";
	std::string finalStatesString = "";

	std::getline(file, statesString);
	std::getline(file, alphabetString);

	this->states = Helper::splitString(statesString, ' ');
	this->alphabet = Helper::splitString(alphabetString, ' ');

	// read transitions
	int noTransitions = 0;
	std::string transitionString;
	std::vector<std::string> transitionVector;
	file >> noTransitions;
	for (int i = 0; i < noTransitions; i++) {
		std::getline(file, transitionString);
		transitionVector = Helper::splitString(transitionString, ' ');
		if (transitionVector.size() == 0) {
			i--;
			continue;
		}
		Transition tr;
		tr.s1 = transitionVector[0];
		tr.t = transitionVector[1];
		tr.s2 = transitionVector[2];
		// check if transition already in
		if (this->checkTransitionExists(tr)) {
			continue;
		}
		// check if valid symbols
		if (!Helper::findInVector(this->states, tr.s1)) {
			this->encounteredError = "[Error: '" + tr.s1 + "' is not a valid state (in transition " + std::to_string(i + 1) + ").]\n";
			return;
		}
		if (!Helper::findInVector(this->states, tr.s2)) {
			this->encounteredError = "[Error: '" + tr.s2 + "' is not a valid state (in transition " + std::to_string(i + 1) + ").]\n";
			return;
		}
		if (!Helper::findInVector(this->alphabet, tr.t)) {
			this->encounteredError = "[Error: '" + tr.t + "' does not belong to the alphabet (in transition " + std::to_string(i + 1) + ").]\n";
			return;
		}
		// check if deterministic
		if (this->move(tr.s1, tr.t) != "") {
			this->encounteredError = "[Error: Finite Automation not deterministic.]\n";
			return;
		}
		this->transitions.push_back(tr);
	}

	std::getline(file, this->initialState);
	if (!Helper::findInVector(this->states, this->initialState)) {
		this->encounteredError = "[Error: '" + this->initialState + "' is not a valid initial state.]\n";
		return;
	}

	std::getline(file, finalStatesString);
	this->finalStates = Helper::splitString(finalStatesString, ' ');
	// check if valid
	for (int i = 0; i < this->finalStates.size(); i++) {
		if (!Helper::findInVector(this->states, this->finalStates[i])) {
			this->encounteredError = "[Error: '" + this->finalStates[i] + "' is not a valid final state.]\n";
			return;
		}
	}
}

bool ParserLab2::verifySequence(std::string sequence, bool verbose) {
	if (verbose) { std::cout << "[Verifying sequence '" << sequence << "']\n"; }
	if (sequence == "") {
		if (Helper::findInVector(this->finalStates, this->initialState)) {
			return true;
		}
		return false;
	}
	std::string currentState = this->initialState;
	for (int i = 0; i < sequence.size(); i++) {
		if (verbose) { std::cout << "(" << currentState << ", " << sequence.substr(i) << ")"; }
		std::string nextState = this->move(currentState, std::string(1, sequence[i]));
		if (nextState == "") {
			if (verbose) { std::cout << '\n'; }
			return false;
		}
		if (verbose) { std::cout << "|-"; }
		currentState = nextState;
	}
	if (verbose) { std::cout << "(" << currentState << ", epsilon) "; }
	if (Helper::findInVector(this->finalStates, currentState)) {
		if (verbose) { std::cout << "where '" << currentState << "' is a final state\n"; }
		return true;
	}
	if (verbose) { std::cout << "where '" << currentState << "' is not a final state\n"; }
	return false;
}

std::string ParserLab2::move(std::string state, std::string symbol) {
	for (int i = 0; i < this->transitions.size(); i++) {
		if (this->transitions[i].s1 == state && this->transitions[i].t == symbol) {
			return this->transitions[i].s2;
		}
	}
	return "";
}

bool ParserLab2::checkTransitionExists(Transition t) {
	for (int i = 0; i < this->transitions.size(); i++) {
		if (transitions[i].s1 == t.s1 && transitions[i].t == t.t && transitions[i].s2 == t.s2) {
			return true;
		}
	}
	return false;
}

void ParserLab2::displayStates() {
	std::cout << "[States ...]\n";
	std::cout << "{ ";
	for (int i = 0; i < this->states.size(); i++) {
		std::cout << this->states[i] << ' ';
	}
	std::cout << "}\n";
	std::cout << "[... done]\n";
}

void ParserLab2::displayAlphabet() {
	std::cout << "[Alphabet ...]\n";
	std::cout << "{ ";
	for (int i = 0; i < this->alphabet.size(); i++) {
		std::cout << this->alphabet[i] << ' ';
	}
	std::cout << "}\n";
	std::cout << "[... done]\n";
}

void ParserLab2::displayTransitions() {
	std::cout << "[Transitions ...]\n";
	for (int i = 0; i < this->transitions.size(); i++) {
		std::cout << "delta(" << this->transitions[i].s1 << ", " << this->transitions[i].t << ") = " << this->transitions[i].s2 << '\n';
	}
	std::cout << "[... done]\n";
}

void ParserLab2::displayInitialState() {
	std::cout << "[Initial state ...]\n";
	std::cout << this->initialState << '\n';
	std::cout << "[... done]\n";
}

void ParserLab2::displayFinalStates() {
	std::cout << "[Final states ...]\n";
	std::cout << "{ ";
	for (int i = 0; i < this->finalStates.size(); i++) {
		std::cout << this->finalStates[i] << ' ';
	}
	std::cout << "}\n";
	std::cout << "[... done]\n";
}

// getters
std::string ParserLab2::getEncounteredError() {
	return this->encounteredError;
}
#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>

// delta(s1,t) = s2
struct Transition {
	std::string s1;
	std::string t;
	std::string s2;
};

class ParserLab2
{
private:
	std::string faPath;
	std::vector<std::string> states;
	std::vector<std::string> alphabet;
	std::vector<Transition> transitions;
	std::string initialState;
	std::vector<std::string> finalStates;
	std::string encounteredError = "";

public:
	ParserLab2(std::string faPath);
	void readFA();
	bool verifySequence(std::string sequence, bool verbose = false);
	std::string move(std::string state, std::string symbol);
	bool checkTransitionExists(Transition t);
	void displayStates();
	void displayAlphabet();
	void displayTransitions();
	void displayInitialState();
	void displayFinalStates();

	// getters
	std::string getEncounteredError();

	// M = (Q, sigma, delta, q0, F)
};
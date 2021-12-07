#pragma once
#include "SymbolTable.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include "ParserLab2.h"
#include <assert.h>

struct PifPair {
    int code; // code from codes vector
    std::pair<int, int> value; // <bucket, position in bucket>
};

class Compiler
{
private:
    std::string tokensPath;
    std::string syntaxPath;
    std::string programPath;
    SymbolTable* symbolTable = new SymbolTable(20);
    std::vector<std::string> codes; // code of token from position i is exactly i
    std::vector<PifPair> pif;
    std::vector<char> alphabet;
    std::vector<std::string> reservedWords;
    std::vector<std::string> separators;
    std::vector<std::string> operators;
    std::string encounteredError = "";
    ParserLab2* parserIdentifiers;
    ParserLab2* parserNumericalConstants;

public:
    Compiler(std::string tokensPath, std::string syntaxPath, std::string programPath);
    void readTokens();
    std::string readNextToken(std::ifstream& f, bool& isNewLine, int currentLine, char& reachedSeparator, std::string& reachedOperator, std::string& lookedAhead);
    void scan();
    void displayCodes();
    void displayPif();
    void displaySymbolTable();
    void logError(std::string error);
    void writeToFiles(std::string pifFileName, std::string stFileName, std::string correctnessFileName);
    void writeToFilesNoCodes(std::string pifFileName, std::string stFileName, std::string correctnessFileName);

    // getters
    int getCode(std::string token);
    std::vector<std::string> getReservedWords();
    std::vector<std::string> getSeparators();
    std::vector<std::string> getOperators();
    bool getIsReservedWord(std::string token);
    bool getIsSeparator(std::string token);
    bool getIsOperator(std::string token);
    bool getIsIdentifier(std::string token);
    bool getIsConstant(std::string token);
    bool getIsInAlphabet(char c);
    bool canBeNumber(std::string str);
    bool getHasError();
};


#include "Compiler.h"
#include "Helper.h"

Compiler::Compiler(std::string tokensPath, std::string syntaxPath, std::string programPath) {
    this->tokensPath = tokensPath;
    this->syntaxPath = syntaxPath;
    this->programPath = programPath;

    this->codes.push_back("id");
    this->codes.push_back("constant");

    this->readTokens();

    this->parserIdentifiers = new ParserLab2("FA_identifiers.in");
    this->parserIdentifiers->readFA();
    this->parserNumericalConstants = new ParserLab2("FA_constants.in");
    this->parserNumericalConstants->readFA();
}

void Compiler::readTokens() {
    // file structure
    // first line: reserved words
    // second line: operators
    // third line: separators
    std::ifstream file(this->tokensPath);
    std::string reservedWordsString = "";
    std::string operatorsString = "";
    std::string separatorsString = "";

    std::getline(file, reservedWordsString);
    std::getline(file, operatorsString);
    std::getline(file, separatorsString);

    this->reservedWords = Helper::splitString(reservedWordsString, ' ');
    this->operators = Helper::splitString(operatorsString, ' ');
    this->separators = Helper::splitString(separatorsString, ' ');

    // addings reserved words, operators and separators to codes vector
    std::cout << "[Codifying reserved words, operators and separators ...]\n";
    for (auto rw : this->reservedWords) {
        this->codes.push_back(rw);
    }
    for (auto op : this->operators) {
        this->codes.push_back(op);
    }
    for (auto sep : this->separators) {
        this->codes.push_back(sep);
    }
    std::cout << "[... done]\n";

    file.close();
}

std::string Compiler::readNextToken(std::ifstream& file, bool& isNewLine, int currentLine, char& reachedSeparator, std::string& reachedOperator, std::string& lookedAhead) {
    std::string token = "";
    char ch;
    bool isChar = false;
    bool foundEndCharQuote = false;
    bool isString = false;
    bool foundEndStringQuote = false;
    bool foundStartSquareBracket = false;
    bool foundEndSquareBracket = false;
    if (lookedAhead != "") {
        if (lookedAhead != " ") {
            token = lookedAhead;
        }
        lookedAhead = "";
    }
    while (file >> std::noskipws >> ch) {
        if (this->getIsSeparator(std::string(1, ch))) {
            reachedSeparator = ch;
            return token;
        }
        if (this->getIsOperator(std::string(1, ch))) {
            reachedOperator = ch;
            if (token != "") {
                //lookedAhead = ch;
                return token;
            }
            if (ch == '+' || ch == '-') {
                char prevChar = ch;
                file >> std::noskipws >> ch;
                lookedAhead = ch;
                if (ch == '0') { // invalid -0, +0
                    token += prevChar;
                    token += ch;
                    this->logError("[LEXICAL ERROR at line " + std::to_string(currentLine) + ": token `" + token + "` is invalid.]\n");
                }
                // for negative numbers
                else if (prevChar == '-' && (ch >= '1' && ch <= '9')) {
                    token += prevChar;
                    token += ch;
                    lookedAhead = ""; // reset
                    reachedOperator = 'x';
                    file >> std::noskipws >> ch;
                    //lookedAhead = ch;
                    while (ch >= '1' && ch <= '9') {
                        token += ch;
                        file >> std::noskipws >> ch;
                        lookedAhead = ch;
                    }
                    if (lookedAhead == "\n") {
                        isNewLine = true;
                        lookedAhead = "";
                    }
                }
            }
            if (ch == '<' || ch == '>') {
                char prevChar = ch;
                file >> std::noskipws >> ch;
                lookedAhead = ch;
                if (ch == '=') {
                    reachedOperator += ch;
                    lookedAhead = "";
                }
            }
            return token;
        }
        if (ch == '"') {
            if (isString == false) {
                isString = true;
            }
            else {
                foundEndStringQuote = true;
                token += ch;
                break;
            }
            token += ch;
            continue;
        }
        if (ch == '\'') {
            if (isChar == false) {
                isChar = true;
            }
            else {
                foundEndCharQuote = true;
                token += ch;
                break;
            }
            token += ch;
            continue;
        }
        if (ch != ' ' && ch != '\n') {
            token += ch;
            continue;
        }
        else {
            if (ch == '\n') {
                isNewLine = true;
            }
            if (isString && !foundEndStringQuote) {
                token += " ";
                continue;
            }
            if (isChar && token.length() > 3) {
                this->logError("[LEXICAL ERROR at line " + std::to_string(currentLine) + ": token `" + token + "` invalid character format.]\n");
            }
            else if (isChar && !foundEndCharQuote) {
                this->logError("[LEXICAL ERROR at line " + std::to_string(currentLine) + ": token `" + token + "` is missing a closing '.]\n");
            }
            return token;
        }
    }
    if (isString && !foundEndStringQuote) {
        this->logError("[LEXICAL ERROR at line " + std::to_string(currentLine) + ": token `" + token + "` is missing a closing \".]\n");

    }
    if (isChar && !foundEndCharQuote) {
        this->logError("[LEXICAL ERROR at line " + std::to_string(currentLine) + ": token `" + token + "` is missing a closing '.]\n");
    }
    return token;
}

void Compiler::scan() {
    std::cout << "[Scanning ...]\n";
    std::ifstream file(this->programPath);
    std::string token;
    std::string lookedAhead = "";
    bool isNewLine = false;
    int currentLine = 1;
    char reachedSeparator = 'x'; // default value, not a separator
    std::string reachedOperator = "x"; // default value, not an operator
    while (true) {
        if (reachedSeparator != 'x') {
            token = reachedSeparator;
            reachedSeparator = 'x';
        }
        else if (reachedOperator != "x") {
            token = reachedOperator;
            reachedOperator = "x";
        }
        else {
            token = this->readNextToken(file, isNewLine, currentLine, reachedSeparator, reachedOperator, lookedAhead);
        }

        if (this->getHasError()) {
            break;
        }

        if (file.eof()) {
            break;
        }

        if (token == "" || token == " ") {
            if (isNewLine) {
                currentLine++;
                isNewLine = false;
            }
            continue;
        }

        std::cout << token << " on line " << currentLine << '\n';

        if (this->getIsReservedWord(token) || this->getIsSeparator(token) || this->getIsOperator(token)) {
            PifPair pair;
            pair.code = this->getCode(token);
            pair.value = std::pair<int, int>(-1, -1);

            if (pair.code == -1) {
                this->logError("[INNER ERROR at line " + std::to_string(currentLine) + ": Token `" + token + "` is a reserved word, separator or operator and it could not have been found in the codes table.");
            }

            this->pif.push_back(pair);
        }
        else {
            // check if token is id or constant
            if (this->getIsIdentifier(token) || this->getIsConstant(token)) {
                // check if token is inside symbolTable
                std::pair<int, int> pos;
                if (this->symbolTable->exists(token)) {
                    // find its position
                    pos = this->symbolTable->getPosition(token);
                }
                else {
                    // insert it
                    pos = this->symbolTable->insert(token);
                }
                // add to pif
                PifPair pifPair;
                if (this->getIsConstant(token)) {
                    pifPair.code = 1;
                }
                else if (this->getIsIdentifier(token)) {
                    pifPair.code = 0;
                }
                else {
                    this->logError("[SYNTACTIC ERROR at line " + std::to_string(currentLine) + ": token `" + token + "` is not an identifier, nor a constant.]\n");
                }

                pifPair.value = pos;
                this->pif.push_back(pifPair);
            }
            else {
                this->logError("[LEXICAL ERROR at line " + std::to_string(currentLine) + ": token `" + token + "` could not be classified.]\n");
            }
        }

        if (isNewLine) {
            currentLine++;
            isNewLine = false;
        }
    }
    file.close();
    std::cout << "[... done]\n";
}

void Compiler::displayCodes() {
    std::cout << "[Codes table ...]\n";
    for (int i = 0; i < this->codes.size(); i++) {
        std::cout << i << "   ";
        if (i < 10) {
            std::cout << ' ';
        }
        std::cout << this->codes[i] << '\n';
    }
    std::cout << "[... done]\n";
}


void Compiler::displayPif() {
    std::cout << "[Pif table ...]\n";
    std::cout << "Code - (bucket, position in bucket) - Keyword/Character\n";
    for (int i = 0; i < this->pif.size(); i++) {
        std::cout << this->pif[i].code << "   ";
        if (this->pif[i].code < 10) {
            std::cout << ' ';
        }
        std::cout << '(' << this->pif[i].value.first << ',' << this->pif[i].value.second << ")   " << this->codes[this->pif[i].code] << '\n';
    }
    std::cout << "[... done]\n";
}

void Compiler::displaySymbolTable() {
    this->symbolTable->print();
}

void Compiler::logError(std::string error) {
    std::cout << error;
    this->encounteredError = error;
}

void Compiler::writeToFiles(std::string pifFileName, std::string stFileName, std::string correctnessFileName) {
    this->symbolTable->toFile(stFileName);

    std::ofstream file(pifFileName);
    file << "Code - (bucket, position in bucket) - Keyword/Character\n";
    for (int i = 0; i < this->pif.size(); i++) {
        file << this->pif[i].code << "   ";
        if (this->pif[i].code < 10) {
            file << ' ';
        }
        file << '(' << this->pif[i].value.first << ',' << this->pif[i].value.second << ")   " << this->codes[this->pif[i].code] << '\n';
    }

    std::ofstream file1(correctnessFileName);
    if (this->getHasError()) {
        file1 << this->encounteredError;
    }
    else {
        file1 << "Lexically correct";
    }

    file1.close();
    file.close();
}

void Compiler::writeToFilesNoCodes(std::string pifFileName, std::string stFileName, std::string correctnessFileName) {
    this->symbolTable->toFile(stFileName);

    std::ofstream file(pifFileName);
    file << "keyword bucket position_in_bucket\n";
    for (int i = 0; i < this->pif.size(); i++) {
        file << this->codes[this->pif[i].code] << ' ';
        file << this->pif[i].value.first << ' ';
        file << this->pif[i].value.second << '\n';
    }

    std::ofstream file1(correctnessFileName);
    if (this->getHasError()) {
        file1 << this->encounteredError;
    }
    else {
        file1 << "Lexically correct";
    }

    file1.close();
    file.close();
}

// getters
int Compiler::getCode(std::string token) {
    for (int i = 0; i < this->codes.size(); i++) {
        if (this->codes.at(i) == token) {
            return i;
        }
    }
    return -1;
}

std::vector<std::string> Compiler::getReservedWords() {
    return this->reservedWords;
}

std::vector<std::string> Compiler::getSeparators() {
    return this->separators;
}

std::vector<std::string> Compiler::getOperators() {
    return this->operators;
}

bool Compiler::getIsReservedWord(std::string token) {
    return Helper::findInVector(this->reservedWords, token);
}

bool Compiler::getIsSeparator(std::string token) {
    return Helper::findInVector(this->separators, token);
}

bool Compiler::getIsOperator(std::string token) {
    return Helper::findInVector(this->operators, token);
}

bool Compiler::getIsIdentifier(std::string token) {
    return this->parserIdentifiers->verifySequence(token);
}

bool Compiler::getIsConstant(std::string token) {
    if (token.empty()) {
        return false;
    }
    int lastPosition = token.size() - 1;
    // character
    if (token[0] == '\'') {
        if (token.size() != 3) {
            return false;
        }
        if (token[2] != '\'') {
            return false;
        }
        // checking for unknown characters
        for (char& c : token) {
            if (c != '\'') {
                if (!this->getIsInAlphabet(c)) {
                    return false;
                }
            }
        }
        return true;
    }
    // string
    else if (token[0] == '\"') {
        if (token[lastPosition] != '\"') {
            return false;
        }
        if (token.size() == 2) {
            return false;
        }
        // checking for unknown characters
        for (char& c : token) {
            if (c != ' ' && c != '\"') {
                if (!this->getIsInAlphabet(c)) {
                    return false;
                }
            }
        }
        return true;
    }
    // numerical constant
    else if (canBeNumber(token)) {
        return true;
    }
    return false;
}

bool Compiler::getIsInAlphabet(char c) {
    if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9')) {
        return true;
    }
    return false;
}

bool Compiler::canBeNumber(std::string str) {
    return this->parserNumericalConstants->verifySequence(str);
}

bool Compiler::getHasError() {
    return this->encounteredError != "";
}
#pragma once
#include <string>
#include <vector>
#include <iostream>
#include "Grammar.h"

struct Row {
	int index;
	std::string information;
	int parent;
	int leftSibling;
};

class ParserOutput
{
private:
	std::string fileName;
	std::vector<Row> rows;
	int currentIndex = 1;
public:
	ParserOutput(std::string fileName);
	int addRow(std::string information, int parent, int leftSibling); // return index of currently added row
	void displayTable();
	void writeToFile();
};


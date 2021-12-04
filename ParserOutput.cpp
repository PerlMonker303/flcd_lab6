#include "ParserOutput.h"

ParserOutput::ParserOutput(std::string fileName) {
	this->fileName = fileName;
}

int ParserOutput::addRow(std::string information, int parent, int leftSibling) {
	Row row;
	row.index = this->currentIndex;
	row.information = information;
	row.parent = parent;
	row.leftSibling = leftSibling;
	this->currentIndex++;
	this->rows.push_back(row);
	return this->currentIndex - 1;
}

void ParserOutput::displayTable() {
	std::cout << "[Table ...]\n";
	std::cout << "Id  Info  Parent  Left-Sibling\n";
	for (int i = 0; i < this->rows.size(); i++) {
		Row row = this->rows[i];
		std::cout << row.index << "  ";
		if (row.index <= 9) {
			std::cout << ' ';
		}
		std::cout << row.information << "  ";
		std::cout << row.parent << "  ";
		std::cout << row.leftSibling << '\n';
	}
	std::cout << "[... done]\n";
}

void ParserOutput::writeToFile() {
	std::ofstream f(this->fileName);
	f << "Id  Info  Parent  Left-Sibling\n";
	for (int i = 0; i < this->rows.size(); i++) {
		Row row = this->rows[i];
		f << row.index << "  ";
		if (row.index <= 9) {
			f << ' ';
		}
		f << row.information << "  ";
		f << row.parent << "  ";
		f << row.leftSibling << '\n';
	}
	f.close();
}
#pragma once
#include <iostream>
#include <vector>
#include <String>
#include <fstream>

struct Node {
	std::string value;
	Node* next;
};

class SymbolTable
{
private:
	int capacity;
	int n;
	Node** elems;

public:
	SymbolTable(int m);
	int getCapacity();
	std::pair<int, int> insert(std::string elem);
	std::pair<int, int> getPosition(std::string elem);
	int hash(std::string elem);
	bool exists(std::string elem);
	void resize();
	std::vector<std::string> getAllElements();
	void print();
	void toFile(std::string filename);
};

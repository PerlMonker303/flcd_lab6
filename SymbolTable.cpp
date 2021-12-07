#include "SymbolTable.h"

/*
	Constructor method
	Input: m - the capacity of the hash table
	Output: -
*/
SymbolTable::SymbolTable(int m) {
	this->capacity = m;
	this->n = 0;
	this->elems = new Node * [m];
	for (int i = 0; i < m; i++) {
		this->elems[i] = nullptr;
	}
}

/*
	Returns the capacity of the table
	Input: -
	Output: the capacity of the table as an integer value
*/
int SymbolTable::getCapacity() {
	return this->capacity;
}

/*
	Insert an element in the symbol table
	Input: elem - element of type string
	Output: - <-1,-1> if the element already exists
			- otherwise the position where the element was added, <int,int> = <bucket,position inside bucket>
*/
std::pair<int, int> SymbolTable::insert(std::string elem) {
	if (this->exists(elem)) {
		return std::pair<int, int>(-1, -1);
	}
	int pos = this->hash(elem);
	Node* n = new Node();
	n->next = nullptr;
	n->value = elem;
	int indexInBucket = 0;
	if (this->elems[pos]) {
		Node* currentNode = this->elems[pos];
		Node* previousNode = nullptr;
		while (currentNode) {
			indexInBucket++;
			previousNode = currentNode;
			currentNode = currentNode->next;
		}
		previousNode->next = n;
	}
	else {
		this->elems[pos] = n;
	}
	this->n++;
	if (this->n >= this->capacity) {
		this->resize();
	}
	return std::pair<int, int>(pos, indexInBucket);
}

/*
	Returns the position of a given element
	Input: elem - element of type string to be searched for
	Output: - <-1,-1> if the element could not be found
			- the position of the element, otherwise
*/
std::pair<int, int> SymbolTable::getPosition(std::string elem) {
	int key = this->hash(elem);
	Node* currentNode = this->elems[key];
	int idx = 0;
	while (currentNode) {
		if (currentNode->value == elem) {
			break;
		}
		idx++;
		currentNode = currentNode->next;
	}
	if (!currentNode) {
		return std::pair<int, int>(-1, -1);
	}
	return std::pair<int, int>(key, idx);
}

/*
	Toy hash function (sum of ASCII codes % capacity)
	Input: elem - element of type string
	Output: sum of ASCII codes of all characters from the input string modulo the capacity
*/
int SymbolTable::hash(std::string elem) {
	// compute ascii code sum
	int sum = 0;
	for (auto& ch : elem) {
		sum += (ch % this->capacity);
	}

	return sum % this->capacity;
}

/*
	Checks whether a given element exists in the table
	Input: elem - element of type string
	Output: - true,  if the element was found
			- false, otherwise
*/
bool SymbolTable::exists(std::string elem) {
	int key = this->hash(elem);
	Node* currentNode = this->elems[key];
	while (currentNode) {
		if (currentNode->value == elem) {
			return true;
		}
		currentNode = currentNode->next;
	}
	return false;
}

/*
	Resizes the symbol table by doubling its capacity and rehashing all elements
	Input: -
	Output: -
*/
void SymbolTable::resize() {
	std::vector<std::string> currentElements = this->getAllElements();
	delete[] this->elems;
	this->capacity *= 2;
	int oldN = this->n;
	this->elems = new Node * [this->capacity];
	for (int i = 0; i < this->capacity; i++) {
		this->elems[i] = nullptr;
	}
	this->n = 0;
	for (auto el : currentElements) {
		this->insert(el);
	}
}

/*
	Retrieves a vector containing all elements from the symbol table
	Input: -
	Output: vector of strings containing all elements, sorted by bucket and position inside the bucket
*/
std::vector<std::string> SymbolTable::getAllElements() {
	std::vector<std::string> elements;

	for (int i = 0; i < this->capacity; i++) {
		Node* current = this->elems[i];
		while (current) {
			elements.push_back(current->value);
			current = current->next;
		}
	}

	return elements;
}

/*
	Displays in the console the contents of the table
	Input: -
	Output: - (console output)
*/
void SymbolTable::print() {
	std::cout << "[Symbol Table ...]\n";
	for (int i = 0; i < this->capacity; i++) {
		std::cout << i << " ";
		if (i < 10) {
			std::cout << ' ';
		}
		std::cout << ": [";
		Node* current = this->elems[i];
		while (current) {
			std::cout << current->value;
			if (current->next) {
				std::cout << ' ';
			}
			current = current->next;
		}
		std::cout << "]\n";
	}

	std::cout << "[... done]\n";
}

void SymbolTable::toFile(std::string filename) {
	std::ofstream file(filename);
	file << "Symbol table based on hash-map with separate chaining conflict resolution\n\n";
	for (int i = 0; i < this->capacity; i++) {
		file << i << " ";
		if (i < 10) {
			file << ' ';
		}
		file << ": [";
		Node* current = this->elems[i];
		while (current) {
			file << current->value;
			if (current->next) {
				file << ' ';
			}
			current = current->next;
		}
		file << "]\n";
	}
	file.close();
}
#pragma once
#include <vector>
#include <iostream>
#include "../../Lexer/src/Lex.h"

class Symbol {
public:
	std::string name;

	Symbol() {};

	Symbol(std::string n) {
		name = n;
	}
};

class SymTable {
private:
	std::vector<std::pair<std::string, Symbol>> dict;
public:
	SymTable() {};

	void add(std::string n) {
		dict.push_back(std::make_pair(n, Symbol(n)));
	}

	Symbol get(std::string n) {
		std::vector<std::pair<std::string, Symbol>>::iterator it = std::find_if(dict.begin(), dict.end(),
			[&n](const std::pair<std::string, Symbol>& element) { return element.first == n; });

		if (it != dict.end())
			return it->second;
		else
			return {};
	}
};
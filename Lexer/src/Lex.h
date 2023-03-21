#pragma once
#include <iostream>
#include <regex>


class Lexeme {
public:
	enum class Type {
		INT, REAL, STRING, ID, DLMTR, OP, KWORD, TYPE_SPEC, SELECT, FUN_NAME, PROG_NAME
	};

	const std::string TypeToString[8] = { "Int", "Real", "String", "Id", "Dlmtr", "Op", "KWord" , "Type_spec" };

	int lex_x;
	int lex_y;
	Type lex_type;
	std::string val;
	std::string src_val = "";

	Lexeme& operator=(const Lexeme& other) {
		this->lex_x = other.lex_x;
		this->lex_y = other.lex_y;
		this->lex_type = other.lex_type;
		this->val = other.val;
		this->src_val = other.src_val;
		return *this;
	}

	Lexeme() {};

	Lexeme(std::string val) {
		this->val = val;
	}

	Lexeme(int _lex_x, int _lex_y, Type _lex_type, std::string _val) {
		this->lex_x = _lex_x;
		this->lex_y = _lex_y;
		this->lex_type = _lex_type;
		this->val = _val;
	}

	Lexeme(int _lex_x, int _lex_y, Type _lex_type, std::string _val, std::string _src_val) {
		this->lex_x = _lex_x;
		this->lex_y = _lex_y;
		this->lex_type = _lex_type;
		this->val = _val;
		this->src_val = _src_val;
	}
};
#pragma once
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include "Lex.h"
#include <map>

class Lexer {
private:

	std::map<std::string, Lexeme::Type> DelimiterToType = {
		{"(", Lexeme::Type::DLMTR}, {")", Lexeme::Type::DLMTR},
		{"{", Lexeme::Type::DLMTR}, {"}", Lexeme::Type::DLMTR}, {"[", Lexeme::Type::DLMTR}, {"]", Lexeme::Type::DLMTR},
		{"'", Lexeme::Type::DLMTR}, {".", Lexeme::Type::DLMTR}, {";", Lexeme::Type::DLMTR}, {",", Lexeme::Type::DLMTR},
		{":", Lexeme::Type::DLMTR}
	};
	std::map<std::string, Lexeme::Type> OperatorToType = {
		{"+", Lexeme::Type::OP}, {"-", Lexeme::Type::OP}, {"*", Lexeme::Type::OP},
		{"/", Lexeme::Type::OP}, {"=", Lexeme::Type::OP}, {"<>", Lexeme::Type::OP},
		{"<", Lexeme::Type::OP}, {"<=", Lexeme::Type::OP}, {":=", Lexeme::Type::OP},
		{">", Lexeme::Type::OP}, {">=", Lexeme::Type::OP}
	};
	std::map<std::string, Lexeme::Type> WordToType = {
		{"begin", Lexeme::Type::KWORD}, {"end", Lexeme::Type::KWORD}, {"return", Lexeme::Type::KWORD},
		{"program", Lexeme::Type::KWORD}, {"var", Lexeme::Type::KWORD}, {"const", Lexeme::Type::KWORD}, {"while", Lexeme::Type::KWORD},
		{"do", Lexeme::Type::KWORD}, {"for", Lexeme::Type::KWORD}, {"range", Lexeme::Type::KWORD}, {"if", Lexeme::Type::KWORD}, {"then", Lexeme::Type::KWORD},
		{"else", Lexeme::Type::KWORD}, {"and", Lexeme::Type::OP}, {"not", Lexeme::Type::OP}, {"or", Lexeme::Type::OP}, {"array", Lexeme::Type::KWORD},
		{"function", Lexeme::Type::KWORD}, {"procedure", Lexeme::Type::KWORD}, {"integer", Lexeme::Type::TYPE_SPEC},
		{"double", Lexeme::Type::TYPE_SPEC}, {"string", Lexeme::Type::TYPE_SPEC}, {"to", Lexeme::Type::KWORD}, {"downto", Lexeme::Type::KWORD}
	};

	std::vector<char> HexadecimalChars = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
	std::vector<char> DecimalChars = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
	std::vector<char> OctalChars = { '0', '1', '2', '3', '4', '5', '6', '7' };
	std::vector<char> BinaryChars = { '0', '1' };

	std::map<char, int> CharsToInt = {
		{'0', 0}, {'1', 1}, {'2', 2}, {'3', 3}, {'4', 4}, {'5', 5}, {'6', 6}, {'7', 7},
		{'8', 8},  {'9', 9}, {'a', 10}, {'b', 11},  {'c', 12},  {'d', 13},  {'e', 14},  {'f', 15}
	};
	std::vector<char> current_system = DecimalChars;
	int current_base = 10;
	long long num_buf = 0, double_counter = 1;
	double double_buf = 0;

	enum States { S, NUM, DOUBLE, STR, SYMBOL, ID, ER, MULTI_COM };

	std::vector<Lexeme> lexems;
	
	std::string src_text = "";
	std::string buffer = "";

	int lex_x = 0;
	int lex_y = 1;
	int skipped_el = 0;
	int i = 0;
	
	int state = S;

public:
	Lexer(std::string filepath) {
		std::ifstream fin(filepath);
		if (fin) {
			std::stringstream str;
			str << fin.rdbuf();
			src_text = str.str();
			src_text += " ";
			fin.close();
		}
	}

	std::vector<Lexeme> getLexems() {
		while (i < src_text.size()) {
			switch (state) {
			case S: {
				lex_x = i + 1;
			
				if (src_text[i] == ' ' || src_text[i] == '\t' || src_text[i] == '\0' || src_text[i] == '\r')
					i++;
				else if (src_text[i] == '\n') {
					lex_y++;
					i++;
					skipped_el = i;
				}
				else if (isalpha(src_text[i])) {
					buffer = src_text[i];
					state = ID;
					i++;
				}
				else if (isdigit(src_text[i])) {
					buffer = src_text[i];
					state = NUM;
					current_system = DecimalChars;
					current_base = 10;
					num_buf = (int)(src_text[i] - '0');
					i++;
				}
				else if (src_text[i] == '$') {
					state = NUM;
					buffer = src_text[i];
					current_system = HexadecimalChars;
					current_base = 16;
					num_buf = 0;
					i++;
				}
				else if (src_text[i] == '&') {
					state = NUM;
					buffer = src_text[i];
					current_system = OctalChars;
					current_base = 8;
					num_buf = 0;
					i++;
				}
				else if (src_text[i] == '%') {
					state = NUM;
					buffer = src_text[i];
					current_system = BinaryChars;
					current_base = 2;
					num_buf = 0;
					i++;
				}
				else if (src_text[i] == '{') {
					state = MULTI_COM;
					i++;
				}
				else if (src_text[i] == '\'') {
					state = STR;
					i++;
					buffer = "";
				}
				else {
					state = SYMBOL;
				}
				break;
			}
			case ID: {
				if (isalnum(src_text[i]) || src_text[i] == '_') {
					buffer += src_text[i];
					i++;
				}
				else {
					state = S;
					std::map<std::string, Lexeme::Type>::iterator it = WordToType.find(buffer);
					if (it != WordToType.end()) {
						lexems.push_back({ lex_x - skipped_el, lex_y, it->second, buffer });
					}
					else {
						lexems.push_back({ lex_x - skipped_el, lex_y, Lexeme::Type::ID, buffer });
					}
				}
				break;
			}

			case NUM: {
				//std::string sss(1, src_text[i]);
				if (isdigit(src_text[i])) {
					num_buf = num_buf * current_base + CharsToInt[src_text[i]];
					buffer += src_text[i];
					if (num_buf > INT_MAX && !(buffer == "2147483648" && lexems[lexems.size() - 1].val == "-")) {
						state = ER;
					}
					i++;
				}
				else if (src_text[i] == '.') {
					if (current_base != 10) {
						state = ER;
						break;
					}
					try {
						if (src_text[i + 1] == '.') {
							lexems.push_back({ lex_x - skipped_el, lex_y, Lexeme::Type::INT, std::to_string(num_buf), buffer });
							state = S;
							break;
						}
					}
					catch (std::exception) {}
					double_buf = num_buf;
					i++;
					state = DOUBLE;
					double_counter = 10;
				}
				else {
					lexems.push_back({ lex_x - skipped_el, lex_y, Lexeme::Type::INT, std::to_string(num_buf), buffer });
					state = S;
				}
				break;
			}

			case DOUBLE: {
				//std::string sss(1, src_text[i]);
				if (isdigit(src_text[i])) {
					double_buf = double_buf + (double)(src_text[i] - '0') / double_counter;
					double_counter *= 10;
					i++;
				}
				else {
					state = S;
					lexems.push_back({ lex_x - skipped_el, lex_y, Lexeme::Type::REAL, std::to_string(double_buf) });
				}
				break;
			}

			case STR: {
				while (src_text[i] != '\'') {
					buffer += src_text[i];
					i++;
				}
				lexems.push_back({ lex_x - skipped_el, lex_y, Lexeme::Type::STRING, buffer });
				i++;
				state = S;
				break;
			}

			case SYMBOL: {
				state = S;
				buffer = src_text[i];
				std::string double_operator_buf = buffer;
				i++;
				double_operator_buf += src_text[i];
				std::map<std::string, Lexeme::Type>::iterator it = OperatorToType.find(double_operator_buf);
				if (it != OperatorToType.end()) {
					buffer += src_text[i];
					i++;
					lexems.push_back({ lex_x - skipped_el, lex_y, it->second, double_operator_buf });
					break;
				}
				else {
					if ((it = OperatorToType.find(buffer)) != OperatorToType.end())
						lexems.push_back({ lex_x - skipped_el, lex_y, it->second, buffer });
					else if ((it = DelimiterToType.find(buffer)) != DelimiterToType.end())
						lexems.push_back({ lex_x - skipped_el, lex_y, it->second, buffer });
					else
						state = ER;
					break;
				}
			}

			case MULTI_COM: {
				if (src_text[i] == '}')
					state = S;
				if (src_text[i] == '\n')
					lex_y++;
				i++;
				break;
			}
			case ER: {
				std::string error_string = "Lexer error at symbol " + std::to_string(lex_x-skipped_el) + " Line: " + std::to_string(lex_y) + " buf: " + buffer;
				throw std::runtime_error(error_string);
				exit(1);
			}
			}
		}
		return lexems;
	}
};
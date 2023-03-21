#pragma once
#include <vector>
#include <iostream>
#include <fstream>
#include "../../Lexer/src/Lex.h"

class SParser {
private:
	std::vector<Lexeme> lexes;
	int token = 0;

	class BinOp {
	public:
		std::string val;
		BinOp* left = nullptr;
		BinOp* right = nullptr;
	};

	BinOp* ParseExpression() {
		BinOp* left_node = ParseTerm();
		if (left_node != nullptr && token < lexes.size() - 1) {
			while (lexes[token].val == "+" || lexes[token].val == "-") {
				BinOp* parent_node = new BinOp();
				parent_node->val = lexes[token].val;
				parent_node->left = left_node;
				token++;
				BinOp* right_node = ParseTerm();
				parent_node->right = right_node;
				left_node = parent_node;
				if (token >= lexes.size() - 1) break;
			}
			return left_node;
		}
		else
			return left_node;
	}

	BinOp* ParseTerm() {
		BinOp* left_node = ParseFactor();
		if (left_node != nullptr && token < lexes.size() - 1) {
			while (lexes[token].val == "*" || lexes[token].val == "/") {
				BinOp* parent_node = new BinOp();
				parent_node->left = left_node;
				parent_node->val = lexes[token].val;
				token++;
				BinOp* right_node = ParseFactor();
				parent_node->right = right_node;
				left_node = parent_node;
				if (token >= lexes.size() - 1) break;
			}
			return left_node;
		}
		else
			return left_node;
	}

	BinOp* ParseFactor() {
		if (lexes[token].lex_type == Lexeme::Type::INT || lexes[token].lex_type == Lexeme::Type::REAL) {
			BinOp* node = new BinOp();
			node->val = lexes[token].val;
			token++;
			return node;
		}
		if (lexes[token].lex_type == Lexeme::Type::ID) {
			BinOp* node = new BinOp();
			node->val = lexes[token].val;
			token++;
			return node;
		}
		if (lexes[token].val == "(") {
			token++;
			BinOp* node = ParseExpression();
			if (lexes[token].val != ")") {
				throw std::exception("No right bracket");
				return nullptr;
			}
			token++;
			return node;
		}
		else {
			throw std::exception("Factor exception");
			return nullptr;
		};
	}
public:
	SParser(std::vector<Lexeme> lexems) {
		lexes = lexems;
	}

	void printBinTree(const std::string& prefix, const BinOp* node, bool isLeft, std::string filepath)
	{
		std::ofstream fout;
		fout.open(filepath, std::ios::app);
		if (node != nullptr)
		{
			fout << prefix;
			fout << "|__";

			fout << node->val << std::endl;
			
			printBinTree(prefix + (isLeft ? "|   " : "    "), node->left, true, filepath);
			printBinTree(prefix + (isLeft ? "|   " : "    "), node->right, false, filepath);
		}
		fout.close();
	}

	void printBinTree(std::string filepath)
	{
		printBinTree("", ParseExpression(), false, filepath);
	}

};
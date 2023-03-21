#pragma once
#include <iostream>
#include <vector>
#include "../../Lexer/src/Lex.h"

class Node {
public:
	Lexeme token;
	Node(Lexeme lex) {
		token = lex;
	}

	virtual void print(const std::string& prefix, std::string filepath) = 0;
};

class BinOpNode : public Node {
public:
	std::unique_ptr<Node> leftNode;
	std::unique_ptr<Node> rightNode;

	BinOpNode(Lexeme lex, std::unique_ptr<Node> _left, std::unique_ptr<Node> _right) : Node(lex) {
		leftNode = std::move(_left);
		rightNode = std::move(_right);
	}

	void print(const std::string& prefix, std::string filepath) {
		std::ofstream fout;
		fout.open(filepath, std::ios::app);
		fout << prefix;
		fout << "|__ ";
		fout << token.val << std::endl;

		leftNode->print(prefix + "|   ", filepath);
		rightNode->print(prefix + "|   ", filepath);
		fout.close();
	}
};

class NoOpNode : public Node {
public:
	NoOpNode(Lexeme lex) : Node(lex) {};

	void print(const std::string& prefix, std::string filepath) {}
};

class UnOpNode : public Node {
public:
	std::unique_ptr<Node> child;

	UnOpNode(Lexeme lex, std::unique_ptr<Node> d) : Node(lex) {
		child = std::move(d);
	}

	void print(const std::string& prefix, std::string filepath) {
		std::ofstream fout;
		fout.open(filepath, std::ios::app);
		fout << prefix;
		fout << "|__ ";
		fout << token.val << std::endl;

		child->print(prefix + "|   ", filepath);
		fout.close();
	}
};

class NumberNode : public Node {
public:
	NumberNode(Lexeme lex) : Node(lex) {};

	void print(const std::string& prefix, std::string filepath) {
		std::ofstream fout;
		fout.open(filepath, std::ios::app);
		fout << prefix;
		fout << "|__ ";
		fout << token.val << std::endl;
		fout.close();
	}
};

class BlockNode : public Node {
public:
	std::vector<std::unique_ptr<Node>> consts;
	std::vector<std::unique_ptr<Node>> declarations;
	std::unique_ptr<Node> compound;

	BlockNode(Lexeme lex, std::vector<std::unique_ptr<Node>> con, std::vector<std::unique_ptr<Node>> decl, std::unique_ptr<Node> com) : Node(lex) {
		consts = std::move(con);
		declarations = std::move(decl);
		compound = std::move(com);
	}

	void print(const std::string& prefix, std::string filepath) {
		std::ofstream fout;
		fout.open(filepath, std::ios::app);
		fout << prefix;
		fout << "|__ ";
		fout << token.val << std::endl;

		for (auto& par : consts) {
			par->print(prefix + "|   ", filepath);
		}

		for (auto& par : declarations) {
			par->print(prefix + "|   ", filepath);
		}

		compound->print(prefix + "|   ", filepath);
		fout.close();
	}
};

class WhileNode : public Node {
public:
	std::unique_ptr<Node> condition;
	std::unique_ptr<Node> body;

	WhileNode(Lexeme lex, std::unique_ptr<Node> c, std::unique_ptr<Node> b) : Node(lex) {
		condition = std::move(c);
		body = std::move(b);
	}

	void print(const std::string& prefix, std::string filepath) {
		std::ofstream fout;
		fout.open(filepath, std::ios::app);
		fout << prefix;
		fout << "|__ ";
		fout << token.val << std::endl;

		condition->print(prefix + "|   ", filepath);
		body->print(prefix + "|   ", filepath);
		fout.close();
	}
};

class ProgramNode : public Node {
public:
	std::vector<std::unique_ptr<Node>> functions;
	std::unique_ptr<Node> block;

	ProgramNode(Lexeme lex, std::vector<std::unique_ptr<Node>> func, std::unique_ptr<Node> bl) : Node(lex) {
		functions = std::move(func);
		block = std::move(bl);
	}

	void print(const std::string& prefix, std::string filepath) {
		std::ofstream fout;
		fout.open(filepath, std::ios::app);
		fout << token.val << std::endl;

		if (functions.size()) {
			fout << "|__ " << "FUNCTIONS" << std::endl;
		}
		for (auto& func : functions) {
			func->print("|   ", filepath);
		}

		block->print("", filepath);
		fout.close();
	}
};

class FunctionNode : public Node {
public:
	std::vector<std::unique_ptr<Node>> params;
	std::unique_ptr<Node> returnType;
	std::unique_ptr<Node> body;

	FunctionNode(Lexeme lex, std::vector<std::unique_ptr<Node>> pars, std::unique_ptr<Node> reT, std::unique_ptr<Node> bd) : Node(lex) {
		params = std::move(pars);
		returnType = std::move(reT);
		body = std::move(bd);
	}

	void print(const std::string& prefix, std::string filepath) {
		std::ofstream fout;
		fout.open(filepath, std::ios::app);
		fout << prefix;
		fout << "|__ ";
		fout << token.val << std::endl;

		returnType->print(prefix + "|   ", filepath);
		body->print(prefix + "|   ", filepath);

		for (auto& par : params) {
			par->print(prefix + "|   ", filepath);
		}
	}
};

class VarDeclNode : public Node {
public:
	std::unique_ptr<Node> var;
	std::unique_ptr<Node> type;

	VarDeclNode(Lexeme lex, std::unique_ptr<Node> v, std::unique_ptr<Node> t) : Node(lex) {
		var = std::move(v);
		type = std::move(t);
	}

	void print(const std::string& prefix, std::string filepath) {
		std::ofstream fout;
		fout.open(filepath, std::ios::app);
		fout << prefix;
		fout << "|__ ";
		fout << token.val << std::endl;

		var->print(prefix + "|   ", filepath);
		type->print(prefix + "|   ", filepath);
		fout.close();
	}
};

class TypeSpecNode : public Node {
public:
	TypeSpecNode(Lexeme lex) : Node(lex) {};

	void print(const std::string& prefix, std::string filepath) {
		std::ofstream fout;
		fout.open(filepath, std::ios::app);
		fout << prefix;
		fout << "|__ ";
		fout << token.val << std::endl;
		fout.close();
	}
};

class ArrSpecNode : public Node {
public:
	Lexeme lHandTok;
	Lexeme rHandTok;
	std::unique_ptr<Node> subType;

	ArrSpecNode(Lexeme lex, Lexeme l, Lexeme r, std::unique_ptr<Node> s) : Node(lex) {
		lHandTok = l;
		rHandTok = r;
		subType = std::move(s);
	}

	void print(const std::string& prefix, std::string filepath) {
		std::ofstream fout;
		fout.open(filepath, std::ios::app);
		fout << prefix;
		fout << "|__ ";
		fout << token.val << std::endl;

		fout << prefix + "|__ " << lHandTok.val << std::endl;
		fout << prefix + "|__ " << rHandTok.val << std::endl;
		subType->print(prefix, filepath);
		fout.close();
	}
};

class ConstNode : public Node {
public:
	std::unique_ptr<Node> constName;
	std::unique_ptr<Node> constValue;

	ConstNode(Lexeme lex, std::unique_ptr<Node> name, std::unique_ptr<Node> value) : Node(lex) {
		constName = std::move(name);
		constValue = std::move(value);
	}

	void print(const std::string& prefix, std::string filepath) {
		std::ofstream fout;
		fout.open(filepath, std::ios::app);
		fout << prefix;
		fout << "|__ ";
		fout << token.val << std::endl;

		constName->print(prefix + "|   ", filepath);
		constValue->print(prefix + "|   ", filepath);
		fout.close();
	}
};

class CompoundNode : public Node {
public:
	std::vector<std::unique_ptr<Node>> children;

	CompoundNode(Lexeme lex) : Node(lex) {};

	CompoundNode(Lexeme lex, std::vector<std::unique_ptr<Node>> ch) : Node(lex) {
		children = std::move(ch);
	};

	void print(const std::string& prefix, std::string filepath) {
		std::ofstream fout;
		fout.open(filepath, std::ios::app);
		fout << prefix;
		fout << "|__ ";
		fout << token.val << std::endl;

		for (auto& ch : children) {
			ch->print(prefix + "|   ", filepath);
		}
		fout.close();
	}
};

class StringNode : public Node {
public:
	StringNode(Lexeme lex) : Node(lex) {};

	void print(const std::string& prefix, std::string filepath) {
		std::ofstream fout;
		fout.open(filepath, std::ios::app);
		fout << prefix;
		fout << "|__ ";
		fout << token.val << std::endl;
		fout.close();
	}
};

class AssignNode : public Node {
public:
	std::unique_ptr<Node> var;
	std::unique_ptr<Node> val;

	AssignNode(Lexeme lex, std::unique_ptr<Node> vr, std::unique_ptr<Node> vl) : Node(lex) {
		var = std::move(vr);
		val = std::move(vl);
	}

	void print(const std::string& prefix, std::string filepath) {
		std::ofstream fout;
		fout.open(filepath, std::ios::app);
		fout << prefix;
		fout << "|__ ";
		fout << token.val << std::endl;

		var->print(prefix + "|   ", filepath);
		val->print(prefix + "|   ", filepath);
		fout.close();
	}
};

class VarNode : public Node {
public:
	VarNode(Lexeme lex) : Node(lex) {};

	void print(const std::string& prefix, std::string filepath) {
		std::ofstream fout;
		fout.open(filepath, std::ios::app);
		fout << prefix;
		fout << "|__ ";
		fout << token.val << std::endl;
		fout.close();
	}
};

class SelectNode : public Node {
public:
	std::unique_ptr<Node> from;
	std::unique_ptr<Node> index;

	SelectNode(Lexeme lex, std::unique_ptr<Node> f, std::unique_ptr<Node> i) : Node(lex) {
		from = std::move(f);
		index = std::move(i);
	}

	void print(const std::string& prefix, std::string filepath) {
		std::ofstream fout;
		fout.open(filepath, std::ios::app);
		fout << prefix;
		fout << "|__ ";
		fout << token.val << std::endl;

		from->print(prefix + "|   ", filepath);
		index->print(prefix + "|   ", filepath);
		fout.close();
	}
};

class CallFuncNode : public Node {
public:
	std::vector<std::unique_ptr<Node>> params;

	CallFuncNode(Lexeme lex, std::vector<std::unique_ptr<Node>> pars) : Node(lex) {
		params = std::move(pars);
	}

	void print(const std::string& prefix, std::string filepath) {
		std::ofstream fout;
		fout.open(filepath, std::ios::app);
		fout << prefix;
		fout << "|__ ";
		fout << token.val << std::endl;

		for (auto& p : params) {
			p->print(prefix + "|   ", filepath);
		}
		fout.close();
	}
};

class ReturnNode : public Node {
public:
	std::unique_ptr<Node> toReturn;

	ReturnNode(Lexeme lex, std::unique_ptr<Node> to) : Node(lex) {
		toReturn = std::move(to);
	}

	void print(const std::string& prefix, std::string filepath) {
		std::ofstream fout;
		fout.open(filepath, std::ios::app);
		fout << prefix;
		fout << "|__ ";
		fout << token.val << std::endl;

		toReturn->print(prefix + "|   ", filepath);
		fout.close();
	}
};

class IfNode : public Node {
public:
	std::unique_ptr<Node> condition;
	std::unique_ptr<Node> body;
	std::unique_ptr<Node> elseBody;

	IfNode(Lexeme lex, std::unique_ptr<Node> c, std::unique_ptr<Node> b, std::unique_ptr<Node> e) : Node(lex) {
		condition = std::move(c);
		body = std::move(b);
		elseBody = std::move(e);
	}

	void print(const std::string& prefix, std::string filepath) {
		std::ofstream fout;
		fout.open(filepath, std::ios::app);
		fout << prefix;
		fout << "|__ ";
		fout << token.val << std::endl;

		condition->print(prefix + "|   ", filepath);
		body->print(prefix + "|   ", filepath);
		elseBody->print(prefix + "|   ", filepath);
		fout.close();
	}
};



class ForNode : public Node {
public:
	std::unique_ptr<Node> iterStatement;
	std::unique_ptr<Node> body;

	ForNode(Lexeme lex, std::unique_ptr<Node> i, std::unique_ptr<Node> b) : Node(lex) {
		iterStatement = std::move(i);
		body = std::move(b);
	}

	void print(const std::string& prefix, std::string filepath) {
		std::ofstream fout;
		fout.open(filepath, std::ios::app);
		fout << prefix;
		fout << "|__ ";
		fout << token.val << std::endl;

		iterStatement->print(prefix + "|   ", filepath);
		body->print(prefix + "|   ", filepath);
		fout.close();
	}
};

class IterationNode : public Node {
public:
	std::unique_ptr<Node> assign;
	std::unique_ptr<Node> condition;
	std::unique_ptr<Node> postAction;

	IterationNode(Lexeme lex, std::unique_ptr<Node> a, std::unique_ptr<Node> c, std::unique_ptr<Node> p) : Node(lex) {
		assign = std::move(a);
		condition = std::move(c);
		postAction = std::move(p);
	}

	void print(const std::string& prefix, std::string filepath) {
		std::ofstream fout;
		fout.open(filepath, std::ios::app);
		fout << prefix;
		fout << "|__ ";
		fout << token.val << std::endl;

		condition->print(prefix + "|   ", filepath);
		postAction->print(prefix + "|   ", filepath);
		fout.close();
	}
};
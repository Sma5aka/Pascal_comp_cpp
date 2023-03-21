#pragma once
#include <vector>
#include <iostream>
#include <fstream>
#include <map>
#include "Node.h"
#include "../../Lexer/src/Lex.h"
#include "ASTFactory.h"

class Parser {
private:
	std::vector<Lexeme> lexems;
	unsigned int tokenId = 0;
public:
	Parser(std::vector<Lexeme>& l) {
		lexems = l;
	}

	Lexeme& getCurLex() {
		return lexems[tokenId];
	}

	void eat(Lexeme::Type type) {
		if (getCurLex().lex_type == type) {
			tokenId++;
		}
		else {
			throw std::runtime_error("Syntax error! Expected " + getCurLex().TypeToString[(int)type] + " at " +
				std::to_string(getCurLex().lex_y) + ":" + std::to_string(getCurLex().lex_x));
		}
	}

	void eat(std::string word) {
		if (getCurLex().val == word) {
			tokenId++;
		}
		else {
			throw std::runtime_error("Syntax error! Expected " + word + " at " +
				std::to_string(getCurLex().lex_y) + ":" + std::to_string(getCurLex().lex_x));
		}
	}

	Lexeme& lookForward() {
		if (tokenId + 1 >= lexems.size())
			throw std::runtime_error("Error! Can't look forward - end of lexems");

		return lexems[tokenId + 1];
	}

	std::unique_ptr<Node> analyze() {
		std::unique_ptr<Node> root = std::move(Program());
		return std::move(root);
	}

	std::unique_ptr<Node> Program() {
		Lexeme program_name;
		if (getCurLex().val == "program") {
			eat("program");
			program_name = getCurLex();
			eat(Lexeme::Type::ID);
			eat(";");
		}
		else {
			program_name = { "UNKNOWN_NAME" };
		}

		std::vector<std::unique_ptr<Node>> functions;
		while (getCurLex().val == "procedure" || getCurLex().val == "function") {
			functions.emplace_back(std::move(FuncDef()));
			eat(";");
		}

		std::unique_ptr<Node> block = std::move(Block());
		eat(".");
		program_name.lex_type = Lexeme::Type::PROG_NAME;
		return ASTFactory::createAST(program_name, std::move(functions), std::move(block));
	}

	std::unique_ptr<Node> FuncDef() {
		Lexeme funcLex = getCurLex();
		if (funcLex.val == "procedure")
			eat("procedure");
		else
			eat("function");

		Lexeme name = getCurLex();
		eat(Lexeme::Type::ID);
		eat("(");

		std::vector<std::unique_ptr<Node>> params;
		while (getCurLex().val != ")") {
			std::vector<std::unique_ptr<Node>> sup = std::move(VarDecl());
			for (auto& ptr : sup)
				params.emplace_back(std::move(ptr));
			if (getCurLex().val == ";")
				eat(";");
		}
		eat(")");

		std::unique_ptr<Node> returnType;
		if (funcLex.val == "function") {
			eat(":");
			returnType = std::move(TypeSpec());
		}
		else {
			Lexeme typeLex = { "void" };
			typeLex.lex_type = Lexeme::Type::TYPE_SPEC;
			returnType = ASTFactory::createAST(typeLex);
		}
		eat(";");
		std::unique_ptr<Node> body = std::move(Block());

		name.lex_type = Lexeme::Type::FUN_NAME;
		return ASTFactory::createAST(name, std::move(params), std::move(returnType), std::move(body));
	}

	std::unique_ptr<Node> Block() {
		std::vector<std::unique_ptr<Node>> consts = std::move(Consts());
		std::vector<std::unique_ptr<Node>> decls = std::move(Vars());
		std::unique_ptr<Node> compound = std::move(CompoundSt());

		return ASTFactory::createAST({ "BLOCK" }, std::move(consts), std::move(decls), std::move(compound));
	}

	std::vector<std::unique_ptr<Node>> Consts() {
		std::vector<std::unique_ptr<Node>> cons;
		if (getCurLex().val == "const") {
			eat("const");
			while (getCurLex().lex_type == Lexeme::Type::ID) {
				cons.emplace_back(std::move(ConstDecl()));
				eat(";");
			}
		}
		return cons;
	}

	std::unique_ptr<Node> ConstDecl() {
		std::unique_ptr<Node> name = ASTFactory::createAST(getCurLex());
		eat(Lexeme::Type::ID);
		eat("=");

		Lexeme val = getCurLex();
		std::unique_ptr<Node> valPtr = std::move(Expression());

		val.val = "CONST_DECL";

		return ASTFactory::createAST(val, std::move(name), std::move(valPtr));
	}

	std::vector<std::unique_ptr<Node>> Vars() {
		std::vector<std::unique_ptr<Node>> vars;
		if (getCurLex().val == "var") {
			eat("var");
			while (getCurLex().lex_type == Lexeme::Type::ID) {
				std::vector<std::unique_ptr<Node>> varDeclList = std::move(VarDecl());
				for (auto& varDeclPTR : varDeclList)
					vars.emplace_back(std::move(varDeclPTR));
				eat(";");
			}
		}
		return vars;
	}

	std::vector<std::unique_ptr<Node>> VarDecl() {
		std::vector<std::unique_ptr<Node>> varsList;
		Lexeme tok = getCurLex();
		varsList.emplace_back(ASTFactory::createAST(tok));
		eat(Lexeme::Type::ID);

		while (getCurLex().val == ",") {
			eat(",");
			Lexeme tok = getCurLex();
			varsList.emplace_back(ASTFactory::createAST(tok));
			eat(Lexeme::Type::ID);
		}
		eat(":");

		std::unique_ptr<Node> typeNode = std::move(TypeSpec());

		std::vector<std::unique_ptr<Node>> varDeclsList;
		for (auto& var : varsList) {
			varDeclsList.emplace_back(ASTFactory::createAST({ "VAR_DECL" }, std::move(var), ASTFactory::copyType(typeNode.get())));
		}
		return varDeclsList;
	}

	std::unique_ptr<Node> TypeSpec() {
		Lexeme typeTok = getCurLex();

		if (typeTok.val != "array") {
			eat(Lexeme::Type::TYPE_SPEC);
			return ASTFactory::createAST(typeTok);
		}
		else {
			Lexeme arrTok = getCurLex();
			eat("array");
			eat("[");
			Lexeme lHandTok = getCurLex();
			eat(Lexeme::Type::INT);
			eat(".");
			eat(".");
			Lexeme rHandTok = getCurLex();
			if (rHandTok.lex_type == Lexeme::Type::INT)
				eat(Lexeme::Type::INT);
			else
				eat(Lexeme::Type::ID);

			eat("]");
			eat("of");

			std::unique_ptr<Node> subType = std::move(TypeSpec());
			return ASTFactory::createAST(arrTok, lHandTok, rHandTok, std::move(subType));
		}
	}

	std::unique_ptr<Node> CompoundSt() {
		Lexeme beginTok = getCurLex();
		eat("begin");
		std::vector<std::unique_ptr<Node>> stList = std::move(StList());
		eat("end");

		beginTok.val = "COMPOUND";
		return ASTFactory::createAST(beginTok, std::move(stList));
	}

	std::vector<std::unique_ptr<Node>> StList() {
		std::unique_ptr<Node> st = std::move(St());
		std::vector<std::unique_ptr<Node>> stList;
		stList.emplace_back(std::move(st));

		while (getCurLex().val == ";") {
			eat(";");
			stList.emplace_back(std::move(St()));
		}

		return stList;
	}

	std::unique_ptr<Node> St() {
		if (getCurLex().val == "begin") {
			return std::move(CompoundSt());
		}
		if (getCurLex().lex_type == Lexeme::Type::ID) {
			if (lookForward().val == ":=" || lookForward().val == "[") {
				return std::move(AssignSt());
			}
			else if (lookForward().val == "(") {
				return std::move(CallSt());
			}
			else {
				eat(Lexeme::Type::ID);
				eat(":=");
			}
		}
		if (getCurLex().val == "while")
			return std::move(WhileSt());
		if (getCurLex().val == "if")
			return std::move(IfSt());
		if (getCurLex().val == "for")
			return std::move(ForSt());
		if (getCurLex().val == "return")
			return std::move(ReturnSt());

		return std::move(EmptySt());
	}

	std::unique_ptr<Node> EmptySt() {
		return ASTFactory::createAST({ "EMPTY" });
	}

	std::unique_ptr<Node> AssignSt() {
		std::unique_ptr<Node> lValue;
		if (lookForward().val == "[")
			lValue = std::move(SelectSt());
		else
			lValue = std::move(Variable());

		Lexeme assignTok = getCurLex();
		eat(":=");

		std::unique_ptr<Node> toAssign = std::move(Expression());
		return ASTFactory::createAST(assignTok, std::move(lValue), std::move(toAssign));
	}

	std::unique_ptr<Node> IfSt() {
		Lexeme ifTok = getCurLex();
		eat("if");
		std::unique_ptr<Node> condition = std::move(Expression());
		eat("then");

		std::unique_ptr<Node> body = std::move(St());
		std::unique_ptr<Node> elseBody = nullptr;
		if (getCurLex().val == "else") {
			eat("else");
			elseBody = std::move(St());
		}
		return ASTFactory::createAST(ifTok, std::move(condition), std::move(body), std::move(elseBody));
	}

	std::unique_ptr<Node> WhileSt() {
		Lexeme whileTok = getCurLex();
		eat("while");
		std::unique_ptr<Node> condition = std::move(Expression());
		eat("do");
		std::unique_ptr<Node> body = std::move(St());
		return ASTFactory::createAST(whileTok, std::move(condition), std::move(body));
	}

	std::unique_ptr<Node> ForSt() {
		Lexeme tok = getCurLex();
		eat("for");
		std::unique_ptr<Node> iterSt = std::move(IterSt());
		eat("do");
		std::unique_ptr<Node> body = std::move(St());
		return ASTFactory::createAST(tok, std::move(iterSt), std::move(body));
	}

	std::unique_ptr<Node> CallSt() {
		Lexeme tok = getCurLex();
		eat(Lexeme::Type::ID);
		eat("(");
		std::vector<std::unique_ptr<Node>> params;
		if (getCurLex().val != ")") {
			std::unique_ptr<Node> param = std::move(Expression());
			params.emplace_back(std::move(param));
		}
		while (getCurLex().val == ",") {
			eat(",");
			std::unique_ptr<Node> param = std::move(Expression());
			params.emplace_back(std::move(param));
		}

		eat(")");
		tok.lex_type = Lexeme::Type::FUN_NAME;
		return ASTFactory::createAST(tok, std::move(params));
	}

	std::unique_ptr<Node> ReturnSt() {
		Lexeme tok = getCurLex();
		eat("return");
		std::unique_ptr<Node> toReturn = std::move(Expression());
		return ASTFactory::createAST(tok, std::move(toReturn));
	}

	std::unique_ptr<Node> Variable() {
		std::unique_ptr<Node> node = ASTFactory::createAST(getCurLex());
		eat(Lexeme::Type::ID);
		return std::move(node);
	}

	std::unique_ptr<Node> SelectSt() {
		Lexeme tok = getCurLex();
		tok.lex_type = Lexeme::Type::SELECT;
		std::unique_ptr<Node> var = std::move(Variable());
		std::vector<std::unique_ptr<Node>> selections;
		while (getCurLex().val == "[") {
			eat("[");
			std::unique_ptr<Node> index = std::move(Expression());
			eat("]");
			selections.emplace_back(std::move(index));
		}
		if (selections.size() != 1) {
			for (int i = 0; i < selections.size() - 1; i++) {
				var = ASTFactory::createAST(tok, std::move(var), std::move(selections[i]));
			}
		}

		return ASTFactory::createAST(tok, std::move(var), std::move(selections[selections.size() - 1]));
	}

	std::unique_ptr<Node> Expression() {
		std::unique_ptr<Node> simpleExpession = std::move(SimpleExpression());
		Lexeme token;

		std::string operators[] = { "<", "<=", "<>", "=", ">", ">=" };

		if (std::find(std::begin(operators), std::end(operators), getCurLex().val) != std::end(operators)) {
			token = getCurLex();
			eat(Lexeme::Type::OP);
			return ASTFactory::createAST(token, std::move(simpleExpession), std::move(SimpleExpression()));
		}
		else {
			return std::move(simpleExpession);
		}
	}

	std::unique_ptr<Node> SimpleExpression() {
		std::unique_ptr<Node> term = std::move(Term());

		std::string operators[] = { "+", "-", "or" };

		while (std::find(std::begin(operators), std::end(operators), getCurLex().val) != std::end(operators)) {
			Lexeme tok = getCurLex();
			eat(tok.lex_type);

			term = ASTFactory::createAST(tok, std::move(term), std::move(Term()));
		}

		return std::move(term);
	}

	std::unique_ptr<Node> Term() {
		std::unique_ptr<Node> factor = std::move(Factor());

		std::string operators[] = { "/", "*", "and" };

		while (std::find(std::begin(operators), std::end(operators), getCurLex().val) != std::end(operators)) {
			Lexeme tok = getCurLex();
			eat(tok.val);

			factor = ASTFactory::createAST(tok, std::move(factor), std::move(Factor()));
		}

		return std::move(factor);
	}

	std::unique_ptr<Node> Factor() {
		Lexeme token = getCurLex();
		if (token.lex_type == Lexeme::Type::INT || token.lex_type == Lexeme::Type::REAL || token.lex_type == Lexeme::Type::STRING) {
			eat(token.lex_type);
			std::unique_ptr<Node> node = ASTFactory::createAST(token);
			return std::move(node);
		}
		if (token.val == "(") {
			eat("(");
			std::unique_ptr<Node> node = std::move(Expression());
			eat(")");
			return std::move(node);
		}
		if (token.val == "+" || token.val == "-" || token.val == "not") {
			eat(token.val);
			std::unique_ptr<Node> node = ASTFactory::createAST(token, std::move(Factor()));
			return std::move(node);
		}
		if (token.lex_type == Lexeme::Type::ID) {
			if (lookForward().val == "(") {
				return std::move(CallSt());
			}
			else if (lookForward().val == "[") {
				return std::move(SelectSt());
			}
			else {
				return std::move(Variable());
			}
		}

		throw std::runtime_error("Unexpected token! " + token.val);
		//return nullptr;
	}

	std::unique_ptr<Node> IterSt() {
		Lexeme varTok = getCurLex();

		std::unique_ptr<Node> initSt = std::move(AssignSt());

		Lexeme toTok = getCurLex();

		if (toTok.val == "downto")
			eat("downto");
		else
			eat("to");

		Lexeme finTok = getCurLex();
		if (finTok.lex_type != Lexeme::Type::INT && finTok.lex_type != Lexeme::Type::REAL
			&& finTok.lex_type != Lexeme::Type::STRING && finTok.lex_type != Lexeme::Type::ID)
			throw std::runtime_error("Expected const or variable as end val of cycle!");

		Factor();

		Lexeme condOp = { "<=" };
		std::unique_ptr<Node> var_cond = ASTFactory::createAST(varTok);
		std::unique_ptr<Node> fin_cond = ASTFactory::createAST(finTok);
		std::unique_ptr<Node> condition = ASTFactory::createAST(condOp, std::move(var_cond), std::move(fin_cond));

		Lexeme postOp = { "+" };
		if (toTok.val == "downto")
			postOp = { "-" };
		Lexeme forOp = { "1" };
		std::unique_ptr<Node> var_postOp = ASTFactory::createAST(varTok);
		std::unique_ptr<Node> fin_postOp = ASTFactory::createAST(forOp);
		std::unique_ptr<Node> postAction = ASTFactory::createAST(postOp, std::move(var_postOp), std::move(fin_postOp));

		return ASTFactory::createAST(toTok, std::move(initSt), std::move(condition), std::move(postAction));
	}
};
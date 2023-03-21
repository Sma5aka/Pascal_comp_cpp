#pragma once
#include "../../Lexer/src/Lex.h"
#include "Node.h"

class ASTFactory {
public:
	static std::unique_ptr<Node> createAST(Lexeme lex) {
		if (lex.lex_type == Lexeme::Type::TYPE_SPEC)
			return std::unique_ptr<Node>(new TypeSpecNode(lex));
		if (lex.lex_type == Lexeme::Type::STRING)
			return std::unique_ptr<Node>(new StringNode(lex));
		if (lex.lex_type == Lexeme::Type::INT)
			return std::unique_ptr<Node>(new NumberNode(lex));
		if (lex.lex_type == Lexeme::Type::REAL)
			return std::unique_ptr<Node>(new NumberNode(lex));
		if (lex.lex_type == Lexeme::Type::ID)
			return std::unique_ptr<Node>(new VarNode(lex));
		if (lex.val == "EMPTY")
			return std::unique_ptr<Node>(new NoOpNode(lex));

		throw std::runtime_error("Factory class exception at " +
			std::to_string(lex.lex_y) + ":" + std::to_string(lex.lex_x));
	}

	static std::unique_ptr<Node> createAST(Lexeme lex, std::unique_ptr<Node> first) {
		if (lex.lex_type == Lexeme::Type::OP)
			return std::unique_ptr<Node>(new UnOpNode(lex, std::move(first)));
		if (lex.val == "return")
			return std::unique_ptr<Node>(new ReturnNode(lex, std::move(first)));

		throw std::runtime_error("Factory class exception at " +
			std::to_string(lex.lex_y) + ":" + std::to_string(lex.lex_x));
	}

	static std::unique_ptr<Node> createAST(Lexeme lex, std::unique_ptr<Node> first, std::unique_ptr<Node> second) {
		if (lex.val == ":=")
			return std::unique_ptr<Node>(new AssignNode(lex, std::move(first), std::move(second)));
		if (lex.val == "while")
			return std::unique_ptr<Node>(new WhileNode(lex, std::move(first), std::move(second)));
		if (lex.val == "for")
			return std::unique_ptr<Node>(new ForNode(lex, std::move(first), std::move(second)));
		if (lex.lex_type == Lexeme::Type::OP)
			return std::unique_ptr<Node>(new BinOpNode(lex, std::move(first), std::move(second)));
		if (lex.val == "VAR_DECL")
			return std::unique_ptr<Node>(new BinOpNode(lex, std::move(first), std::move(second)));
		if (lex.lex_type == Lexeme::Type::SELECT) {
			lex.val = "SELECT";
			return std::unique_ptr<Node>(new VarDeclNode(lex, std::move(first), std::move(second)));
		}
		if (lex.val == "CONSTDECL") {
			return std::unique_ptr<Node>(new VarDeclNode(lex, std::move(first), std::move(second)));
		}

		throw std::runtime_error("Factory class exception at " +
			std::to_string(lex.lex_y) + ":" + std::to_string(lex.lex_x));
	}

	static std::unique_ptr<Node> createAST(Lexeme lex, std::unique_ptr<Node> first, std::unique_ptr<Node> second, std::unique_ptr<Node> third) {
		if (lex.val == "if")
			return std::unique_ptr<Node>(new IfNode(lex, std::move(first), std::move(second), std::move(third)));
		if (lex.val == "to")
			return std::unique_ptr<Node>(new IterationNode(lex, std::move(first), std::move(second), std::move(third)));
		if (lex.val == "downto")
			return std::unique_ptr<Node>(new IterationNode(lex, std::move(first), std::move(second), std::move(third)));

		throw std::runtime_error("Factory class exception at " +
			std::to_string(lex.lex_y) + ":" + std::to_string(lex.lex_x));
	}

	static std::unique_ptr<Node> createAST(Lexeme lex, std::vector<std::unique_ptr<Node>> params) {
		if (lex.val == "COMPOUND") {
			return std::unique_ptr<CompoundNode>(new CompoundNode(lex, std::move(params)));
		}
		if (lex.lex_type == Lexeme::Type::FUN_NAME) {
			return std::unique_ptr<Node>(new CallFuncNode(lex, std::move(params)));
		}

		throw std::runtime_error("Factory class exception at " +
			std::to_string(lex.lex_y) + ":" + std::to_string(lex.lex_x));
	}

	static std::unique_ptr<Node> createAST(Lexeme lex, std::vector<std::unique_ptr<Node>> params, std::unique_ptr<Node> first) {
		if (lex.lex_type == Lexeme::Type::PROG_NAME) {
			return std::unique_ptr<ProgramNode>(new ProgramNode(lex, std::move(params), std::move(first)));
		}

		throw std::runtime_error("Factory class exception at " +
			std::to_string(lex.lex_y) + ":" + std::to_string(lex.lex_x));
	}

	static std::unique_ptr<Node> createAST(Lexeme lex, std::vector<std::unique_ptr<Node>> params, std::unique_ptr<Node> first, std::unique_ptr<Node> second) {
		if (lex.lex_type == Lexeme::Type::FUN_NAME)
			return std::unique_ptr<Node>(new FunctionNode(lex, std::move(params), std::move(first), std::move(second)));

		throw std::runtime_error("Factory class exception at " +
			std::to_string(lex.lex_y) + ":" + std::to_string(lex.lex_x));
	}

	static std::unique_ptr<Node> createAST(Lexeme lex, std::vector<std::unique_ptr<Node>> constDecls, std::vector<std::unique_ptr<Node>> varDecls, std::unique_ptr<Node> compound) {
		if (lex.val == "BLOCK")
			return std::unique_ptr<Node>(new BlockNode(lex, std::move(constDecls), std::move(varDecls), std::move(compound)));

		throw std::runtime_error("Factory class exception at " +
			std::to_string(lex.lex_y) + ":" + std::to_string(lex.lex_x));
	}

	static std::unique_ptr<Node> createAST(Lexeme firstTok, Lexeme secondTok, Lexeme thirdTok, std::unique_ptr<Node> first) {
		if (firstTok.val == "array")
			return std::unique_ptr<Node>(new ArrSpecNode(firstTok, secondTok, thirdTok, std::move(first)));
		throw std::runtime_error("Factory class exception!");
	}

	static std::unique_ptr<Node> copyType(Node* ptr) {
		if (ptr->token.lex_type == Lexeme::Type::TYPE_SPEC)
			return createAST(ptr->token);
		if (ptr->token.val == "array") {
			ArrSpecNode* rawPtr = static_cast<ArrSpecNode*>(ptr);
			return std::unique_ptr<Node>(createAST(rawPtr->token, rawPtr->lHandTok, rawPtr->rHandTok, copyType(rawPtr->subType.get())));
		}

		throw std::runtime_error("Using copyType() for non-type node!");
	}
};
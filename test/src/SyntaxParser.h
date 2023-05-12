#pragma once
#include "TokenType.h"
#include "Token.h"
#include "SyntaxNode.h"
#include <fstream>
#include "tokenType.h"
#include <vector>
#include <iostream>
#include "Error.h"
using namespace Node;

class SyntaxParser
{
	inline void judgeDL();
public:
	SyntaxParser(std::vector<Token>& tokens) :_tokens(tokens), _tokenIndex(-1) 
	{
	}
	optional<Token> currToken();

	SyntaxNode* parseProgram();

	SyntaxNode* BeginParse();

	SyntaxNode* solveArrDeclare();

	SyntaxNode* solveFunction();

	SyntaxNode* solveDeclare();

	SyntaxNode* solveCodeBlock(SyntaxNode* functionNode);

    SyntaxNode* solveExprList(SyntaxNode* functionNode);

	SyntaxNode* parseExpr(SyntaxNode* functionNode);

	SyntaxNode* parseArithORStrORBoolExpr();

	SyntaxNode* parseTermA();

	SyntaxNode* parseTermB();

	SyntaxNode* parseTermC();

	SyntaxNode* parseTermD();

	SyntaxNode* parseTermE();

	SyntaxNode* parseAtom();

	SyntaxNode* parseBlock(SyntaxNode* father);

	SyntaxNode* parseSimpleAssign(Token var);

	SyntaxNode* parseCall(Token var);

	optional<Token> seeSpecToken(int index);



private:
	std::vector<Token> _tokens;
	int _tokenIndex;
};

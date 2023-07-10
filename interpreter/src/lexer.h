#pragma once
#include "TextLoader.h"
#include "Error.h"
#include "Token.h"
#include <vector>
#include <string>
#include <optional>
using namespace std;


class Lexer
{
private:
	Lexer() {};
	Lexer(const Lexer& rvalue) = delete;
	Lexer& operator=(const Lexer& rvalue) = delete;
public:
	static Lexer& Get();
	//API 入口
	vector<Token> beginSolveCodeFromStr(string code);

private:
	//未开放API入口
	vector<Token> beginSolveCodeFromFile(string path);
	//词法解析器主体
	vector<Token> solveAllCode(vector<string>& lines);

	string error(string msg);

	optional<char> getCurrChar();

	void solveCurrLine();

	void solveNum();

	void init();

private:
	 vector<Token> _TokenResultVec;
	 string _currLineStr;
	 int _currLinePos;
	 int _currCharPos;
};

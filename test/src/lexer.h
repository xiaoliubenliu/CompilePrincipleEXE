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
	//API ���
	vector<Token> beginSolveCodeFromStr(string code);

private:
	//δ����API���
	vector<Token> beginSolveCodeFromFile(string path);
	//�ʷ�����������
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

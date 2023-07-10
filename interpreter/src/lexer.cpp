#include "lexer.h"
#include <iostream>
using namespace std;
Lexer& Lexer::Get()
{
	static Lexer instance;
	return instance;
}

string Lexer::error(string msg)
{
	return to_string(_currLinePos) + "行" + to_string(_currCharPos) + "列" + msg;
}

vector<Token> Lexer::beginSolveCodeFromFile(string path)
{
	vector<string> vec;
	TextLoader::LoadFromFile(path, vec);//读取每一行。
	return solveAllCode(vec);
}

vector<Token> Lexer::beginSolveCodeFromStr(string code)
{
	vector<string> vec;
	TextLoader::LoadFromString(code, vec);//读取每一行。
	return solveAllCode(vec);
}

void Lexer::init()
{
	_currLinePos = 0;
	_currCharPos = 0;
	//_TokenResultVec = std::vector<Token>();
	_TokenResultVec.clear();

}

vector<Token> Lexer::solveAllCode(vector<string>& lines)
{
	init();
	for (const auto& line : lines)
	{
		++_currLinePos;
		_currCharPos = 0;
		_currLineStr = line;
		solveCurrLine();
	}
	_TokenResultVec.push_back(Token(_EOF, -1, -1));
	return _TokenResultVec;
}

optional<char> Lexer::getCurrChar()
{
	if (_currCharPos < _currLineStr.length()) {
		return _currLineStr[_currCharPos];
	}
	else {
		return std::nullopt;
	}
}

void Lexer::solveCurrLine()
{
	while (getCurrChar())
	{
		if (getCurrChar() >= '0' && getCurrChar() <= '9')
			solveNum();
		else if (getCurrChar() == '+')
		{
			++_currCharPos;
			_TokenResultVec.push_back(Token(_PLUS, _currLinePos, _currCharPos));
		}
		else if (getCurrChar() == '-')
		{
			++_currCharPos;
			if (getCurrChar() == '>')
				_TokenResultVec.push_back(Token(_RETURN, _currLinePos, _currCharPos++));
			else
				_TokenResultVec.push_back(Token(_MINUS, _currLinePos, _currCharPos));
		}
		else if (getCurrChar() == '*')
		{
			++_currCharPos;
			if (!getCurrChar().has_value() || getCurrChar() != '*')
			{
				_TokenResultVec.push_back(Token(_MUL, _currLinePos, _currCharPos));
			}
			else
			{
				++_currCharPos;
				_TokenResultVec.push_back(Token(_POW, _currLinePos, _currCharPos));
			}
		}
		else if (getCurrChar() == '/')
		{
			++_currCharPos;
			if (getCurrChar().has_value() && getCurrChar() == '/')
			{
				while (getCurrChar())//去除注释
					++_currCharPos;
			}
			else
				_TokenResultVec.push_back(Token(_DIV, _currLinePos, _currCharPos));
		}
		else if (getCurrChar() == '%')
		{
			++_currCharPos;
			_TokenResultVec.push_back(Token(_MODULE, _currLinePos, _currCharPos));
		}
		else if (getCurrChar() == '&')
		{
			++_currCharPos;
			_TokenResultVec.push_back(Token(_AND, _currLinePos, _currCharPos));
		}
		else if (getCurrChar() == '|')
		{
			++_currCharPos;
			_TokenResultVec.push_back(Token(_OR, _currLinePos, _currCharPos));
		}
		else if (getCurrChar() == '>')
		{
			++_currCharPos;
			if (getCurrChar() == '=')
			{
				++_currCharPos;
				_TokenResultVec.push_back(Token(_COMP, _currLinePos, _currCharPos, ">="));
			}
			else
				_TokenResultVec.push_back(Token(_COMP, _currLinePos, _currCharPos, ">"));
		}
		else if (getCurrChar() == '<')
		{
			++_currCharPos;
			if (getCurrChar() == '=')
			{
				++_currCharPos;
				_TokenResultVec.push_back(Token(_COMP, _currLinePos, _currCharPos, "<="));
			}
			else
				_TokenResultVec.push_back(Token(_COMP, _currLinePos, _currCharPos, "<"));
		}
		else if (getCurrChar() == '!')
		{
			++_currCharPos;
			if (getCurrChar() == '=')
			{
				++_currCharPos;
				_TokenResultVec.push_back(Token(_COMP, _currLinePos, _currCharPos, "!="));
			}
			else
				_TokenResultVec.push_back(Token(_NOT, _currLinePos, _currCharPos));
		}
		else if (getCurrChar() == '=')
		{
			++_currCharPos;
			if (getCurrChar().has_value() && getCurrChar() == '=') {
				++_currCharPos;
				_TokenResultVec.push_back(Token(_COMP, _currLinePos, _currCharPos, "=="));
			}
			else
				_TokenResultVec.push_back(Token(_ASSIGN, _currLinePos, _currCharPos));
		}
		else if (getCurrChar() == '(')
		{
			++_currCharPos;
			_TokenResultVec.push_back(Token(_LPAREN, _currLinePos, _currCharPos));
		}
		else if (getCurrChar() == ')')
		{
			++_currCharPos;
			_TokenResultVec.push_back(Token(_RPAREN, _currLinePos, _currCharPos));
		}
		else if (getCurrChar() == '{')
		{
			++_currCharPos;
			_TokenResultVec.push_back(Token(_LCB, _currLinePos, _currCharPos));
		}
		else if (getCurrChar() == '}')
		{
			++_currCharPos;
			_TokenResultVec.push_back(Token(_RCB, _currLinePos, _currCharPos));
		}
		else if (getCurrChar() == '[')
		{
			++_currCharPos;
			_TokenResultVec.push_back(Token(_LSB, _currLinePos, _currCharPos));
		}
		else if (getCurrChar() == ']')
		{
			++_currCharPos;
			_TokenResultVec.push_back(Token(_RSB, _currLinePos, _currCharPos));
		}
		else if (getCurrChar() == ';')
		{
			++_currCharPos;
			_TokenResultVec.push_back(Token(_DL, _currLinePos, _currCharPos));
		}
		else {
			//string或定义类型
			string tempstr;
			string str;
			if (!isalpha(getCurrChar().value()))//不是字母
			{
				//不是字符和内置token
				if (getCurrChar() != '"')
				{
					if (getCurrChar() != ' ' && getCurrChar() != '\t'&&getCurrChar()!=',') {
						Error::AddError(error(string("无效的字符:")+getCurrChar().value()));
						++_currCharPos;
					}
					else
						++_currCharPos;
				}
				//是"",读取string
				else {
					++_currCharPos;
					while (getCurrChar() && getCurrChar() != '"') {
						tempstr += getCurrChar().value();
						++_currCharPos;
					}
					if (getCurrChar() && getCurrChar() == '"') {
						++_currCharPos;
						str = tempstr;
						_TokenResultVec.emplace_back(Token(_STRING, _currLinePos, _currCharPos, str));
					}
					//读取一整行之后没有发现”
					else
					{
						Error::AddError(error("无法识别的string！"));
					}
				}
			}
			else {//是字母，但不是内置普通标识符
				tempstr = string();
				tempstr += getCurrChar().value();
				++_currCharPos;

				//读取整个定义名称，允许字母数字和_;
				while (getCurrChar() && getCurrChar() != ' ' && getCurrChar() != '\t' && (isalnum(getCurrChar().value()) || getCurrChar().value() == '_')) {
					tempstr += getCurrChar().value();
					++_currCharPos;
				}

				str = tempstr;//判断是否为内置函数和类型。
				if (str == "int") {
					_TokenResultVec.push_back(Token(_KW, _currLinePos, _currCharPos, str));
				}
				else if (str == "float") {
					_TokenResultVec.push_back(Token(_KW, _currLinePos, _currCharPos, str));
				}
				else if (str == "string") {
					_TokenResultVec.push_back(Token(_KW, _currLinePos, _currCharPos, str));
				}
				else if (str == "bool") {
					_TokenResultVec.push_back(Token(_KW, _currLinePos, _currCharPos, str));
				}
				else if (str == "true" || str == "false")
					_TokenResultVec.push_back(Token(_BOOL, _currLinePos, _currCharPos, str));
				else
				{
					if (str == "print")
						_TokenResultVec.push_back(Token(_PRINT, _currLinePos, _currCharPos));
					else if (str == "fn")
						_TokenResultVec.push_back(Token(_FN, _currLinePos, _currCharPos));
					else if (str == "if")
						_TokenResultVec.push_back(Token(_IF, _currLinePos, _currCharPos));
					else if (str == "else")
						_TokenResultVec.push_back(Token(_ELSE, _currLinePos, _currCharPos));
					else if (str == "while")
						_TokenResultVec.push_back(Token(_WHILE, _currLinePos, _currCharPos));
					else if (str == "break")
						_TokenResultVec.push_back(Token(_BREAK, _currLinePos, _currCharPos));
					else if (str == "continue")
						_TokenResultVec.push_back(Token(_CONTINUE, _currLinePos, _currCharPos));
					else if (str == "void")
						_TokenResultVec.push_back(Token(_VOID, _currLinePos, _currCharPos));
					else if (str == "return")
						_TokenResultVec.push_back(Token(_RETURN, _currLinePos, _currCharPos));
					else if (str == "readInt")
						_TokenResultVec.push_back(Token(_READINT, _currLinePos, _currCharPos));
					else if (str == "readFloat")
						_TokenResultVec.push_back(Token(_READFLOAT, _currLinePos, _currCharPos));
					else if (str == "readString")
						_TokenResultVec.push_back(Token(_READSTRING, _currLinePos, _currCharPos));
					else if (str == "readBool")
						_TokenResultVec.push_back(Token(_READBOOL, _currLinePos, _currCharPos));
					else
						_TokenResultVec.push_back(Token(_ID, _currLinePos, _currCharPos, str));
				}
			}
		}
	}
}

void Lexer::solveNum()
{
	string s;
	int dotCount = 0;
	for (auto c = getCurrChar(); c  && (c >= '0' && c <= '9' || c == '.'); c = getCurrChar()) {
		if (c == '.') {
			if (dotCount > 0) {
				Error::AddError(error("错误的float"));
			}
			else {
				dotCount = 1;
				s.push_back(c.value());
			}
		}
		else {
			s.push_back(c.value());
		}
		++_currCharPos;
	}
	if (getCurrChar().has_value() && isalpha(getCurrChar().value())) {
		Error::AddError(error("非法的字符！"));
	}
	if (dotCount == 0) {
		_TokenResultVec.emplace_back(Token(_INT, _currLinePos, _currCharPos - s.length() + 1, s));
	}
	else {
		_TokenResultVec.emplace_back(Token(_FLOAT, _currLinePos, _currCharPos - s.length() + 1, s));
	}
}
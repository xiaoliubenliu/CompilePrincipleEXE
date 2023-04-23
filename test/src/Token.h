#pragma once
#include "tokenType.h"
#include <vector>
#include <optional>
#include <string>
using namespace std;
class Token
{
	Token() = delete;
public:
	Token(ETokenType type, int line, int col) :_type(type), _line(line), _col(col),_value(std::nullopt)
	{
	}
	Token(ETokenType type, int line, int col, string str) :_type(type), _line(line), _col(col), _value(str)
	{
	}
	string toString() const;
	string TokenName() const;
	ETokenType type() { return _type; }
	optional<string> value() { return _value; }
private:
	ETokenType _type;
	optional<string> _value;
	int _line;
	int _col;
};
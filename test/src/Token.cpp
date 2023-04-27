#include "Token.h"
#include <iostream>

string Token::toString() const
{
	if (_value)
	{
		return (/*to_string(this->_line) + "行," + to_string(this->_col) + "列:"*/ string() + token_names[(int)_type] + "->" + _value.value());
	}
	else
	{
		return  (/*to_string(this->_line) + "行," + to_string(this->_col) + "列:"*/string() + token_names[(int)_type]);
	}
}

string Token::TokenName() const
{
	if (_value)
		return string(token_names[(int)_type]) + ":"+ _value.value()+" ";
	else return string(token_names[(int)_type]) + " ";
}


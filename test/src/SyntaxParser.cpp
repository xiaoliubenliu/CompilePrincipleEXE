#include "SyntaxParser.h"

optional<Token> SyntaxParser::currToken()
{
	if (_tokenIndex < _tokens.size())
		return _tokens[_tokenIndex];
	else
		return std::nullopt;
}

SyntaxNode* SyntaxParser::BeginParse()
{
	++_tokenIndex;
	SyntaxNode* resultAST = parseProgram();
	if (currToken().value().type() != _EOF)
		throw runtime_error(currToken().value().toString() + "语法错误,程序未正常结束>");
	else
		return resultAST;
}

SyntaxNode* SyntaxParser::parseProgram()
{
	ProgramNode* pn = new ProgramNode();
	//全局数组声明。

	while (currToken().value().type() != _EOF)
	{
		if (currToken().value().type() == _KW)
			pn->addArray(solveArrDeclare());
		else
			pn->addFunc(solveFunction());
	}
	return pn;
}

SyntaxNode* SyntaxParser::solveArrDeclare()
{
	Token kw = currToken().value();
	++_tokenIndex;
	if (currToken().value().type() != (int)_ID)
		throw runtime_error(currToken().value().toString() + "找不到数组名称");
	else {
		Token id = currToken().value();
		++_tokenIndex;
		if (currToken().value().type() != _LSB)
			throw runtime_error(currToken().value().toString() + "需要'['");
		else {
			++_tokenIndex;
			if (currToken().value().type() != _INT) {
				throw runtime_error(currToken().value().toString() + "需要整数");
			}
			else {
				long length = stol(currToken().value().value().value());
				++_tokenIndex;
				if (currToken().value().type() != _RSB)
					throw runtime_error(currToken().value().toString() + "需要']'");
				else {
					++_tokenIndex;
					return new ArrayDeclareNode(kw, id, length);
				}
			}
		}
	}
}

SyntaxNode* SyntaxParser::solveFunction() {
	//fn之后不是方法声明
	if (currToken().value().type() != _FN)
		throw runtime_error(currToken().value().toString() + "非法的方法声明");
	else
	{
		++_tokenIndex;
		if (currToken().value().type() != _ID) //不是变量声明
		{
			auto temp = currToken().value().type();
			//判断是否为内置的函数。
			if (temp != _PRINT && temp != _READINT && temp != _READFLOAT && temp != _READSTRING && temp != _READBOOL)
				throw runtime_error(currToken().value().toString() + "需要方法名称");
			else
				throw runtime_error(currToken().value().toString() + "函数重定义");
		}
		else //函数名称声明
		{
			FunctionNode* fn = new FunctionNode(currToken().value());
			++_tokenIndex;
			Token curr = currToken().value();
			if (curr.type() != _LPAREN) 
				throw runtime_error(currToken().value().toString() + "缺少'('");
			else {
				++_tokenIndex;

				SyntaxNode* funBlock;
				//添加所有的参数。
				for (curr = currToken().value(); curr.type() == _KW; curr = currToken().value())
				{
					funBlock = solveDeclare();
					fn->addParam(funBlock);
				}

				curr = currToken().value();
				if (curr.type() != _RPAREN)
				{
					throw runtime_error(currToken().value().toString() + "缺少')'");
				}
				else
				{
					//解决返回值问题。
					++_tokenIndex;
					if (currToken().value().type() == _RETURN)
					{
						++_tokenIndex;
						fn->setRetNode(new retNode(currToken().value())); // ->
						++_tokenIndex;
						funBlock = solveCodeBlock(fn);  //TODO:解决 代码块。
						fn->setBody(funBlock);
						return fn;
					}
					else
					{
						fn->setRetNode(new retNode(Token(_VOID,currToken().value().getLine(),currToken().value().getCol())));
						funBlock = solveCodeBlock(fn);  //TODO:解决 代码块。
						fn->setBody(funBlock);
						return fn;
					}
				}
			}
		}
	}
}

SyntaxNode* SyntaxParser::solveDeclare()
{
	Token curr = currToken().value();
	if (curr.type() == _KW) 
	{
		++_tokenIndex;
		Token kw = curr;
		curr = currToken().value();
		if (curr.type() != _ID) 
			throw runtime_error(curr.toString() + "缺少变量名称");
		else 
		{
			++_tokenIndex;
			return new DeclareNode(kw, curr);
		}
	}
	else
		throw runtime_error(curr.toString() + "语法错误");

}

SyntaxNode* SyntaxParser::solveCodeBlock(SyntaxNode* functionNode)
{
	Token curr = currToken().value();
	if (curr.type() == _LCB) 
	{
		++_tokenIndex;
		SyntaxNode* blockExprList = solveExprList(functionNode);
		curr = currToken().value();
		if (curr.type() != _RCB)
			throw runtime_error(curr.toString() + "需要'}' 结束语句语句块");
		else 
		{
			++_tokenIndex;
			return blockExprList;
		}
	}
	else 
		throw runtime_error(currToken().value().toString() + "需要'{'");
}

SyntaxNode* SyntaxParser::solveExprList(SyntaxNode* functionNode)
{
	ExprListNode* eln = new ExprListNode(functionNode);

	while (currToken().value().type() != _EOF)
	{
		SyntaxNode* expr = parseExpr(functionNode);
		if (expr == nullptr) 
			break;

		eln->add(expr);
	}

	return eln;
}



inline void SyntaxParser::judgeDL()
{
	if (currToken().value().type() != _DL)
		throw runtime_error("缺少';'");
	++_tokenIndex;
}

//解析表达式
SyntaxNode* SyntaxParser::parseExpr(SyntaxNode* functionNode)
{
	Token curr = currToken().value();
	SyntaxNode* expr;
	if (curr.type() == _RETURN)
	{
		++_tokenIndex;
		if (currToken().value().type() == _DL)
		{
			++_tokenIndex;
			return new ReturnNode(curr, nullptr);
		}
		expr = parseArithORStrORBoolExpr();
		judgeDL();
		return new ReturnNode(curr, expr);
	}
	else if (curr.type() == _CONTINUE) {
		++_tokenIndex;
		judgeDL();
		return new ContinueNode(curr);
	}
	else if (curr.type() == _BREAK) {
		++_tokenIndex;
		judgeDL();
		return new BreakNode(curr);
	}
	else {
		SyntaxNode* indexExpr;
		if (curr.type() == _WHILE) {
			++_tokenIndex;
			expr = parseArithORStrORBoolExpr();
			judgeDL();
			indexExpr = parseBlock(nullptr);
			return new WhileNode(expr, indexExpr);
		}
		else {
			SyntaxNode* idEQexpr;
			SyntaxNode* valueExpr;
			if (curr.type() == _IF) {
				Token var = curr;
				++_tokenIndex;
				indexExpr = parseArithORStrORBoolExpr();
				judgeDL();
				valueExpr = parseBlock(nullptr);
				curr = currToken().value();
				if (curr.type() == _ELSE) {
					++_tokenIndex;
					idEQexpr = parseBlock(nullptr);
					return new IfElseNode(indexExpr, valueExpr, idEQexpr, var);
				}
				else {
					return new IfElseNode(indexExpr, valueExpr, nullptr, var);
				}
			}
			else if (curr.type() == _PRINT) {
				++_tokenIndex;
				curr = currToken().value();
				if (curr.type() != _LPAREN) {
					throw runtime_error( currToken().value().toString()+ "需要左括号");
				}
				else {
					++_tokenIndex;
					expr = parseArithORStrORBoolExpr();
					judgeDL();
					curr = currToken().value();
					if (curr.type() != _RPAREN) {
						throw runtime_error(currToken().value().toString() + "需要右括号");
					}
					else {
						++_tokenIndex;
						return new PrintNode(expr);
					}
				}
			}
			else if (curr.type() == _ID) {
				Token var = curr;
				++_tokenIndex;
				curr = currToken().value();
				if (curr.type() != _ASSIGN && curr.type() != _LPAREN && curr.type() != _LSB) {
					throw runtime_error(currToken().value().toString()+ "缺'='或'('或'['");
				}
				else if (curr.type() == _ASSIGN) {
					return parseSimpleAssign(var);
				}
				else if (curr.type() == _LSB) {
					++_tokenIndex;
					indexExpr = parseArithORStrORBoolExpr();
					judgeDL();
					if (curr.type() == _RSB) {
						throw runtime_error(currToken().value().toString()+"缺少']'" );
					}
					else {
						++_tokenIndex;
						if (curr.type() == _ASSIGN) {
							throw runtime_error( currToken().value().toString() + "缺少'='");
						}
						else {
							++_tokenIndex;

							try {
								valueExpr = parseArithORStrORBoolExpr();
								judgeDL();
								return new ArrayAssignNode(var, indexExpr, valueExpr);
							}
							catch (exception var5) {
								throw runtime_error(var.toString()+"语法错误");
							}
						}
					}
				}
				else if (curr.type() == _LPAREN) {
					return parseCall(var);
				}
				else {
					throw runtime_error(currToken().value().toString()+"语法错误");
				}
			}
			else if (curr.type() != _KW) {
				return nullptr;
			}
			else {
				++_tokenIndex;
				DeclareMULNode* dmn = new DeclareMULNode(curr);

				while (currToken().value().type() == _ID) {
					if (seeSpecToken(_tokenIndex + 1).value().type() == _ASSIGN) {
						Token var = currToken().value();
						++_tokenIndex;
						idEQexpr = parseSimpleAssign(var);
						dmn->add(idEQexpr);
					}
					else {
						Token var = currToken().value();
						++_tokenIndex;
						dmn->add(new VariableNode(var));
					}

					if (currToken().value().type() == _DL) {
						++_tokenIndex;
						break;
					}
				}

				return dmn;
			}
		}
	}
}

SyntaxNode* SyntaxParser::parseArithORStrORBoolExpr()
{
	SyntaxNode* termLeft = parseTermA();

	for (Token opt = currToken().value(); opt.type() == _OR; opt = currToken().value()) 
	{
		++_tokenIndex;
		SyntaxNode* termRight = parseTermA();
		termLeft = new BinaryOPNode(termLeft, opt, termRight);
	}

	return termLeft;
}

//解析一个逻辑与运算式，
SyntaxNode* SyntaxParser::parseTermA()
{
	SyntaxNode* termLeft = parseTermB();

	for (Token opt = currToken().value(); opt.type() == _AND; opt = currToken().value()) {
		++_tokenIndex;
		SyntaxNode* termRight = parseTermB();
		termLeft = new BinaryOPNode(termLeft, opt, termRight);
	}

	return termLeft;
}

//解析一个比较运算式
SyntaxNode* SyntaxParser::parseTermB()
{
	SyntaxNode* termLeft = parseTermC();

	for (Token opt = currToken().value(); opt.type() == _COMP; opt = currToken().value()) {
		++_tokenIndex;
		SyntaxNode* termRight = parseTermC();
		termLeft = new BinaryOPNode(termLeft, opt, termRight);
	}

	return termLeft;
}

SyntaxNode* SyntaxParser::parseTermC()
{
	SyntaxNode* termLeft = parseTermD();

	for (Token opt = currToken().value(); opt.type() == _PLUS || opt.type() == _MINUS; opt = currToken().value()) {
		++_tokenIndex;
		SyntaxNode* termRight = parseTermD();
		termLeft = new BinaryOPNode(termLeft, opt, termRight);
	}

	return termLeft;
}

SyntaxNode* SyntaxParser::parseTermD()
{
	SyntaxNode* facLeft = parseTermE();

	for (Token opt = currToken().value(); opt.type() == _MUL || opt.type() == _DIV || opt.type() == _MODULE; opt = currToken().value()) {
		++_tokenIndex;
		SyntaxNode* facRight = parseTermE();
		facLeft = new BinaryOPNode(facLeft, opt, facRight);
	}

	return facLeft;
}

SyntaxNode* SyntaxParser::parseTermE()
{
	SyntaxNode* atomLeft = parseAtom();

	for (Token opt = currToken().value(); opt.type() == _POW; opt = currToken().value()) {
		++_tokenIndex;
		SyntaxNode* facRight = parseAtom();
		atomLeft = new BinaryOPNode(atomLeft, opt, facRight);
	}

	return atomLeft;
}

SyntaxNode* SyntaxParser::parseAtom()
{
	Token curr = currToken().value();
	if (curr.type() != _INT && curr.type() != _FLOAT) {
		if (curr.type() == _STRING) {
			++_tokenIndex;
			return new StringNode(curr);
		}
		else if (curr.type() == _BOOL) {
			++_tokenIndex;
			return new BoolNode(curr);
		}
		else if (curr.type() != _PLUS && curr.type() != _MINUS && curr.type() != _NOT) {
			if (curr.type() == _LPAREN) {
				++_tokenIndex;
				SyntaxNode* expr = parseArithORStrORBoolExpr();
				judgeDL();
				Token newCurr = currToken().value();
				if (newCurr.type() == _RPAREN) {
					++_tokenIndex;
					return expr;
				}
				else {
					throw runtime_error(curr.toString() + "括号不匹配");
				}
			}
			else if (curr.type() == _ID) {
				++_tokenIndex;
				Token newCurr = currToken().value();
				if (newCurr.type() == _LPAREN) {
					return parseCall(curr);
				}
				else if (newCurr.type() == _LSB) {
					++_tokenIndex;
					SyntaxNode* exprIndex = parseArithORStrORBoolExpr();
					judgeDL();
					if (currToken().value().type() != _RSB) {
						throw runtime_error( currToken().value().toString() + "需要右括号']'");
					}
					else {
						++_tokenIndex;
						return new ArrayValueNode(curr, exprIndex);
					}
				}
				else {
					return new VariableNode(curr);
				}
			}
			else if (curr.type() == _READINT) {
				++_tokenIndex;
				if (currToken().value().type() != _LPAREN) {
					throw runtime_error(curr.toString()+ "需要左括号'('" );
				}
				else {
					++_tokenIndex;
					if (currToken().value().type() != _RPAREN) {
						throw runtime_error(curr.toString() + "需要左括号')'");
					}
					else {
						++_tokenIndex;
						return new ReadNode(READINT);
					}
				}
			}
			else if (curr.type() == _READFLOAT) {
				++_tokenIndex;
				if (currToken().value().type() != _LPAREN) {
					throw runtime_error(curr.toString()+"需要左括号'('" );
				}
				else {
					++_tokenIndex;
					if (currToken().value().type() != _RPAREN) {
						throw runtime_error(curr.toString() + "需要左括号')'" );
					}
					else {
						++_tokenIndex;
						return new ReadNode(READFLOAT);
					}
				}
			}
			else if (curr.type() == _READSTRING) {
				++_tokenIndex;
				if (currToken().value().type() != _LPAREN) {
					throw runtime_error(curr.toString() + "需要左括号'('" );
				}
				else {
					++_tokenIndex;
					if (currToken().value().type() != _RPAREN) {
						throw runtime_error(curr.toString() + "需要左括号')'" );
					}
					else {
						++_tokenIndex;
						return new ReadNode(READSTRING);
					}
				}
			}
			else if (curr.type() == _READBOOL) {
				++_tokenIndex;
				if (currToken().value().type() != _LPAREN) {
					throw runtime_error(curr.toString() + "需要左括号'('");
				}
				else {
					++_tokenIndex;
					if (currToken().value().type() != _RPAREN) {
						throw runtime_error(curr.toString() + "需要左括号')'" );
					}
					else {
						++_tokenIndex;
						return new ReadNode(READBOOL);
					}
				}
			}
			else {
				throw runtime_error(curr.toString() + "语法错误");
			}
		}
		else {
			++_tokenIndex;
			return new UnaryOPNode(curr, parseAtom());
		}
	}
	else {
		++_tokenIndex;
		return new NumberNode(curr);
	}
}
SyntaxNode* SyntaxParser::parseBlock(SyntaxNode* father)
{
	Token curr = currToken().value();
	if (curr.type() == _LCB) {
		++_tokenIndex;
		SyntaxNode* blockExprList = solveExprList(father);
		curr = currToken().value();
		if (curr.type() != _RCB) {
			throw runtime_error(currToken().value().toString()+"需要'}' 语句块应该结束");
		}
		else {
			++_tokenIndex;
			return blockExprList;
		}
	}
	else {
		throw runtime_error( currToken().value().toString()+"需要'{'");
	}
}

SyntaxNode* SyntaxParser::parseSimpleAssign(Token var)
{
	++_tokenIndex;
	SyntaxNode* expr = parseArithORStrORBoolExpr();
	judgeDL();
	return new AssignNode(new VariableNode(var), expr);
}

SyntaxNode* SyntaxParser::parseCall(Token var)
{
	++_tokenIndex;
	Token curr = currToken().value();

	CallNode* cn;
	for (cn = new CallNode(var); curr.type() != _RPAREN; curr = currToken().value()) {
		SyntaxNode* exprParam = parseArithORStrORBoolExpr();
		judgeDL();
		cn->addParam(exprParam);
	}

	if (curr.type() == _RPAREN) {
		++_tokenIndex;
		return cn;
	}
	else {
		throw runtime_error(currToken().value().toString() + "需要右括号");
	}
}

optional<Token> SyntaxParser::seeSpecToken(int index)
{
	if (index < _tokens.size())
		return _tokens[index];
	else
		return std::nullopt;
}




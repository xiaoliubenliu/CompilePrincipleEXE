#pragma once
#include <string>
#include <stdexcept>
#include "runresult.h"
#include "SyntaxNode.h"
#include <vector>
using namespace std;

class Error
{
	Error() {};
	Error(Error& lvalue) = delete;
	vector<string> errorResult;
	void addError(string str);
public:
	static Error& Get();
	static void AddError(string str);
	static void ClearError();
	static string PrintError();
	static const vector<string>& getError();
};


class ReturnException:public runtime_error
{
public:
	Result::RunResult* result;
public:
	 ReturnException(Result::RunResult* r):result(r),runtime_error("ReturnException") {}
};


class BreakException :public runtime_error
{
public:
	Node::SyntaxNode* sn;
public:
	 BreakException(Node::SyntaxNode* sn) :sn(sn), runtime_error("BreakException") {
	}
};

class ContinueException :public runtime_error
{
public:
	Node::SyntaxNode* sn;
public:
	ContinueException(Node::SyntaxNode* sn) :sn(sn), runtime_error("ContinueException") {
	}
};
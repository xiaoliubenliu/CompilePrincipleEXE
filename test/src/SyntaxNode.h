#pragma once
#include "Token.h"
#include "tokenType.h"
#include <iostream>
#include <vector>
using namespace std;

namespace Node
{
	class SyntaxNode
	{
	public:
		SyntaxNode() {}
		virtual string toString() { return "SyntaxNode"; }
		virtual ~SyntaxNode() {}
	};

	class ProgramNode :public SyntaxNode //如果没有public那么将无法实现多态。
	{
	public:
		void addFunc(SyntaxNode* fun)
		{
			this->funvec.push_back(fun);
		}

		void addArray(SyntaxNode* arr)
		{
			this->arrayvec.push_back(arr);
		}

		string toString() { return "ProgramNode"; };
	private:
		vector<SyntaxNode*> funvec;
		vector<SyntaxNode*> arrayvec;
	};

	class FunctionNode :public SyntaxNode
	{
		Token funName;
		vector<SyntaxNode*> paramList;
		SyntaxNode* retNode;
		SyntaxNode* funBody;
	public:
		FunctionNode(Token funName) :funName(funName)
		{
		}

		void addParam(SyntaxNode* dn) {
			paramList.push_back(dn);
		}

		void setBody(SyntaxNode* funBody) {
			this->funBody = funBody;
		}

		void setRetNode(SyntaxNode* retNode) {
			this->retNode = retNode;
		}

		string toString() {
			return "Function Node" + funName.toString() + "\n";
		}
	};

	class ArrayDeclareNode :public SyntaxNode
	{
		Token kw;
		Token id;
		long length;
	public:
		ArrayDeclareNode(Token kw, Token id, long length) :kw(kw), id(id), length(length) {
		}

		string toString() {
			return "ArrayDeclareNode " + this->kw.toString() + " " + id.toString() + "[" + to_string(length) + "]\n";
		}
	};

	class DeclareNode :public SyntaxNode
	{
		Token kw;
		Token id;
	public:
		DeclareNode(Token kw, Token id) :kw(kw), id(id)
		{
		}

		string toString() {
			return "声明 " + kw.toString() + " " + id.toString() + "\n";
		}
	};

	class CodeBlockNode :public SyntaxNode
	{

	};

	class ExprListNode :public SyntaxNode
	{
		vector<SyntaxNode* > exprVec;
		SyntaxNode* father;
	public:
		ExprListNode(SyntaxNode* father) 
		{
			father = father;
		}

		void add(SyntaxNode* node) {
			exprVec.push_back(node);
		}

		string toString() {
			return "ExprListNode " + to_string(exprVec.size()) + "\n";
		}
	};

	class retNode :public SyntaxNode
	{
		ETokenType _retType;
	public:
		retNode(ETokenType rettype) :_retType(rettype)
		{
		}
	};

	class ReturnNode :public SyntaxNode 
	{
		Token t;
		SyntaxNode* expr;
	public:
		ReturnNode(Token t, SyntaxNode* expr) :t(t), expr(expr)
		{
		}

		string toString() {
			return "ReturnNode " + t.toString() + "\n";
		}
	};

	class BinaryOPNode :public SyntaxNode 
	{
		Token t;
		SyntaxNode* left;
		SyntaxNode* right;
	public:
		BinaryOPNode(SyntaxNode* left, Token t, SyntaxNode* right) :left(left), right(right), t(t)
		{
		}

		string toString() {
			return "BinaryOPNode " + left->toString() + t.toString() + right->toString() + "\n";
		}
	};

	class StringNode :public SyntaxNode 
	{
		Token t;
	public:
		StringNode(Token t) :t(t)
		{
		}

		string toString() {
			return "StringNode " + t.toString() + "\n";
		}
	};

	class BoolNode :public SyntaxNode
	{
		 Token t;
	public:
		 BoolNode(Token t) :t(t)
		 {
		 }

		 string toString() {
			return "BoolNode " + t.toString() + "\n";
		 }
	};

	class NumberNode :public SyntaxNode
	{
		Token t;
	public:
		NumberNode(Token t):t(t)
		{
		}

		string toString() {
			return "NumberNode " + t.toString() + "\n";
		}
	};

	class ContinueNode :public SyntaxNode
	{
		Token t;
	public:
		 ContinueNode(Token t):t(t) {
		}

		string toString() {
			return "ContinueNode " + t.toString() + "\n";
		}
	};

	class BreakNode :public SyntaxNode {
		Token t;
	public:
		BreakNode(Token t) :t(t)
		{
		}

		string toString() {
			return "BreakNode " + t.toString() + "\n";
		}
	};


	class WhileNode :public SyntaxNode {
		SyntaxNode* logicExpr;
		SyntaxNode* whileExpr;
	public:
		WhileNode(SyntaxNode* logicExpr, SyntaxNode* whileExpr) {
			logicExpr = logicExpr;
			whileExpr = whileExpr;
		}

		string toString() {
			return "whileNode \n";
		}
	};

	class IfElseNode:public SyntaxNode {
		SyntaxNode* logicExpr;
		SyntaxNode* ifExpr;
		SyntaxNode* elseExpr;
		Token ifToken;
	public:
		IfElseNode(SyntaxNode* logicExpr, SyntaxNode* ifExpr, SyntaxNode* elseExpr, Token ifToken) :
			logicExpr(logicExpr),
			ifExpr(ifExpr),
			elseExpr(elseExpr),
			ifToken(ifToken)
		{
		}

		string toString() {
			return "IfElseNode " + ifToken.toString() + "\n";
		}
	};

	class PrintNode :public SyntaxNode {
		SyntaxNode* exprForPrint;
	public:
		PrintNode(SyntaxNode* exprForPrint) {
			exprForPrint = exprForPrint;
		}

		string toString() {
			return "打印 " + exprForPrint->toString() + "\n";
		}
	};

	class AssignNode :public SyntaxNode {
		SyntaxNode* left;
		SyntaxNode* right;
	public:
		AssignNode(SyntaxNode* left, SyntaxNode* right) :left(left), right(right)
		{
		}

		string toString() {
			return "AssignNode " + left->toString() + "=" + right->toString() + "\n";
		}
	};

	class VariableNode :public SyntaxNode
	{
		Token id;
	public:
		VariableNode(Token t) :id(t) {
		}

		string toString() {
			return "VariableNode " + id.toString() + "\n";
		}
	};

	class ArrayAssignNode :public SyntaxNode {
		Token id;
		SyntaxNode* index;
		SyntaxNode* value;

	public:
		ArrayAssignNode(Token id, SyntaxNode* index, SyntaxNode* value)
			:id(id), index(index), value(value)
		{
		}

		string toString() {
			return "ArrayAssignNode " + id.toString() + "\n";
		}
	};

	class CallNode :public SyntaxNode {
		Token funName;
		vector<SyntaxNode*> params;
	public:
		CallNode(Token funName) :funName(funName) {
		}

		void addParam(SyntaxNode* param) {
			params.push_back(param);
		}

		string toString() {
			return "CallNode " + funName.toString() + " " + "\n";
		}
	};

	class DeclareMULNode :public SyntaxNode {
		Token kw;
		vector<SyntaxNode*> assignList;
	public:
		DeclareMULNode(Token kw) :kw(kw) {
		}

		string toString() {
			return "DeclareMULNode 声明 " + kw.toString() + "\n";
		}

		void add(SyntaxNode* node)
		{
			assignList.push_back(node);
		}
	};

	class ArrayValueNode :public SyntaxNode {
		Token id;
		SyntaxNode* index;
	public:
		ArrayValueNode(Token id, SyntaxNode* index) :id(id), index(index) {
		}

		string toString() {
			return "ArrayValueNode " + id.toString() + "\n";
		}
	};

	class ReadNode :public SyntaxNode {
		ReadNodeType rnt;
	public:
		ReadNode(ReadNodeType rnt) :rnt(rnt) {
		}
	};

	class UnaryOPNode :public SyntaxNode {
		Token t;
		SyntaxNode* right;

	public:
		UnaryOPNode(Token t, SyntaxNode* right) :right(right), t(t) {
		}

		string toString() 
		{
			return "UnaryOPNode " + t.toString() + "\n";
		}
	};
}
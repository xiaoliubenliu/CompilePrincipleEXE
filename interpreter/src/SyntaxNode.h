#pragma once
#include "Token.h"
#include "tokenType.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
using namespace std;

namespace Node
{
	class SyntaxNode
	{
	public:
		SyntaxNode() {}
		virtual string toString() { return "SyntaxNode"; }
		virtual void print(int width=0,ostream& out = std::cout) { out <<setw(width)<<" "<< "rootNode" << ""; }
		virtual ~SyntaxNode() {}
	};

	class ProgramNode :public SyntaxNode //如果没有public那么父类的方法会直接被private
	{
	public:
		vector<SyntaxNode*> funvec;
		vector<SyntaxNode*> arrayvec;
	public:
		void addFunc(SyntaxNode* fun)
		{
			this->funvec.push_back(fun);
		}

		void addArray(SyntaxNode* arr)
		{
			this->arrayvec.push_back(arr);
		}

		void print(int width=0,ostream& out = std::cout) override
		{
			out << setw(width) << " "<< toString()<<"::" << endl;
			out << setw(width) <<"" << "{" << endl;
			for (auto& item : funvec)
			{
				if (item != nullptr)
					item->print(width+4,out);
			}
			for (auto& item : arrayvec)
			{
				if (item != nullptr)
					item->print(width + 4,out);
			}
			out << setw(width) << " "<<"}" << endl;
		}

		string toString() { return "Global"; };
		vector<SyntaxNode*>& getFunvec() { return funvec; }
		vector<SyntaxNode*>& getArrayvec() { return arrayvec; }

	};

	class FunctionNode :public SyntaxNode
	{
	public:
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

		void print(int width=0,ostream& out = std::cout) override
		{
			out << setw(width) << " " << toString() << endl;
			out << setw(width) << " " << "{" << endl;

			for (auto& item : paramList)
			{
				if (item != nullptr)
					item->print(width + 4,out);
			}
			retNode->print(width + 4,out);
			funBody->print(width + 4,out);
			out << setw(width) << " " << "}" << endl;
		}

		string toString() {
			return "FunctionNode" + funName.toString();
		}
	};

	class ArrayDeclareNode :public SyntaxNode
	{
	public:
		Token kw;
		Token id;
		long length;
	public:
		ArrayDeclareNode(Token kw, Token id, long length) :kw(kw), id(id), length(length) {
		}

		void print(int width=0,ostream& out = std::cout) override
		{
			out << setw(width) << "" << toString() << endl;
		}

		string toString() {
			return "ArrayDeclareNode " + this->kw.toString() + " " + id.toString() + "[" + to_string(length) + "]";
		}
	};

	class DeclareNode :public SyntaxNode
	{
	public:
		Token kw;
		Token id;
	public:
		DeclareNode(Token kw, Token id) :kw(kw), id(id)
		{
		}

		void print(int width=0,ostream& out = std::cout) override
		{
			out << setw(width) << "" <<toString() << endl;
		}

		string toString() {
			return "DeclareNode " + kw.toString() + " " + id.toString();
		}
	};

	class ExprListNode :public SyntaxNode
	{
	public:
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

		void print(int width=0,ostream& out = std::cout) override
		{
			out << setw(width)<<"" << toString() << endl;
			out << setw(width)<<"" << "{" << endl;
			for (auto& item : exprVec)
			{
				if (item != nullptr)
				{
					item->print(width+4 ,out);
				}
			}
			out << setw(width) << "" << "}" << endl;
		}

		string toString() {
			return "ExprListNode ";
		}
	};

	class retNode :public SyntaxNode
	{
	public:
		Token _retType;
	public:
		retNode(Token retType) :_retType(retType)
		{
		}

		void print(int width=0,ostream& out = std::cout) override
		{
			out << setw(width) << " " <<"return:" << toString() << endl;
		}

		string toString() override
		{
			return _retType.toString();
		}
	};

	class ReturnNode :public SyntaxNode 
	{
	public:
		Token t;
		SyntaxNode* expr;
	public:
		ReturnNode(Token t, SyntaxNode* expr) :t(t), expr(expr)
		{
		}

		void print(int width=0,ostream& out = std::cout) override
		{
			out << setw(width) << "" << toString() << endl;
			out << setw(width) << "{" << endl;
			if (expr != nullptr)
				expr->print(width+4,out);
			out << setw(width) << "}" << endl;
		}

		string toString() {
			return "ReturnNode " + t.toString() ;
		}
	};

	class BinaryOPNode :public SyntaxNode 
	{
	public:
		Token t;
		SyntaxNode* left;
		SyntaxNode* right;
	public:
		void print(int width=0,ostream& out = std::cout) override
		{
			out << setw(width) << "" << toString() << endl;
			out << setw(width) << "{" << endl;
			if (left != nullptr)
				left->print(width + 4,out);
			if (right != nullptr)
				right->print(width + 4,out);
			out << setw(width) << "}" << endl;
		}

		BinaryOPNode(SyntaxNode* left, Token t, SyntaxNode* right) :left(left), right(right), t(t)
		{
		}

		string toString() {
			return "BinaryOPNode " + left->toString() + t.toString() + right->toString();
		}
	};

	class StringNode :public SyntaxNode 
	{
	public:
		Token t;
	public:
		StringNode(Token t) :t(t)
		{
		}

		void print(int width=0,ostream& out = std::cout) override
		{
			out << setw(width) << "" << toString() << endl;
		}

		string toString() {
			return "StringNode " + t.toString() ;
		}
	};

	class BoolNode :public SyntaxNode
	{
	public:
		 Token t;
	public:
		 BoolNode(Token t) :t(t)
		 {
		 }

		 void print(int width=0,ostream& out = std::cout) override
		 {
			 out << setw(width) << "" << toString() << endl;
		 }
		 string toString() {
			return "BoolNode " + t.toString() ;
		 }
	};

	class NumberNode :public SyntaxNode
	{
	public:
		Token t;
	public:
		NumberNode(Token t):t(t)
		{
		}

		void print(int width=0,ostream& out = std::cout) override
		{
			out << setw(width) << "" << toString() << endl;
		}
		string toString() {
			return "NumberNode " + t.toString() ;
		}
	};

	class ContinueNode :public SyntaxNode
	{
		Token t;
	public:
		 ContinueNode(Token t):t(t) {
		}

		 void print(int width=0,ostream& out = std::cout) override
		 {
			 out << setw(width) << "" << toString() << endl;
		 }
		string toString() {
			return "ContinueNode " + t.toString() ;
		}
	};

	class BreakNode :public SyntaxNode {
		Token t;
	public:
		BreakNode(Token t) :t(t)
		{
		}

		void print(int width=0,ostream& out = std::cout) override
		{
			out << setw(width) << "" << toString() << endl;
		}
		string toString() {
			return "BreakNode " + t.toString() ;
		}
	};


	class WhileNode :public SyntaxNode {
	public:
		SyntaxNode* logicExpr;
		SyntaxNode* whileExpr;
	public:
		WhileNode(SyntaxNode* logicExpr, SyntaxNode* whileExpr):logicExpr(logicExpr),whileExpr(whileExpr) {
		}

		void print(int width=0,ostream& out = std::cout) override
		{
			out << setw(width) << "" << toString() << endl;
			out << setw(width) << "" << "{" << endl;
			if(logicExpr!=nullptr)
				logicExpr->print(width + 4,out);
			if(whileExpr !=nullptr)
				whileExpr->print(width + 4,out);
			out << setw(width) << "" << "}" << endl;
		}
		string toString() {
			return "whileNode ";
		}
	};

	class IfElseNode:public SyntaxNode {
	public:
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

		void print(int width=0,ostream& out = std::cout) override
		{
			out << setw(width) << "" << toString() << endl;
			out << setw(width) << "" << "{" << endl;
			logicExpr->print(width + 4,out);
			ifExpr->print(width + 4,out);
			if(elseExpr!=nullptr)
				elseExpr->print(width + 4,out);
			out << setw(width) << "" << "}" << endl;
		}

		string toString() {
			return "IfElseNode " + ifToken.toString() ;
		}
	};

	class PrintNode :public SyntaxNode {
	public:
		SyntaxNode* exprForPrint;
	public:
		PrintNode(SyntaxNode* exprForPrint):exprForPrint(exprForPrint) {
		}

		void print(int width=0,ostream& out = std::cout) override
		{
			out << setw(width) << "" << toString() << endl;
			out << setw(width) << "" << "{" << endl;
			exprForPrint->print(width+4 ,out);
			out << setw(width) << "" << "}" << endl;
		}
		string toString() {
			return "打印 " + exprForPrint->toString() ;
		}
	};

	class AssignNode :public SyntaxNode {
	public:
		SyntaxNode* left;
		SyntaxNode* right;
	public:
		AssignNode(SyntaxNode* left, SyntaxNode* right) :left(left), right(right)
		{
		}

		void print(int width=0,ostream& out = std::cout) override
		{
			out << setw(width) << "" << toString() << endl;
			out << setw(width) << "" << "{" << endl;
			left->print(width + 4,out);
			right->print(width + 4,out);
			out << setw(width) << "" << "}" << endl;
		}
		string toString() {
			return "AssignNode " + left->toString() + "=" + right->toString() ;
		}
	};

	class VariableNode :public SyntaxNode
	{
	public:
		Token id;
	public:
		VariableNode(Token t) :id(t) {
		}

		void print(int width=0,ostream& out = std::cout) override
		{
			out << setw(width) << "" << toString() << endl;
		}

		string toString() {
			return "VariableNode " + id.toString() ;
		}
	};

	class ArrayAssignNode :public SyntaxNode {
	public:
		Token id;
		SyntaxNode* index;
		SyntaxNode* value;
	public:
		ArrayAssignNode(Token id, SyntaxNode* index, SyntaxNode* value)
			:id(id), index(index), value(value)
		{
		}

		void print(int width=0,ostream& out = std::cout) override
		{
			out << setw(width) << "" << toString() << endl;
			out << setw(width) << "" << "{" << endl;
			index->print(width + 4,out);
			value->print(width + 4,out);
			out << setw(width) << "" << "}" << endl;
		}
		string toString() {
			return "ArrayAssignNode " + id.toString() ;
		}
	};

	class CallNode :public SyntaxNode {
	public:
		Token funName;
		vector<SyntaxNode*> params;
	public:
		CallNode(Token funName) :funName(funName) {
		}

		void addParam(SyntaxNode* param) {
			params.push_back(param);
		}

		void print(int width=0,ostream& out = std::cout) override
		{
			out << setw(width) << "" << toString() << endl;
			out << setw(width) << "" << "{" << endl;
			for (auto& item : params)
			{
				if (item != nullptr)
					item->print(width + 4,out);
			}
			out << setw(width) << "" << "}" << endl;
		}
		string toString() {
			return "CallNode " + funName.toString() + " " ;
		}
	};

	class DeclareMULNode :public SyntaxNode {
	public:
		Token kw;
		vector<SyntaxNode*> assignList;
	public:
		DeclareMULNode(Token kw) :kw(kw) {
		}

		string toString() {
			return "DeclareMULNode " + kw.toString() ;
		}

		void print(int width=0,ostream& out = std::cout) override
		{
			out << setw(width) << "" << toString() << endl;
			out << setw(width) << "" << "{" << endl;
			for (auto& item : assignList)
			{
				if (item != nullptr)
					item->print(width + 4,out);
			}
			out << setw(width) << "" << "}" << endl;
		}
		void add(SyntaxNode* node)
		{
			assignList.push_back(node);
		}
	};

	class ArrayValueNode :public SyntaxNode {
	public:
		Token id;
		SyntaxNode* index;
	public:
		ArrayValueNode(Token id, SyntaxNode* index) :id(id), index(index) {
		}


		void print(int width=0,ostream& out = std::cout) override
		{
			out << setw(width) << "" << toString() << endl;
			index->print(width + 4,out);
		}
		string toString() {
			return "ArrayValueNode " + id.toString() ;
		}
	};

	class ReadNode :public SyntaxNode {
	public:
		ReadNodeType rnt;
	public:
		ReadNode(ReadNodeType rnt) :rnt(rnt) {
		}

		void print(int width=0,ostream& out = std::cout) override
		{
			out << setw(width) << "" << toString() << endl;
		}
		string toString()override
		{
			return "ReadNodeType";
		}
	};

	class UnaryOPNode :public SyntaxNode {
	public:
		Token t;
		SyntaxNode* right;
	public:
		UnaryOPNode(Token t, SyntaxNode* right) :right(right), t(t) {
		}

		string toString() 
		{
			return "UnaryOPNode " + t.toString() ;
		}
	};
}
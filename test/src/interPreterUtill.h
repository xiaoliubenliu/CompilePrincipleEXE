#pragma once
#include "tokenType.h"
#include "Token.h"
#include <iostream>
#include "Error.h"
#include "runresult.h"
#include <unordered_map>
using namespace Result;

namespace interPreterUtil
{
	class GlobalArray
	{
	public:
		VariableType type;
		vector<int> intV;
		vector<float> floatV;
		vector<std::string> stringV;
		vector<bool> boolV;
	public:
		GlobalArray(string typeStr, int length);
		void setValue(long index, NumRunResult* nrr, string stringV, bool boolV);
	};

	class Variable {
	public:
		VariableType type;
		long intV;
		double floatV;
		string stringV;
		bool boolV;
		bool initFlag;

		Variable(string typeStr);

		Variable(VariableType type) :type(type), initFlag(false) {
		}

		void setValue(NumRunResult* nrr, string stringV, bool boolV);

		RunResult* getValue();
	};

	enum SingleRunStackType {
		FUN,
		WHILE,
		OTHER
	};

	class SingleRunStack
	{
	public:
		unordered_map<string, Variable*> currVmap;
		SingleRunStackType type;

		SingleRunStack(SingleRunStackType type) :type(type) {
		}
	};

	class RunStackUtil {
	public:
		vector<SingleRunStack*> callStack;
		int top;
	public:
		RunStackUtil() { callStack.clear();  top = -1; }

		void push(SingleRunStack* currRS) { callStack.push_back(currRS); ++top; }

		void pop() { callStack.pop_back(); --top;}

		SingleRunStack* getTop() { return callStack[top];}

		bool isVarExist(string varName);

		void addVar(string varName, Variable* v) {callStack[top]->currVmap[varName] = v;}

		Variable* getVar(string varName);
	};

}
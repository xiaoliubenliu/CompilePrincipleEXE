#include "interPreterUtill.h"

namespace interPreterUtil
{
	GlobalArray::GlobalArray(string typeStr, int length)
	{
		if (typeStr == "int") {
			this->type = INT;
			intV.resize(length,0);
		}
		else if (typeStr == "float") {
			floatV.resize(length,0.0f);
			this->type = FLOAT;
		}
		else if (typeStr == "string") {
			stringV.resize(length,"");
			this->type = STRING;
		}
		else if (typeStr == "bool") {
			boolV.resize(length, false);
			this->type = BOOL;
		}
	}
	void GlobalArray::setValue(long index, NumRunResult* nrr, string stringv, bool boolv)
	{
		if (this->type == INT) 
		{
			intV.at((int)index) = nrr->intV;
		}
		else if (this->type == FLOAT) {
			floatV.at((int)index) = nrr->floatV;
		}
		else if (this->type == STRING) {
			stringV.at((int)index) = stringv;
		}
		else if (this->type == BOOL) {
			boolV.at((int)index) = boolv;
		}
	}


	Variable::Variable(string typeStr):initFlag(false) {
		if (typeStr == "int")
			this->type = VariableType::INT;
		else if (typeStr == "float")
			this->type = VariableType::FLOAT;
		else if (typeStr == "string")
			this->type = VariableType::STRING;
		else if (typeStr == "bool")
			this->type = VariableType::BOOL;
	}
	void Variable::setValue(NumRunResult* nrr, string stringV, bool boolV)
	{
		this->initFlag = true;
		if (this->type == VariableType::INT) {
			this->intV = nrr->intV;
		}
		else if (this->type == VariableType::FLOAT) {
			this->floatV = nrr->floatV;
		}
		else if (this->type == VariableType::STRING) {
			this->stringV = stringV;
		}
		else if (this->type == VariableType::BOOL) {
			this->boolV = boolV;
		}
	}
	RunResult* Variable::getValue()
	{
		if (this->type == VariableType::INT)
			return new NumRunResult(true, this->intV, 0.0);
		else if (this->type == VariableType::FLOAT)
			return new NumRunResult(false, 0L, this->floatV);
		else if (this->type == VariableType::STRING)
			return new StringRunResult(this->stringV);
		else
			return this->type == VariableType::BOOL ? new BoolRunResult(this->boolV) : nullptr;
	}
	bool RunStackUtil::isVarExist(string varName)
	{
		for (int tempIndex = this->top; !callStack[tempIndex]->currVmap.count(varName); --tempIndex) 
		{
			if (callStack[tempIndex]->type == SingleRunStackType::FUN) 
				return false;
		}
		return true;
	}
	Variable* RunStackUtil::getVar(string varName)
	{
		int tempIndex;
		for (tempIndex = top; tempIndex>=0; --tempIndex) {
			//if (callStack[tempIndex]->type == SingleRunStackType::FUN) 
			{
				if (callStack[tempIndex]->currVmap.count(varName))
				{
					auto var = callStack[tempIndex]->currVmap[varName];
					return var;
				}
				else continue;
			}
		}
	}
}

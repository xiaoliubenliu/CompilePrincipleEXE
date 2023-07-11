#pragma once
#include "tokenType.h"
#include "iostream"
#include <string>
using std::string, std::to_string;


namespace Result
{
	class RunResult {
	public:
		virtual RunResult* add(RunResult* var1) = 0;
		virtual string toString() = 0;
	};

	class NumRunResult :public RunResult
	{
	public:
		bool isInt;
		long intV;
		double floatV;
	public:
		NumRunResult(bool isInt, long intV, double floatV);

		void setNegative();

		RunResult* comp(RunResult* v, string compOP);

		RunResult* add(RunResult* v);

		NumRunResult* sub(NumRunResult* v);

		NumRunResult* mul(NumRunResult* v);

		NumRunResult* div(NumRunResult* v);

		NumRunResult* mod(NumRunResult* v);

		NumRunResult* pow(NumRunResult* v);

		inline string toString()  {return isInt ? to_string(intV) : to_string(floatV); }
	};

	class BoolRunResult :public RunResult
	{
	public:
		bool b;
	public:
		BoolRunResult(bool b) :b(b) {}

		void toNot()  { b = !b; }

		inline string toString()  {return b ? "true" : "false"; }

		RunResult* add(RunResult* v);
	};

	class StringRunResult : public RunResult 
	{
	public:
		string s;
	public:

		StringRunResult(string s):s(s) {}

		RunResult* add(RunResult* v);

		inline string toString()  {return this->s;}
	};
}

#include "runresult.h"
namespace Result
{
	NumRunResult::NumRunResult(bool isInt, long intV, double floatV)
		:isInt(isInt), intV(intV), floatV(floatV)
	{
	}

	void NumRunResult::setNegative()
	{
		if (isInt)
			intV = -intV;
		else
			floatV = -floatV;
	}

	RunResult* NumRunResult::comp(RunResult* v, string compOP)
	{
		double selfV = 0.0;
		double otherV = 0.0;
		NumRunResult* other = (NumRunResult*)v;
		if (isInt)
			selfV = (double)intV;
		else
			selfV = floatV;

		if (other->isInt)
			otherV = (double)other->intV;
		else
			selfV = other->floatV;

		if (compOP == ">") {
			return new BoolRunResult(selfV > otherV);
		}
		else if (compOP == "<") {
			return new BoolRunResult(selfV < otherV);
		}
		else if (compOP == ">=") {
			return new BoolRunResult(selfV >= otherV);
		}
		else if (compOP == "<=") {
			return new BoolRunResult(selfV <= otherV);
		}
		else if (compOP == "==") {
			return new BoolRunResult(selfV == otherV);
		}
		else {
			return compOP == "!=" ? new BoolRunResult(selfV != otherV) : nullptr;
		}
	}
	RunResult* NumRunResult::add(RunResult* v)
	{
		if (dynamic_cast<StringRunResult*>(v))
		{
			string sori = "";
			if (isInt)
				sori = "" + to_string(intV);
			else
				sori = "" + to_string(floatV);
			return new StringRunResult(sori + ((StringRunResult*)v)->s);
		}
		else if (isInt && ((NumRunResult*)v)->isInt)
			return new NumRunResult(true, intV + ((NumRunResult*)v)->intV, 0.0);
		else
		{
			auto p = ((NumRunResult*)v);
			double vl = 0.0;
			double vr = 0.0;
			if (isInt)
				vl = (double)intV;
			else
				vl = floatV;

			if (p->isInt)
				vr = (double)p->intV;
			else
				vr = p->floatV;

			return new NumRunResult(false, 0L, vl + vr);
		}
	}
	NumRunResult* NumRunResult::sub(NumRunResult* v)
	{
		if (isInt && v->isInt) {
			return new NumRunResult(true, intV - v->intV, 0.0);
		}
		else {
			double vl = 0.0;
			double vr = 0.0;
			if (isInt) {
				vl = (double)intV;
			}
			else {
				vl = floatV;
			}

			if (v->isInt) {
				vr = (double)v->intV;
			}
			else {
				vr = v->floatV;
			}

			return new NumRunResult(false, 0L, vl - vr);
		}
	}
	NumRunResult* NumRunResult::mul(NumRunResult* v)
	{
		if (isInt && v->isInt) {
			return new NumRunResult(true, intV * v->intV, 0.0);
		}
		else {
			double vl = 0.0;
			double vr = 0.0;
			if (isInt) {
				vl = (double)intV;
			}
			else {
				vl = floatV;
			}

			if (v->isInt) {
				vr = (double)v->intV;
			}
			else {
				vr = v->floatV;
			}

			return new NumRunResult(false, 0L, vl * vr);
		}
	}
	NumRunResult* NumRunResult::div(NumRunResult* v)
	{
		if (isInt && v->isInt) {
			return new NumRunResult(true, intV / v->intV, 0.0);
		}
		else {
			double vl = 0.0;
			double vr = 0.0;
			if (isInt) {
				vl = (double)intV;
			}
			else {
				vl = floatV;
			}

			if (v->isInt) {
				vr = (double)v->intV;
			}
			else {
				vr = v->floatV;
			}

			return new NumRunResult(false, 0L, vl / vr);
		}
	}
	NumRunResult* NumRunResult::mod(NumRunResult* v)
	{
		return new NumRunResult(true, fmod(intV, v->intV), 0.0);
	}
	NumRunResult* NumRunResult::pow(NumRunResult* v)
	{
		double vl;
		if (isInt && v->isInt) 
		{
			vl = std::pow((double)intV, (double)v->intV);
			return (double)round(vl) == vl ? new NumRunResult(true, (long)vl, 0.0) : new NumRunResult(false, 0L, vl);
		}
		else 
		{
			vl = 0.0;
			double vr = 0.0;
			if (isInt) {
				vl = (double)intV;
			}
			else {
				vl = floatV;
			}

			if (v->isInt) {
				vr = (double)v->intV;
			}
			else {
				vr = v->floatV;
			}

			return new NumRunResult(false, 0L, std::pow(vl, vr));
		}
	}
	RunResult* BoolRunResult::add(RunResult* v)
	{
		string sin;
		if (StringRunResult* p = dynamic_cast<StringRunResult*>(v)) {
			sin = p->s;
		}

		return new StringRunResult(b ? "true" : "false" + sin);
	}
	RunResult* StringRunResult::add(RunResult* v)
	{
		string sin = "";
		if (StringRunResult* p = dynamic_cast<StringRunResult*>(v)) {
			sin = p->s;
		}
		else if (BoolRunResult* p = dynamic_cast<BoolRunResult*>(v)) {
			sin = to_string(p->b);
		}
		else {
			NumRunResult* nrr = dynamic_cast<NumRunResult*>(v);
			if (nrr->isInt) {
				sin = to_string(nrr->intV);
			}
			else {
				sin = to_string(nrr->floatV);
			}
		}

		return new StringRunResult(this->s + sin);
	}
}




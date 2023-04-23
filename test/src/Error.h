#pragma once
#include <string>
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
	static const vector<string>& getError();
};
#include "lexer.h"
#include <iostream>
#include <fstream>
#include "SyntaxNode.h"
#include <sstream>
#include "SyntaxParser.h"

vector<Token> Parse(const char* sourceStr)
{
	stringstream ss;
	auto ret = Lexer::Get().beginSolveCodeFromStr(sourceStr);
	return ret;
}
int main()
{
	ifstream file("C:/Users/xiaoliu/Desktop/CompilationPrinciples3/test/test/src/test.txt");
	string str;
	string code;
	if (file.is_open()) {
		while (getline(file,str)) {
			code += str+'\n';
		}
		file.close();
	}
	else
	{
		cout << "file open filed" << endl;
	}

	vector<Token> ret= Parse(code.c_str());
	for (auto& item : ret)
	{
		cout << item.TokenName() << "  ";
	}

	SyntaxParser s1(ret);


	SyntaxNode* rootnode = s1.BeginParse();
	cout << "\n________________________" << endl;
	rootnode->print(0);
}


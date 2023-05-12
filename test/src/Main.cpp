#include "lexer.h"
#include <iostream>
#include <fstream>
#include "SyntaxNode.h"
#include <sstream>
#include "SyntaxParser.h"
#include "Interpreter.h"

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

	//for (auto& item : ret)
	//{
	//	cout << item.TokenName() << "  ";
	//}

	//cout << "_______________________" << endl;

	try
	{
		SyntaxParser s1(ret);
		SyntaxNode* root = s1.BeginParse();
		//root->print();

		Interpreter pareter;
		pareter.runAST((ProgramNode*)root);
	}
	catch (const std::exception&e)
	{
		cout << e.what() << endl;
	}

}


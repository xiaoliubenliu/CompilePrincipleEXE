#include "lexer.h"
#include <iostream>
#include <fstream>
#include "SyntaxNode.h"
#include <sstream>
#include "SyntaxParser.h"
#include <queue>
#include "Interpreter.h"
vector<Token> Parse(const char* sourceStr)
{
	stringstream ss;
	auto ret = Lexer::Get().beginSolveCodeFromStr(sourceStr);
	return ret;
}

std::ofstream output;
int l_flag = 0;
int p_flag = 0;


int main(int argc, char* argv[])
{
	if (argc == 1)
	{
		cout << "未指定文件" << endl;
	}
	queue<string> que;
	for (int i = 2; i < argc; i++) 
		que.push(string(argv[i]));

	while (que.size())
	{
		if (que.front() == "-l")
		{
			que.pop();
			l_flag = 1;
		}
		else if (que.front() == "-p")
		{
			que.pop();
			p_flag = 1;
		}
		else if (que.front() == "-o")
		{
			que.pop();
			if (que.size() && que.front().find('-') == string::npos)
			{
				if (output.is_open())
					output.close();
				output.open(que.front());
				que.pop();
				if (l_flag)
					l_flag++;
				if (p_flag)
					p_flag++;
			}
			else
			{
				if(output.is_open())
					output.close();
			}
			if (!output.is_open())
			{
				cout << "未指定输出目录" << endl;
				return 0;
			}
		}
		else
		{
			cout << "未识别指令" << endl;
			return 1;
		}
	}

	string filen;
	filen += argv[1];
	ifstream file(filen);
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
		return -1;
	}

	vector<Token> ret;
	SyntaxNode* root=nullptr;
	try
	{
		ret = Parse(code.c_str());

		SyntaxParser s1(ret);
		root = s1.BeginParse();

		Interpreter pareter;
		pareter.runAST((ProgramNode*)root);
	}
	catch (exception e)
	{
		cout << e.what() << endl;;
	}

	
	if (l_flag==1)
	{
		for (auto& item : ret)
			cout << item.TokenName() << "  ";
		cout << endl;
	}
	else if(l_flag>1)
	{
		for (auto& item : ret)
			output << item.TokenName() << "  ";
		cout << endl;
	}
	if (p_flag==1&&root!=nullptr)
	{
		root->print(0, cout);
		cout << endl;
	}
	else if(p_flag>1 && root != nullptr)
	{
		root->print(0, output);
		cout << endl;
	}
	std::system("pause");
}


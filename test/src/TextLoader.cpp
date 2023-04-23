#pragma once
#include "TextLoader.h"
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
using namespace std;

TextLoader& TextLoader::Get()
{
	static TextLoader instance;
	return instance;
}

bool TextLoader::LoadFromFile(string path,vector<string>& str)
{
	ifstream ifs;
	ifs.open(path);
	if (ifs.is_open())
	{
		string temp;
		while (getline(ifs, temp))
			str.emplace_back(temp);
		str.emplace_back("EOF");
		return true;
	}
	else
	{
		return false;
	}
}

bool TextLoader::LoadFromString(string code , vector<string>& str)
{
	stringstream ss(code);
	string temp;
	while (getline(ss,temp))
	{
		str.push_back(temp);
	}
	return true;
}

#pragma once
#include <string>
#include <vector>
using namespace std;
class TextLoader
{
private:
    TextLoader() {} // 私有构造函数
    TextLoader(const TextLoader&) = delete; // 禁止拷贝构造函数
    TextLoader& operator=(const TextLoader&) = delete; // 禁止赋值构造函数
public:
	static TextLoader& Get();
    static bool LoadFromFile(string path, vector<string>& str);
    static bool LoadFromString(string code, vector<string>& str);
};
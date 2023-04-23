#pragma once
#include <string>
#include <vector>
using namespace std;
class TextLoader
{
private:
    TextLoader() {} // ˽�й��캯��
    TextLoader(const TextLoader&) = delete; // ��ֹ�������캯��
    TextLoader& operator=(const TextLoader&) = delete; // ��ֹ��ֵ���캯��
public:
	static TextLoader& Get();
    static bool LoadFromFile(string path, vector<string>& str);
    static bool LoadFromString(string code, vector<string>& str);
};
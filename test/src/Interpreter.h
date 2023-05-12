#pragma once
#include "SyntaxNode.h"
#include <unordered_map>
#include <iostream>
#include <string>
#include "interPreterUtill.h"
#include "tokenType.h"
#include "runresult.h"
#include <unordered_set>

using namespace Node;
using namespace interPreterUtil;
using namespace Result;

static inline string ltrim(std::string s) {
    s.erase(0, s.find_first_not_of(" \t\r\n"));
    return s;
}
static inline string rtrim(std::string s) {
    s.erase(s.find_last_not_of(" \t\r\n") + 1);
    return s;
}
static inline string trim(std::string s) {
    return rtrim(ltrim(s));
}

class Interpreter
{
    ETokenType currfuncRetType;
    RunStackUtil runStack;
    inline string error(Token t,string msg)  { return to_string(t.getLine()) + "行" + to_string(t.getCol()) + "列" + msg; }
    unordered_map<string, GlobalArray*> ArrayMap;
    unordered_map<string, FunctionNode*> funMap; 
public:
    void runAST(ProgramNode* root); //解析语法树

    RunResult* traversalASTForRun(SyntaxNode* root, SingleRunStackType srstUpper);

    RunResult* callFun(CallNode* root);

    RunResult* traversalArithORStrORBoolExprForRun(SyntaxNode* root);

    void processAssignNode(AssignNode* an);

    void processArrayAssignNode(ArrayAssignNode* aan);

    void newVarable(string typeStr, string vName, SyntaxNode* root);
};
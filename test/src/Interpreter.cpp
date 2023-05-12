#include "Interpreter.h"

void Interpreter::runAST(ProgramNode* root)
{
    ProgramNode* pn = (ProgramNode*)root;
    auto& arr = pn->getArrayvec();  //获取全局数组。

    for (auto it = arr.begin(); it != arr.end(); ++it)
    {
        ArrayDeclareNode* an = (ArrayDeclareNode*)(*it);
        auto kwstr = trim(an->id.value().value());
        if (ArrayMap.count(kwstr) != 0)
            Error::AddError(error(an->id,"数组名" + kwstr + "已经存在"));
        else
        {
            GlobalArray* ga = new GlobalArray(an->kw.value().value(), an->length);
            ArrayMap[kwstr] = ga;
        }
    }

    bool hasMainFun = false;
    unordered_set<string> fname;
    auto& pnarr = pn->getFunvec();
    for (auto it = pnarr.begin(); it != pnarr.end(); ++it)
    {
        FunctionNode* fn = (FunctionNode*)(*it);
        auto funcstr = trim(fn->funName.value().value());

        if (fname.count(funcstr)) 
            Error::AddError(error(fn->funName, "方法不能重名"));

        if (funcstr=="main") 
        {
            hasMainFun = true;
            if (fn->paramList.size() != 0) 
                Error::AddError(error(fn->funName,"main方法不允许有参数！"));
        }

        fname.insert(funcstr);
        funMap[funcstr]= fn;
    }
    if (!hasMainFun)
    {
        Error::AddError("缺少main入口！");
        for (auto item : Error::getError())
        {
            cout << item << endl;
        }
        Error::ClearError();
    }
    else 
    {
        //main 函数栈。
        SingleRunStack* currRS = new SingleRunStack(SingleRunStackType::FUN);
        runStack.push(currRS);


         //开始执行代码；
            traversalASTForRun((SyntaxNode*)funMap["main"], SingleRunStackType::FUN);
        //catch (exception e) 
        //{

        //    cout << e.what();
        //}
    }
}

//解释执行。
RunResult* Interpreter::traversalASTForRun(SyntaxNode* root, SingleRunStackType srstUpper)
{
    if (dynamic_cast<CallNode*>(root)) 
       
        return callFun((CallNode*)root);
    else if (dynamic_cast<FunctionNode*>(root)) 
    {
        FunctionNode* fn = (FunctionNode*)root;
        return traversalASTForRun(fn->funBody,  SingleRunStackType::FUN);
    }
    else {
        if (dynamic_cast<AssignNode*>(root)) {
            AssignNode* an = (AssignNode*)root;
            processAssignNode(an);
        }
        else if (dynamic_cast<ArrayAssignNode*>(root)) 
        {
            ArrayAssignNode* aan = (ArrayAssignNode*)root;
            processArrayAssignNode(aan);
        }
        else if (dynamic_cast<DeclareNode*> (root)) 
        {
            DeclareNode* dn = (DeclareNode*)root;
            newVarable(trim(dn->kw.value().value()), trim(dn->id.value().value()), root);
        }
        else if (dynamic_cast<DeclareMULNode*>(root))
        {
            DeclareMULNode* dmn = (DeclareMULNode*)root;
            string typeStr = trim(dmn->kw.value().value());

            for (auto it = dmn->assignList.begin(); it != dmn->assignList.end(); ++it)
            {
                SyntaxNode* sn = (SyntaxNode*)(*it);
                VariableNode* v = nullptr;
                if (dynamic_cast<AssignNode*> (sn)) 
                {
                    AssignNode* an = (AssignNode*)sn;
                    v = (VariableNode*)an->left;
                    newVarable(typeStr, trim(v->id.value().value()), root);
                    processAssignNode(an);
                }
                else {
                    v = (VariableNode*)sn;
                    newVarable(typeStr, trim(v->id.value().value()), root);
                }
            }
        }
        else {
            RunResult* rr;
            if (dynamic_cast<PrintNode*>(root)) 
            {
                PrintNode* pn = (PrintNode*)root;
                if (
                    !dynamic_cast<BinaryOPNode*>(pn->exprForPrint) && !dynamic_cast<NumberNode*>(pn->exprForPrint)
                    && !dynamic_cast<UnaryOPNode*>(pn->exprForPrint) && !dynamic_cast<VariableNode*>(pn->exprForPrint)
                    && !dynamic_cast<StringNode*>(pn->exprForPrint) && !dynamic_cast<BoolNode*>(pn->exprForPrint)
                    && !dynamic_cast<CallNode*>(pn->exprForPrint) && !dynamic_cast<ArrayValueNode*>(pn->exprForPrint)
                    && !dynamic_cast<ReadNode*>(pn->exprForPrint)
                    ) 
                {
                    throw runtime_error("只能打印表达式" + root->toString());
                }

                rr = traversalArithORStrORBoolExprForRun(pn->exprForPrint);
                //TODO::to solove print to window;
                string str = rr->toString();
                std::size_t pos = str.find("\\n");
                if (pos != std::string::npos) {
                    str.replace(pos, str.length(), "\n");
                }

                std::cout << str;
            }
            else if (dynamic_cast<ExprListNode*>(root))
            {
                ExprListNode* eln = (ExprListNode*)root;
                if (eln->father == nullptr) 
                {
                    SingleRunStack* srs = new SingleRunStack(srstUpper);
                    runStack.push(srs);
                }
                for (auto it = eln->exprVec.begin(); it != eln->exprVec.end(); ++it)
                {
                    SyntaxNode* sn = (SyntaxNode*)(*it);
                    traversalASTForRun(sn, SingleRunStackType::OTHER);
                }
                if (eln->father == nullptr) 
                {
                    runStack.pop();
                }
            }
            else if (dynamic_cast<IfElseNode*> (root) )
            {
                IfElseNode* ien = (IfElseNode*)root;
                rr = traversalArithORStrORBoolExprForRun(ien->logicExpr);
                if (!(dynamic_cast<BoolRunResult*>(rr))) 
                {
                    throw runtime_error("需要逻辑表达式" + root->toString());
                }

                if (((BoolRunResult*)rr)->b) 
                {
                    traversalASTForRun(ien->ifExpr, SingleRunStackType::OTHER);
                }
                else if (ien->elseExpr != nullptr) 
                {
                    traversalASTForRun(ien->elseExpr, SingleRunStackType::OTHER);
                }
            }
            else 
            {
                if (!dynamic_cast<WhileNode*>(root)) 
                {
                    if (dynamic_cast<BreakNode*>(root)) 
                    {
                        while (runStack.getTop()->type != SingleRunStackType::WHILE) 
                        {
                            runStack.pop();
                        }

                        runStack.pop();
                        throw BreakException(root);
                    }

                    if (dynamic_cast<ContinueNode*>(root)) 
                    {
                        while (runStack.getTop()->type != SingleRunStackType::WHILE)
                        {
                            runStack.pop();
                        }

                        runStack.pop();
                        throw ContinueException(root);
                    }

                    if (!dynamic_cast<ReturnNode*>(root)) 
                    {
                        throw runtime_error("语法错误" + root->toString());
                    }

                    ReturnNode* rn = (ReturnNode*)root;
                    if (rn->t.type() == ETokenType::_RETURN&&currfuncRetType==_VOID)
                    {
                        while (runStack.getTop()->type != SingleRunStackType::FUN) 
                        {
                            runStack.pop();
                        }

                        throw ReturnException(nullptr);
                    }

                    rr = traversalArithORStrORBoolExprForRun(rn->expr);

                    while (runStack.getTop()->type != SingleRunStackType::FUN) {
                        runStack.pop();
                    }

                    throw ReturnException(rr);
                }

                WhileNode* wn = (WhileNode*)root;

                while (true) 
                {
                    if (!wn->whileExpr) return nullptr;
                    rr = traversalArithORStrORBoolExprForRun(wn->logicExpr);
                    if (!(dynamic_cast<BoolRunResult*> (rr)))
                    {
                        throw runtime_error("需要逻辑表达式" + root->toString());
                    }

                    BoolRunResult* brr = (BoolRunResult*)rr;
                    if (!brr->b) {
                        break;
                    }

                    try {
                        traversalASTForRun(wn->whileExpr, SingleRunStackType::WHILE);
                    }
                    catch (BreakException e) {
                        break;
                    }
                    catch (ContinueException e) {
                    }
                }
            }
        }

        return nullptr;
    }
}

RunResult* Interpreter::callFun(CallNode* root)
{
    if (Error::getError().size() != 0)
    {
        cout << Error::PrintError();
        throw runtime_error("runtime:存在错误,程序终止。");
    }

    CallNode* cn = root;
    auto funstr = trim(root->funName.value().value());
    if (!funMap.count(funstr)) 
        throw runtime_error(error(cn->funName, "方法" + funstr + "未声明！"));
    else 
    {
        FunctionNode* fn = (FunctionNode*)funMap[funstr];
        if (fn->paramList.size() != root->params.size()) 
            throw runtime_error("参数数量不匹配 需要" + to_string(fn->paramList.size()) + "个提供了" + to_string(root->params.size()) + "个" + root->toString());
        else {
            SingleRunStack* currRSTemp = new SingleRunStack(SingleRunStackType::FUN);

            for (int i = 0; i < fn->paramList.size(); ++i) 
            {
                //获取参数并在栈内创建变量。进行参数类型匹配判断
                DeclareNode* dn = (DeclareNode*)fn->paramList[i];
                string typeStr = trim(dn->kw.value().value());  
                Variable* v = new Variable(typeStr);
                string nameStr = trim(dn->id.value().value());

                RunResult* rr = traversalArithORStrORBoolExprForRun((SyntaxNode*)cn->params[i]);
                if (typeStr=="int" || typeStr=="float") 
                {
                    if (!(dynamic_cast<NumRunResult*>(rr)))
                    {
                        throw runtime_error("第" + to_string(i + 1) + "个参数类型不匹配" + cn->funName.toString());
                    }

                    NumRunResult* nrr = (NumRunResult*)rr;
                    if (typeStr=="int" && !nrr->isInt) 
                    {
                        throw runtime_error("第" + to_string(i + 1) + "个参数类型不匹配" + cn->funName.toString());
                    }

                    v->setValue((NumRunResult*)rr, "", false);
                    currRSTemp->currVmap[nameStr]=v;
                }

                if (typeStr=="string") 
                {
                    if (!(dynamic_cast<StringRunResult*>(rr))) {
                        throw runtime_error("第" + to_string(i + 1) + "个参数类型不匹配" + cn->funName.toString());
                    }

                    StringRunResult* srr = (StringRunResult*)rr;
                    v->setValue(nullptr, srr->s, false);
                    currRSTemp->currVmap[nameStr]= v;
                }

                if (typeStr=="bool") 
                {
                    if (!dynamic_cast<BoolRunResult*>(rr)) 
                        throw runtime_error("第" + to_string(i + 1) + "个参数类型不匹配" + cn->funName.toString());

                    BoolRunResult* brr = (BoolRunResult*)rr;
                    v->setValue(nullptr, "", brr->b);
                    currRSTemp->currVmap[nameStr]=v;
                }
            }

            RunResult* result = nullptr;
            runStack.push(currRSTemp);

            try 
            {
                currfuncRetType = ((retNode*)fn->retNode)->_retType.type();//kw  或者 void
                result = traversalASTForRun(fn, SingleRunStackType::FUN);
            }
            catch (ReturnException var12) {
                result = var12.result;
            }

            runStack.pop();
            return result;
        }
    }
}

RunResult* Interpreter::traversalArithORStrORBoolExprForRun(SyntaxNode* root)
{
    if (dynamic_cast<CallNode*>(root))
        return callFun((CallNode*)root);
    else if (dynamic_cast<NumberNode*>(root))
    {
        NumberNode* nn = (NumberNode*)root;
        return nn->t.type() == ETokenType::_INT ? new NumRunResult(true,stol(nn->t.value().value()), 0.0) : new NumRunResult(false, 0L, (double)stol(nn->t.value().value()));
    }
    else if (dynamic_cast<StringNode*>(root)) {
        return new StringRunResult(((StringNode*)root)->t.value().value());
    }
    else if (dynamic_cast<BoolNode*>(root)) 
    {
        bool b = trim(((BoolNode*)root)->t.value().value())== "true";
        return new BoolRunResult(b);
    }
    else {
        RunResult* rrr;
        if (dynamic_cast<UnaryOPNode*>(root)) 
        {
            UnaryOPNode* uon = (UnaryOPNode*)root;
            rrr = traversalArithORStrORBoolExprForRun(((UnaryOPNode*)root)->right);
            if (dynamic_cast<BoolRunResult*>(rrr)) 
            {
                BoolRunResult* brr = (BoolRunResult*)rrr;
                if (uon->t.type() == ETokenType::_NOT) 
                {
                    brr->toNot();
                    return brr;
                }
                else 
                {
                    throw runtime_error("类型和运算不匹配" + ((UnaryOPNode*)root)->t.toString());
                }
            }
            else if (!(dynamic_cast<NumRunResult*>(rrr))) 
            {
                throw runtime_error("类型和运算不匹配" + ((UnaryOPNode*)root)->t.toString());
            }
            else {
                NumRunResult* v = (NumRunResult*)rrr;
                if (uon->t.type() == ETokenType::_PLUS) 
                {
                    return v;
                }
                else {
                    v->setNegative();
                    return v;
                }
            }
        }
        else {
            if (dynamic_cast<BinaryOPNode*>(root))
            {
                RunResult* rrl = traversalArithORStrORBoolExprForRun(((BinaryOPNode*)root)->left);
                rrr = traversalArithORStrORBoolExprForRun(((BinaryOPNode*)root)->right);
                BinaryOPNode* bon = (BinaryOPNode*)root;
                if (dynamic_cast<BoolRunResult*>(rrl) && !(dynamic_cast<BoolRunResult*>(rrr) && bon->t.type() != ETokenType::_PLUS) )
                {
                    throw runtime_error("布尔值只能和布尔值进行非加法运算" + ((BinaryOPNode*)root)->t.toString());
                }

                if (dynamic_cast<BoolRunResult*>(rrr) && !dynamic_cast<BoolRunResult*>(rrl) && bon->t.type() != ETokenType::_PLUS) 
                {
                    throw runtime_error("布尔值只能和布尔值进行非加法运算" + ((BinaryOPNode*)root)->t.toString());
                }

                if (dynamic_cast<BoolRunResult*>(rrl)  && dynamic_cast<BoolRunResult*>(rrr) ) {
                    BoolRunResult* bl = (BoolRunResult*)rrl;
                    BoolRunResult* br = (BoolRunResult*)rrr;
                    if (bon->t.type() == ETokenType::_AND) {
                        return new BoolRunResult(bl->b & br->b);
                    }

                    if (bon->t.type() == ETokenType::_OR) {
                        return new BoolRunResult(bl->b | br->b);
                    }

                    throw runtime_error("布尔值只能进行布尔运算" + ((BinaryOPNode*)root)->t.toString());
                }

                NumRunResult* vr;
                NumRunResult* vl;
                if (bon->t.type() == ETokenType::_COMP) 
                {
                    if (dynamic_cast<StringRunResult*>(rrl)  && !(dynamic_cast<StringRunResult*>(rrr) )) {
                        throw runtime_error("字符串只能和字符串进行比较" + ((BinaryOPNode*)root)->t.toString());
                    }

                    if (dynamic_cast<StringRunResult*>(rrr)  && !(dynamic_cast< StringRunResult* >(rrl) )) {
                        throw runtime_error("字符串只能和字符串进行比较" + ((BinaryOPNode*)root)->t.toString());
                    }

                    if (dynamic_cast<NumRunResult*>(rrl) && !(dynamic_cast<NumRunResult*>(rrr) )) {
                        throw runtime_error("数值只能和数值进行比较" + ((BinaryOPNode*)root)->t.toString());
                    }

                    if (dynamic_cast<NumRunResult*>(rrr)  && !(dynamic_cast<NumRunResult*>(rrl) )) {
                        throw runtime_error("数值只能和数值进行比较" + ((BinaryOPNode*)root)->t.toString());
                    }

                    if (!(dynamic_cast<StringRunResult*>(rrl) )) {
                        vl = (NumRunResult*)rrl;
                        vr = (NumRunResult*)rrr;
                        return vl->comp(vr, trim(bon->t.value().value()));
                    }

                    StringRunResult* sl = (StringRunResult*)rrl;
                    StringRunResult* sr = (StringRunResult*)rrr;
                    int compS = sl->s.compare(sr->s);
                    if (trim(bon->t.value().value())=="") {
                        return new BoolRunResult(compS > 0);
                    }

                    if (trim(bon->t.value().value())=="<") {
                        return new BoolRunResult(compS < 0);
                    }

                    if (trim(bon->t.value().value())==">=") {
                        return new BoolRunResult(compS >= 0);
                    }

                    if (trim(bon->t.value().value())=="<=") {
                        return new BoolRunResult(compS <= 0);
                    }

                    if (trim(bon->t.value().value())=="==") {
                        return new BoolRunResult(compS == 0);
                    }

                    if (trim(bon->t.value().value())=="!=") {
                        return new BoolRunResult(compS != 0);
                    }
                }
                else {
                    if (bon->t.type() == ETokenType::_PLUS) {
                        if (!(dynamic_cast<StringRunResult*>(rrl) ) && !(dynamic_cast<StringRunResult*>(rrr) ) && (!(dynamic_cast<NumRunResult*>(rrl) ) 
                            || !(dynamic_cast<NumRunResult*>(rrr) ))) 
                        {
                            throw runtime_error("加法运算只能在数值之间或字符串和其他类型之间" + ((BinaryOPNode*)root)->t.toString());
                        }

                        return rrl->add(rrr);
                    }

                    if (!(dynamic_cast<StringRunResult*>(rrl) ) && !(dynamic_cast<StringRunResult*>(rrr) )) {
                        vl = (NumRunResult*)rrl;
                        vr = (NumRunResult*)rrr;
                        if (bon->t.type() == ETokenType::_MINUS) 
                            return vl->sub(vr);

                        if (bon->t.type() == ETokenType::_MUL) {
                            return vl->mul(vr);
                        }

                        if (bon->t.type() != ETokenType::_DIV && bon->t.type() != ETokenType::_MODULE) {
                            if (bon->t.type() == ETokenType::_POW) { 
                                return vl->pow(vr);
                            }

                            return new NumRunResult(false, 0L, 0.0);
                        }

                        if (vr->isInt) {
                            if (vr->intV == 0L) {
                                throw runtime_error("不能除以0" + ((BinaryOPNode*)root)->t.toString());
                            }
                        }
                        else if (vr->floatV == 0.0) {
                            throw runtime_error("不能除以0" + ((BinaryOPNode*)root)->t.toString());
                        }

                        if (bon->t.type() == ETokenType::_DIV) {
                            return vl->div(vr);
                        }

                        if (vl->isInt && vr->isInt) {
                            return vl->mod(vr);
                        }

                        throw runtime_error("仅支持整数取模" + ((BinaryOPNode*)root)->t.toString());
                    }

                    throw runtime_error("字符串不能进行算术运算" + ((BinaryOPNode*)root)->t.toString());
                }
            }
            else {
                if (dynamic_cast<VariableNode*>(root) ) 
                {
                    VariableNode* vn = (VariableNode*)root;
                    auto varstr = trim(vn->id.value().value());
                    Variable* v = runStack.getVar(varstr);
                    if (v == nullptr) 
                    {
                        throw runtime_error("变量不存在" + root->toString());
                    }

                    if (!v->initFlag) 
                    {
                        throw runtime_error("变量未初始化" + root->toString());
                    }

                    return v->getValue();
                }

                if (dynamic_cast<ArrayValueNode*>(root) ) 
                {
                    ArrayValueNode* avn = (ArrayValueNode*)root;
                    rrr = traversalArithORStrORBoolExprForRun(avn->index);
                    if (!(dynamic_cast<NumRunResult*>(rrr) )) 
                    {
                        throw runtime_error("数组的索引必须为整数" + avn->toString());
                    }

                    if (!((NumRunResult*)rrr)->isInt) 
                    {
                        throw runtime_error("数组的索引必须为整数" + avn->toString());
                    }

                    int index = (int)((NumRunResult*)rrr)->intV;
                    GlobalArray* ga = (GlobalArray*)this->ArrayMap[trim(avn->id.value().value())];
                    if (ga->type == VariableType::INT) {
                        if (index >= ga->intV.size()) {
                            throw runtime_error("数组下标越界" + avn->toString());
                        }

                        return new NumRunResult(true, ga->intV[index], 0.0);
                    }

                    if (ga->type == VariableType::FLOAT) 
                    {
                        if (index >= ga->floatV.size()) 
                        {
                            throw runtime_error("数组下标越界" + avn->toString());
                        }

                        return new NumRunResult(false, 0L, ga->floatV[index]);
                    }

                    if (ga->type == VariableType::STRING) {
                        if (index >= ga->stringV.size()) {
                            throw runtime_error("数组下标越界" + avn->toString());
                        }

                        return new StringRunResult(ga->stringV[index]);
                    }

                    if (ga->type == VariableType::BOOL) {
                        if (index >= ga->boolV.size()) {
                            throw runtime_error("数组下标越界" + avn->toString());
                        }

                        return new BoolRunResult(ga->boolV[index]);
                    }
                }
                else {
                    if (!(dynamic_cast<ReadNode*>(root) )) {
                        throw runtime_error("语法错误" + root->toString());
                    }

                    ReadNode* rn = (ReadNode*)root;
                    string str;
                    if (rn->rnt == ReadNodeType::READINT) {
                        long intV = 0L;

                        while (true) 
                        {
                            cin >> str;
                            try 
                            {
                                intV = stol(str);
                                return new NumRunResult(true, intV, 0.0);
                            }
                            catch (exception e) 
                            {
                                cout << "输入错误。" << endl;
                            }
                        }
                    }

                    if (rn->rnt == ReadNodeType::READFLOAT) {
                        double doubleV = 0.0;

                        while (true) 
                        {
                            cin >> str;
                            try {
                                doubleV = stod(str);
                                return new NumRunResult(false, 0L, doubleV);
                            }
                            catch (exception e) {
                                cout << "输入错误。" << endl;
                            }
                        }
                    }

                    if (rn->rnt == ReadNodeType::READSTRING) {
                        string strV = "";
                        cin >> strV;
                        return new StringRunResult(strV);
                    }

                    if (rn->rnt == ReadNodeType::READBOOL) {
                        bool boolV = false;

                        while (true) 
                        {
                            string str;
                            cin >> str;
                            if (str=="true" || str=="false") {
                                if (str=="true")
                                    boolV = true;
                                else 
                                    boolV = false;

                                return new BoolRunResult(boolV);
                            }
                            cout << "输入错误" << endl;
                        }
                    }
                }
            }
            return new NumRunResult(false, 0L, 0.0);
        }
    }
}

void Interpreter::processArrayAssignNode(ArrayAssignNode* aan)
{
    string name = trim(aan->id.value().value());
    if (!ArrayMap.count(name)) 
    {
        throw runtime_error("数组" + name + "不存在" + aan->toString());
    }
    else {
        GlobalArray* ga = (GlobalArray*)ArrayMap[name];
        RunResult* rr = traversalArithORStrORBoolExprForRun(aan->index);
        if (!(dynamic_cast<NumRunResult*>(rr) )) {
            throw runtime_error("数组索引必须为整数" + aan->index->toString());
        }
        else {
            NumRunResult* nrr = (NumRunResult*)rr;
            if (!nrr->isInt) {
                throw runtime_error("数组索引必须为整数" + aan->index->toString());
            }
            else {
                rr = traversalArithORStrORBoolExprForRun(aan->value);
                if (ga->type == VariableType::INT && dynamic_cast<NumRunResult*>(rr)  && ((NumRunResult*)rr)->isInt) 
                {
                    try 
                    {
                        ga->setValue(nrr->intV, (NumRunResult*)rr, "", false);
                    }
                    catch (exception var10) {
                        throw runtime_error("数组下标越界" + aan->index->toString());
                    }
                }
                else if (ga->type == VariableType::FLOAT && dynamic_cast<NumRunResult*>(rr) ) {
                    try {
                        ga->setValue(nrr->intV, (NumRunResult*)rr, "", false);
                    }
                    catch (exception var9) {
                        throw runtime_error("数组下标越界" + aan->index->toString());
                    }
                }
                else if (ga->type == VariableType::STRING && dynamic_cast< StringRunResult*>(rr) ) {
                    try {
                        ga->setValue(nrr->intV, (NumRunResult*)nullptr, ((StringRunResult*)rr)->s, false);
                    }
                    catch (exception var8) {
                        throw runtime_error("数组下标越界" + aan->index->toString());
                    }
                }
                else {
                    if (ga->type != VariableType::BOOL || !(dynamic_cast<BoolRunResult *>(rr))) {
                        throw runtime_error("赋值类型不匹配" + aan->toString());
                    }

                    try {
                        ga->setValue(nrr->intV, (NumRunResult*)nullptr, "", ((BoolRunResult)rr).b);
                    }
                    catch (exception var7) {
                        throw runtime_error("数组下标越界" + aan->index->toString());
                    }
                }

            }
        }
    }
}

void Interpreter::newVarable(string typeStr, string vName, SyntaxNode* root)
{
    Variable* v = nullptr;
    if (typeStr=="int") {
        v = new Variable(VariableType::INT);
    }
    else if (typeStr=="float") {
        v = new Variable(VariableType::FLOAT);
    }
    else if (typeStr=="string") {
        v = new Variable(VariableType::STRING);
    }
    else if (typeStr=="bool") {
        v = new Variable(VariableType::BOOL);
    }
    else if (typeStr=="void") {
        throw runtime_error("void仅仅能用于方法声明" + root->toString());
    }

    if (runStack.isVarExist(vName)) 
    {
        throw runtime_error("变量" + vName + "已经存在" + root->toString());
    }
    else 
    {
        runStack.addVar(vName, v);
    }
}

void Interpreter::processAssignNode(AssignNode* an)
{
    Variable* v;
    if (dynamic_cast<DeclareNode*>(an->left))
    {
        traversalASTForRun(an->left, SingleRunStackType::OTHER);
    }
    else 
    {
        if (!(dynamic_cast<VariableNode*>(an->left) ))
            throw runtime_error("赋值运算符左侧必须是变量名" + an->toString());

        VariableNode* vn = (VariableNode*)an->left;
        v = runStack.getVar(trim(vn->id.value().value()));
        if (v == nullptr) {
            throw runtime_error("变量未声明" + vn->toString());
        }
    }

    if (!(dynamic_cast<BinaryOPNode *>(an->right) ) && !(dynamic_cast<NumberNode*>(an->right) ) && !(dynamic_cast<UnaryOPNode*>(an->right) ) 
        && !(dynamic_cast<VariableNode*>(an->right) ) && !(dynamic_cast<StringNode*>(an->right) ) && !(dynamic_cast<BoolNode*>(an->right) ) 
        && !(dynamic_cast<CallNode*>(an->right) ) && !(dynamic_cast<ReadNode*>(an->right) ) && !(dynamic_cast<ArrayValueNode*>(an->right) )) {
        throw runtime_error("赋值运算符右侧必须是表达式" + an->toString());
    }
    else {
        RunResult* rr = traversalArithORStrORBoolExprForRun(an->right);
        if (rr == nullptr) {
            throw runtime_error("调用的方法没有返回值" + an->toString());
        }
        else {
            v = nullptr;
            if (dynamic_cast<DeclareNode*>(an->left) ) {
                DeclareNode* dn = (DeclareNode*)an->left;
                v = runStack.getVar(trim(dn->id.value().value()));
            }
            else {
                VariableNode* vn = (VariableNode*)an->left;
                auto vstr = trim(vn->id.value().value());
                v = runStack.getVar(vstr);
            }

            if (dynamic_cast<NumRunResult*>(rr) ) {
                if (v->type == VariableType::STRING || v->type == VariableType::BOOL) {
                    throw runtime_error("不能将数值赋给非数值型变量" + an->toString());
                }

                NumRunResult* nrr = (NumRunResult*)rr;
                if (v->type == VariableType::INT && !nrr->isInt) {
                    throw runtime_error("不能将浮点值赋给整型变量" + an->toString());
                }

                v->setValue(nrr,"", false);
            }
            else if (dynamic_cast< StringRunResult*>(rr) ) {
                if (v->type != VariableType::STRING) {
                    throw runtime_error("不能将字符串赋给非字符串型变量" + an->toString());
                }

                v->setValue(nullptr, ((StringRunResult*)rr)->s, false);
            }
            else if (dynamic_cast<BoolRunResult *>(rr) ) {
                if (v->type != VariableType::BOOL) {
                    throw runtime_error("不能将布尔值赋给非布尔型变量" + an->toString());
                }

                BoolRunResult* brr = (BoolRunResult*)rr;
                v->setValue(nullptr, "", brr->b);
            }

        }
    }
}

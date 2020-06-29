#pragma once
#include "Tables.h"
#include "SymbolList.h"
#include "LexicalAnalyser.h"

//语法、语义分析、生成四元式
int GrammarAnalyse();

//接受错误号并退出程序
int SendError(int err_id);

//递归下降子程序（均以‘g’开头）

//程序
TOKEN gProgram(TOKEN preTOKEN);
//程序头声明
TOKEN gHeadOfProgram(TOKEN preTOKEN);
//标识符
TOKEN gIdentifier(TOKEN preTOKEN);
//全局变量声明区
TOKEN gDeclVars(TOKEN preTOKEN);
//变量定义
TOKEN gVarDef(TOKEN preTOKEN);
//数据类型
TOKEN gDataType(TOKEN preTOKEN);
//函数声明区
TOKEN gDeclFuncs(TOKEN preTOKEN);
//函数定义
TOKEN gFuncDef(TOKEN preTOKEN);
//参数声明区
TOKEN gDeclArgs(TOKEN preTOKEN);
//赋值形参声明区
TOKEN gDeclVfs(TOKEN preTOKEN);
//换名形参声明区
TOKEN gDeclVns(TOKEN preTOKEN);
//函数变量定义
TOKEN gFuncVarDef(TOKEN preTOKEN);
//函数体
TOKEN gFuncBody(TOKEN preTOKEN);
//函数语句
TOKEN gFuncCode(TOKEN preTOKEN);
//返回语句
TOKEN gCodeReturn(TOKEN preTOKEN);
//程序体
TOKEN gProgramBody(TOKEN preTOKEN);
//复合语句
TOKEN gCompCode(TOKEN preTOKEN);
//赋值语句
TOKEN gCodeAssign(TOKEN preTOKEN);
//打印语句
TOKEN gCodePrint(TOKEN preTOKEN);
//结束语句
TOKEN gCodeEnd(TOKEN preTOKEN);
//条件语句
TOKEN gCodeIF(TOKEN preTOKEN);
//循环语句
TOKEN gCodeWhile(TOKEN preTOKEN);
//函数调用语句
TOKEN gCodeCall(TOKEN preTOKEN);
//表达式
TOKEN gExpr(TOKEN preTOKEN);
//逻辑后缀式
TOKEN gLogicSuffix(TOKEN preTOKEN);
//关系表达式
TOKEN gRelationExpr(TOKEN preTOKEN);
//关系后缀式
TOKEN gRelationSuffix(TOKEN preTOKEN);
//算术表达式
TOKEN gMathExpr(TOKEN preTOKEN);
//加法后缀式
TOKEN gAddSuffix(TOKEN preTOKEN);
//乘法表达式
TOKEN gMulExpr(TOKEN preTOKEN);
//乘法后缀式
TOKEN gMulSuffix(TOKEN preTOKEN);
//项
TOKEN gTerm(TOKEN preTOKEN);
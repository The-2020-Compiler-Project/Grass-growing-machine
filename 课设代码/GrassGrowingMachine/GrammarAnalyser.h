#pragma once
#include <string.h>
#include "Tables.h"
#include "SymbolList.h"
#include "LexicalAnalyser.h"

//语法、语义分析、生成四元式
int GrammarAnalyse();

//填写符号表所需要的状态量
int iAvalDSegOffset; //当前数据段可用区距
PFINFLITEM* currentFunc; //当前所在的函数定义区域，若不在函数区域中则为NULL
int iArgNum; //当前函数的参数个数
ARGLITEM* ptrFirstArg; //当前函数的第一个参数
int iFuncVarDefMode; //函数内变量定义模式，0为赋值形参，1为换名形参，2为局部变量
int iAvalFuncOffset; //当前函数可用区距
struct exprProcessedType
{
	enum type{exprNULL, exprINT, exprREAL, exprID}type;
	union content
	{
		int d;
		double f;
		char *ptrID; //指向经过表达式识别处理后的标识符名或临时变量名
	}content;
}exprProcessed;

//接受错误号并退出程序
int SendError(int err_id);

//递归下降子程序（均以‘g’开头）

//程序
TOKEN gProgram(TOKEN preTOKEN);
//程序头声明
TOKEN gHeadOfProgram(TOKEN preTOKEN);
//全局变量声明区
TOKEN gDeclVars(TOKEN preTOKEN);
//变量定义
TOKEN gVarDef(TOKEN preTOKEN);
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
//临时变量声明区
TOKEN gDeclFuncVars(TOKEN preTOKEN);
//函数变量定义
TOKEN gFuncVarDef(TOKEN preTOKEN);
//函数体
TOKEN gFuncBody(TOKEN preTOKEN);
//函数复合语句
TOKEN gFuncCode(TOKEN preTOKEN);
//返回语句
TOKEN gCodeReturn(TOKEN preTOKEN);
//程序体
TOKEN gProgramBody(TOKEN preTOKEN);
//程序语句
TOKEN gProgCode(TOKEN preTOKEN);
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
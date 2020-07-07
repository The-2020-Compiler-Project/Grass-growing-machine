#pragma once
#include <string.h>
#include "Tables.h"
#include "SymbolList.h"
#include "LexicalAnalyser.h"
#define MAX_EXPR_STACK_LEN 50

//语法、语义分析、生成四元式
int GrammarAnalyse();

//填写符号表所需要的状态量
int iAvalDSegOffset; //当前数据段可用区距
PFINFLITEM* currentFunc; //当前所在的函数定义区域，若不在函数区域中则为NULL
TYPELITEM* currentFuncType; // 当前函数的返回值类型
int iArgNum; //当前函数的参数个数
ARGLITEM* ptrFirstArg; //当前函数的第一个参数
int iFuncVarDefMode; //函数内变量定义模式，0为赋值形参，1为换名形参，2为局部变量
int iAvalFuncOffset; //当前函数可用区距
int iMidVarNum; //中间临时变量数目

//表达式处理相关

typedef struct exprProcessedType
{
	enum exprType { exprNULL, exprDC, exprFC, exprCHAR, exprID } type; //空，整常数，浮点常数，字符常数，标识符
	TYPELITEM* datatype;
	union exprContent
	{
		int d;
		char ch;
		double f;
		char* strID; //指向经过表达式识别处理后的标识符名(符号表主表中的)或临时变量名(临时变量表中的)
	} content;
} exprProcessedType;

exprProcessedType exprProcessed;

int exprSP; //表达式栈指针
exprProcessedType exprStack[MAX_EXPR_STACK_LEN]; //表达式项栈
//入栈，参数为需要入栈的表达式值, 成功返回1，失败返回0，该函数正常情况下只应被exprGenerateSeq()和gTerm()调用
int exprPUSH(exprProcessedType expr);
//出栈，参数为表达式输出的目标字符串，成功返回1，失败返回0，该函数正常情况下只应被exprGenerateSeq()或gExpr()调用
int exprPOP(exprProcessedType *dest);
//生成表达式相关的四元式，op为表达式操作符，argnum为该操作符的操作数数量（单目为1，双目为2）,成功返回1，失败返回0
int exprGenerateSeq(OPR op, int argnum); 
//申请建立一个临时变量，将该临时变量添加至临时变量表和主表中, 成功则返回变量名（在临时变量表中），否则返回空指针
char* allocMidVar(TYPELITEM* type);

//接受错误号并退出程序
int SendError(int err_id);

//根据exprProcessed填写一个SEQUENCEARG,失败返回0
int fillExprSeqArg(SEQARG* seqarg);

//表查询函数

//在SYMBL表中查找指定名称的项, startpos表示从某个编号开始查找, 返回查找到的编号，无结果则返回-1
int Find_SymblItemName(int startpos, char* name);
//判断某个标识符是否是当前作用域可访问的变量名，若是则返回其在符号表中的编号，否则返回-1
int getAvailableVar(char* name);

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
//逻辑表达式
TOKEN gLogicExpr(TOKEN preTOKEN);
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
#pragma once
#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4244)
#include "Tables.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef enum TOKENTYPE
{
	//未知，关键字，标识符，界符，字符，字符串，整数常数，浮点常数
	NOTYPE = 0, KTYPE, ITYPE, PTYPE, 
	CTYPE, STYPE, DCTYPE, FCTYPE
}TOKENTYPE;

typedef enum KEYWORD	//关键字种类
{
	kINT = 0, kCHAR, kREAL, kPROGRAM, 
	kFUNCTION, kVAR, kARGS, kVAL, 
	kPTR, kBODY, kRETURN, kIF, kELSE,
	kWHILE, kENDP, kPUTC, kAND, kOR
}KEYWORD;

typedef enum PSYMBOL	//界符种类
{
	pLE = 0, pGE, pEQL, pASSI, 	//<=, >=, ==, =
	pLT, pGT, pNEQ,pADD, pSUB, //<, >, != , +, -
	pMUL, pDIV, pMOD, pLBRACE, //*, /, %, {
	pRBRACE, pLBRACKET, pRBRACKET, pSEMI //}, (, ), ;
}PSYMBOL;

typedef struct TOKEN
{
	TOKENTYPE type; //类型
	int id; //编号
}TOKEN;

//生成的Token序列
TOKEN tokenFinal[10000];
//Token序列的长度
int tokenSize;
//当前读到的Token序列位置
int tokenPos;

//当前单词
char word[100];
//当前单词大小
int wordSize;
//初始化当前单词和单词的大小，成功则返回1
int initWord();
//初始化Token序列数组，成功则返回1
int initToken();
//弹出word的最后一位字符，成功则返回1
int wordPop();

//确定自动机下一状态，参数：前一状态，读到的下一个字符，当前读到的单词的长度，当前读到的单词，返回自动机的下一状态码
int transition(int preState, char nextChar, int wordSize, char* word);
//自动机状态转换，参数：前一状态，要读取的文件指针，文件读到结尾时返回1
int ToNext(int preState, FILE* fp);

//确定当前状态为0时的下一个状态，返回自动机的下一状态码，参数：读到的下一个字符
int next0(char nextChar);
//确定当前状态为1时的下一个状态，返回自动机的下一状态码，参数：读到的下一个字符
int next1(char nextChar);
//确定当前状态为2时的下一个状态，返回自动机的下一状态码，参数：读到的下一个字符
int next2(char nextChar);
//确定当前状态为3时的下一个状态，返回自动机的下一状态码，参数：读到的下一个字符
int next3(char nextChar);
//确定当前状态为4时的下一个状态，返回自动机的下一状态码，参数：读到的下一个字符
int next4(char nextChar);
//确定当前状态为5时的下一个状态，返回自动机的下一状态码，参数：读到的下一个字符
int next5(char nextChar);
//确定当前状态为6时的下一个状态，返回自动机的下一状态码，参数：读到的下一个字符
int next6(char nextChar);
//确定当前状态为7时的下一个状态，返回自动机的下一状态码，参数：读到的下一个字符
int next7(char nextChar);
//确定当前状态为8时的下一个状态，返回自动机的下一状态码，参数：读到的下一个字符
int next8(char nextChar);
//确定当前状态为9时的下一个状态，返回自动机的下一状态码，参数：读到的下一个字符
int next9(char nextChar);
//确定当前状态为10时的下一个状态，返回自动机的下一状态码，参数：读到的下一个字符
int next10(char nextChar);

//查询当前单词是否为关键字，是则生成对应Token项，否则运行标识符处理函数，成功时返回1
int KTfunc();
//查询当前单词是否是已有标识符，是则生成对应Token项，否则将其填入标识符表再输出对应的Token序列，成功时返回1
int iTfunc();
//将当前字符型常量置入字符表中，并生成对应的Token序列，成功时返回1
int cTfunc();
//将当前字符串型常量置入字符表中，并生成对应的Token序列，成功时返回1
int sTfunc();
//将当前整数型常量置入字符表中，并生成对应的Token序列，成功时返回1
int dcTfunc();
//将当前浮点数型常量置入字符表中，并生成对应的Token序列，成功时返回1
int fcTfunc();
//查询当前单词是否在界符表中，是则生成对应Token序列，返回1，否则输出错误信息
int PTfunc();

//输出Token，参数：单词类型，单词编号
TOKEN getNext(TOKENTYPE type, int id);

//读取下一个符号的Token项，参数：单词类型，单词编号
TOKEN Next();

//词法分析器调用，参数：文件指针
int LexicalAnalyser(FILE* fp);

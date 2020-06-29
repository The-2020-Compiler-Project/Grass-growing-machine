#pragma once
#include "Tables.h"

typedef enum TOKENTYPE
{
	//未知，关键字，标识符，界符，字符，字符串，整数常数，浮点常数
	NOTYPE, KTYPE, ITYPE, PTYPE, 
	CTYPE, STYPE, DCTYPE, FCTYPE
}TOKENTYPE;

typedef enum KEYWORD	//关键字种类
{
	kINT, kCHAR, kREAL, kPROGRAM, 
	kFUNCTION, kVAR, kARGS, kVAL, 
	kPTR, kBODY, kRETURN, kIF, 
	kWHILE, kENDP, kPUTC, kAND, kOR
}KEYWORD;

typedef enum PSYMBOL	//界符种类
{
	pLE, pGE, pEQL, pASSI, 	//<=, >=, ==, =
	pLT, pGT, pADD, pSUB, //<, >, +, -
	pMUL, pDIV, pMOD, pLBRACE, //*, /, %, {
	pRBRACE, pLBRACKET, pRBRACKET, pSEMI //}, (, ), ;
}PSYMBOL;

typedef struct TOKEN
{
	TOKENTYPE type; //类型
	int id; //编号
}TOKEN;

//词法分析器读入下一个单词，输出TOKEN
TOKEN Next();
#pragma once
#include "Tables.h"

typedef enum TOKENTYPE
{
	//δ֪���ؼ��֣���ʶ����������ַ����ַ������������������㳣��
	NOTYPE, KTYPE, ITYPE, PTYPE, 
	CTYPE, STYPE, DCTYPE, FCTYPE
}TOKENTYPE;

typedef enum KEYWORD	//�ؼ�������
{
	kINT, kCHAR, kREAL, kPROGRAM, 
	kFUNCTION, kVAR, kARGS, kVAL, 
	kPTR, kBODY, kRETURN, kIF, 
	kWHILE, kENDP, kPUTC, kAND, kOR
}KEYWORD;

typedef enum PSYMBOL	//�������
{
	pLE, pGE, pEQL, pASSI, 	//<=, >=, ==, =
	pLT, pGT, pADD, pSUB, //<, >, +, -
	pMUL, pDIV, pMOD, pLBRACE, //*, /, %, {
	pRBRACE, pLBRACKET, pRBRACKET, pSEMI //}, (, ), ;
}PSYMBOL;

typedef struct TOKEN
{
	TOKENTYPE type; //����
	int id; //���
}TOKEN;

//�ʷ�������������һ�����ʣ����TOKEN
TOKEN Next();
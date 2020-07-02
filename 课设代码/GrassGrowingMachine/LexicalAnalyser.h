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
	//δ֪���ؼ��֣���ʶ����������ַ����ַ������������������㳣��
	NOTYPE, KTYPE, ITYPE, PTYPE,
	CTYPE, STYPE, DCTYPE, FCTYPE
}TOKENTYPE;

typedef enum KEYWORD	//�ؼ�������
{
	kINT, kCHAR, kREAL, kPROGRAM,
	kFUNCTION, kVAR, kARGS, kVAL,
	kPTR, kBODY, kRETURN, kIF, kELSE,
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

//�ؼ��ֱ�
char* KT[18] = { "int" , "char" , "real" , "program" , "function" , "var" , "args" , "val" ,
				 "ptr" , "body" , "return" , "if" , "else" , "while" , "endp" , "putc" , "and" , "or" };
//�ؼ��ֱ�ĳ���
int KTSize = 18;

//�����
char* PT[16] = { "<=" , ">=" , "==" , "=" , "<" , ">" , "=" , "+" , "-" , "*" , "/" , "%" , "{" ,
				 "}" , "(" , ")" };
//�����ĳ���
int PTSize = 16;

//���ɵ�Token����
TOKEN tokenFinal[1000];
//Token���еĳ���
int tokenSize;

//��ǰ����
char word[100];
//��ǰ���ʴ�С
int wordSize;
//��ʼ����ǰ���ʺ͵��ʵĴ�С���ɹ��򷵻�1
int initWord();
//��ʼ��Token�������飬�ɹ��򷵻�1
int initToken();
//����word�����һλ�ַ����ɹ��򷵻�1
int wordPop();

//ȷ���Զ�����һ״̬��������ǰһ״̬����������һ���ַ�����ǰ�����ĵ��ʵĳ��ȣ���ǰ�����ĵ��ʣ������Զ�������һ״̬��
int transition(int preState, char nextChar, int wordSize, char* word);
//�Զ���״̬ת����������ǰһ״̬��Ҫ��ȡ���ļ�ָ�룬�ļ�������βʱ����1
int ToNext(int preState, FILE* fp);

//ȷ����ǰ״̬Ϊ0ʱ����һ��״̬�������Զ�������һ״̬�룬��������������һ���ַ�����ǰ�������ֵĳ��ȣ���ǰ��������
int next0(char nextChar, int wordSize, char* word);
//ȷ����ǰ״̬Ϊ1ʱ����һ��״̬�������Զ�������һ״̬�룬��������������һ���ַ�����ǰ�������ֵĳ��ȣ���ǰ��������
int next1(char nextChar, int wordSize, char* word);
//ȷ����ǰ״̬Ϊ2ʱ����һ��״̬�������Զ�������һ״̬�룬��������������һ���ַ�����ǰ�������ֵĳ��ȣ���ǰ��������
int next2(char nextChar, int wordSize, char* word);
//ȷ����ǰ״̬Ϊ3ʱ����һ��״̬�������Զ�������һ״̬�룬��������������һ���ַ�����ǰ�������ֵĳ��ȣ���ǰ��������
int next3(char nextChar, int wordSize, char* word);
//ȷ����ǰ״̬Ϊ4ʱ����һ��״̬�������Զ�������һ״̬�룬��������������һ���ַ�����ǰ�������ֵĳ��ȣ���ǰ��������
int next4(char nextChar, int wordSize, char* word);
//ȷ����ǰ״̬Ϊ5ʱ����һ��״̬�������Զ�������һ״̬�룬��������������һ���ַ�����ǰ�������ֵĳ��ȣ���ǰ��������
int next5(char nextChar, int wordSize, char* word);
//ȷ����ǰ״̬Ϊ6ʱ����һ��״̬�������Զ�������һ״̬�룬��������������һ���ַ�����ǰ�������ֵĳ��ȣ���ǰ��������
int next6(char nextChar, int wordSize, char* word);
//ȷ����ǰ״̬Ϊ7ʱ����һ��״̬�������Զ�������һ״̬�룬��������������һ���ַ�����ǰ�������ֵĳ��ȣ���ǰ��������
int next7(char nextChar, int wordSize, char* word);
//ȷ����ǰ״̬Ϊ8ʱ����һ��״̬�������Զ�������һ״̬�룬��������������һ���ַ�����ǰ�������ֵĳ��ȣ���ǰ��������
int next8(char nextChar, int wordSize, char* word);
//ȷ����ǰ״̬Ϊ9ʱ����һ��״̬�������Զ�������һ״̬�룬��������������һ���ַ�����ǰ�������ֵĳ��ȣ���ǰ��������
int next9(char nextChar, int wordSize, char* word);

//��ѯ��ǰ�����Ƿ�Ϊ�ؼ��֣��������ɶ�ӦToken��������б�ʶ�����������ɹ�ʱ����1����������ǰ����
int KTfunc(char* word);
//��ѯ��ǰ�����Ƿ������б�ʶ�����������ɶ�ӦToken������������ʶ�����������Ӧ��Token���У��ɹ�ʱ����1����������ǰ����
int iTfunc(char* word);
//����ǰ�ַ��ͳ��������ַ����У������ɶ�Ӧ��Token���У��ɹ�ʱ����1����������ǰ����
int cTfunc(char* word);
//����ǰ�ַ����ͳ��������ַ����У������ɶ�Ӧ��Token���У��ɹ�ʱ����1����������ǰ����
int sTfunc(char* word);
//����ǰ�����ͳ��������ַ����У������ɶ�Ӧ��Token���У��ɹ�ʱ����1����������ǰ����
int dcTfunc(char* word);
//����ǰ�������ͳ��������ַ����У������ɶ�Ӧ��Token���У��ɹ�ʱ����1����������ǰ����
int fcTfunc(char* word);
//��ѯ��ǰ�����Ƿ��ڽ�����У��������ɶ�ӦToken���У�����1���������������Ϣ����������ǰ����
int PTfunc(char* word);

//���Token���������������ͣ����ʱ��
TOKEN next(TOKENTYPE type, int id);

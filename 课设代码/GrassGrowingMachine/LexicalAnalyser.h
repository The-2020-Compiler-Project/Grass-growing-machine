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
	NOTYPE = 0, KTYPE, ITYPE, PTYPE, 
	CTYPE, STYPE, DCTYPE, FCTYPE
}TOKENTYPE;

typedef enum KEYWORD	//�ؼ�������
{
	kINT = 0, kCHAR, kREAL, kPROGRAM, 
	kFUNCTION, kVAR, kARGS, kVAL, 
	kPTR, kBODY, kRETURN, kIF, kELSE,
	kWHILE, kENDP, kPUTC, kAND, kOR
}KEYWORD;

typedef enum PSYMBOL	//�������
{
	pLE = 0, pGE, pEQL, pASSI, 	//<=, >=, ==, =
	pLT, pGT, pNEQ,pADD, pSUB, //<, >, != , +, -
	pMUL, pDIV, pMOD, pLBRACE, //*, /, %, {
	pRBRACE, pLBRACKET, pRBRACKET, pSEMI //}, (, ), ;
}PSYMBOL;

typedef struct TOKEN
{
	TOKENTYPE type; //����
	int id; //���
}TOKEN;

//���ɵ�Token����
TOKEN tokenFinal[10000];
//Token���еĳ���
int tokenSize;
//��ǰ������Token����λ��
int tokenPos;

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

//ȷ����ǰ״̬Ϊ0ʱ����һ��״̬�������Զ�������һ״̬�룬��������������һ���ַ�
int next0(char nextChar);
//ȷ����ǰ״̬Ϊ1ʱ����һ��״̬�������Զ�������һ״̬�룬��������������һ���ַ�
int next1(char nextChar);
//ȷ����ǰ״̬Ϊ2ʱ����һ��״̬�������Զ�������һ״̬�룬��������������һ���ַ�
int next2(char nextChar);
//ȷ����ǰ״̬Ϊ3ʱ����һ��״̬�������Զ�������һ״̬�룬��������������һ���ַ�
int next3(char nextChar);
//ȷ����ǰ״̬Ϊ4ʱ����һ��״̬�������Զ�������һ״̬�룬��������������һ���ַ�
int next4(char nextChar);
//ȷ����ǰ״̬Ϊ5ʱ����һ��״̬�������Զ�������һ״̬�룬��������������һ���ַ�
int next5(char nextChar);
//ȷ����ǰ״̬Ϊ6ʱ����һ��״̬�������Զ�������һ״̬�룬��������������һ���ַ�
int next6(char nextChar);
//ȷ����ǰ״̬Ϊ7ʱ����һ��״̬�������Զ�������һ״̬�룬��������������һ���ַ�
int next7(char nextChar);
//ȷ����ǰ״̬Ϊ8ʱ����һ��״̬�������Զ�������һ״̬�룬��������������һ���ַ�
int next8(char nextChar);
//ȷ����ǰ״̬Ϊ9ʱ����һ��״̬�������Զ�������һ״̬�룬��������������һ���ַ�
int next9(char nextChar);
//ȷ����ǰ״̬Ϊ10ʱ����һ��״̬�������Զ�������һ״̬�룬��������������һ���ַ�
int next10(char nextChar);

//��ѯ��ǰ�����Ƿ�Ϊ�ؼ��֣��������ɶ�ӦToken��������б�ʶ�����������ɹ�ʱ����1
int KTfunc();
//��ѯ��ǰ�����Ƿ������б�ʶ�����������ɶ�ӦToken������������ʶ�����������Ӧ��Token���У��ɹ�ʱ����1
int iTfunc();
//����ǰ�ַ��ͳ��������ַ����У������ɶ�Ӧ��Token���У��ɹ�ʱ����1
int cTfunc();
//����ǰ�ַ����ͳ��������ַ����У������ɶ�Ӧ��Token���У��ɹ�ʱ����1
int sTfunc();
//����ǰ�����ͳ��������ַ����У������ɶ�Ӧ��Token���У��ɹ�ʱ����1
int dcTfunc();
//����ǰ�������ͳ��������ַ����У������ɶ�Ӧ��Token���У��ɹ�ʱ����1
int fcTfunc();
//��ѯ��ǰ�����Ƿ��ڽ�����У��������ɶ�ӦToken���У�����1���������������Ϣ
int PTfunc();

//���Token���������������ͣ����ʱ��
TOKEN getNext(TOKENTYPE type, int id);

//��ȡ��һ�����ŵ�Token��������������ͣ����ʱ��
TOKEN Next();

//�ʷ����������ã��������ļ�ָ��
int LexicalAnalyser(FILE* fp);

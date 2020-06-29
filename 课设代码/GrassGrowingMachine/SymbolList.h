#pragma once
#include <string.h>
#include "Tables.h"
#define MAXSEQLISTSIZE 500
#define MAX_SYMBLISTSIZE 200

typedef enum OPR //��Ԫʽ������
{
	PROG, ENT, EXIT, END,
	VAR, VF, VN, VT, 
	ADD, SUB, MUL, DIV, MOD, ASSI,
	GT, GE, LT, LE, EQ, NE, AND, OR, 
	PARAM, CALL, RET, FUNC, EF,
	IF, EL, IE, 
	WH, DO, WE, 
	PUTC
}OPR;

typedef enum SEQARGTYPE
{
	//�գ���ʱ��������ʶ�������ͳ�����ʵ�ͳ���
	seqNONE, seqMIDVAR, seqID, seqDC, seqFC
}SEQARGTYPE;

typedef struct SEQARG //��Ԫʽ����
{
	SEQARGTYPE type; //����
	union content //����
	{
		void* ptr; //ָ���ʶ����ָ��
		int d;
		double f;
	}content;
}SEQARG;

typedef struct SEQUENCE //��Ԫʽ
{
	OPR op;	//������
	SEQARG arg1, arg2, target;
}SEQUENCE;

int SeqLine; //��ǰ��ռ�õ�������Ҳ����˵��Ч���ݵķ�ΧΪSequenceList[0~SeqLine-1]
SEQUENCE SequenceList[MAXSEQLISTSIZE];

//��ʼ�����ű�
int init_symbl();

/*
���ű�ϵͳ����������SYMBL��������Ϣ��PFINFL��������ARGL��
���ͱ�TYPEL��������CONSL�����ȱ�LENL���м������MIDVL
*/

typedef enum TVAL //����
{
	//���ͣ�ʵ�ͣ��ַ��ͣ������ͣ�Ԥ�������ṹ�ͣ�Ԥ����
	iTVAL��rTVAL, cTVAL, aVAL, dVAL
}TVAL;

typedef struct TYPELITEM	//���ͱ���
{
	TVAL tval; //����������
	void* tpoint; //Ԥ��������TVALָ��������ṹ���ָ��
}TYPELITEM;

int TypelLine; //��ǰ��ռ�õ�������Ҳ����˵��Ч���ݵķ�ΧΪTYPEL[0~TypelLine-1]
TYPELITEM TYPEL[MAX_SYMBLISTSIZE]; //���ͱ�

typedef enum SYMCAT //���ű��е�������
{
	//���������������ͣ������������������βΣ���ֵ�β�
	fCAT, cCAT, tCAT, vCAT, vnCAT, vfCAT
}SYMCAT;

typedef struct SYMBLITEM	//������
{
	char name[MAX_IDLEN]; //��ʶ����
	TYPELITEM *type;	//��������
	SYMCAT cat;  //����
	void* addr; //����catָ�������������ָ��
}SYMBLITEM;

int SymblLine; //��ǰ��ռ�õ�������Ҳ����˵��Ч���ݵķ�ΧΪSYMBL[0~SymblLine-1]
SYMBLITEM SYMBL[MAX_SYMBLISTSIZE]; //���ű�����

typedef struct ARGLITEM //��������
{
	char name[MAX_IDLEN]; //������
	TYPELITEM *type;	//��������
	SYMCAT cat;  //����
	int OFFSET; //����catָ�������������ָ��
}ARGLITEM;

int ArglLine; //��ǰ��ռ�õ�������Ҳ����˵��Ч���ݵķ�ΧΪARGL[0~ArglLine-1]
ARGLITEM ARGL[MAX_SYMBLISTSIZE]; //������

typedef struct PFINFLITEM //������Ϣ����
{
	int LEVEL; //��κţ���GGM Language��һ��Ϊ1
	int OFFSET; //����
	int FN; //��������
	int ENTRY; //��ڵ�ַ��Ԥ�����ݲ��ã�
	ARGLITEM* PARAM; //ָ���βα��ú����ĵ�һ���βΣ�
}PFINFLITEM;

int PFInflLine; //��ǰ��ռ�õ�������Ҳ����˵��Ч���ݵķ�ΧΪPFINFL[0~PFInflLine-1]
PFINFLITEM PFINFL[MAX_SYMBLISTSIZE]; //������Ϣ��

typedef union CONSLITEM //��������
{
	int d;
	double f;
}CONSLITEM;

int ConslLine; //��ǰ��ռ�õ�������Ҳ����˵��Ч���ݵķ�ΧΪCONSL[0~ConslLine-1]
CONSLITEM CONSL[MAX_SYMBLISTSIZE]; //������

int LenlLine; //��ǰ��ռ�õ�������Ҳ����˵��Ч���ݵķ�ΧΪLENL[0~LenlLine-1]
int LENL[MAX_SYMBLISTSIZE]; //���ȱ�

typedef struct MIDVLITEM //�м䣨��ʱ����������
{
	char name[MAX_IDLEN]; //������
	int DEPTH; //�������ڵݹ���ȣ���������壬��δ���ú���ʱΪ0��
	TYPELITEM *type; //��������
	int OFFSET; //����
}MIDVLITEM;

int MidvlLine; //��ǰ��ռ�õ�������Ҳ����˵��Ч���ݵķ�ΧΪMIDVL[0~MidvlLine-1]
MIDVLITEM MIDVL[MAX_SYMBLISTSIZE]; //�м䣨��ʱ���������ڱ��ʽ��ֵ��ʹ��

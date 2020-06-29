#pragma once
#include <string.h>
#define MAXSEQLISTSIZE 500

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
	int content; //����
	int extra; //�������ݶ�
}SEQARG;

typedef struct SEQUENCE //��Ԫʽ
{
	OPR op;	//������
	SEQARG arg1, arg2, target;
}SEQUENCE;

int SeqLine; //��ǰһ��ռ�õ�������Ҳ����˵��Ч���ݵķ�ΧΪSequenceList[0~SeqLine-1]

SEQUENCE SequenceList[MAXSEQLISTSIZE];

//��ʼ�����ű�
int init_symbl();
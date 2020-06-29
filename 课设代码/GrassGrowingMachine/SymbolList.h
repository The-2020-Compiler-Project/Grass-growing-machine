#pragma once
#include <string.h>
#define MAXSEQLISTSIZE 500

typedef enum OPR //四元式操作符
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
	//空，临时变量，标识符，整型常数，实型常数
	seqNONE, seqMIDVAR, seqID, seqDC, seqFC
}SEQARGTYPE;

typedef struct SEQARG //四元式参数
{
	SEQARGTYPE type; //类型
	int content; //内容
	int extra; //附加数据段
}SEQARG;

typedef struct SEQUENCE //四元式
{
	OPR op;	//操作符
	SEQARG arg1, arg2, target;
}SEQUENCE;

int SeqLine; //当前一共占用的行数，也就是说有效数据的范围为SequenceList[0~SeqLine-1]

SEQUENCE SequenceList[MAXSEQLISTSIZE];

//初始化符号表
int init_symbl();
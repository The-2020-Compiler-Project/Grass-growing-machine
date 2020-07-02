#pragma once
#include <string.h>
#include <stdbool.h>
#include "Tables.h"
#define MAXSEQLISTSIZE 500
#define MAX_SYMBLISTSIZE 200
#define DEST_INT_SIZE 2 //目标代码INT类型所占字节数
#define DEST_CHAR_SIZE 2 //目标代码CHAR类型所占字节数
#define DEST_REAL_SIZE 4 //目标代码REAL类型所占字节数
#define DEST_PTR_SIZE 2 //目标代码指针类型（地址）所占字节数

typedef enum OPR //四元式操作符
{
	PROG, GV, GVE, ENT, EXIT, END,
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
	//空，临时变量，标识符，整型常数，实型常数, 字符型常数
	seqNONE, seqMIDVAR, seqID, seqDC, seqFC, seqCHAR
}SEQARGTYPE;

typedef struct SEQARG //四元式参数
{
	SEQARGTYPE type; //类型
	union content //内容
	{
		char* str; //指向标识符表中字符串或临时变量表中字符串的指针
		int d;
		char ch;
		double f;
	}content;
	bool active; //活跃信息
}SEQARG;

typedef struct SEQUENCE //四元式
{
	OPR op;	//操作符
	SEQARG arg1, arg2, target;
}SEQUENCE;

int SeqLine; //当前已占用的行数，也就是说有效数据的范围为SequenceList[0~SeqLine-1]
SEQUENCE SequenceList[MAXSEQLISTSIZE]; //四元式列表
int OptSeqLine; //当前已占用的行数，也就是说有效数据的范围为OptimizedSeqList[0~OptSeqLine-1]
SEQUENCE OptimizedSeqList[MAXSEQLISTSIZE]; //优化后的四元式列表

//发送一条四元式至SequenceList[SeqLine],并++SeqLine
int sendSequence(SEQUENCE seq);

//初始化符号表
int init_symbl();

/*
符号表系统包括：主表SYMBL，过函信息表PFINFL，参数表ARGL，
类型表TYPEL，常数表CONSL，长度表LENL，中间变量表MIDVL
*/

typedef enum TVAL //类码
{
	//整型，实型，字符型，数组型（预留），结构型（预留）
	iTVAL, rTVAL, cTVAL, aVAL, dVAL
}TVAL;

typedef struct TYPELITEM	//类型表项
{
	TVAL tval; //数据类型码
	void* tpoint; //根据TVAL,表示数据类型所占字节数或指向数组表或结构表的指针
}TYPELITEM;

int TypelLine; //当前已占用的行数，也就是说有效数据的范围为TYPEL[0~TypelLine-1]
TYPELITEM TYPEL[MAX_SYMBLISTSIZE]; //类型表

typedef enum SYMCAT //符号表中的种类项
{
	//函数，常量（预留），类型（预留），域名（预留），全局变量，换名形参，赋值形参，局部变量
	fCAT, cCAT, tCAT, gvCAT, vnCAT, vfCAT, svCAT
}SYMCAT;

typedef struct SYMBLITEM	//主表项
{
	char name[MAX_IDLEN]; //标识符名
	TYPELITEM *type;	//数据类型
	SYMCAT cat;  //种类
	void* addr; //根据cat指向其他各表项的指针
	int OFFSET; //区距
}SYMBLITEM;

int SymblLine; //当前已占用的行数，也就是说有效数据的范围为SYMBL[0~SymblLine-1]
SYMBLITEM SYMBL[MAX_SYMBLISTSIZE]; //符号表主表

typedef struct ARGLITEM //参数表项
{
	char name[MAX_IDLEN]; //参数名
	TYPELITEM *type;	//数据类型
	SYMCAT cat;  //种类
	int OFFSET; //根据cat指向其他各表项的指针
}ARGLITEM;

int ArglLine; //当前已占用的行数，也就是说有效数据的范围为ARGL[0~ArglLine-1]
ARGLITEM ARGL[MAX_SYMBLISTSIZE]; //参数表

typedef struct PFINFLITEM //过函信息表项
{
	int LEVEL; //层次号，在GGM Language中一定为1
	int OFFSET; //区距，函数实际操作区域（连接区域外）底部距离函数数据区基址的距离
	int FN; //参数个数
	int ENTRY; //入口地址（预留，暂不用）
	ARGLITEM* PARAM; //指向形参表（该函数的第一个形参）
}PFINFLITEM;

int PFInflLine; //当前已占用的行数，也就是说有效数据的范围为PFINFL[0~PFInflLine-1]
PFINFLITEM PFINFL[MAX_SYMBLISTSIZE]; //过函信息表

typedef union CONSLITEM //常数表项
{
	int d;
	double f;
}CONSLITEM;

int ConslLine; //当前已占用的行数，也就是说有效数据的范围为CONSL[0~ConslLine-1]
CONSLITEM CONSL[MAX_SYMBLISTSIZE]; //常数表

int LenlLine; //当前已占用的行数，也就是说有效数据的范围为LENL[0~LenlLine-1]
int LENL[MAX_SYMBLISTSIZE]; //长度表

typedef struct MIDVLITEM //中间（临时）变量表项
{
	char name[MAX_IDLEN]; //变量名
	TYPELITEM *type; //数据类型
	int OFFSET; //区距
}MIDVLITEM;

int MidvlLine; //当前已占用的行数，也就是说有效数据的范围为MIDVL[0~MidvlLine-1]
MIDVLITEM MIDVL[MAX_SYMBLISTSIZE]; //中间（临时）变量表，在表达式求值中使用

#pragma once
#include <stdbool.h>
#include "SymbolList.h"
#include "GrammarAnalyser.h"

//对四元式列表生成目标代码， SeqList为四元式列表， SeqLine为四元式的数目, target为输出的文件，成功返回1， 失败返回0
int DestGenerator(SEQUENCE *SeqList, int SeqLine, FILE *target);
//生成单句四元式的目标代码，成功返回1， 失败返回0
int SingleDestGenerate(SEQUENCE Seq);

char buff[MAX_STRLEN]; //缓冲区

int param_num; //用于记录已经传参的个数，遇PARAM语句自增，遇CALL语句清零

int IF_SP; //IF编号的栈指针
int IF_Stack[MAX_SYMBLISTSIZE]; //IF编号栈
int WHILE_SP; //WHILE编号的栈指针
int WHILE_Stack[MAX_SYMBLISTSIZE]; //WHILE编号栈

//子处理函数

//IF编号压栈，成功返回1，失败返回0
int IF_Push(int id);
//IF编号出栈，成功返回1，失败返回0
int IF_Pop();
//WHILE编号压栈，成功返回1，失败返回0
int WHILE_Push(int id);
//WHILE编号出栈，成功返回1，失败返回0
int WHILE_Pop();

//将AX中的值输出至SYMBL[RDL_SYMBLID]，并将RDL_SYMBLID置为-1，成功返回1，失败（RDL_SYMBLID超出SYMBL范围）返回0，每个跳转语句的开头（即基本块结尾）都必须调用该函数！
int Reg_ST();
//将SYMBL[symbl_id]的值读入AX，成功返回1， 失败（symbl_id超出SYMBL范围）返回0
int Reg_LD(int symbl_id);

//处理函数局部变量定义, Seq为对应四元式, 成功返回1，失败返回0
int sub_VT(SEQUENCE Seq);
//处理加法, Seq为对应四元式, 成功返回1，失败返回0
int sub_ADD(SEQUENCE Seq);
//处理减法, Seq为对应四元式, 成功返回1，失败返回0
int sub_SUB(SEQUENCE Seq);
//处理乘法, Seq为对应四元式, 成功返回1，失败返回0
int sub_MUL(SEQUENCE Seq);
//处理除法, Seq为对应四元式, MOD为True时代表取余，成功返回1，失败返回0
int sub_DIV(SEQUENCE Seq, bool isMOD);
//处理赋值, Seq为对应四元式，成功返回1，失败返回0
int sub_ASSI(SEQUENCE Seq);
//处理关系运算符, Seq为对应四元式，成功返回1，失败返回0
int sub_Relation(SEQUENCE Seq);
//处理逻辑运算符, Seq为对应四元式，成功返回1，失败返回0
int sub_Logic(SEQUENCE Seq);
//处理传参语句, Seq为对应四元式，成功返回1，失败返回0
int sub_PARAM(SEQUENCE Seq);
//处理函数调用语句, Seq为对应四元式，成功返回1，失败返回0
int sub_CALL(SEQUENCE Seq);
//处理函数返回语句, Seq为对应四元式，成功返回1，失败返回0
int sub_RET(SEQUENCE Seq);
//处理函数声明, Seq为对应四元式，成功返回1，失败返回0
int sub_FUNC(SEQUENCE Seq);
//处理函数声明结尾, Seq为对应四元式，成功返回1，失败返回0
int sub_EF(SEQUENCE Seq);
//处理IF语句开头, Seq为对应四元式，成功返回1，失败返回0
int sub_IF(SEQUENCE Seq);
//处理ELSE语句, Seq为对应四元式，成功返回1，失败返回0
int sub_EL(SEQUENCE Seq);
//处理IE, Seq为对应四元式，成功返回1，失败返回0
int sub_IE(SEQUENCE Seq);
//处理WH, Seq为对应四元式，成功返回1，失败返回0
int sub_WHILE(SEQUENCE Seq);
//处理DO, Seq为对应四元式，成功返回1，失败返回0
int sub_DO(SEQUENCE Seq);
//处理WE, Seq为对应四元式，成功返回1，失败返回0
int sub_WE(SEQUENCE Seq);
//处理PUTC, Seq为对应四元式，成功返回1，失败返回0
int sub_PUTC(SEQUENCE Seq);
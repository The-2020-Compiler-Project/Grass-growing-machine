#pragma once
#include "SymbolList.h"
#include "GrammarAnalyser.h"

//对四元式列表生成目标代码， SeqList为四元式列表， SeqLine为四元式的数目, target为输出的文件，成功返回1， 失败返回0
int DestGenerator(SEQUENCE *SeqList, int SeqLine, FILE *target);
//生成单句四元式的目标代码，成功返回1， 失败返回0
int SingleDestGenerate(SEQUENCE Seq);

char buff[MAX_STRLEN]; //缓冲区

int param_num; //用于记录已经传参的个数，遇PARAM语句自增，遇CALL语句清零

//子处理函数

//处理函数局部变量定义, Seq为对应四元式, 成功返回1，失败返回0
int sub_VT(SEQUENCE Seq);
//处理加法, Seq为对应四元式, 成功返回1，失败返回0
int sub_ADD(SEQUENCE Seq);
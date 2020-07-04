#pragma once
#include <stdio.h>
#include <string.h>
#define MAX_TABLELEN 1024
#define MAX_STRLEN 256
#define MAX_IDLEN 30

FILE* srcfile; //文件指针

int iTline;	//当前一共占用的行数，也就是说有效数据的范围为iTable[0~iTline-1]
char iTable[MAX_TABLELEN][MAX_IDLEN]; //标识符表

int cTline;	//当前一共占用的行数，也就是说有效数据的范围为cTable[0~cTline-1]
char cTable[MAX_TABLELEN]; //字符型常量表

int sTline; //当前一共占用的行数，也就是说有效数据的范围为sTable[0~sTline-1]
char sTable[MAX_TABLELEN][MAX_STRLEN]; //字符串常量表

int dcTline; //当前一共占用的行数，也就是说有效数据的范围为dcTable[0~dcTline-1]
int dcTable[MAX_TABLELEN]; //整数常数表

int fcTline; //当前一共占用的行数，也就是说有效数据的范围为fcTable[0~fcTline-1]
double fcTable[MAX_TABLELEN]; //浮点常数表

//初始化表以及有效行数
int init_tables();

//向标识符表中添加新项，并++iTline，成功时返回1，失败返回0
int new_iTableItem(char* name);
//向字符表中添加新项，并++cTline，成功时返回1，失败返回0
int new_cTableItem(char ch);
//向字符串表中添加新项，并++sTline，成功时返回1，失败返回0
int new_sTableItem(char* str);
//向整数常数表中添加新项，并++dcTline，成功时返回1，失败返回0
int new_dcTableItem(int dcn);
//向浮点常数表中添加新项，并++fcTline，成功时返回1，失败返回0
int new_fcTableItem(double fcn);
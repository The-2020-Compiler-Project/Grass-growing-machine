#include "Tables.h"

//初始化表以及有效行数
int init_tables()
{
	iTline = 0;
	memset(iTable, MAX_TABLELEN * MAX_STRLEN * sizeof(char), 0);

	cTline = 0;
	memset(cTable, MAX_TABLELEN * sizeof(char), 0);

	sTline = 0;
	memset(sTable, MAX_TABLELEN * MAX_STRLEN * sizeof(char), 0);

	dcTline = 0;
	memset(dcTable, MAX_TABLELEN * sizeof(int), 0);

	fcTline = 0;
	memset(fcTable, MAX_TABLELEN * sizeof(double), 0);

	return 1;
}

//向标识符表中添加新项，并++iTline，成功时返回1，失败返回0
int new_iTableItem(char* name)
{
	if (iTline < MAX_TABLELEN)
	{
		for (int i = 0; i < MAX_IDLEN; i++)
		{
			iTable[iTline][i] = name[i];
		}
		iTline++;
		return 1;
	}
	else
	{
		printf("The iTable has reached its max length");
		return 0;
	}
}

//向字符表中添加新项，并++cTline，成功时返回1，失败返回0
int new_cTableItem(char ch)
{
	if (cTline < MAX_TABLELEN)
	{
		cTable[cTline] = ch;
		cTline++;
		return 1;
	}
	else
	{
		printf("The cTable has reached its max length");
		return 0;
	}
}

//向字符串表中添加新项，并++sTline，成功时返回1，失败返回0
int new_sTableItem(char* str)
{
	if (sTline < MAX_TABLELEN)
	{
		for (int i = 0; i < 30; i++)
		{
			sTable[sTline][i] = str[i];
			sTline++;
		}
		return 1;
	}
	else
	{
		printf("The sTable has reached its max length");
		return 0;
	}
}

//向整数常数表中添加新项，并++dcTline，成功时返回1，失败返回0
int new_dcTableItem(int dcn)
{
	if (dcTline < MAX_TABLELEN)
	{
		dcTable[dcTline] = dcn;
		dcTline++;
		return 1;
	}
	else
	{
		printf("The dcTable has reached its max length");
		return 0;
	}
}

//向浮点常数表中添加新项，并++fcTline，成功时返回1，失败返回0
int new_fcTableItem(double fcn)
{
	if (fcTline < MAX_TABLELEN)
	{
		fcTable[fcTline] = fcn;
		fcTline++;
		return 1;
	}
	else
	{
		printf("The fcTable has reached its max length");
		return 0;
	}
}


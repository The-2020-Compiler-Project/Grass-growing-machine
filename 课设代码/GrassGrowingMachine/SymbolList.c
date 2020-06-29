#include "SymbolList.h"

int init_symbl()
{
	//四元式表清空
	SeqLine = 0;
	memset(SequenceList, 0, MAXSEQLISTSIZE * sizeof(SEQUENCE));
	//主表清空
	SymblLine = 0;
	memset(SYMBL, 0, MAX_SYMBLISTSIZE * sizeof(SYMBLITEM));
	//过函信息表清空
	PFInflLine = 0;
	memset(PFINFL, 0, MAX_SYMBLISTSIZE * sizeof(PFINFLITEM));
	//参数表清空
	ArglLine = 0;
	memset(ARGL, 0, MAX_SYMBLISTSIZE * sizeof(ARGLITEM));
	//类型表清空
	TypelLine = 0;
	memset(TYPEL, 0, MAX_SYMBLISTSIZE * sizeof(TYPELITEM));
	//常数表清空
	ConslLine = 0;
	memset(CONSL, 0, MAX_SYMBLISTSIZE * sizeof(CONSLITEM));
	//长度表清空
	LenlLine = 0;
	memset(LENL, 0, MAX_SYMBLISTSIZE * sizeof(int));
	//中间变量表清空
	MidvlLine = 0;
	memset(MIDVL, 0, MAX_SYMBLISTSIZE * sizeof(MIDVLITEM));
	return 1;
}
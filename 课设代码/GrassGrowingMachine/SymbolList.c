#include "SymbolList.h"

int init_symbl()
{
	//初始化表
	//四元式表清空
	SeqLine = 0;
	memset(SequenceList, 0, MAXSEQLISTSIZE * sizeof(SEQUENCE));
	OptSeqLine = 0;
	memset(OptimizedSeqList, 0, MAXSEQLISTSIZE * sizeof(SEQUENCE));
	//主表清空
	SymblLine = 0;
	memset(SYMBL, 0, MAX_SYMBLISTSIZE * sizeof(SYMBLITEM));
	//过函信息表清空
	PFInflLine = 0;
	memset(PFINFL, 0, MAX_SYMBLISTSIZE * sizeof(PFINFLITEM));
	//参数表清空
	ArglLine = 0;
	memset(ARGL, 0, MAX_SYMBLISTSIZE * sizeof(ARGLITEM));
	//类型表清空&设置三类基础类型
	TypelLine = 0;
	memset(TYPEL, 0, MAX_SYMBLISTSIZE * sizeof(TYPELITEM));
	TYPEL[0].tval = iTVAL; TYPEL[0].tpoint = DEST_INT_SIZE;
	TYPEL[1].tval = rTVAL; TYPEL[1].tpoint = DEST_REAL_SIZE;
	TYPEL[2].tval = cTVAL; TYPEL[2].tpoint = DEST_CHAR_SIZE;
	TypelLine = 3;
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

int sendSequence(SEQUENCE seq)
{
	if (SeqLine == MAXSEQLISTSIZE) return 0;
	SequenceList[SeqLine] = seq;
	++SeqLine;
	return 1;
}
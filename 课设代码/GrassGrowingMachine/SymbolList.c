#define _CRT_SECURE_NO_WARNINGS
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

int Output_SeqList(SEQUENCE* SeqList, int SeqLine, FILE* fp)
{
	if (fp == NULL) return 0;
	for (int i = 0; i < SeqLine; ++i)
	{
		SEQUENCE seq = SeqList[i];
		char opstr[10];
		OPRtoStr(seq.op, opstr);
		fprintf(fp, "( %s ,\t", opstr);
		switch (seq.arg1.type)
		{
		case seqNONE:
			fprintf(fp, "_,\t");
			break;
		case seqDC:
			fprintf(fp, "%d,\t", seq.arg1.content.d);
			break;
		case seqFC:
			fprintf(fp, "%.2lf,\t", seq.arg1.content.f);
			break;
		case seqCHAR:
			fprintf(fp, "'%c',\t", seq.arg1.content.ch);
			break;
		case seqID:
			fprintf(fp, "%s[%d],\t", seq.arg1.content.str, seq.arg1.active);
			break;
		default:
			return 0;
			break;
		}
		switch (seq.arg2.type)
		{
		case seqNONE:
			fprintf(fp, "_,\t");
			break;
		case seqDC:
			fprintf(fp, "%d,\t", seq.arg2.content.d);
			break;
		case seqFC:
			fprintf(fp, "%.2lf,\t", seq.arg2.content.f);
			break;
		case seqCHAR:
			fprintf(fp, "'%c',\t", seq.arg2.content.ch);
			break;
		case seqID:
			fprintf(fp, "%s[%d],\t", seq.arg2.content.str, seq.arg2.active);
			break;
		default:
			return -1;
			break;
		}
		switch (seq.target.type)
		{
		case seqNONE:
			fprintf(fp, "_ )\n");
			break;
		case seqID:
			fprintf(fp, "%s[%d] )\n", seq.target.content.str, seq.target.active);
			break;
		default:
			return -2;
			break;
		}
	}
	return 1;
}

int Output_SYMBL(FILE* fp)
{
	if (fp == NULL) return 0;
	for (int i = 0; i < SymblLine; ++i)
	{
		fprintf(fp, "%s,\t", SYMBL[i].name);
		if (SYMBL[i].type == TYPEL_INT)
		{
			fprintf(fp, "itp,\t");
		}
		else if (SYMBL[i].type == TYPEL_REAL)
		{
			fprintf(fp, "rtp,\t");
		}
		else if (SYMBL[i].type == TYPEL_CHAR)
		{
			fprintf(fp, "ctp,\t");
		}
		else
		{
			fprintf(fp, "others,\t");
		}
		switch (SYMBL[i].cat)
		{
		case gvCAT:
			fprintf(fp, "gvCAT,\t");
			fprintf(fp, "---,\t");
			fprintf(fp, "%d\n", SYMBL[i].OFFSET);
			break;
		case svCAT:
			fprintf(fp, "svCAT,\t");
			fprintf(fp, "%d,\t", SYMBL[i].addr);
			fprintf(fp, "%d\n", SYMBL[i].OFFSET);
			break;
		case vfCAT:
			fprintf(fp, "vfCAT,\t");
			fprintf(fp, "%d,\t", SYMBL[i].addr);
			fprintf(fp, "%d\n", SYMBL[i].OFFSET);
			break;
		case fCAT:
			fprintf(fp, "fCAT,\t");
			fprintf(fp, "%d,\t", SYMBL[i].addr);
			fprintf(fp, "%d\n", SYMBL[i].OFFSET);
			break;
		case vnCAT:
			fprintf(fp, "vnCAT,\t");
			fprintf(fp, "%d,\t", SYMBL[i].addr);
			fprintf(fp, "%d\n", SYMBL[i].OFFSET);
			break;
		default:
			fprintf(fp, "?CAT, \t");
			fprintf(fp, "%d,\t", SYMBL[i].addr);
			fprintf(fp, "%d\n", SYMBL[i].OFFSET);
			break;
		}
	}
	return 0;
}

int OPRtoStr(OPR op, char* str)
{
	switch (op)
	{
	case PROG:
		strcpy(str, "PROG");
		break;
	case GV:
		strcpy(str, "GV");
		break;
	case GVE:
		strcpy(str, "GVE");
		break;
	case ENT:
		strcpy(str, "ENT");
		break;
	case EXIT:
		strcpy(str, "EXIT");
		break;
	case END:
		strcpy(str, "END");
		break;
	case VAR:
		strcpy(str, "VAR");
		break;
	case VT:
		strcpy(str, "VT");
		break;
	case ADD:
		strcpy(str, "ADD");
		break;
	case SUB:
		strcpy(str, "SUB");
		break;
	case MUL:
		strcpy(str, "MUL");
		break;
	case DIV:
		strcpy(str, "DIV");
		break;
	case MOD:
		strcpy(str, "MOD");
		break;
	case ASSI:
		strcpy(str, "ASSI");
		break;
	case GT:
		strcpy(str, "GT");
		break;
	case GE:
		strcpy(str, "GE");
		break;
	case LT:
		strcpy(str, "LT");
		break;
	case LE:
		strcpy(str, "LE");
		break;
	case EQ:
		strcpy(str, "EQ");
		break;
	case NE:
		strcpy(str, "NE");
		break;
	case AND:
		strcpy(str, "AND");
		break;
	case OR:
		strcpy(str, "OR");
		break;
	case PARAM:
		strcpy(str, "PARAM");
		break;
	case CALL:
		strcpy(str, "CALL");
		break;
	case RET:
		strcpy(str, "RET");
		break;
	case FUNC:
		strcpy(str, "FUNC");
		break;
	case EF:
		strcpy(str, "EF");
		break;
	case IF:
		strcpy(str, "IF");
		break;
	case EL:
		strcpy(str, "EL");
		break;
	case IE:
		strcpy(str, "IE");
		break;
	case WH:
		strcpy(str, "WH");
		break;
	case DO:
		strcpy(str, "DO");
		break;
	case WE:
		strcpy(str, "WE");
		break;
	case PUTC:
		strcpy(str, "PUTC");
		break;
	default:
		return 0;
		break;
	}
	return 1;
}
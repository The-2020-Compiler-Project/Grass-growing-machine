#include "DestGenerator.h"

int DestGenerator(SEQUENCE* SeqList, int SeqLine, FILE *target)
{
	if (!target) return 0;
	for (int i = 0; i < SeqLine; ++i)
	{
		if (!SingleDestGenerate(SeqList[i]))
		{
			return 0;
		}
		fprintf(target, buff);
	}
	return 1;
}

int sub_VT(SEQUENCE Seq)
{
	if (!(Seq.op == VT)) return 0;
	int get_id = getAvailableVar(Seq.arg1.content.str);
	if (get_id == -1) return 0;
	int size; //该数据所占字节数
	if (SYMBL[get_id].type == TYPEL_INT)
	{
		size = DEST_INT_SIZE;
	}
	else if (SYMBL[get_id].type == TYPEL_REAL)
	{
		size = DEST_REAL_SIZE;
	}
	else if (SYMBL[get_id].type == TYPEL_CHAR)
	{
		size = DEST_CHAR_SIZE;
	}
	else if (SYMBL[get_id].type->tval == aVAL)
	{
		AINFL* list = (AINFL*)SYMBL[get_id].type->tpoint;
		size = (list->up - list->low + 1) * list->CLEN * 2; //CLEN默认为字长
	}
	else if (SYMBL[get_id].type->tval == dVAL)
	{
		RINFL* list = (RINFL*)SYMBL[get_id].type->tpoint;
		size = list->totalsize * 2; //totalsize默认为字长
	}
	else
	{
		return 0;
	}
	sprintf(buff, "\tSUB SP, %d\n", size);
	return 1;
}

int sub_ADD(SEQUENCE Seq)
{
	if (!(Seq.op == ADD)) return 0;
	bool isArg1AnID = false; //参数1是否为标识符
	int val_arg1; //若参数1是常数时的值
	switch (Seq.arg1.type)
	{
	case seqDC:
		val_arg1 = Seq.arg1.content.d;
		break;
	case seqCHAR:
		val_arg1 = Seq.arg1.content.ch;
		break;
	case seqID:
		isArg1AnID = true;
		break;
	default:
		break;
	}
}

int SingleDestGenerate(SEQUENCE Seq)
{
	switch (Seq.op)
	{
	case PROG:
		sprintf(buff, "_SSEG SEGMENT STACK\n_STK DW 255 DUP(0)\n_SSEG ENDS\n");
		break;
	case GV:
		sprintf(buff, "_DSEG SEGMENT\n_VARS DB %d DUP(0)\n_DSEG ENDS\n_CSEG SEGMENT\n\tASSUME CS:CSEG, DS:DSEG, SS:SSEG\n", iAvalDSegOffset);
		break; 
	case GVE:
		//不需要做任何事
		break;
	case ENT:
		sprintf(buff, "_ENTRY:");
		break; 
	case EXIT:
		sprintf(buff, "\tMOV AX, 4C00H\n\tINT 21H\n");
		break; 
	case END:
		sprintf(buff, "CSEG ENDS\n\tEND _ENTRY");
		break;
	case VAR:
		//不需要做任何事
		break;
	case VT:
		sub_VT(Seq);
		break;
	case ADD:
		
		break;
	case SUB:
		
		break;
	case MUL:
		
		break;
	case DIV:
		
		break;
	case MOD:
		
		break;
	case ASSI:

		break;
	case GT:
		
		break;
	case GE:
		
		break;
	case LT:

		break;
	case LE:
		
		break;
	case EQ:
		
		break;
	case NE:
		
		break;
	case AND:
		
		break;
	case OR:

		break;
	case PARAM:
		
		break;
	case CALL:
		
		break;
	case RET:
		
		break;
	case FUNC:
		
		break;
	case EF:

		break;
	case IF:
		
		break;
	case EL:
		
		break;
	case IE:

		break;
	case WH:
		
		break;
	case DO:
		
		break;
	case WE:

		break;
	case PUTC:

		break;
	default:
		break;
	}
}
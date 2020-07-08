#define _CRT_SECURE_NO_WARNINGS
#include "DestGenerator.h"

int RDL_SYMBLID = -1; //当前寄存器所存的数据在符号表中的ID
int iCurrentParamNum = 0; //当前已经传参的个数
char strCurrentFuncName[MAX_IDLEN]; //当前的函数名
int iIfNum = 0; //目前的if语句个数
int iWhileNum = 0; //目前的while语句个数
int IF_SP = 0; //IF编号的栈指针
int WHILE_SP = 0; //WHILE编号的栈指针
char* buffptr = buff; //总是指向buff的结尾（\0）
int totalFuncOffset = -1; //当前函数返回值+OLDBP+形参区所占的大小
int currentFuncSize = -1; //当前函数返回值区大小

int IF_Push(int id)
{
	if (IF_SP == MAX_SYMBLISTSIZE - 1) return 0;
	IF_Stack[IF_SP] = id;
	++IF_SP;
	return 1;
}

int IF_Pop()
{
	if (IF_SP == 0) return 0;
	--IF_SP;
	return 1;
}

int WHILE_Push(int id)
{
	if (WHILE_SP == MAX_SYMBLISTSIZE - 1) return 0;
	WHILE_Stack[WHILE_SP] = id;
	++WHILE_SP;
	return 1;
}

int WHILE_Pop()
{
	if (WHILE_SP == 0) return 0;
	--WHILE_SP;
	return 1;
}

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
		strcpy(buff, "\0");
		buffptr = buff;
	}
	return 1;
}

int Reg_ST()
{
	if (RDL_SYMBLID < 0 || RDL_SYMBLID >= SymblLine) return 0;
	switch (SYMBL[RDL_SYMBLID].cat)
	{
	case gvCAT:
		buffptr += sprintf(buffptr, "\tMOV BX, %d\n", SYMBL[RDL_SYMBLID].OFFSET);
		buffptr += sprintf(buffptr, "\tMOV [BX], AX\n");
		RDL_SYMBLID = -1;
		break;
	case svCAT: 
	case vfCAT:
		buffptr += sprintf(buffptr, "\tMOV [BP - %d], AX\n", SYMBL[RDL_SYMBLID].OFFSET+2);
		RDL_SYMBLID = -1;
		break;
	case vnCAT:
		buffptr += sprintf(buffptr, "\tMOV BX, [BP - %d]\n", SYMBL[RDL_SYMBLID].OFFSET+2);
		buffptr += sprintf(buffptr, "\tMOV ES, BX\n");
		buffptr += sprintf(buffptr, "\tMOV BX, [BP - %d - 02H]\n", SYMBL[RDL_SYMBLID].OFFSET+2);
		buffptr += sprintf(buffptr, "\tMOV ES:[BX], AX\n");
		RDL_SYMBLID = -1;
		break;
	default:
		break;
	}
	return 1;
}

int Reg_LD(int symbl_id)
{
	if (symbl_id < 0 || symbl_id >= SymblLine) return 0;
	switch (SYMBL[symbl_id].cat)
	{
	case gvCAT:
		buffptr += sprintf(buffptr, "\tMOV AX, DS:[%d]\n", SYMBL[symbl_id].OFFSET);
		break;
	case vfCAT:
	case svCAT:
		buffptr += sprintf(buffptr, "\tMOV AX, [BP - %d]\n", SYMBL[symbl_id].OFFSET+2);
		break;
	case vnCAT:
		buffptr += sprintf(buffptr, "\tMOV BX, [BP - %d]\n", SYMBL[symbl_id].OFFSET+2);
		buffptr += sprintf(buffptr, "\tMOV ES, BX\n");
		buffptr += sprintf(buffptr, "\tMOV BX, [BP - %d - 02H]\n", SYMBL[symbl_id].OFFSET+2);
		buffptr += sprintf(buffptr, "\tMOV AX, ES:[BX]\n");
		break;
	default:
		break;
	}

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
	buffptr += sprintf(buffptr, "\tSUB SP, %d\n", size); //倒序压栈
	return 1;
}

int sub_ADD(SEQUENCE Seq)
{
	if (!(Seq.op == ADD)) return 0;
	bool isArg1AnID = false; //参数1是否为标识符
	int val_arg1; //若参数1是常数时的值
	char name_arg1[MAX_IDLEN]; //若参数1是标识符时的名字
	bool isArg2AnID = false; //参数2是否为标识符
	int val_arg2; //若参数2是常数时的值
	char name_arg2[MAX_IDLEN]; //若参数2是标识符时的名字
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
		strcpy(name_arg1, Seq.arg1.content.str);
		break;
	default:
		break;
	}
	if (!isArg1AnID)
	{
		buffptr += sprintf(buffptr, "\tMOV AX, %d\n", val_arg1);
	}
	else //arg1为标识符
	{
		int get_id = getAvailableVar(name_arg1); //arg1在SYMBL中的编号
		if (get_id == -1) return 0;
		if (RDL_SYMBLID != get_id)
		{
			//将AX中的值输出给SYMBL[RDL_SYMBLID]
			Reg_ST(); //该函数中会判断RDL_SYMBLID是否为0
			//将第一个参数的值读入AX
			Reg_LD(get_id);
		}
		else if (Seq.arg1.active == true) //相等但是参数1活跃
		{
			//存储参数1 
			Reg_ST();
		}
	}
	switch (Seq.arg2.type)
	{
	case seqDC:
		val_arg2 = Seq.arg2.content.d;
		break;
	case seqCHAR:
		val_arg2 = Seq.arg2.content.ch;
		break;
	case seqID:
		isArg2AnID = true;
		strcpy(name_arg2, Seq.arg2.content.str);
		break;
	default:
		break;
	}
	if (!isArg2AnID)
	{
		Reg_ST();
		buffptr += sprintf(buffptr, "\tADD AX, %d\n", val_arg2);
	}
	else //arg2为标识符
	{
		int get_id = getAvailableVar(name_arg2); //arg2在SYMBL中的编号
		if (get_id == -1) return 0;
		switch (SYMBL[get_id].cat)
		{
		case gvCAT:
			buffptr += sprintf(buffptr, "\tADD AX, DS:[%d]\n", SYMBL[get_id].OFFSET);
			break;
		case vfCAT:
		case svCAT:
			buffptr += sprintf(buffptr, "\tADD AX, [BP - %d]\n", SYMBL[get_id].OFFSET+2);
			break;
		case vnCAT:
			buffptr += sprintf(buffptr, "\tMOV BX, [BP - %d]\n", SYMBL[get_id].OFFSET+2);
			buffptr += sprintf(buffptr, "\tMOV ES, BX\n");
			buffptr += sprintf(buffptr, "\tMOV BX, [BP - %d - 02H]\n", SYMBL[get_id].OFFSET+2);
			buffptr += sprintf(buffptr, "\tADD AX, ES:[BX]\n");
			break;
		default:
			break;
		}
	}
	//置RDL_SYMBLID为目标
	int target_id = getAvailableVar(Seq.target.content.str);
	if (target_id == -1) return 0;
	RDL_SYMBLID = target_id;
	return 1;
}

int sub_SUB(SEQUENCE Seq)
{
	if (!(Seq.op == SUB)) return 0;
	bool isArg1AnID = false; //参数1是否为标识符
	int val_arg1; //若参数1是常数时的值
	char name_arg1[MAX_IDLEN]; //若参数1是标识符时的名字
	bool isArg2AnID = false; //参数2是否为标识符
	int val_arg2; //若参数2是常数时的值
	char name_arg2[MAX_IDLEN]; //若参数2是标识符时的名字
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
		strcpy(name_arg1, Seq.arg1.content.str);
		break;
	default:
		break;
	}
	if (!isArg1AnID)
	{
		Reg_ST();
		buffptr += sprintf(buffptr, "\tMOV AX, %d\n", val_arg1);
	}
	else //arg1为标识符
	{
		int get_id = getAvailableVar(name_arg1); //arg1在SYMBL中的编号
		if (get_id == -1) return 0;
		if (RDL_SYMBLID != get_id)
		{
			//将AX中的值输出给SYMBL[RDL_SYMBLID]
			Reg_ST(); //该函数中会判断RDL_SYMBLID是否为0
			//将第一个参数的值读入AX
			Reg_LD(get_id);
		}
		else if (Seq.arg1.active == true) //相等但是参数1活跃
		{
			//存储参数1 
			Reg_ST();
		}
	}
	switch (Seq.arg2.type)
	{
	case seqDC:
		val_arg2 = Seq.arg2.content.d;
		break;
	case seqCHAR:
		val_arg2 = Seq.arg2.content.ch;
		break;
	case seqID:
		isArg2AnID = true;
		strcpy(name_arg2, Seq.arg2.content.str);
		break;
	default:
		break;
	}
	if (!isArg2AnID)
	{
		buffptr += sprintf(buffptr, "\tSUB AX, %d\n", val_arg2);
	}
	else //arg2为标识符
	{
		int get_id = getAvailableVar(name_arg2); //arg2在SYMBL中的编号
		if (get_id == -1) return 0;
		switch (SYMBL[get_id].cat)
		{
		case gvCAT:
			buffptr += sprintf(buffptr, "\tSUB AX, DS:[%d]\n", SYMBL[get_id].OFFSET);
			break;
		case vfCAT:
		case svCAT:
			buffptr += sprintf(buffptr, "\tSUB AX, [BP - %d]\n", SYMBL[get_id].OFFSET+2);
			break;
		case vnCAT:
			buffptr += sprintf(buffptr, "\tMOV BX, [BP - %d]\n", SYMBL[get_id].OFFSET+2);
			buffptr += sprintf(buffptr, "\tMOV ES, BX\n");
			buffptr += sprintf(buffptr, "\tMOV BX, [BP - %d - 02H]\n", SYMBL[get_id].OFFSET+2);
			buffptr += sprintf(buffptr, "\tSUB AX, ES:[BX]\n");
			break;
		default:
			break;
		}
	}
	//置RDL_SYMBLID为目标
	int target_id = getAvailableVar(Seq.target.content.str);
	if (target_id == -1) return 0;
	RDL_SYMBLID = target_id;
	return 1;
}

int sub_MUL(SEQUENCE Seq)
{
	if (!(Seq.op == MUL)) return 0;
	bool isArg1AnID = false; //参数1是否为标识符
	int val_arg1; //若参数1是常数时的值
	char name_arg1[MAX_IDLEN]; //若参数1是标识符时的名字
	bool isArg2AnID = false; //参数2是否为标识符
	int val_arg2; //若参数2是常数时的值
	char name_arg2[MAX_IDLEN]; //若参数2是标识符时的名字
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
		strcpy(name_arg1, Seq.arg1.content.str);
		break;
	default:
		break;
	}
	if (!isArg1AnID)
	{
		Reg_ST();
		buffptr += sprintf(buffptr, "\tMOV AX, %d\n", val_arg1);
	}
	else //arg1为标识符
	{
		int get_id = getAvailableVar(name_arg1); //arg1在SYMBL中的编号
		if (get_id == -1) return 0;
		if (RDL_SYMBLID != get_id)
		{
			//将AX中的值输出给SYMBL[RDL_SYMBLID]
			Reg_ST(); //该函数中会判断RDL_SYMBLID是否为0
			//将第一个参数的值读入AX
			Reg_LD(get_id);
		}
		else if (Seq.arg1.active == true) //相等但是参数1活跃
		{
			//存储参数1 
			Reg_ST();
		}
	}
	switch (Seq.arg2.type)
	{
	case seqDC:
		val_arg2 = Seq.arg2.content.d;
		break;
	case seqCHAR:
		val_arg2 = Seq.arg2.content.ch;
		break;
	case seqID:
		isArg2AnID = true;
		strcpy(name_arg2, Seq.arg2.content.str);
		break;
	default:
		break;
	}
	if (!isArg2AnID)
	{
		buffptr += sprintf(buffptr, "\tMOV DX, %d\n", val_arg2);
		buffptr += sprintf(buffptr, "\tIMUL DX\n");
	}
	else //arg2为标识符
	{
		int get_id = getAvailableVar(name_arg2); //arg2在SYMBL中的编号
		if (get_id == -1) return 0;
		switch (SYMBL[get_id].cat)
		{
		case gvCAT:
			buffptr += sprintf(buffptr, "\tMOV DX, DS:[%d]\n", SYMBL[get_id].OFFSET);
			buffptr += sprintf(buffptr, "\tIMUL DX\n");
			break;
		case vfCAT:
		case svCAT:
			buffptr += sprintf(buffptr, "\tMOV DX, [BP - %d]\n", SYMBL[get_id].OFFSET+2);
			buffptr += sprintf(buffptr, "\tIMUL DX\n");
			break;
		case vnCAT:
			buffptr += sprintf(buffptr, "\tMOV BX, [BP - %d]\n", SYMBL[get_id].OFFSET+2);
			buffptr += sprintf(buffptr, "\tMOV ES, BX\n");
			buffptr += sprintf(buffptr, "\tMOV BX, [BP - %d - 02H]\n", SYMBL[get_id].OFFSET+2);
			buffptr += sprintf(buffptr, "\tMOV DX, ES:[BX]\n");
			buffptr += sprintf(buffptr, "\tIMUL DX\n");
			break;
		default:
			break;
		}
	}
	//置RDL_SYMBLID为目标
	int target_id = getAvailableVar(Seq.target.content.str);
	if (target_id == -1) return 0;
	RDL_SYMBLID = target_id;
	return 1;
}

int sub_DIV(SEQUENCE Seq, bool isMOD)
{
	if (!(Seq.op == DIV || Seq.op == MOD)) return 0;
	bool isArg1AnID = false; //参数1是否为标识符
	int val_arg1; //若参数1是常数时的值
	char name_arg1[MAX_IDLEN]; //若参数1是标识符时的名字
	bool isArg2AnID = false; //参数2是否为标识符
	int val_arg2; //若参数2是常数时的值
	char name_arg2[MAX_IDLEN]; //若参数2是标识符时的名字
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
		strcpy(name_arg1, Seq.arg1.content.str);
		break;
	default:
		break;
	}
	if (!isArg1AnID)
	{
		Reg_ST();
		buffptr += sprintf(buffptr, "\tMOV AX, %d\n", val_arg1);
	}
	else //arg1为标识符
	{
		int get_id = getAvailableVar(name_arg1); //arg1在SYMBL中的编号
		if (get_id == -1) return 0;
		if (RDL_SYMBLID != get_id)
		{
			//将AX中的值输出给SYMBL[RDL_SYMBLID]
			Reg_ST(); //该函数中会判断RDL_SYMBLID是否为0
			//将第一个参数的值读入AX
			Reg_LD(get_id);
		}
		else if (Seq.arg1.active == true) //相等但是参数1活跃
		{
			//存储参数1 
			Reg_ST();
		}
	}
	switch (Seq.arg2.type)
	{
	case seqDC:
		val_arg2 = Seq.arg2.content.d;
		break;
	case seqCHAR:
		val_arg2 = Seq.arg2.content.ch;
		break;
	case seqID:
		isArg2AnID = true;
		strcpy(name_arg2, Seq.arg2.content.str);
		break;
	default:
		break;
	}
	if (!isArg2AnID)
	{
		buffptr += sprintf(buffptr, "\tMOV BX, %d\n", val_arg2);
		buffptr += sprintf(buffptr, "\tCWD\n");
		buffptr += sprintf(buffptr, "\tIDIV BX\n");
	}
	else //arg2为标识符
	{
		int get_id = getAvailableVar(name_arg2); //arg2在SYMBL中的编号
		if (get_id == -1) return 0;
		switch (SYMBL[get_id].cat)
		{
		case gvCAT:
			buffptr += sprintf(buffptr, "\tMOV BX, DS:[%d]\n", SYMBL[get_id].OFFSET);
			buffptr += sprintf(buffptr, "\tCWD\n");
			buffptr += sprintf(buffptr, "\tIDIV BX\n");
			break;
		case vfCAT:
		case svCAT:
			buffptr += sprintf(buffptr, "\tMOV BX, [BP - %d]\n", SYMBL[get_id].OFFSET+2);
			buffptr += sprintf(buffptr, "\tCWD\n");
			buffptr += sprintf(buffptr, "\tIDIV BX\n");
			break;
		case vnCAT:
			buffptr += sprintf(buffptr, "\tMOV BX, [BP - %d]\n", SYMBL[get_id].OFFSET+2);
			buffptr += sprintf(buffptr, "\tMOV ES, BX\n");
			buffptr += sprintf(buffptr, "\tMOV BX, [BP - %d - 02H]\n", SYMBL[get_id].OFFSET+2);
			buffptr += sprintf(buffptr, "\tMOV BX, ES:[BX]\n");
			buffptr += sprintf(buffptr, "\tCWD\n");
			buffptr += sprintf(buffptr, "\tIDIV BX\n");
			break;
		default:
			break;
		}
	}
	if (isMOD == true)
	{
		buffptr += sprintf(buffptr, "\tMOV AX, DX\n");
	}
	//置RDL_SYMBLID为目标
	int target_id = getAvailableVar(Seq.target.content.str);
	if (target_id == -1) return 0;
	RDL_SYMBLID = target_id;
	return 1;
}

int sub_ASSI(SEQUENCE Seq)
{
	if (!(Seq.op == ASSI)) return 0;
	bool isArg1AnID = false; //参数1是否为标识符
	int val_arg1; //若参数1是常数时的值
	char name_arg1[MAX_IDLEN]; //若参数1是标识符时的名字
	bool isArg2AnID = false; //参数2是否为标识符
	int val_arg2; //若参数2是常数时的值
	char name_arg2[MAX_IDLEN]; //若参数2是标识符时的名字
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
		strcpy(name_arg1, Seq.arg1.content.str);
		break;
	default:
		break;
	}
	if (!isArg1AnID)
	{
		Reg_ST();
		buffptr += sprintf(buffptr, "\tMOV AX, %d\n", val_arg1);
	}
	else //arg1为标识符
	{
		int get_id = getAvailableVar(name_arg1); //arg1在SYMBL中的编号
		if (get_id == -1) return 0;
		if (RDL_SYMBLID != get_id)
		{
			//将AX中的值输出给SYMBL[RDL_SYMBLID]
			Reg_ST(); //该函数中会判断RDL_SYMBLID是否为0
			//将第一个参数的值读入AX
			Reg_LD(get_id);
		}
		else if (Seq.arg1.active == true) //相等但是参数1活跃
		{
			//存储参数1 
			Reg_ST();
		}
	}
	//置RDL_SYMBLID为目标
	int target_id = getAvailableVar(Seq.target.content.str);
	if (target_id == -1) return 0;
	RDL_SYMBLID = target_id;
	return 1;
}

int sub_Relation(SEQUENCE Seq)
{
	if (!(Seq.op == GT || Seq.op == GE || Seq.op == LT || Seq.op == LE || Seq.op == EQ || Seq.op == NE)) return 0;
	bool isArg1AnID = false; //参数1是否为标识符
	int val_arg1; //若参数1是常数时的值
	char name_arg1[MAX_IDLEN]; //若参数1是标识符时的名字
	bool isArg2AnID = false; //参数2是否为标识符
	int val_arg2; //若参数2是常数时的值
	char name_arg2[MAX_IDLEN]; //若参数2是标识符时的名字
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
		strcpy(name_arg1, Seq.arg1.content.str);
		break;
	default:
		break;
	}
	if (!isArg1AnID)
	{
		Reg_ST();
		buffptr += sprintf(buffptr, "\tMOV AX, %d\n", val_arg1);
	}
	else //arg1为标识符
	{
		int get_id = getAvailableVar(name_arg1); //arg1在SYMBL中的编号
		if (get_id == -1) return 0;
		if (RDL_SYMBLID != get_id)
		{
			//将AX中的值输出给SYMBL[RDL_SYMBLID]
			Reg_ST(); //该函数中会判断RDL_SYMBLID是否为0
			//将第一个参数的值读入AX
			Reg_LD(get_id);
		}
		else if (Seq.arg1.active == true) //相等但是参数1活跃
		{
			//存储参数1 
			Reg_ST();
		}
	}
	switch (Seq.arg2.type)
	{
	case seqDC:
		val_arg2 = Seq.arg2.content.d;
		break;
	case seqCHAR:
		val_arg2 = Seq.arg2.content.ch;
		break;
	case seqID:
		isArg2AnID = true;
		strcpy(name_arg2, Seq.arg2.content.str);
		break;
	default:
		break;
	}
	if (!isArg2AnID)
	{
		buffptr += sprintf(buffptr, "\tSUB AX, %d\n", val_arg2);
	}
	else //arg2为标识符
	{
		int get_id = getAvailableVar(name_arg2); //arg2在SYMBL中的编号
		if (get_id == -1) return 0;
		switch (SYMBL[get_id].cat)
		{
		case gvCAT:
			buffptr += sprintf(buffptr, "\tSUB AX, DS:[%d]\n", SYMBL[get_id].OFFSET);
			break;
		case vfCAT:
		case svCAT:
			buffptr += sprintf(buffptr, "\tSUB AX, [BP - %d]\n", SYMBL[get_id].OFFSET+2);
			break;
		case vnCAT:
			buffptr += sprintf(buffptr, "\tMOV BX, [BP - %d]\n", SYMBL[get_id].OFFSET+2);
			buffptr += sprintf(buffptr, "\tMOV ES, BX\n");
			buffptr += sprintf(buffptr, "\tMOV BX, [BP - %d - 02H]\n", SYMBL[get_id].OFFSET+2);
			buffptr += sprintf(buffptr, "\tSUB AX, ES:[BX]\n");
			break;
		default:
			break;
		}
	}
	//处理标志位
	switch (Seq.op)
	{
	case GT:
		//SF=0且ZF=0
		buffptr += sprintf(buffptr, "\tLAHF\n");
		buffptr += sprintf(buffptr, "\tMOV BX, AX\n"); 
		buffptr += sprintf(buffptr, "\tAND AX, 8000H\n"); //取SF
		buffptr += sprintf(buffptr, "\tXOR AH, 80H\n"); //置反
		buffptr += sprintf(buffptr, "\tROL AX, 1\n"); //移至最低位
		buffptr += sprintf(buffptr, "\tAND BX, 4000H\n"); //取ZF
		buffptr += sprintf(buffptr, "\tXOR BH, 40H\n"); //置反
		buffptr += sprintf(buffptr, "\tROL BX, 1\n"); 
		buffptr += sprintf(buffptr, "\tROL BX, 1\n"); //循环左移两次移至最低位
		buffptr += sprintf(buffptr, "\tAND AX, BX\n"); //此时当条件成立时AX=1，否则AX=0
		break;
	case GE:
		//SF=0
		buffptr += sprintf(buffptr, "\tLAHF\n");
		buffptr += sprintf(buffptr, "\tAND AX, 8000H\n"); //取SF
		buffptr += sprintf(buffptr, "\tXOR AH, 80H\n"); //置反
		buffptr += sprintf(buffptr, "\tROL AX, 1\n"); //移至最低位
		break;
	case LT:
		//SF=1
		buffptr += sprintf(buffptr, "\tLAHF\n");
		buffptr += sprintf(buffptr, "\tAND AX, 8000H\n"); //取SF
		buffptr += sprintf(buffptr, "\tROL AX, 1\n"); //此时当条件成立时AX=1，否则AX=0
		break;
	case LE:
		//ZF=1或SF=1
		buffptr += sprintf(buffptr, "\tLAHF\n");
		buffptr += sprintf(buffptr, "\tMOV BX, AX\n");
		buffptr += sprintf(buffptr, "\tAND AX, 8000H\n"); //取SF
		buffptr += sprintf(buffptr, "\tROL AX, 1\n"); //移至最低位
		buffptr += sprintf(buffptr, "\tAND BX, 4000H\n"); //取ZF
		buffptr += sprintf(buffptr, "\tROL BX, 1\n");
		buffptr += sprintf(buffptr, "\tROL BX, 1\n"); //循环左移两次移至最低位
		buffptr += sprintf(buffptr, "\tOR AX, BX\n"); //此时当条件成立时AX=1，否则AX=0
		break;
	case EQ:
		//ZF=1
		buffptr += sprintf(buffptr, "\tLAHF\n");
		buffptr += sprintf(buffptr, "\tAND AX, 4000H\n"); //取ZF
		buffptr += sprintf(buffptr, "\tROL AX, 1\n");
		buffptr += sprintf(buffptr, "\tROL AX, 1\n"); //循环左移两次移至最低位
		break;
	case NE:
		//ZF=0
		buffptr += sprintf(buffptr, "\tLAHF\n");
		buffptr += sprintf(buffptr, "\tAND AX, 4000H\n"); //取ZF
		buffptr += sprintf(buffptr, "\tXOR AH, 40H\n"); //置反
		buffptr += sprintf(buffptr, "\tROL AX, 1\n");
		buffptr += sprintf(buffptr, "\tROL AX, 1\n"); //循环左移两次移至最低位
		break;
	default:
		break;
	}
	//置RDL_SYMBLID为目标
	int target_id = getAvailableVar(Seq.target.content.str);
	if (target_id == -1) return 0;
	RDL_SYMBLID = target_id;
	return 1;
}

int sub_Logic(SEQUENCE Seq)
{
	if (!(Seq.op == AND || Seq.op == OR)) return 0;
	bool isArg1AnID = false; //参数1是否为标识符
	int val_arg1; //若参数1是常数时的值
	char name_arg1[MAX_IDLEN]; //若参数1是标识符时的名字
	bool isArg2AnID = false; //参数2是否为标识符
	int val_arg2; //若参数2是常数时的值
	char name_arg2[MAX_IDLEN]; //若参数2是标识符时的名字
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
		strcpy(name_arg1, Seq.arg1.content.str);
		break;
	default:
		break;
	}
	if (!isArg1AnID)
	{
		Reg_ST();
		buffptr += sprintf(buffptr, "\tMOV AX, %d\n", val_arg1);
	}
	else //arg1为标识符
	{
		int get_id = getAvailableVar(name_arg1); //arg1在SYMBL中的编号
		if (get_id == -1) return 0;
		if (RDL_SYMBLID != get_id)
		{
			//将AX中的值输出给SYMBL[RDL_SYMBLID]
			Reg_ST(); //该函数中会判断RDL_SYMBLID是否为0
			//将第一个参数的值读入AX
			Reg_LD(get_id);
		}
		else if (Seq.arg1.active == true) //相等但是参数1活跃
		{
			//存储参数1 
			Reg_ST();
		}
	}
	//处理第一个参数，将结果置于CX
	buffptr += sprintf(buffptr, "\tMOV CX, AX\n");
	buffptr += sprintf(buffptr, "\tTEST CX, CX\n");
	buffptr += sprintf(buffptr, "\tLAHF\n");
	buffptr += sprintf(buffptr, "\tAND AX, 4000H\n"); //取ZF
	buffptr += sprintf(buffptr, "\tXOR AH, 40H\n"); //置反
	buffptr += sprintf(buffptr, "\tROL AX, 1\n");
	buffptr += sprintf(buffptr, "\tROL AX, 1\n"); //循环左移两次移至最低位
	buffptr += sprintf(buffptr, "\tMOV CX, AX\n");
	switch (Seq.arg2.type)
	{
	case seqDC:
		val_arg2 = Seq.arg2.content.d;
		break;
	case seqCHAR:
		val_arg2 = Seq.arg2.content.ch;
		break;
	case seqID:
		isArg2AnID = true;
		strcpy(name_arg2, Seq.arg2.content.str);
		break;
	default:
		break;
	}
	if (!isArg2AnID)
	{
		buffptr += sprintf(buffptr, "\tMOV DX, %d\n", val_arg2);
	}
	else //arg2为标识符
	{
		int get_id = getAvailableVar(name_arg2); //arg2在SYMBL中的编号
		if (get_id == -1) return 0;
		switch (SYMBL[get_id].cat)
		{
		case gvCAT:
			buffptr += sprintf(buffptr, "\tMOV DX, DS:[%d]\n", SYMBL[get_id].OFFSET);
			break;
		case vfCAT:
		case svCAT:
			buffptr += sprintf(buffptr, "\tMOV DX, [BP - %d]\n", SYMBL[get_id].OFFSET+2);
			break;
		case vnCAT:
			buffptr += sprintf(buffptr, "\tMOV BX, [BP - %d]\n", SYMBL[get_id].OFFSET+2);
			buffptr += sprintf(buffptr, "\tMOV ES, BX\n");
			buffptr += sprintf(buffptr, "\tMOV BX, [BP - %d - 02H]\n", SYMBL[get_id].OFFSET+2);
			buffptr += sprintf(buffptr, "\tMOV DX, ES:[BX]\n");
			break;
		default:
			break;
		}
	}
	//处理第二个参数，将结果置于AX
	buffptr += sprintf(buffptr, "\tMOV DX, AX\n");
	buffptr += sprintf(buffptr, "\tTEST AX, AX\n");
	buffptr += sprintf(buffptr, "\tLAHF\n");
	buffptr += sprintf(buffptr, "\tAND AX, 4000H\n"); //取ZF
	buffptr += sprintf(buffptr, "\tXOR AH, 40H\n"); //置反
	buffptr += sprintf(buffptr, "\tROL AX, 1\n");
	buffptr += sprintf(buffptr, "\tROL AX, 1\n"); //循环左移两次移至最低位
	//进行逻辑运算
	switch (Seq.op)
	{
	case AND:
		buffptr += sprintf(buffptr, "\tAND AX, CX\n");
		break;
	case OR:
		buffptr += sprintf(buffptr, "\tOR AX, CX\n");
		break;
	default:
		break;
	}
	//置RDL_SYMBLID为目标
	int target_id = getAvailableVar(Seq.target.content.str);
	if (target_id == -1) return 0;
	RDL_SYMBLID = target_id;
	return 1;
}

int sub_PARAM(SEQUENCE Seq)
{
	if (!(Seq.op == PARAM)) return 0;
	bool isArg1AnID = false; //参数1是否为标识符
	int val_arg1; //若参数1是常数时的值
	char name_arg1[MAX_IDLEN]; //若参数1是标识符时的名字
	int func_symblID = Find_SymblItemName(0, Seq.target.content.str);
	if (func_symblID == -1) return 0;
	ARGLITEM* func_paramlist = ((PFINFLITEM*)SYMBL[func_symblID].addr)->PARAM;
	if (iCurrentParamNum >= ((PFINFLITEM*)SYMBL[func_symblID].addr)->FN)
	{
		return 0;
	}
	if (iCurrentParamNum == 0)
	{
		buffptr += sprintf(buffptr, "\tMOV SI, SP\n"); //记录当前栈顶指针至SI
		int argnum = ((PFINFLITEM*)SYMBL[func_symblID].addr)->FN;
		ARGLITEM* lastarg = &((PFINFLITEM*)SYMBL[func_symblID].addr)->PARAM[argnum - 1];
		int lastarg_size = 0;
		switch (lastarg->cat)
		{
		case vfCAT:
			if (lastarg->type == TYPEL_INT)
			{
				lastarg_size = DEST_INT_SIZE;
			}
			else if (lastarg->type == TYPEL_REAL)
			{
				lastarg_size = DEST_REAL_SIZE;
			}
			else if (lastarg->type == TYPEL_CHAR)
			{
				lastarg_size = DEST_CHAR_SIZE;
			}
			break;
		case vnCAT:
			lastarg_size = DEST_VN_SIZE;
			break;
		default:
			break;
		}
		int FuncOffset = lastarg->OFFSET + lastarg_size;
		buffptr += sprintf(buffptr, "\tLEA SP, [SI - %d]\n", FuncOffset);
	}
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
		strcpy(name_arg1, Seq.arg1.content.str);
		break;
	default:
		break;
	}
	switch (func_paramlist[iCurrentParamNum].cat)
	{
	case vfCAT:
	{
		if (!isArg1AnID)
		{
			Reg_ST();
			buffptr += sprintf(buffptr, "\tMOV AX, %d\n", val_arg1);
		}
		else //arg1为标识符
		{
			int get_id = getAvailableVar(name_arg1); //arg1在SYMBL中的编号
			if (get_id == -1) return 0;
			if (RDL_SYMBLID != get_id)
			{
				//将AX中的值输出给SYMBL[RDL_SYMBLID]
				Reg_ST(); //该函数中会判断RDL_SYMBLID是否为0
				//将第一个参数的值读入AX
				Reg_LD(get_id);
			}
			else if (Seq.arg1.active == true) //相等但是参数1活跃
			{
				//存储参数1 
				Reg_ST();
			}
		}
		buffptr += sprintf(buffptr, "\tMOV SS:[SI - %d], AX\n", func_paramlist[iCurrentParamNum].OFFSET + 2);
		++iCurrentParamNum;
	}
		break;
	case vnCAT:
	{
		if (!isArg1AnID)
		{
			return 0;
		}
		else //arg1为标识符
		{
			int get_id = getAvailableVar(name_arg1); //arg1在SYMBL中的编号
			if (get_id == -1) return 0;
			switch (SYMBL[get_id].cat)
			{
			case svCAT:
			case vfCAT:
				buffptr += sprintf(buffptr, "\tMOV AX, SS\n");
				buffptr += sprintf(buffptr, "\tMOV SS:[SI - %d], AX\n", func_paramlist[iCurrentParamNum].OFFSET + 2);
				buffptr += sprintf(buffptr, "\tLEA AX, [BP - %d]\n", SYMBL[get_id].OFFSET + 2);
				buffptr += sprintf(buffptr, "\tMOV SS:[SI - %d - 02H], AX\n", func_paramlist[iCurrentParamNum].OFFSET + 2);
				break;
			case gvCAT:
				buffptr += sprintf(buffptr, "\tMOV AX, DS\n");
				buffptr += sprintf(buffptr, "\tMOV SS:[SI - %d], AX\n", func_paramlist[iCurrentParamNum].OFFSET + 2);
				buffptr += sprintf(buffptr, "\tMOV BX, %d\n", SYMBL[get_id].OFFSET);
				buffptr += sprintf(buffptr, "\tLEA AX, [BX]\n");
				buffptr += sprintf(buffptr, "\tMOV SS:[SI - %d - 02H], AX\n", func_paramlist[iCurrentParamNum].OFFSET + 2);
				break;
			case vnCAT:
				buffptr += sprintf(buffptr, "\tMOV AX, [BP - %d]\n", SYMBL[get_id].OFFSET + 2);
				buffptr += sprintf(buffptr, "\tMOV SS:[SI - %d], AX\n", func_paramlist[iCurrentParamNum].OFFSET + 2);
				buffptr += sprintf(buffptr, "\tMOV AX, [BP - %d - 02H]\n", SYMBL[get_id].OFFSET + 2);
				buffptr += sprintf(buffptr, "\tMOV SS:[SI - %d - 02H], AX\n", func_paramlist[iCurrentParamNum].OFFSET + 2);
				break;
			default:
				break;
			}
		}
		++iCurrentParamNum;
	}
		break;
	default:
		break;
	}
	return 1;
}

int sub_CALL(SEQUENCE Seq)
{
	if (Seq.op != CALL) return 0;
	if (RDL_SYMBLID != -1)
	{
		Reg_ST();
	}
	buffptr += sprintf(buffptr, "\tMOV SS:[SI-4], BP\n"); //OLD BP //默认函数返回值为单字
	buffptr += sprintf(buffptr, "\tMOV BP, SI\n");
	buffptr += sprintf(buffptr, "\tCALL FAR PTR %s\n", Seq.arg1.content.str);
	buffptr += sprintf(buffptr, "\tPOP AX\n");
	int get_targetid = getAvailableVar(Seq.target.content.str);
	if (get_targetid == -1) return 0;
	buffptr += sprintf(buffptr, "\tMOV DS:[%d], AX\n", SYMBL[get_targetid].OFFSET);
	iCurrentParamNum = 0;
	return 1;
}

int sub_RET(SEQUENCE Seq)
{
	if (Seq.op != RET) return 0;
	bool isArg1AnID = false; //参数1是否为标识符
	int val_arg1; //若参数1是常数时的值
	char name_arg1[MAX_IDLEN]; //若参数1是标识符时的名字
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
		strcpy(name_arg1, Seq.arg1.content.str);
		break;
	default:
		break;
	}
	if (!isArg1AnID)
	{
		Reg_ST();
		buffptr += sprintf(buffptr, "\tMOV AX, %d\n", val_arg1);
	}
	else //arg1为标识符
	{
		int get_id = getAvailableVar(name_arg1); //arg1在SYMBL中的编号
		if (get_id == -1) return 0;
		if (RDL_SYMBLID != get_id)
		{
			//将AX中的值输出给SYMBL[RDL_SYMBLID]
			Reg_ST(); //该函数中会判断RDL_SYMBLID是否为0
			//将第一个参数的值读入AX
			Reg_LD(get_id);
		}
		else if (Seq.arg1.active == true) //相等但是参数1活跃
		{
			//存储参数1 
			Reg_ST();
		}
	}
	//将AX中的值放置到函数返回值存储区
	buffptr += sprintf(buffptr, "\tMOV [BP - 2], AX\n");
	buffptr += sprintf(buffptr, "\tLEA SP, [BP - %d]\n", totalFuncOffset + 2 * DEST_PTR_SIZE); //将SP调整至OLDCSIP所在位置
	buffptr += sprintf(buffptr, "\tMOV BP, [BP - 4]\n"); //恢复BP
	buffptr += sprintf(buffptr, "\tRET %d\n", totalFuncOffset - currentFuncSize); //函数返回，并将栈顶设置为函数返回值单元
	return 1;
}

int sub_FUNC(SEQUENCE Seq)
{
	if (Seq.op != FUNC) return 0;
	strcpy(strCurrentFuncName, Seq.arg1.content.str);
	int get_funcID = Find_SymblItemName(0, strCurrentFuncName);
	if (get_funcID == -1) return 0;
	int argnum = ((PFINFLITEM*)SYMBL[get_funcID].addr)->FN;
	ARGLITEM* lastarg = &((PFINFLITEM*)SYMBL[get_funcID].addr)->PARAM[argnum - 1];
	int lastarg_size = 0;
	switch (lastarg->cat)
	{
	case vfCAT:
		if (lastarg->type == TYPEL_INT)
		{
			lastarg_size = DEST_INT_SIZE;
		}
		else if (lastarg->type == TYPEL_REAL)
		{
			lastarg_size = DEST_REAL_SIZE;
		}
		else if (lastarg->type == TYPEL_CHAR)
		{
			lastarg_size = DEST_CHAR_SIZE;
		}
		break;
	case vnCAT:
		lastarg_size = DEST_VN_SIZE;
		break;
	default:
		break;
	}
	if (SYMBL[get_funcID].type == TYPEL_INT)
	{
		currentFuncSize = DEST_INT_SIZE;
	}
	else if (SYMBL[get_funcID].type == TYPEL_REAL)
	{
		currentFuncSize = DEST_REAL_SIZE;
	}
	else if (SYMBL[get_funcID].type == TYPEL_CHAR)
	{
		currentFuncSize = DEST_CHAR_SIZE;
	}
	totalFuncOffset = lastarg->OFFSET + lastarg_size;
	currentFunc = (PFINFLITEM*)SYMBL[get_funcID].addr;
	buffptr += sprintf(buffptr, "%s\tPROC FAR\n", strCurrentFuncName); //子程序声明
	return 1;
}

int sub_EF(SEQUENCE Seq)
{
	if (Seq.op != EF) return 0;
	buffptr += sprintf(buffptr, "%s\tENDP\n", strCurrentFuncName);
	strcpy(strCurrentFuncName, "\0");
	//currentFunc置空
	currentFunc = NULL;
	currentFuncSize = -1;
	totalFuncOffset = -1;
	return 1;
}

int sub_IF(SEQUENCE Seq)
{
	if (Seq.op != IF) return 0;
	bool isArg1AnID = false; //参数1是否为标识符
	int val_arg1; //若参数1是常数时的值
	char name_arg1[MAX_IDLEN]; //若参数1是标识符时的名字
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
		strcpy(name_arg1, Seq.arg1.content.str);
		break;
	default:
		break;
	}
	if (!isArg1AnID)
	{
		Reg_ST();
		buffptr += sprintf(buffptr, "\tMOV AX, %d\n", val_arg1);
	}
	else //arg1为标识符
	{
		int get_id = getAvailableVar(name_arg1); //arg1在SYMBL中的编号
		if (get_id == -1) return 0;
		if (RDL_SYMBLID != get_id)
		{
			//将AX中的值输出给SYMBL[RDL_SYMBLID]
			Reg_ST(); //该函数中会判断RDL_SYMBLID是否为0
			//将第一个参数的值读入AX
			Reg_LD(get_id);
		}
		else if (Seq.arg1.active == true) //相等但是参数1活跃
		{
			//存储参数1 
			Reg_ST();
		}
	}
	IF_Push(iIfNum);
	++iIfNum;
	buffptr += sprintf(buffptr, "_IF%d:", IF_Stack[IF_SP - 1]); //IF_Stack[IF_SP-1]为栈顶元素
	buffptr += sprintf(buffptr, "\tTEST AX, AX\n");
	buffptr += sprintf(buffptr, "\tJZ _EL%d\n", IF_Stack[IF_SP - 1]);
	return 1;
}

int sub_EL(SEQUENCE Seq)
{
	if (Seq.op != EL) return 0;
	if (RDL_SYMBLID != -1)
	{
		Reg_ST();
	}
	buffptr += sprintf(buffptr, "\tJMP _IE%d\n", IF_Stack[IF_SP - 1]);
	buffptr += sprintf(buffptr, "_EL%d:\tNOP\n", IF_Stack[IF_SP - 1]);
	return 1;
}

int sub_IE(SEQUENCE Seq)
{
	if (Seq.op != IE) return 0;
	buffptr += sprintf(buffptr, "_IE%d:\tNOP\n", IF_Stack[IF_SP - 1]);
	IF_Pop();
	return 1;
}

int sub_WH(SEQUENCE Seq)
{
	if (Seq.op != WH) return 0;
	if (RDL_SYMBLID != -1)
	{
		Reg_ST();
	}
	WHILE_Push(iWhileNum);
	++iWhileNum;
	buffptr += sprintf(buffptr, "_WH%d:", WHILE_Stack[WHILE_SP - 1]);
	return 1;
}

int sub_DO(SEQUENCE Seq)
{
	if (Seq.op != DO) return 0;
	bool isArg1AnID = false; //参数1是否为标识符
	int val_arg1; //若参数1是常数时的值
	char name_arg1[MAX_IDLEN]; //若参数1是标识符时的名字
	buffptr += sprintf(buffptr, "_DO%d:", WHILE_Stack[WHILE_SP - 1]);
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
		strcpy(name_arg1, Seq.arg1.content.str);
		break;
	default:
		break;
	}
	if (!isArg1AnID)
	{
		Reg_ST();
		buffptr += sprintf(buffptr, "\tMOV AX, %d\n", val_arg1);
	}
	else //arg1为标识符
	{
		int get_id = getAvailableVar(name_arg1); //arg1在SYMBL中的编号
		if (get_id == -1) return 0;
		if (RDL_SYMBLID != get_id)
		{
			//将AX中的值输出给SYMBL[RDL_SYMBLID]
			Reg_ST(); //该函数中会判断RDL_SYMBLID是否为0
			//将第一个参数的值读入AX
			Reg_LD(get_id);
		}
		else if (Seq.arg1.active == true) //相等但是参数1活跃
		{
			//存储参数1 
			Reg_ST();
		}
	}
	buffptr += sprintf(buffptr, "\tTEST AX, AX\n");
	buffptr += sprintf(buffptr, "\tJZ _WE%d\n", WHILE_Stack[WHILE_SP - 1]);
	return 1;
}

int sub_WE(SEQUENCE Seq)
{
	if (Seq.op != WE) return 0;
	if (RDL_SYMBLID != -1)
	{
		Reg_ST();
	}
	buffptr += sprintf(buffptr, "\tJMP _WH%d\n", WHILE_Stack[WHILE_SP - 1]);
	buffptr += sprintf(buffptr, "_WE%d:\tNOP\n", WHILE_Stack[WHILE_SP - 1]);
	return 1;
}

int sub_PUTC(SEQUENCE Seq)
{
	if (Seq.op != PUTC) return 0;
	bool isArg1AnID = false; //参数1是否为标识符
	int val_arg1; //若参数1是常数时的值
	char name_arg1[MAX_IDLEN]; //若参数1是标识符时的名字
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
		strcpy(name_arg1, Seq.arg1.content.str);
		break;
	default:
		break;
	}
	if (!isArg1AnID)
	{
		Reg_ST();
		buffptr += sprintf(buffptr, "\tMOV AX, %d\n", val_arg1);
	}
	else //arg1为标识符
	{
		int get_id = getAvailableVar(name_arg1); //arg1在SYMBL中的编号
		if (get_id == -1) return 0;
		if (RDL_SYMBLID != get_id)
		{
			//将AX中的值输出给SYMBL[RDL_SYMBLID]
			Reg_ST(); //该函数中会判断RDL_SYMBLID是否为0
			//将第一个参数的值读入AX
			Reg_LD(get_id);
		}
		else if (Seq.arg1.active == true) //相等但是参数1活跃
		{
			//存储参数1 
			Reg_ST();
		}
	}
	buffptr += sprintf(buffptr, "\tMOV DL, AL\n");
	buffptr += sprintf(buffptr, "\tMOV AH, 02H\n");
	buffptr += sprintf(buffptr, "\tINT 21H\n");
	return 1;
}

int SingleDestGenerate(SEQUENCE Seq)
{
	switch (Seq.op)
	{
	case PROG:
		buffptr += sprintf(buffptr, "_SSEG\tSEGMENT STACK\n_STK\tDW 255 DUP(0)\n_SSEG\tENDS\n");
		break;
	case GV:
		buffptr += sprintf(buffptr, "_DSEG\tSEGMENT\n_VARS\tDB %d DUP(0)\n_DSEG\tENDS\n_CSEG\tSEGMENT\n\tASSUME CS:_CSEG, DS:_DSEG, SS:_SSEG\n", iAvalDSegOffset);
		break; 
	case GVE:
		//不需要做任何事
		break;
	case ENT:
		buffptr += sprintf(buffptr, "_ENTRY:\tMOV AX, _DSEG\n\tMOV DS, AX\n\tMOV AX, _SSEG\n\tMOV SS, AX\n\tMOV SP, SIZE _STK\n\tMOV BP, SIZE _STK\n");
		break; 
	case EXIT:
		buffptr += sprintf(buffptr, "\tMOV AX, 4C00H\n\tINT 21H\n");
		break; 
	case END:
		buffptr += sprintf(buffptr, "_CSEG\tENDS\n\tEND _ENTRY");
		break;
	case VAR:
		//不需要做任何事
		break;
	case VT:
		sub_VT(Seq);
		break;
	case ADD:
		sub_ADD(Seq);
		break;
	case SUB:
		sub_SUB(Seq);
		break;
	case MUL:
		//乘法结果为单字，可能溢出
		sub_MUL(Seq);
		break;
	case DIV:
		sub_DIV(Seq, false);
		break;
	case MOD:
		sub_DIV(Seq, true);
		break;
	case ASSI:
		sub_ASSI(Seq);
		break;
	case GT:
		sub_Relation(Seq);
		break;
	case GE:
		sub_Relation(Seq);
		break;
	case LT:
		sub_Relation(Seq);
		break;
	case LE:
		sub_Relation(Seq);
		break;
	case EQ:
		sub_Relation(Seq);
		break;
	case NE:
		sub_Relation(Seq);
		break;
	case AND:
		sub_Logic(Seq);
		break;
	case OR:
		sub_Logic(Seq);
		break;
	case PARAM:
		sub_PARAM(Seq);
		break;
	case CALL:
		sub_CALL(Seq);
		break;
	case RET:
		sub_RET(Seq);
		break;
	case FUNC:
		//将栈顶调整至函数栈顶部
		sub_FUNC(Seq);
		break;
	case EF:
		sub_EF(Seq);
		break;
	case IF:
		sub_IF(Seq);
		break;
	case EL:
		sub_EL(Seq);
		break;
	case IE:
		sub_IE(Seq);
		break;
	case WH:
		sub_WH(Seq);
		break;
	case DO:
		sub_DO(Seq);
		break;
	case WE:
		sub_WE(Seq);
		break;
	case PUTC:
		sub_PUTC(Seq);
		break;
	default:
		break;
	}
	return 1;
}
#define _CRT_SECURE_NO_WARNINGS
#include "DestGenerator.h"

int RDL_SYMBLID = -1; //��ǰ�Ĵ�������������ڷ��ű��е�ID
int iCurrentParamNum = 0; //��ǰ�Ѿ����εĸ���
char strCurrentFuncName[MAX_IDLEN]; //��ǰ�ĺ�����
int iIfNum = 0; //Ŀǰ��if������
int iWhileNum = 0; //Ŀǰ��while������
int IF_SP = 0; //IF��ŵ�ջָ��
int WHILE_SP = 0; //WHILE��ŵ�ջָ��
char* buffptr = buff; //����ָ��buff�Ľ�β��\0��
int totalFuncOffset = -1; //��ǰ��������ֵ+OLDBP+�β�����ռ�Ĵ�С
int currentFuncSize = -1; //��ǰ��������ֵ����С

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
	int size; //��������ռ�ֽ���
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
		size = (list->up - list->low + 1) * list->CLEN * 2; //CLENĬ��Ϊ�ֳ�
	}
	else if (SYMBL[get_id].type->tval == dVAL)
	{
		RINFL* list = (RINFL*)SYMBL[get_id].type->tpoint;
		size = list->totalsize * 2; //totalsizeĬ��Ϊ�ֳ�
	}
	else
	{
		return 0;
	}
	buffptr += sprintf(buffptr, "\tSUB SP, %d\n", size); //����ѹջ
	return 1;
}

int sub_ADD(SEQUENCE Seq)
{
	if (!(Seq.op == ADD)) return 0;
	bool isArg1AnID = false; //����1�Ƿ�Ϊ��ʶ��
	int val_arg1; //������1�ǳ���ʱ��ֵ
	char name_arg1[MAX_IDLEN]; //������1�Ǳ�ʶ��ʱ������
	bool isArg2AnID = false; //����2�Ƿ�Ϊ��ʶ��
	int val_arg2; //������2�ǳ���ʱ��ֵ
	char name_arg2[MAX_IDLEN]; //������2�Ǳ�ʶ��ʱ������
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
	else //arg1Ϊ��ʶ��
	{
		int get_id = getAvailableVar(name_arg1); //arg1��SYMBL�еı��
		if (get_id == -1) return 0;
		if (RDL_SYMBLID != get_id)
		{
			//��AX�е�ֵ�����SYMBL[RDL_SYMBLID]
			Reg_ST(); //�ú����л��ж�RDL_SYMBLID�Ƿ�Ϊ0
			//����һ��������ֵ����AX
			Reg_LD(get_id);
		}
		else if (Seq.arg1.active == true) //��ȵ��ǲ���1��Ծ
		{
			//�洢����1 
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
	else //arg2Ϊ��ʶ��
	{
		int get_id = getAvailableVar(name_arg2); //arg2��SYMBL�еı��
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
	//��RDL_SYMBLIDΪĿ��
	int target_id = getAvailableVar(Seq.target.content.str);
	if (target_id == -1) return 0;
	RDL_SYMBLID = target_id;
	return 1;
}

int sub_SUB(SEQUENCE Seq)
{
	if (!(Seq.op == SUB)) return 0;
	bool isArg1AnID = false; //����1�Ƿ�Ϊ��ʶ��
	int val_arg1; //������1�ǳ���ʱ��ֵ
	char name_arg1[MAX_IDLEN]; //������1�Ǳ�ʶ��ʱ������
	bool isArg2AnID = false; //����2�Ƿ�Ϊ��ʶ��
	int val_arg2; //������2�ǳ���ʱ��ֵ
	char name_arg2[MAX_IDLEN]; //������2�Ǳ�ʶ��ʱ������
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
	else //arg1Ϊ��ʶ��
	{
		int get_id = getAvailableVar(name_arg1); //arg1��SYMBL�еı��
		if (get_id == -1) return 0;
		if (RDL_SYMBLID != get_id)
		{
			//��AX�е�ֵ�����SYMBL[RDL_SYMBLID]
			Reg_ST(); //�ú����л��ж�RDL_SYMBLID�Ƿ�Ϊ0
			//����һ��������ֵ����AX
			Reg_LD(get_id);
		}
		else if (Seq.arg1.active == true) //��ȵ��ǲ���1��Ծ
		{
			//�洢����1 
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
	else //arg2Ϊ��ʶ��
	{
		int get_id = getAvailableVar(name_arg2); //arg2��SYMBL�еı��
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
	//��RDL_SYMBLIDΪĿ��
	int target_id = getAvailableVar(Seq.target.content.str);
	if (target_id == -1) return 0;
	RDL_SYMBLID = target_id;
	return 1;
}

int sub_MUL(SEQUENCE Seq)
{
	if (!(Seq.op == MUL)) return 0;
	bool isArg1AnID = false; //����1�Ƿ�Ϊ��ʶ��
	int val_arg1; //������1�ǳ���ʱ��ֵ
	char name_arg1[MAX_IDLEN]; //������1�Ǳ�ʶ��ʱ������
	bool isArg2AnID = false; //����2�Ƿ�Ϊ��ʶ��
	int val_arg2; //������2�ǳ���ʱ��ֵ
	char name_arg2[MAX_IDLEN]; //������2�Ǳ�ʶ��ʱ������
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
	else //arg1Ϊ��ʶ��
	{
		int get_id = getAvailableVar(name_arg1); //arg1��SYMBL�еı��
		if (get_id == -1) return 0;
		if (RDL_SYMBLID != get_id)
		{
			//��AX�е�ֵ�����SYMBL[RDL_SYMBLID]
			Reg_ST(); //�ú����л��ж�RDL_SYMBLID�Ƿ�Ϊ0
			//����һ��������ֵ����AX
			Reg_LD(get_id);
		}
		else if (Seq.arg1.active == true) //��ȵ��ǲ���1��Ծ
		{
			//�洢����1 
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
	else //arg2Ϊ��ʶ��
	{
		int get_id = getAvailableVar(name_arg2); //arg2��SYMBL�еı��
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
	//��RDL_SYMBLIDΪĿ��
	int target_id = getAvailableVar(Seq.target.content.str);
	if (target_id == -1) return 0;
	RDL_SYMBLID = target_id;
	return 1;
}

int sub_DIV(SEQUENCE Seq, bool isMOD)
{
	if (!(Seq.op == DIV || Seq.op == MOD)) return 0;
	bool isArg1AnID = false; //����1�Ƿ�Ϊ��ʶ��
	int val_arg1; //������1�ǳ���ʱ��ֵ
	char name_arg1[MAX_IDLEN]; //������1�Ǳ�ʶ��ʱ������
	bool isArg2AnID = false; //����2�Ƿ�Ϊ��ʶ��
	int val_arg2; //������2�ǳ���ʱ��ֵ
	char name_arg2[MAX_IDLEN]; //������2�Ǳ�ʶ��ʱ������
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
	else //arg1Ϊ��ʶ��
	{
		int get_id = getAvailableVar(name_arg1); //arg1��SYMBL�еı��
		if (get_id == -1) return 0;
		if (RDL_SYMBLID != get_id)
		{
			//��AX�е�ֵ�����SYMBL[RDL_SYMBLID]
			Reg_ST(); //�ú����л��ж�RDL_SYMBLID�Ƿ�Ϊ0
			//����һ��������ֵ����AX
			Reg_LD(get_id);
		}
		else if (Seq.arg1.active == true) //��ȵ��ǲ���1��Ծ
		{
			//�洢����1 
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
	else //arg2Ϊ��ʶ��
	{
		int get_id = getAvailableVar(name_arg2); //arg2��SYMBL�еı��
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
	//��RDL_SYMBLIDΪĿ��
	int target_id = getAvailableVar(Seq.target.content.str);
	if (target_id == -1) return 0;
	RDL_SYMBLID = target_id;
	return 1;
}

int sub_ASSI(SEQUENCE Seq)
{
	if (!(Seq.op == ASSI)) return 0;
	bool isArg1AnID = false; //����1�Ƿ�Ϊ��ʶ��
	int val_arg1; //������1�ǳ���ʱ��ֵ
	char name_arg1[MAX_IDLEN]; //������1�Ǳ�ʶ��ʱ������
	bool isArg2AnID = false; //����2�Ƿ�Ϊ��ʶ��
	int val_arg2; //������2�ǳ���ʱ��ֵ
	char name_arg2[MAX_IDLEN]; //������2�Ǳ�ʶ��ʱ������
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
	else //arg1Ϊ��ʶ��
	{
		int get_id = getAvailableVar(name_arg1); //arg1��SYMBL�еı��
		if (get_id == -1) return 0;
		if (RDL_SYMBLID != get_id)
		{
			//��AX�е�ֵ�����SYMBL[RDL_SYMBLID]
			Reg_ST(); //�ú����л��ж�RDL_SYMBLID�Ƿ�Ϊ0
			//����һ��������ֵ����AX
			Reg_LD(get_id);
		}
		else if (Seq.arg1.active == true) //��ȵ��ǲ���1��Ծ
		{
			//�洢����1 
			Reg_ST();
		}
	}
	//��RDL_SYMBLIDΪĿ��
	int target_id = getAvailableVar(Seq.target.content.str);
	if (target_id == -1) return 0;
	RDL_SYMBLID = target_id;
	return 1;
}

int sub_Relation(SEQUENCE Seq)
{
	if (!(Seq.op == GT || Seq.op == GE || Seq.op == LT || Seq.op == LE || Seq.op == EQ || Seq.op == NE)) return 0;
	bool isArg1AnID = false; //����1�Ƿ�Ϊ��ʶ��
	int val_arg1; //������1�ǳ���ʱ��ֵ
	char name_arg1[MAX_IDLEN]; //������1�Ǳ�ʶ��ʱ������
	bool isArg2AnID = false; //����2�Ƿ�Ϊ��ʶ��
	int val_arg2; //������2�ǳ���ʱ��ֵ
	char name_arg2[MAX_IDLEN]; //������2�Ǳ�ʶ��ʱ������
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
	else //arg1Ϊ��ʶ��
	{
		int get_id = getAvailableVar(name_arg1); //arg1��SYMBL�еı��
		if (get_id == -1) return 0;
		if (RDL_SYMBLID != get_id)
		{
			//��AX�е�ֵ�����SYMBL[RDL_SYMBLID]
			Reg_ST(); //�ú����л��ж�RDL_SYMBLID�Ƿ�Ϊ0
			//����һ��������ֵ����AX
			Reg_LD(get_id);
		}
		else if (Seq.arg1.active == true) //��ȵ��ǲ���1��Ծ
		{
			//�洢����1 
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
	else //arg2Ϊ��ʶ��
	{
		int get_id = getAvailableVar(name_arg2); //arg2��SYMBL�еı��
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
	//�����־λ
	switch (Seq.op)
	{
	case GT:
		//SF=0��ZF=0
		buffptr += sprintf(buffptr, "\tLAHF\n");
		buffptr += sprintf(buffptr, "\tMOV BX, AX\n"); 
		buffptr += sprintf(buffptr, "\tAND AX, 8000H\n"); //ȡSF
		buffptr += sprintf(buffptr, "\tXOR AH, 80H\n"); //�÷�
		buffptr += sprintf(buffptr, "\tROL AX, 1\n"); //�������λ
		buffptr += sprintf(buffptr, "\tAND BX, 4000H\n"); //ȡZF
		buffptr += sprintf(buffptr, "\tXOR BH, 40H\n"); //�÷�
		buffptr += sprintf(buffptr, "\tROL BX, 1\n"); 
		buffptr += sprintf(buffptr, "\tROL BX, 1\n"); //ѭ�����������������λ
		buffptr += sprintf(buffptr, "\tAND AX, BX\n"); //��ʱ����������ʱAX=1������AX=0
		break;
	case GE:
		//SF=0
		buffptr += sprintf(buffptr, "\tLAHF\n");
		buffptr += sprintf(buffptr, "\tAND AX, 8000H\n"); //ȡSF
		buffptr += sprintf(buffptr, "\tXOR AH, 80H\n"); //�÷�
		buffptr += sprintf(buffptr, "\tROL AX, 1\n"); //�������λ
		break;
	case LT:
		//SF=1
		buffptr += sprintf(buffptr, "\tLAHF\n");
		buffptr += sprintf(buffptr, "\tAND AX, 8000H\n"); //ȡSF
		buffptr += sprintf(buffptr, "\tROL AX, 1\n"); //��ʱ����������ʱAX=1������AX=0
		break;
	case LE:
		//ZF=1��SF=1
		buffptr += sprintf(buffptr, "\tLAHF\n");
		buffptr += sprintf(buffptr, "\tMOV BX, AX\n");
		buffptr += sprintf(buffptr, "\tAND AX, 8000H\n"); //ȡSF
		buffptr += sprintf(buffptr, "\tROL AX, 1\n"); //�������λ
		buffptr += sprintf(buffptr, "\tAND BX, 4000H\n"); //ȡZF
		buffptr += sprintf(buffptr, "\tROL BX, 1\n");
		buffptr += sprintf(buffptr, "\tROL BX, 1\n"); //ѭ�����������������λ
		buffptr += sprintf(buffptr, "\tOR AX, BX\n"); //��ʱ����������ʱAX=1������AX=0
		break;
	case EQ:
		//ZF=1
		buffptr += sprintf(buffptr, "\tLAHF\n");
		buffptr += sprintf(buffptr, "\tAND AX, 4000H\n"); //ȡZF
		buffptr += sprintf(buffptr, "\tROL AX, 1\n");
		buffptr += sprintf(buffptr, "\tROL AX, 1\n"); //ѭ�����������������λ
		break;
	case NE:
		//ZF=0
		buffptr += sprintf(buffptr, "\tLAHF\n");
		buffptr += sprintf(buffptr, "\tAND AX, 4000H\n"); //ȡZF
		buffptr += sprintf(buffptr, "\tXOR AH, 40H\n"); //�÷�
		buffptr += sprintf(buffptr, "\tROL AX, 1\n");
		buffptr += sprintf(buffptr, "\tROL AX, 1\n"); //ѭ�����������������λ
		break;
	default:
		break;
	}
	//��RDL_SYMBLIDΪĿ��
	int target_id = getAvailableVar(Seq.target.content.str);
	if (target_id == -1) return 0;
	RDL_SYMBLID = target_id;
	return 1;
}

int sub_Logic(SEQUENCE Seq)
{
	if (!(Seq.op == AND || Seq.op == OR)) return 0;
	bool isArg1AnID = false; //����1�Ƿ�Ϊ��ʶ��
	int val_arg1; //������1�ǳ���ʱ��ֵ
	char name_arg1[MAX_IDLEN]; //������1�Ǳ�ʶ��ʱ������
	bool isArg2AnID = false; //����2�Ƿ�Ϊ��ʶ��
	int val_arg2; //������2�ǳ���ʱ��ֵ
	char name_arg2[MAX_IDLEN]; //������2�Ǳ�ʶ��ʱ������
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
	else //arg1Ϊ��ʶ��
	{
		int get_id = getAvailableVar(name_arg1); //arg1��SYMBL�еı��
		if (get_id == -1) return 0;
		if (RDL_SYMBLID != get_id)
		{
			//��AX�е�ֵ�����SYMBL[RDL_SYMBLID]
			Reg_ST(); //�ú����л��ж�RDL_SYMBLID�Ƿ�Ϊ0
			//����һ��������ֵ����AX
			Reg_LD(get_id);
		}
		else if (Seq.arg1.active == true) //��ȵ��ǲ���1��Ծ
		{
			//�洢����1 
			Reg_ST();
		}
	}
	//�����һ�����������������CX
	buffptr += sprintf(buffptr, "\tMOV CX, AX\n");
	buffptr += sprintf(buffptr, "\tTEST CX, CX\n");
	buffptr += sprintf(buffptr, "\tLAHF\n");
	buffptr += sprintf(buffptr, "\tAND AX, 4000H\n"); //ȡZF
	buffptr += sprintf(buffptr, "\tXOR AH, 40H\n"); //�÷�
	buffptr += sprintf(buffptr, "\tROL AX, 1\n");
	buffptr += sprintf(buffptr, "\tROL AX, 1\n"); //ѭ�����������������λ
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
	else //arg2Ϊ��ʶ��
	{
		int get_id = getAvailableVar(name_arg2); //arg2��SYMBL�еı��
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
	//����ڶ������������������AX
	buffptr += sprintf(buffptr, "\tMOV DX, AX\n");
	buffptr += sprintf(buffptr, "\tTEST AX, AX\n");
	buffptr += sprintf(buffptr, "\tLAHF\n");
	buffptr += sprintf(buffptr, "\tAND AX, 4000H\n"); //ȡZF
	buffptr += sprintf(buffptr, "\tXOR AH, 40H\n"); //�÷�
	buffptr += sprintf(buffptr, "\tROL AX, 1\n");
	buffptr += sprintf(buffptr, "\tROL AX, 1\n"); //ѭ�����������������λ
	//�����߼�����
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
	//��RDL_SYMBLIDΪĿ��
	int target_id = getAvailableVar(Seq.target.content.str);
	if (target_id == -1) return 0;
	RDL_SYMBLID = target_id;
	return 1;
}

int sub_PARAM(SEQUENCE Seq)
{
	if (!(Seq.op == PARAM)) return 0;
	bool isArg1AnID = false; //����1�Ƿ�Ϊ��ʶ��
	int val_arg1; //������1�ǳ���ʱ��ֵ
	char name_arg1[MAX_IDLEN]; //������1�Ǳ�ʶ��ʱ������
	int func_symblID = Find_SymblItemName(0, Seq.target.content.str);
	if (func_symblID == -1) return 0;
	ARGLITEM* func_paramlist = ((PFINFLITEM*)SYMBL[func_symblID].addr)->PARAM;
	if (iCurrentParamNum >= ((PFINFLITEM*)SYMBL[func_symblID].addr)->FN)
	{
		return 0;
	}
	if (iCurrentParamNum == 0)
	{
		buffptr += sprintf(buffptr, "\tMOV SI, SP\n"); //��¼��ǰջ��ָ����SI
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
		else //arg1Ϊ��ʶ��
		{
			int get_id = getAvailableVar(name_arg1); //arg1��SYMBL�еı��
			if (get_id == -1) return 0;
			if (RDL_SYMBLID != get_id)
			{
				//��AX�е�ֵ�����SYMBL[RDL_SYMBLID]
				Reg_ST(); //�ú����л��ж�RDL_SYMBLID�Ƿ�Ϊ0
				//����һ��������ֵ����AX
				Reg_LD(get_id);
			}
			else if (Seq.arg1.active == true) //��ȵ��ǲ���1��Ծ
			{
				//�洢����1 
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
		else //arg1Ϊ��ʶ��
		{
			int get_id = getAvailableVar(name_arg1); //arg1��SYMBL�еı��
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
	buffptr += sprintf(buffptr, "\tMOV SS:[SI-4], BP\n"); //OLD BP //Ĭ�Ϻ�������ֵΪ����
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
	bool isArg1AnID = false; //����1�Ƿ�Ϊ��ʶ��
	int val_arg1; //������1�ǳ���ʱ��ֵ
	char name_arg1[MAX_IDLEN]; //������1�Ǳ�ʶ��ʱ������
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
	else //arg1Ϊ��ʶ��
	{
		int get_id = getAvailableVar(name_arg1); //arg1��SYMBL�еı��
		if (get_id == -1) return 0;
		if (RDL_SYMBLID != get_id)
		{
			//��AX�е�ֵ�����SYMBL[RDL_SYMBLID]
			Reg_ST(); //�ú����л��ж�RDL_SYMBLID�Ƿ�Ϊ0
			//����һ��������ֵ����AX
			Reg_LD(get_id);
		}
		else if (Seq.arg1.active == true) //��ȵ��ǲ���1��Ծ
		{
			//�洢����1 
			Reg_ST();
		}
	}
	//��AX�е�ֵ���õ���������ֵ�洢��
	buffptr += sprintf(buffptr, "\tMOV [BP - 2], AX\n");
	buffptr += sprintf(buffptr, "\tLEA SP, [BP - %d]\n", totalFuncOffset + 2 * DEST_PTR_SIZE); //��SP������OLDCSIP����λ��
	buffptr += sprintf(buffptr, "\tMOV BP, [BP - 4]\n"); //�ָ�BP
	buffptr += sprintf(buffptr, "\tRET %d\n", totalFuncOffset - currentFuncSize); //�������أ�����ջ������Ϊ��������ֵ��Ԫ
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
	buffptr += sprintf(buffptr, "%s\tPROC FAR\n", strCurrentFuncName); //�ӳ�������
	return 1;
}

int sub_EF(SEQUENCE Seq)
{
	if (Seq.op != EF) return 0;
	buffptr += sprintf(buffptr, "%s\tENDP\n", strCurrentFuncName);
	strcpy(strCurrentFuncName, "\0");
	//currentFunc�ÿ�
	currentFunc = NULL;
	currentFuncSize = -1;
	totalFuncOffset = -1;
	return 1;
}

int sub_IF(SEQUENCE Seq)
{
	if (Seq.op != IF) return 0;
	bool isArg1AnID = false; //����1�Ƿ�Ϊ��ʶ��
	int val_arg1; //������1�ǳ���ʱ��ֵ
	char name_arg1[MAX_IDLEN]; //������1�Ǳ�ʶ��ʱ������
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
	else //arg1Ϊ��ʶ��
	{
		int get_id = getAvailableVar(name_arg1); //arg1��SYMBL�еı��
		if (get_id == -1) return 0;
		if (RDL_SYMBLID != get_id)
		{
			//��AX�е�ֵ�����SYMBL[RDL_SYMBLID]
			Reg_ST(); //�ú����л��ж�RDL_SYMBLID�Ƿ�Ϊ0
			//����һ��������ֵ����AX
			Reg_LD(get_id);
		}
		else if (Seq.arg1.active == true) //��ȵ��ǲ���1��Ծ
		{
			//�洢����1 
			Reg_ST();
		}
	}
	IF_Push(iIfNum);
	++iIfNum;
	buffptr += sprintf(buffptr, "_IF%d:", IF_Stack[IF_SP - 1]); //IF_Stack[IF_SP-1]Ϊջ��Ԫ��
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
	bool isArg1AnID = false; //����1�Ƿ�Ϊ��ʶ��
	int val_arg1; //������1�ǳ���ʱ��ֵ
	char name_arg1[MAX_IDLEN]; //������1�Ǳ�ʶ��ʱ������
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
	else //arg1Ϊ��ʶ��
	{
		int get_id = getAvailableVar(name_arg1); //arg1��SYMBL�еı��
		if (get_id == -1) return 0;
		if (RDL_SYMBLID != get_id)
		{
			//��AX�е�ֵ�����SYMBL[RDL_SYMBLID]
			Reg_ST(); //�ú����л��ж�RDL_SYMBLID�Ƿ�Ϊ0
			//����һ��������ֵ����AX
			Reg_LD(get_id);
		}
		else if (Seq.arg1.active == true) //��ȵ��ǲ���1��Ծ
		{
			//�洢����1 
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
	bool isArg1AnID = false; //����1�Ƿ�Ϊ��ʶ��
	int val_arg1; //������1�ǳ���ʱ��ֵ
	char name_arg1[MAX_IDLEN]; //������1�Ǳ�ʶ��ʱ������
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
	else //arg1Ϊ��ʶ��
	{
		int get_id = getAvailableVar(name_arg1); //arg1��SYMBL�еı��
		if (get_id == -1) return 0;
		if (RDL_SYMBLID != get_id)
		{
			//��AX�е�ֵ�����SYMBL[RDL_SYMBLID]
			Reg_ST(); //�ú����л��ж�RDL_SYMBLID�Ƿ�Ϊ0
			//����һ��������ֵ����AX
			Reg_LD(get_id);
		}
		else if (Seq.arg1.active == true) //��ȵ��ǲ���1��Ծ
		{
			//�洢����1 
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
		//����Ҫ���κ���
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
		//����Ҫ���κ���
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
		//�˷����Ϊ���֣��������
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
		//��ջ������������ջ����
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
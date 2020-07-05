#define _CRT_SECURE_NO_WARNINGS
#include "GrammarAnalyser.h"

int GrammarAnalyse()
{
	//init
	iAvalDSegOffset = 0;
	currentFunc = NULL;
	iArgNum = 0;
	iFuncVarDefMode = -1;
	exprProcessed.type = exprNULL;
	ptrFirstArg = NULL;
	iAvalFuncOffset = 0;
	iMidVarNum = 0;
	exprSP = 0;
	//start
	TOKEN passTOKEN = Next();
	passTOKEN = gProgram(passTOKEN);
	return 1;
}

int Find_SymblItemName(int startpos, char* name)
{
	int cursor = startpos; //��ǰ���ҵ�λ��
	for (; cursor < SymblLine; ++cursor)
	{
		if (strcmp(name, SYMBL[cursor].name) == 0) //�������
		{
			return cursor;
		}
	}
	return -1;
}

int getAvailableVar(char* name)
{
	int found = -1;
	int foundgv = -1;	//ȫ�ֱ���
	int foundfuncv = -1; //��������
	if (currentFunc == NULL) //��ǰ������Ϊ������,�����Ƿ�Ϊȫ�ֱ���
	{
		do
		{
			found = Find_SymblItemName(found+1, name);
			if (found != -1)
			{
				if (SYMBL[found].cat == gvCAT)
				{
					foundgv = found;
				}
			}
		} while (found != -1);
	}
	else //��ǰ������ΪcurrentFuncָ��ĺ���
	{
		do
		{
			found = Find_SymblItemName(found+1, name);
			if (found != -1)
			{
				if (SYMBL[found].cat == gvCAT) //����ȫ�ֱ������ȼ�¼
				{
					foundgv = found;
				}
				else if (SYMBL[found].cat == vfCAT || SYMBL[found].cat == vnCAT || SYMBL[found].cat == svCAT)
				{
					//�ж��Ƿ�����ڵ�ǰ����
					if (SYMBL[found].addr == currentFunc)
					{
						foundfuncv = found;
					}
				}
			}
		} while (found != -1);
	}
	//���
	if (foundfuncv != -1)
	{
		return foundfuncv;
	}
	else if (foundgv != -1)
	{
		return foundgv;
	}
	else
	{
		return -1;
	}
}

int fillExprSeqArg(SEQARG* seqarg)
{
	switch (exprProcessed.type)
	{
	case exprDC:
		seqarg->type = seqDC;
		seqarg->content.d = exprProcessed.content.d;
		break;
	case exprFC:
		seqarg->type = seqFC;
		seqarg->content.f = exprProcessed.content.f;
		break;
	case exprCHAR:
		seqarg->type = seqCHAR;
		seqarg->content.ch = exprProcessed.content.ch;
		break;
	case exprID:
		seqarg->type = seqID;
		seqarg->content.str = exprProcessed.content.strID;
	default:
		return 0;
		break;
	}
	return 1;
}

int exprPUSH(exprProcessedType expr)
{
	if (exprSP > MAX_EXPR_STACK_LEN) return 0; //ջ��
	exprStack[exprSP] = expr;
	++exprSP;
	return 1;
}

int exprPOP(exprProcessedType* dest)
{
	if (exprSP == 0) return 0;
	--exprSP;
	*dest = exprStack[exprSP];
	return 1;
}

int exprGenerateSeq(OPR op, int argnum)
{
	if (argnum > 2 || argnum <= 0) return 0;
	if (argnum == 2) //Ŀǰֻ��˫Ŀ
	{
		//ջ��Ϊ����Ĳ���
		exprProcessedType expr_arg2;
		if (!exprPOP(&expr_arg2))
		{
			//error
		}
		exprProcessedType expr_arg1;
		if (!exprPOP(&expr_arg1))
		{
			//error
		}
		//������������Ƿ���ͬ
		if (expr_arg1.datatype != expr_arg2.datatype)
		{
			//error
		}
		char* gotten_midvarname = NULL; //���ɵ��м������
		exprProcessedType exprmidvar; //���ɵ��м�����ı��ʽ�����ջ
		exprmidvar.type = exprID;
		exprmidvar.datatype = expr_arg1.datatype;
		if (op != AND && op != OR && op != GT && op != GE && op != LT && op != LE && op != EQ && op != NE)
		{
			gotten_midvarname = allocMidVar(expr_arg1.datatype);
			if (!gotten_midvarname)
			{
				//error
			}
		}
		else
		{
			gotten_midvarname = allocMidVar(TYPEL_INT); //�߼����һ��ʹ��INT��ʽ�洢
			exprmidvar.datatype = TYPEL_INT; //��exprmidvar�������͸�ΪINT
		}
		exprmidvar.content.strID = gotten_midvarname;
		exprPUSH(exprmidvar); //�����ɵ��м�����ı��ʽ��ѹջ

		//��д��Ԫʽ
		SEQUENCE seq = { op, {seqNONE, 0, true}, {seqNONE, 0, true} , {seqNONE, 0, true} };
		switch (expr_arg1.type) //������������������ͬ,��seq���Ϳ��ܲ���ͬ��һ���ǳ�����һ���Ǳ�ʶ����
		{
		case exprDC:
			seq.arg1.type = seqDC;
			seq.arg1.content.d = expr_arg1.content.d;
			break;
		case exprFC:
			seq.arg1.type = seqFC;
			seq.arg1.content.f = expr_arg1.content.f;
			break;
		case exprCHAR:
			seq.arg1.type = seqCHAR;
			seq.arg1.content.ch = expr_arg1.content.ch;
			break;
		case exprID:
			seq.arg1.type = seqID;
			seq.arg1.content.str = expr_arg1.content.strID;
			break;
		default:
			//error
			break;
		}
		switch (expr_arg2.type) //������������������ͬ,��seq���Ϳ��ܲ���ͬ��һ���ǳ�����һ���Ǳ�ʶ����
		{
		case exprDC:
			seq.arg2.type = seqDC;
			seq.arg2.content.d = expr_arg2.content.d;
			break;
		case exprFC:
			seq.arg2.type = seqFC;
			seq.arg2.content.f = expr_arg2.content.f;
			break;
		case exprCHAR:
			seq.arg2.type = seqCHAR;
			seq.arg2.content.ch = expr_arg2.content.ch;
			break;
		case exprID:
			seq.arg2.type = seqID;
			seq.arg2.content.str = expr_arg2.content.strID;
			break;
		default:
			//error
			break;
		}
		seq.target.type = seqID;
		seq.target.content.str = gotten_midvarname;
		sendSequence(seq);
		return 1;
	}
	return 0;
}

char* allocMidVar(TYPELITEM* type)
{
	if (type == TYPEL_INT)
	{
		//��д�м������
		sprintf(MIDVL[MidvlLine].name, "_t%d", iMidVarNum); //Լ����ʱ����ǰ׺Ϊ��_��
		++iMidVarNum;
		MIDVL[MidvlLine].type = TYPEL_INT;
		MIDVL[MidvlLine].OFFSET = iAvalDSegOffset; //iAvalDSegOffset�����������������
		//��д���ű�����
		strcpy(SYMBL[SymblLine].name, MIDVL[MidvlLine].name);
		SYMBL[SymblLine].type = TYPEL_INT;
		SYMBL[SymblLine].cat = gvCAT; //��ʱ����Ĭ��gvCAT��
		SYMBL[SymblLine].addr = &MIDVL[MidvlLine];
		SYMBL[SymblLine].OFFSET = iAvalDSegOffset;
		iAvalDSegOffset += DEST_INT_SIZE;
		++MidvlLine;
		++SymblLine;
	}
	else if (type == TYPEL_REAL)
	{
		//��д�м������
		sprintf(MIDVL[MidvlLine].name, "_t%d", iMidVarNum); //Լ����ʱ����ǰ׺Ϊ��_��
		++iMidVarNum;
		MIDVL[MidvlLine].type = TYPEL_REAL;
		MIDVL[MidvlLine].OFFSET = iAvalDSegOffset; //iAvalDSegOffset�����������������
		//��д���ű�����
		strcpy(SYMBL[SymblLine].name, MIDVL[MidvlLine].name);
		SYMBL[SymblLine].type = TYPEL_REAL;
		SYMBL[SymblLine].cat = gvCAT; //��ʱ����Ĭ��gvCAT��
		SYMBL[SymblLine].addr = &MIDVL[MidvlLine];
		SYMBL[SymblLine].OFFSET = iAvalDSegOffset;
		iAvalDSegOffset += DEST_REAL_SIZE;
		++MidvlLine;
		++SymblLine;
	}
	else if (type == TYPEL_CHAR)
	{
		//��д�м������
		sprintf(MIDVL[MidvlLine].name, "_t%d", iMidVarNum); //Լ����ʱ����ǰ׺Ϊ��_��
		++iMidVarNum;
		MIDVL[MidvlLine].type = TYPEL_CHAR;
		MIDVL[MidvlLine].OFFSET = iAvalDSegOffset; //iAvalDSegOffset�����������������
		//��д���ű�����
		strcpy(SYMBL[SymblLine].name, MIDVL[MidvlLine].name);
		SYMBL[SymblLine].type = TYPEL_CHAR;
		SYMBL[SymblLine].cat = gvCAT; //��ʱ����Ĭ��gvCAT��
		SYMBL[SymblLine].addr = &MIDVL[MidvlLine];
		SYMBL[SymblLine].OFFSET = iAvalDSegOffset;
		iAvalDSegOffset += DEST_CHAR_SIZE;
		++MidvlLine;
		++SymblLine;
	}
	else
	{
		return NULL;
	}
	return MIDVL[MidvlLine - 1].name;
}

TOKEN gProgram(TOKEN preTOKEN)
{
	TOKEN passTOKEN = preTOKEN;
	passTOKEN = gHeadOfProgram(passTOKEN);
	passTOKEN = gDeclVars(passTOKEN);
	passTOKEN = gDeclFuncs(passTOKEN);
	passTOKEN = gProgramBody(passTOKEN);
	return passTOKEN;
}

TOKEN gHeadOfProgram(TOKEN preTOKEN)
{
	TOKEN passTOKEN = preTOKEN;
	if (!(passTOKEN.type == KTYPE && passTOKEN.id == kPROGRAM))
	{
		//error
	}
	passTOKEN = Next();
	if (!(passTOKEN.type == ITYPE))
	{
		//error
	}
	passTOKEN = Next();
	if (!(passTOKEN.type == PTYPE && passTOKEN.id == pSEMI))
	{
		//error
	}
	//�������ͷ��Ԫʽ
	SEQUENCE seq = { PROG, {seqNONE, 0, false}, {seqNONE, 0, false}, {seqNONE, 0, false} };
	//��Ԫʽ���
	sendSequence(seq);
	passTOKEN = Next();
	return passTOKEN;
}

TOKEN gDeclVars(TOKEN preTOKEN)
{
	TOKEN passTOKEN = preTOKEN;
	if (!(passTOKEN.type == KTYPE && passTOKEN.id == kVAR))
	{
		//error
	}
	passTOKEN = Next();
	if (!(passTOKEN.type == PTYPE && passTOKEN.id == pLBRACE))
	{
		//error
	}
	SEQUENCE seq = { GV, {seqNONE, 0, false}, {seqNONE, 0, false}, {seqNONE, 0, false} };
	sendSequence(seq);
	passTOKEN = Next();
	while (!(passTOKEN.type == PTYPE && passTOKEN.id == pRBRACE))
	{
		passTOKEN = gVarDef(passTOKEN);
	}
	SEQUENCE seq2 = { GVE, {seqNONE, 0, false}, {seqNONE, 0, false}, {seqNONE, 0, false} };
	sendSequence(seq2);
	passTOKEN = Next();
	return passTOKEN;
}

TOKEN gVarDef(TOKEN preTOKEN)
{
	TOKEN passTOKEN = preTOKEN;
	if (!(passTOKEN.type == KTYPE && (passTOKEN.id == kINT || passTOKEN.id == kCHAR || passTOKEN.id == kREAL)))
	{
		//error
	}
	//������������
	KEYWORD vartype = passTOKEN.id;
	passTOKEN = Next();
	if (!(passTOKEN.type == ITYPE))
	{
		//error
	}
	//���±�����
	char varname[MAX_IDLEN];
	//���±������ڱ�ʶ�����еı��
	int varnameid = passTOKEN.id;
	strcpy(varname, iTable[varnameid]);
	passTOKEN = Next();
	if (!(passTOKEN.type == PTYPE && passTOKEN.id == pSEMI))
	{
		//error
	}
	//��д���ű�
	strcpy(SYMBL[SymblLine].name, varname);
	SYMBL[SymblLine].cat = gvCAT;
	SYMBL[SymblLine].addr = NULL; //ȫ�ֱ�������ΪNULL
	SYMBL[SymblLine].OFFSET = iAvalDSegOffset;
	int varsize = 0;
	switch (vartype)
	{
	case kINT:
		varsize = DEST_INT_SIZE;
		SYMBL[SymblLine].type = TYPEL_INT; //�涨TYPEL��0Ϊint,1Ϊreal,2Ϊchar
		break;
	case kREAL:
		varsize = DEST_REAL_SIZE;
		SYMBL[SymblLine].type = TYPEL_REAL;
		break;
	case kCHAR:
		varsize = DEST_CHAR_SIZE;
		SYMBL[SymblLine].type = TYPEL_CHAR;
		break;
	default:
		//error
		break;
	}
	iAvalDSegOffset += varsize;
	++SymblLine;
	//���ű���д���
	//������Ԫʽ
	SEQUENCE seq = { VAR, {seqID, iTable[varnameid], true}, {seqDC, varsize, false}, {seqNONE, 0, false} };
	sendSequence(seq);
	passTOKEN = Next();
	return passTOKEN;
}

TOKEN gDeclFuncs(TOKEN preTOKEN)
{
	TOKEN passTOKEN = preTOKEN;
	if (!(passTOKEN.type == KTYPE && passTOKEN.id == kFUNCTION))
	{
		//error
	}
	passTOKEN = Next();
	if (!(passTOKEN.type == PTYPE && passTOKEN.id == pLBRACE))
	{
		//error
	}
	passTOKEN = Next();
	while (!(passTOKEN.type == PTYPE && passTOKEN.id == pRBRACE))
	{
		passTOKEN = gFuncDef(passTOKEN);
	}
	passTOKEN = Next();
	return passTOKEN;
}

TOKEN gFuncDef(TOKEN preTOKEN)
{
	TOKEN passTOKEN = preTOKEN;
	if (!(passTOKEN.type == KTYPE && (passTOKEN.id == kINT || passTOKEN.id == kREAL || passTOKEN.id == kCHAR)))
	{
		//error
	}
	KEYWORD functype = passTOKEN.id;
	passTOKEN = Next();
	if (!(passTOKEN.type == ITYPE))
	{
		//error
	}
	char funcname[MAX_IDLEN];
	int funcnameid = passTOKEN.id;
	strcpy(funcname, iTable[funcnameid]);
	passTOKEN = Next();
	if (!(passTOKEN.type == PTYPE && passTOKEN.id == pLBRACE))
	{
		//error
	}
	//��ǰ����״̬�ı�
	currentFunc = &PFINFL[PFInflLine];
	//��¼�����ű�����
	strcpy(SYMBL[SymblLine].name, funcname);
	SYMBL[SymblLine].cat = fCAT;
	SYMBL[SymblLine].addr = &PFINFL[PFInflLine]; //��ʱ�ú�����δ����PFINFL��
	int curFuncSymblLine = SymblLine; //���ڻ�������
	//��ǰ��д������Ϣ���е����࣬���������д�������� 
	int funcsize; //����ֵռ�ÿռ�
	switch (functype)
	{
	case kINT:
		funcsize = DEST_INT_SIZE;
		SYMBL[SymblLine].type = TYPEL_INT;
		//��ǰ��������ֵ����ȷ��
		currentFuncType = TYPEL_INT;
		break;
	case kREAL:
		funcsize = DEST_REAL_SIZE;
		SYMBL[SymblLine].type = TYPEL_REAL;
		currentFuncType = TYPEL_REAL;
		break;
	case kCHAR:
		funcsize = DEST_CHAR_SIZE;
		SYMBL[SymblLine].type = TYPEL_CHAR;
		currentFuncType = TYPEL_CHAR;
		break;
	default:
		//error
		break;
	}
	int funcoffset = funcsize + DEST_PTR_SIZE + DEST_PTR_SIZE; //sizeof(ret) + sizeof(OLD BP) + sizeof(OLD IP)
	PFINFL[PFInflLine].OFFSET = funcoffset;
	iAvalFuncOffset = funcoffset;
	//������Ԫʽ
	SEQUENCE seq = { FUNC, {seqID, iTable[funcnameid], true}, {seqNONE, 0, false}, {seqNONE, 0, false} };
	sendSequence(seq);
	++SymblLine;
	passTOKEN = Next();
	passTOKEN = gDeclArgs(passTOKEN);
	passTOKEN = gDeclFuncVars(passTOKEN);
	passTOKEN = gFuncBody(passTOKEN);
	if (!(passTOKEN.type == PTYPE && passTOKEN.id == pRBRACE))
	{
		//error
	}
	//��¼��������Ϣ��
	PFINFL[PFInflLine].LEVEL = 1;
	PFINFL[PFInflLine].FN = iArgNum;
	PFINFL[PFInflLine].ENTRY = 0;
	PFINFL[PFInflLine].PARAM = ptrFirstArg;
	SYMBL[curFuncSymblLine].OFFSET = PFINFL[PFInflLine].OFFSET; //��������
	++PFInflLine;
	//������Ԫʽ
	SEQUENCE seq2 = { EF, {seqNONE, 0, false}, {seqNONE, 0, false}, {seqNONE, 0, false} };
	sendSequence(seq2);
	currentFunc = NULL;
	currentFuncType = NULL;
	iArgNum = 0;
	ptrFirstArg = NULL;
	iAvalFuncOffset = 0; //�������
	passTOKEN = Next();
	return passTOKEN;
}

TOKEN gDeclArgs(TOKEN preTOKEN)
{
	TOKEN passTOKEN = preTOKEN;
	if (!(passTOKEN.type == KTYPE && passTOKEN.id == kARGS))
	{
		//error
	}
	passTOKEN = Next();
	if (!(passTOKEN.type == PTYPE && passTOKEN.id == pLBRACE))
	{
		//error
	}
	passTOKEN = Next();
	passTOKEN = gDeclVfs(passTOKEN);
	passTOKEN = gDeclVns(passTOKEN);
	if (!(passTOKEN.type == PTYPE && passTOKEN.id == pRBRACE))
	{
		//error
	}
	passTOKEN = Next();
	return passTOKEN;
}

TOKEN gDeclVfs(TOKEN preTOKEN)
{
	TOKEN passTOKEN = preTOKEN;
	if (!(passTOKEN.type == KTYPE && passTOKEN.id == kVAL))
	{
		//error
	}
	passTOKEN = Next();
	if (!(passTOKEN.type == PTYPE && passTOKEN.id == pLBRACE))
	{
		//error
	}
	passTOKEN = Next();
	iFuncVarDefMode = 0; //��ֵ�βζ���ģʽ
	while (!(passTOKEN.type == PTYPE && passTOKEN.id == pRBRACE))
	{
		passTOKEN = gFuncVarDef(passTOKEN);
		++iArgNum; //��ǰ��������������һ
	}
	iFuncVarDefMode = -1; //���ú�����������ģʽ
	passTOKEN = Next();
	return passTOKEN;
}

TOKEN gDeclVns(TOKEN preTOKEN)
{
	TOKEN passTOKEN = preTOKEN;
	if (!(passTOKEN.type == KTYPE && passTOKEN.id == kPTR))
	{
		//error
	}
	passTOKEN = Next();
	if (!(passTOKEN.type == PTYPE && passTOKEN.id == pLBRACE))
	{
		//error
	}
	passTOKEN = Next();
	iFuncVarDefMode = 1; //�����βζ���ģʽ
	while (!(passTOKEN.type == PTYPE && passTOKEN.id == pRBRACE))
	{
		passTOKEN = gFuncVarDef(passTOKEN);
		++iArgNum; //��ǰ��������������һ
	}
	iFuncVarDefMode = -1; //���ú�����������ģʽ
	passTOKEN = Next();
	return passTOKEN;
}

TOKEN gDeclFuncVars(TOKEN preTOKEN)
{
	TOKEN passTOKEN = preTOKEN;
	if (!(passTOKEN.type == KTYPE && passTOKEN.id == kPTR))
	{
		//error
	}
	passTOKEN = Next();
	if (!(passTOKEN.type == PTYPE && passTOKEN.id == pLBRACE))
	{
		//error
	}
	passTOKEN = Next();
	iFuncVarDefMode = 2; //�ֲ���������ģʽ
	while (!(passTOKEN.type == PTYPE && passTOKEN.id == pRBRACE))
	{
		passTOKEN = gFuncVarDef(passTOKEN);
		//�˴�����Ҫ���Ӳ�������
	}
	iFuncVarDefMode = -1; //���ú�����������ģʽ
	passTOKEN = Next();
	return passTOKEN;
}

TOKEN gFuncVarDef(TOKEN preTOKEN)
{
	TOKEN passTOKEN = preTOKEN;
	if (!(passTOKEN.type == KTYPE && (passTOKEN.id == kINT || passTOKEN.id == kCHAR || passTOKEN.id == kREAL)))
	{
		//error
	}
	KEYWORD vartype = passTOKEN.id;
	passTOKEN = Next();
	if (!(passTOKEN.type == ITYPE))
	{
		//error
	}
	char varname[MAX_IDLEN];
	int varnameid = passTOKEN.id;
	strcpy(varname, iTable[varnameid]);
	passTOKEN = Next();
	if (!(passTOKEN.type = PTYPE && passTOKEN.id == pSEMI))
	{
		//error
	}
	//��д���ű�
	strcpy(SYMBL[SymblLine].name, varname);
	SYMBL[SymblLine].addr = &PFINFL[PFInflLine];
	int varsize = 0;
	switch (vartype)
	{
	case kINT:
		varsize = DEST_INT_SIZE;
		SYMBL[SymblLine].type = TYPEL_INT; //�涨TYPEL��0Ϊint,1Ϊreal,2Ϊchar
		SYMBL[SymblLine].OFFSET = iAvalFuncOffset;
		if (iFuncVarDefMode == 1)
		{
			iAvalFuncOffset += DEST_VN_SIZE;
		}
		else
		{
			iAvalFuncOffset += varsize;
		}
		break;
	case kREAL:
		varsize = DEST_REAL_SIZE;
		SYMBL[SymblLine].type = TYPEL_REAL;
		SYMBL[SymblLine].OFFSET = iAvalFuncOffset;
		if (iFuncVarDefMode == 1)
		{
			iAvalFuncOffset += DEST_VN_SIZE;
		}
		else
		{
			iAvalFuncOffset += varsize;
		}
		break;
	case kCHAR:
		varsize = DEST_CHAR_SIZE;
		SYMBL[SymblLine].type = TYPEL_CHAR;
		SYMBL[SymblLine].OFFSET = iAvalFuncOffset;
		if (iFuncVarDefMode == 1)
		{
			iAvalFuncOffset += DEST_VN_SIZE;
		}
		else
		{
			iAvalFuncOffset += varsize;
		}
		break;
	default:
		//error
		break;
	}
	switch (iFuncVarDefMode)
	{
	case 0:
		SYMBL[SymblLine].cat = vfCAT;
		//iArgNum������ӳ����Ѽӹ�
		//��д�βα�
		strcpy(ARGL[ArglLine].name, SYMBL[SymblLine].name);
		ARGL[ArglLine].cat = vfCAT;
		ARGL[ArglLine].OFFSET = SYMBL[SymblLine].OFFSET;
		ARGL[ArglLine].type = SYMBL[SymblLine].type;
		if (ptrFirstArg == NULL)
		{
			ptrFirstArg = &ARGL[ArglLine];
		}
		++ArglLine;
		break;
	case 1:
		SYMBL[SymblLine].cat = vnCAT;
		//��д�βα�
		strcpy(ARGL[ArglLine].name, SYMBL[SymblLine].name);
		ARGL[ArglLine].cat = vnCAT;
		ARGL[ArglLine].OFFSET = SYMBL[SymblLine].OFFSET;
		ARGL[ArglLine].type = SYMBL[SymblLine].type;
		if (ptrFirstArg == NULL)
		{
			ptrFirstArg = &ARGL[ArglLine];
		}
		++ArglLine;
		break;
	case 2:
		SYMBL[SymblLine].cat = svCAT;
		//���ɾֲ�����������Ԫʽ
		SEQUENCE seq = { VT, {seqID, iTable[varnameid], true}, {seqDC, varsize, false}, {seqNONE, 0, false} };
		sendSequence(seq);
		break;
	default:
		//error
		break;
	}
	++SymblLine;
	passTOKEN = Next();
	return passTOKEN;
}

TOKEN gFuncBody(TOKEN preTOKEN)
{
	TOKEN passTOKEN = preTOKEN;
	if (!(passTOKEN.type == KTYPE && passTOKEN.id == kBODY))
	{
		//error
	}
	passTOKEN = Next();
	if (!(passTOKEN.type = PTYPE && passTOKEN.id == pLBRACE))
	{
		//error
	}
	passTOKEN = Next();
	while (!(passTOKEN.type == PTYPE && passTOKEN.id == pRBRACE))
	{
		passTOKEN = gFuncCode(passTOKEN);
	}
	passTOKEN = Next();
	return passTOKEN;
}

TOKEN gFuncCode(TOKEN preTOKEN)
{
	TOKEN passTOKEN = preTOKEN;
	if (passTOKEN.type == KTYPE)
	{
		switch (passTOKEN.id)
		{
		case kPUTC:
			passTOKEN = gCodePrint(passTOKEN);
			break;
		case kENDP:
			passTOKEN = gCodeEnd(passTOKEN);
			break;
		case kIF:
			passTOKEN = gCodeIF(passTOKEN);
			break;
		case kWHILE:
			passTOKEN = gCodeWhile(passTOKEN);
			break;
		case kRETURN:
			passTOKEN = gCodeReturn(passTOKEN);
			break;
		default:
			//error
			break;
		}
	}
	else if (passTOKEN.type == ITYPE)
	{
		passTOKEN = gCodeAssign(passTOKEN);
	}
	else
	{
		//error
	}
	return passTOKEN;
}

TOKEN gCodeReturn(TOKEN preTOKEN)
{
	TOKEN passTOKEN = preTOKEN;
	if (!(passTOKEN.type == KTYPE && passTOKEN.id == kRETURN))
	{
		//error
	}
	passTOKEN = Next();
	passTOKEN = gExpr(passTOKEN);
	//����Ƿ���Ϸ���ֵ���ͣ�δ����
	//������Ԫʽ
	SEQUENCE seq = { RET, {seqNONE, 0, true}, {seqNONE, 0, false} , {seqNONE, 0, false} };
	fillExprSeqArg(&seq.arg1);
	sendSequence(seq);
	//ʹ����exprProcessed��Ҫ�ÿ�
	exprProcessed.type = exprNULL;
	if (!(passTOKEN.type == PTYPE && passTOKEN.id == pSEMI))
	{
		//error
	}
	passTOKEN = Next();
	return passTOKEN;
}

TOKEN gProgramBody(TOKEN preTOKEN)
{
	TOKEN passTOKEN = preTOKEN;
	if (!(passTOKEN.type == KTYPE && passTOKEN.id == kBODY))
	{
		//error
	}
	passTOKEN = Next();
	if (!(passTOKEN.type == PTYPE && passTOKEN.id == pLBRACE))
	{
		//error
	}
	SEQUENCE seq = { ENT, {seqNONE, 0, false}, {seqNONE, 0, false}, {seqNONE, 0, false} };
	sendSequence(seq);
	passTOKEN = Next();
	while (!(passTOKEN.type == PTYPE && passTOKEN.id == pRBRACE))
	{
		passTOKEN = gProgCode(passTOKEN);
	}
	//����ṹ������Ԫʽ
	SEQUENCE seq2 = { END, {seqNONE, 0, false}, {seqNONE, 0, false}, {seqNONE, 0, false} };
	sendSequence(seq2);
	passTOKEN = Next();
	return passTOKEN;
}

TOKEN gProgCode(TOKEN preTOKEN)
{
	TOKEN passTOKEN = preTOKEN;
	if (passTOKEN.type == KTYPE)
	{
		switch (passTOKEN.id)
		{
		case kPUTC:
			passTOKEN = gCodePrint(passTOKEN);
			break;
		case kENDP:
			passTOKEN = gCodeEnd(passTOKEN);
			break;
		case kIF:
			passTOKEN = gCodeIF(passTOKEN);
			break;
		case kWHILE:
			passTOKEN = gCodeWhile(passTOKEN);
			break;
		default:
			//error
			break;
		}
	}
	else if (passTOKEN.type == ITYPE)
	{
		passTOKEN = gCodeAssign(passTOKEN);
	}
	else
	{
		//error
	}
	return passTOKEN;
}

TOKEN gCodeAssign(TOKEN preTOKEN)
{
	TOKEN passTOKEN = preTOKEN;
	if (!(passTOKEN.type == ITYPE))
	{
		//error
	}
	char dest_varname[MAX_IDLEN];
	int dest_varnameid = passTOKEN.id;
	strcpy(dest_varname, iTable[dest_varnameid]);
	//�жϸñ�ʶ���Ƿ�Ϸ���1.�Ƿ��Ǳ����� 2.�Ƿ������Ŀǰ���������б����ʣ�
	int dest_var_symblID = getAvailableVar(dest_varname);
	if (dest_var_symblID == -1)
	{
		//error �Ƿ���ʶ�����޷����ʵı���
	}
	passTOKEN = Next();
	if (!(passTOKEN.type == PTYPE && passTOKEN.id == pASSI))
	{
		//error
	}
	passTOKEN = Next();
	if (passTOKEN.type == ITYPE)
	{
		//���ñ�ʶ���Ǻ��������Ǳ��ʽ�е��GGML����������Ϊ���ʽ�
		char strNameToCheck[MAX_IDLEN];
		int iNameIdToCheck = passTOKEN.id;
		strcpy(strNameToCheck, iTable[iNameIdToCheck]);
		int check_symblID = Find_SymblItemName(0, strNameToCheck);
		if (SYMBL[check_symblID].cat == fCAT) //�涨�����������������ͬ
		{
			int func_symblID = check_symblID;
			passTOKEN = gCodeCall(passTOKEN);
			//�жϷ���ֵ�����Ƿ��뱻��ֵ������ͬ
			if (SYMBL[dest_var_symblID].type != SYMBL[func_symblID].type)
			{
				//error ����ֵ���Ͳ���Ӧ
			}
			char *midvarname = allocMidVar(SYMBL[func_symblID].type);
			if (midvarname == NULL)
			{
				//ջ�ռ��������
			}
			//���ɺ���������Ԫʽ
			SEQUENCE seq = { CALL, {seqID, iTable[iNameIdToCheck], true}, {seqNONE, 0, false}, {seqID, midvarname, true} };
			sendSequence(seq);
			SEQUENCE seq2 = { ASSI, {seqID, midvarname, true}, {seqNONE, 0, false}, {seqID, iTable[dest_varnameid], true} };
			sendSequence(seq2);
		}
		else
		{
			passTOKEN = gExpr(passTOKEN);
		}
	}
	else if (passTOKEN.type == CTYPE || passTOKEN.type == DCTYPE || passTOKEN.type == FCTYPE)
	{
		passTOKEN = gExpr(passTOKEN);
	}
	else
	{
		//error
	}
	if (!exprProcessed.type == exprNULL) //���ոս����˱��ʽ����
	{
		SEQUENCE seq = { ASSI,{seqNONE, 0, true}, {seqNONE, 0, false}, {seqID, iTable[dest_varnameid], true} };
		fillExprSeqArg(&seq.arg1);
		sendSequence(seq);
		exprProcessed.type = exprNULL;
	}
	if (!(passTOKEN.type == PTYPE && passTOKEN.id == pSEMI))
	{
		//error
	}
	passTOKEN = Next();
	return passTOKEN;
}

TOKEN gCodePrint(TOKEN preTOKEN)
{
	TOKEN passTOKEN = preTOKEN;
	if (!(passTOKEN.type == KTYPE && passTOKEN.id == kPUTC))
	{
		//error
	}
	passTOKEN = Next();
	if (!(passTOKEN.type == PTYPE && passTOKEN.id == pLBRACKET))
	{
		//error
	}
	passTOKEN = Next();
	passTOKEN = gExpr(passTOKEN);
	SEQUENCE seq = { PUTC, {seqNONE, 0, true}, {seqNONE, 0, false}, {seqNONE, 0, false} };
	fillExprSeqArg(&seq.arg1);
	sendSequence(seq);
	exprProcessed.type = exprNULL;
	if (!(passTOKEN.type == PTYPE && passTOKEN.id == pSEMI))
	{
		//error
	}
	passTOKEN = Next();
	if (!(passTOKEN.type == PTYPE && passTOKEN.id == pRBRACKET))
	{
		//error
	}
	passTOKEN = Next();
	if (!(passTOKEN.type == PTYPE && passTOKEN.id == pSEMI))
	{
		//error
	}
	passTOKEN = Next();
	return passTOKEN;
}

TOKEN gCodeEnd(TOKEN preTOKEN)
{
	TOKEN passTOKEN = preTOKEN;
	if (!(passTOKEN.type == KTYPE && passTOKEN.id == kENDP))
	{
		//error
	}
	SEQUENCE seq = { EXIT, {seqNONE, 0, false}, {seqNONE, 0, false} , {seqNONE, 0, false} };
	sendSequence(seq);
	passTOKEN = Next();
	if (!(passTOKEN.type == PTYPE && passTOKEN.id == pSEMI))
	{
		//error
	}
	passTOKEN = Next();
	return passTOKEN;
}

TOKEN gCodeIF(TOKEN preTOKEN)
{
	TOKEN passTOKEN = preTOKEN;
	if (!(passTOKEN.type == KTYPE && passTOKEN.id == kIF))
	{
		//error
	}
	passTOKEN = Next();
	if (!(passTOKEN.type == PTYPE && passTOKEN.id == pLBRACKET))
	{
		//error
	}
	passTOKEN = Next();
	passTOKEN = gExpr(passTOKEN);
	SEQUENCE seq = { IF, {seqNONE, 0, true}, {seqNONE, 0, false}, {seqNONE, 0, false} };
	fillExprSeqArg(&seq.arg1);
	sendSequence(seq);
	exprProcessed.type = exprNULL;
	if (!(passTOKEN.type == PTYPE && passTOKEN.id == pRBRACKET))
	{
		//error
	}
	passTOKEN = Next();
	if (!(passTOKEN.type == PTYPE && passTOKEN.id == pLBRACE))
	{
		//error
	}
	passTOKEN = Next();
	while (!(passTOKEN.type == PTYPE && passTOKEN.id == pRBRACE))
	{
		if (currentFunc != NULL) //�ں�����
		{
			passTOKEN = gFuncCode(passTOKEN);
		}
		else
		{
			passTOKEN = gProgCode(passTOKEN);
		}
	}
	passTOKEN = Next();
	if (!(passTOKEN.type == KTYPE && passTOKEN.id == kELSE))
	{
		//error
	}
	SEQUENCE seq2 = { EL, {seqNONE, 0, false}, {seqNONE, 0, false}, {seqNONE, 0, false} };
	sendSequence(seq2);
	passTOKEN = Next();
	if (!(passTOKEN.type == PTYPE && passTOKEN.id == pLBRACE))
	{
		//error
	}
	passTOKEN = Next();
	while (!(passTOKEN.type == PTYPE && passTOKEN.id == pRBRACE))
	{
		if (currentFunc != NULL) //�ں�����
		{
			passTOKEN = gFuncCode(passTOKEN);
		}
		else
		{
			passTOKEN = gProgCode(passTOKEN);
		}
	}
	SEQUENCE seq3 = { IE, {seqNONE, 0, false}, {seqNONE, 0, false}, {seqNONE, 0, false} };
	sendSequence(seq3);
	passTOKEN = Next();
	return passTOKEN;
}

TOKEN gCodeWhile(TOKEN preTOKEN)
{
	TOKEN passTOKEN = preTOKEN;
	if (!(passTOKEN.type == KTYPE && passTOKEN.id == kWHILE))
	{
		//error
	}
	SEQUENCE seq = { WH, {seqNONE, 0, false}, {seqNONE, 0, false}, {seqNONE, 0, false} };
	sendSequence(seq);
	passTOKEN = Next();
	if (!(passTOKEN.type == PTYPE && passTOKEN.id == pLBRACKET))
	{
		//error
	}
	passTOKEN = Next();
	passTOKEN = gExpr(passTOKEN);
	SEQUENCE seq2 = { DO, {seqNONE, 0, true}, {seqNONE, 0, false}, {seqNONE, 0, false} };
	fillExprSeqArg(&seq2.arg1);
	sendSequence(seq2);
	exprProcessed.type = exprNULL;
	if (!(passTOKEN.type == PTYPE && passTOKEN.id == pRBRACKET))
	{
		//error
	}
	passTOKEN = Next();
	if (!(passTOKEN.type == PTYPE && passTOKEN.id == pLBRACE))
	{
		//error
	}
	passTOKEN = Next();
	while (!(passTOKEN.type == PTYPE && passTOKEN.id == pRBRACE))
	{
		if (currentFunc != NULL) //�ں�����
		{
			passTOKEN = gFuncCode(passTOKEN);
		}
		else
		{
			passTOKEN = gProgCode(passTOKEN);
		}
	}
	SEQUENCE seq3 = { WE, {seqNONE, 0, false}, {seqNONE, 0, false}, {seqNONE, 0, false} };
	sendSequence(seq3);
	passTOKEN = Next();
	return passTOKEN;
}

TOKEN gCodeCall(TOKEN preTOKEN)
{
	TOKEN passTOKEN = preTOKEN;
	//�ڸ�ֵ����ӳ������Ѿ��жϹ��Ƿ�Ϊ������ʶ���Լ����ɵ�����Ԫʽ
	//�˴�ֻ���ж��﷨�ɷ��Ƿ���ȷ
	if (!(passTOKEN.type == ITYPE))
	{
		//error
	}
	int funcnameid = passTOKEN.id;
	passTOKEN = Next();
	if (!(passTOKEN.type == PTYPE && passTOKEN.id == pLBRACKET))
	{
		//error
	}
	passTOKEN = Next();
	while (!(passTOKEN.type == PTYPE && passTOKEN.id == pRBRACKET))
	{
		//����
		passTOKEN = gExpr(passTOKEN);
		SEQUENCE seq = { PARAM, {seqNONE, 0, true}, {seqNONE, 0, false}, {seqID, iTable[funcnameid], true} };
		fillExprSeqArg(&seq.arg1);
		sendSequence(seq);
		exprProcessed.type = exprNULL;
		if (!(passTOKEN.type == PTYPE && passTOKEN.id == pSEMI))
		{
			//error
		}
		passTOKEN = Next();
	}
	passTOKEN = Next();
	return passTOKEN;
}

TOKEN gExpr(TOKEN preTOKEN)
{
	TOKEN passTOKEN = preTOKEN;
	passTOKEN = gLogicExpr(passTOKEN);
	//��дexprProcessed
	if (!exprPOP(&exprProcessed))
	{
		printf("gExpr():���ʽջ����\n");
	}
	return passTOKEN;
}

TOKEN gLogicExpr(TOKEN preTOKEN)
{
	TOKEN passTOKEN = preTOKEN;
	passTOKEN = gRelationExpr(passTOKEN);
	passTOKEN = gLogicSuffix(passTOKEN);
	return passTOKEN;
}

TOKEN gLogicSuffix(TOKEN preTOKEN)
{
	TOKEN passTOKEN = preTOKEN;
	//�ж��Ƿ�Ϊ�շ���
	if (passTOKEN.type == PTYPE && (passTOKEN.id == pSEMI || passTOKEN.id == pRBRACKET))
	{
		//�շ��ţ�ת��ʶ���¸��﷨�ɷ�
		return passTOKEN;
	}
	//��Ϊ��
	if (!(passTOKEN.type == KTYPE && (passTOKEN.id == kAND || passTOKEN.id == kOR)))
	{
		//error
	}
	OPR op;
	switch (passTOKEN.id)
	{
	case kAND:
		op = AND;
		break;
	case kOR:
		op = OR;
		break;
	default:
		break;
	}
	passTOKEN = Next();
	passTOKEN = gRelationExpr(passTOKEN);
	exprGenerateSeq(op, 2);
	passTOKEN = gLogicSuffix(passTOKEN);
	return passTOKEN;
}

TOKEN gRelationExpr(TOKEN preTOKEN)
{
	TOKEN passTOKEN = preTOKEN;
	passTOKEN = gMathExpr(passTOKEN);
	passTOKEN = gRelationSuffix(passTOKEN);
	return passTOKEN;
}

TOKEN gRelationSuffix(TOKEN preTOKEN)
{
	TOKEN passTOKEN = preTOKEN;
	//����Ƿ�Ϊ�շ���
	if ((passTOKEN.type == KTYPE && (passTOKEN.id == kAND || passTOKEN.id == kOR))
		|| (passTOKEN.type == PTYPE && (passTOKEN.id == pSEMI || passTOKEN.id == pRBRACKET)))
	{
		return passTOKEN;
	}
	//��Ϊ��
	if (!(passTOKEN.type == PTYPE &&
		(passTOKEN.id == pGE || passTOKEN.id == pGT || passTOKEN.id == pLE || passTOKEN.id == pLT || passTOKEN.id == pEQL || passTOKEN.id == pNEQ)))
	{
		//error
	}
	OPR op;
	switch (passTOKEN.id)
	{
	case pGE:
		op = GE;
		break;
	case pGT:
		op = GT;
		break;
	case pLE:
		op = LE;
		break;
	case pLT:
		op = LT;
		break;
	case pEQL:
		op = EQ;
		break;
	case pNEQ:
		op = NE;
		break;
	default:
		//err
		break;
	}
	passTOKEN = Next();
	passTOKEN = gMathExpr(passTOKEN);
	exprGenerateSeq(op, 2);
	passTOKEN = gRelationSuffix(passTOKEN);
	return passTOKEN;
}

TOKEN gMathExpr(TOKEN preTOKEN)
{
	TOKEN passTOKEN = preTOKEN;
	passTOKEN = gMulExpr(passTOKEN);
	passTOKEN = gAddSuffix(passTOKEN);
	return passTOKEN;
}

TOKEN gAddSuffix(TOKEN preTOKEN)
{
	TOKEN passTOKEN = preTOKEN;
	//�жϿշ���
	if ((passTOKEN.type == PTYPE && (passTOKEN.id == pGE || passTOKEN.id == pGT || passTOKEN.id == pLE || passTOKEN.id == pLT || passTOKEN.id == pEQL || passTOKEN.id == pNEQ || passTOKEN.id == pSEMI || passTOKEN.id == pRBRACKET))
		|| (passTOKEN.type == KTYPE && (passTOKEN.id == kAND || passTOKEN.id == kOR)))
	{
		return passTOKEN;
	}
	//�ǿ�
	if (!(passTOKEN.type == PTYPE && (passTOKEN.id == pADD || passTOKEN.id == pMUL)))
	{
		//error
	}
	OPR op;
	switch (passTOKEN.id)
	{
	case pADD:
		op = ADD;
		break;
	case pSUB:
		op = SUB;
		break;
	default:
		break;
	}
	passTOKEN = Next();
	passTOKEN = gMulExpr(passTOKEN);
	exprGenerateSeq(op, 2);
	passTOKEN = gAddSuffix(passTOKEN);
	return passTOKEN;
}

TOKEN gMulExpr(TOKEN preTOKEN)
{
	TOKEN passTOKEN = preTOKEN;
	passTOKEN = gTerm(passTOKEN);
	passTOKEN = gMulSuffix(passTOKEN);
	return passTOKEN;
}

TOKEN gMulSuffix(TOKEN preTOKEN)
{
	TOKEN passTOKEN = preTOKEN;
	//�ж��Ƿ�Ϊ��
	if ((passTOKEN.type == PTYPE && (passTOKEN.id == pADD || passTOKEN.id == pSUB || passTOKEN.id == pGE || passTOKEN.id == pGT || passTOKEN.id == pLE || passTOKEN.id == pLT || passTOKEN.id == pEQL || passTOKEN.id == pNEQ || passTOKEN.id == pSEMI || passTOKEN.id == pRBRACKET))
		|| (passTOKEN.type == KTYPE && (passTOKEN.id == kAND || passTOKEN.id == kOR)))
	{
		return passTOKEN;
	}
	//�ǿ�
	if (!(passTOKEN.type == PTYPE && (passTOKEN.id == pMUL || passTOKEN.id == pADD || passTOKEN.id == pSUB)))
	{
		//error
	}
	OPR op;
	switch (passTOKEN.id)
	{
	case pMUL:
		op = MUL;
		break;
	case pDIV:
		op = DIV;
		break;
	case pMOD:
		op = MOD;
		break;
	default:
		break;
	}
	passTOKEN = Next();
	passTOKEN = gTerm(passTOKEN);
	exprGenerateSeq(op, 2);
	passTOKEN = gMulSuffix(passTOKEN);
	return passTOKEN;
}

TOKEN gTerm(TOKEN preTOKEN)
{
	TOKEN passTOKEN = preTOKEN;
	//�����ڱ��ʽ
	if (passTOKEN.type == PTYPE && passTOKEN.id == pLBRACKET)
	{
		passTOKEN = gExpr(passTOKEN);
		if (!(passTOKEN.type == PTYPE && passTOKEN.id == pRBRACKET))
		{
			//error
		}
		passTOKEN = Next();
		return passTOKEN;
	}
	exprProcessedType term_expr;
	//�������ڱ��ʽ
	switch (passTOKEN.type)
	{
	case ITYPE:
	{
		int found = getAvailableVar(iTable[passTOKEN.id]);
		if (found == -1)
		{
			//error
		}
		term_expr.type = exprID;
		term_expr.datatype = SYMBL[found].type;
		term_expr.content.strID = SYMBL[found].name;
	}
		break;
	case DCTYPE:
		term_expr.type = exprDC;
		term_expr.datatype = TYPEL_INT;
		term_expr.content.d = dcTable[passTOKEN.id];
		break;
	case FCTYPE:
		term_expr.type = exprFC;
		term_expr.datatype = TYPEL_REAL;
		term_expr.content.f = fcTable[passTOKEN.id];
		break;
	case CTYPE:
		term_expr.type = exprCHAR;
		term_expr.datatype = TYPEL_CHAR;
		term_expr.content.ch = cTable[passTOKEN.id];
		break;

	default:
		//error
		break;
	}
	exprPUSH(term_expr);
	passTOKEN = Next();
	return passTOKEN;
}






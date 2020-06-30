#include "GrammarAnalyser.h"

int GrammarAnalyse()
{
	//init
	iAvalDSegOffset = 0;
	currentFunc = NULL;
	iArgNum = 0;
	iFuncVarDefMode = -1;
	exprProcessed.type = exprNULL;
	//start
	TOKEN passTOKEN = Next();
	passTOKEN = gProgram(passTOKEN);
	return 1;
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
	SEQUENCE seq = { PROG, {seqNONE, 0}, {seqNONE, 0}, {seqNONE, 0} };
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
	SEQUENCE seq = { GV, {seqNONE, 0}, {seqNONE, 0}, {seqNONE, 0} };
	sendSequence(seq);
	passTOKEN = Next();
	while (!(passTOKEN.type == PTYPE && passTOKEN.id == pRBRACE))
	{
		passTOKEN = gVarDef(passTOKEN);
	}
	SEQUENCE seq2 = { GE, {seqNONE, 0}, {seqNONE, 0}, {seqNONE, 0} };
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
		SYMBL[SymblLine].type = &TYPEL[0]; //�涨TYPEL��0Ϊint,1Ϊreal,2Ϊchar
		break;
	case kREAL:
		varsize = DEST_REAL_SIZE;
		SYMBL[SymblLine].type = &TYPEL[1];
		break;
	case kCHAR:
		varsize = DEST_CHAR_SIZE;
		SYMBL[SymblLine].type = &TYPEL[2];
		break;
	default:
		//error
		break;
	}
	iAvalDSegOffset += varsize;
	++SymblLine;
	//���ű���д���
	//������Ԫʽ
	SEQUENCE seq = { VAR, {seqID, iTable[varnameid]}, {seqDC, varsize}, {seqNONE, 0} };
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
	//��¼�����ű�����
	strcpy(SYMBL[SymblLine].name, funcname);
	SYMBL[SymblLine].type = NULL;
	SYMBL[SymblLine].cat = fCAT;
	SYMBL[SymblLine].addr = &PFINFL[PFInflLine]; //��ʱ�ú�����δ����PFINFL��
	SYMBL[SymblLine].OFFSET = &(PFINFL[PFInflLine].OFFSET); //��������Ϊָ������Ϣ���е������ָ��
	++SymblLine;
	//��ǰ��д������Ϣ���е����࣬���������д�������� 
	int funcsize; //����ֵռ�ÿռ�
	switch (functype)
	{
	case kINT:
		funcsize = DEST_INT_SIZE;
		break;
	case kREAL:
		funcsize = DEST_REAL_SIZE;
		break;
	case kCHAR:
		funcsize = DEST_CHAR_SIZE;
		break;
	default:
		//error
		break;
	}
	int funcoffset = DEST_PTR_SIZE + funcsize + DEST_PTR_SIZE; //sizeof(OLD BP) + sizeof(ret) + sizeof(OLD IP)
	PFINFL[PFInflLine].OFFSET = funcoffset;
	//������Ԫʽ
	SEQUENCE seq = { FUNC, {seqID, funcnameid}, {seqNONE, 0}, {seqNONE, 0} };
	sendSequence(seq);
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
	++PFInflLine;
	//������Ԫʽ
	SEQUENCE seq2 = { EF, {seqNONE, 0}, {seqNONE, 0}, {seqNONE, 0} };
	sendSequence(seq2);
	currentFunc = NULL;
	iArgNum = 0;
	ptrFirstArg = NULL;
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
		SYMBL[SymblLine].type = &TYPEL[0]; //�涨TYPEL��0Ϊint,1Ϊreal,2Ϊchar
		SYMBL[SymblLine].OFFSET = iAvalFuncOffset;
		if (iFuncVarDefMode == 1)
		{
			iAvalFuncOffset += DEST_PTR_SIZE;
		}
		else
		{
			iAvalFuncOffset += varsize;
		}
		break;
	case kREAL:
		varsize = DEST_REAL_SIZE;
		SYMBL[SymblLine].type = &TYPEL[1];
		SYMBL[SymblLine].OFFSET = iAvalFuncOffset;
		if (iFuncVarDefMode == 1)
		{
			iAvalFuncOffset += DEST_PTR_SIZE;
		}
		else
		{
			iAvalFuncOffset += varsize;
		}
		break;
	case kCHAR:
		varsize = DEST_CHAR_SIZE;
		SYMBL[SymblLine].type = &TYPEL[2];
		SYMBL[SymblLine].OFFSET = iAvalFuncOffset;
		if (iFuncVarDefMode == 1)
		{
			iAvalFuncOffset += DEST_PTR_SIZE;
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
		break;
	case 1:
		SYMBL[SymblLine].cat = vnCAT;
		break;
	case 2:
		SYMBL[SymblLine].cat = svCAT;
		//���ɾֲ�����������Ԫʽ
		SEQUENCE seq = { VT, {seqID, iTable[varnameid]}, {seqDC, varsize}, {seqNONE, 0} };
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
	SEQUENCE seq = { RET, {seqNONE, 0}, {seqNONE, 0} , {seqNONE, 0} };
	switch (exprProcessed.type)
	{
	case exprINT:
		seq.arg1.type = seqDC;
		seq.arg1.content.d = exprProcessed.content.d;
		break;
	case exprREAL:
		seq.arg1.type = seqFC;
		seq.arg1.content.f = exprProcessed.content.f;
		break;
	case exprID:
		seq.arg1.type = seqID;
		seq.arg1.content.str = exprProcessed.content.ptrID;
	default:
		//error
		break;
	}
	sendSequence(seq);
	if (!(passTOKEN.type == PTYPE && passTOKEN.id == pSEMI))
	{
		//error
	}
	passTOKEN = Next();
	return passTOKEN;
}





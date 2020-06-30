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
	//输出程序头四元式
	SEQUENCE seq = { PROG, {seqNONE, 0}, {seqNONE, 0}, {seqNONE, 0} };
	//四元式输出
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
	//记下数据类型
	KEYWORD vartype = passTOKEN.id;
	passTOKEN = Next();
	if (!(passTOKEN.type == ITYPE))
	{
		//error
	}
	//记下变量名
	char varname[MAX_IDLEN];
	//记下变量名在标识符表中的编号
	int varnameid = passTOKEN.id;
	strcpy(varname, iTable[varnameid]);
	passTOKEN = Next();
	if (!(passTOKEN.type == PTYPE && passTOKEN.id == pSEMI))
	{
		//error
	}
	//填写符号表
	strcpy(SYMBL[SymblLine].name, varname);
	SYMBL[SymblLine].cat = gvCAT;
	SYMBL[SymblLine].addr = NULL; //全局变量该项为NULL
	SYMBL[SymblLine].OFFSET = iAvalDSegOffset;
	int varsize = 0;
	switch (vartype)
	{
	case kINT:
		varsize = DEST_INT_SIZE;
		SYMBL[SymblLine].type = &TYPEL[0]; //规定TYPEL中0为int,1为real,2为char
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
	//符号表填写完成
	//生成四元式
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
	//记录至符号表主表
	strcpy(SYMBL[SymblLine].name, funcname);
	SYMBL[SymblLine].type = NULL;
	SYMBL[SymblLine].cat = fCAT;
	SYMBL[SymblLine].addr = &PFINFL[PFInflLine]; //此时该函数尚未填至PFINFL表
	SYMBL[SymblLine].OFFSET = &(PFINFL[PFInflLine].OFFSET); //函数此项为指向函数信息表中的区距的指针
	++SymblLine;
	//提前填写函数信息表中的区距，方便后续填写参数区距 
	int funcsize; //返回值占用空间
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
	//生成四元式
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
	//记录至函数信息表
	PFINFL[PFInflLine].LEVEL = 1;
	PFINFL[PFInflLine].FN = iArgNum;
	PFINFL[PFInflLine].ENTRY = 0;
	PFINFL[PFInflLine].PARAM = ptrFirstArg;
	++PFInflLine;
	//生成四元式
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
	iFuncVarDefMode = 0; //赋值形参定义模式
	while (!(passTOKEN.type == PTYPE && passTOKEN.id == pRBRACE))
	{
		passTOKEN = gFuncVarDef(passTOKEN);
		++iArgNum; //当前函数参数个数加一
	}
	iFuncVarDefMode = -1; //重置函数变量定义模式
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
	iFuncVarDefMode = 1; //换名形参定义模式
	while (!(passTOKEN.type == PTYPE && passTOKEN.id == pRBRACE))
	{
		passTOKEN = gFuncVarDef(passTOKEN);
		++iArgNum; //当前函数参数个数加一
	}
	iFuncVarDefMode = -1; //重置函数变量定义模式
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
	iFuncVarDefMode = 2; //局部变量定义模式
	while (!(passTOKEN.type == PTYPE && passTOKEN.id == pRBRACE))
	{
		passTOKEN = gFuncVarDef(passTOKEN);
		//此处不需要增加参数个数
	}
	iFuncVarDefMode = -1; //重置函数变量定义模式
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
	//填写符号表
	strcpy(SYMBL[SymblLine].name, varname);
	SYMBL[SymblLine].addr = &PFINFL[PFInflLine];
	int varsize = 0;
	switch (vartype)
	{
	case kINT:
		varsize = DEST_INT_SIZE;
		SYMBL[SymblLine].type = &TYPEL[0]; //规定TYPEL中0为int,1为real,2为char
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
		//iArgNum在外层子程序已加过
		break;
	case 1:
		SYMBL[SymblLine].cat = vnCAT;
		break;
	case 2:
		SYMBL[SymblLine].cat = svCAT;
		//生成局部变量定义四元式
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





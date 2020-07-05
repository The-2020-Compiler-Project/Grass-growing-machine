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
	int cursor = startpos; //当前查找的位置
	for (; cursor < SymblLine; ++cursor)
	{
		if (strcmp(name, SYMBL[cursor].name) == 0) //名字相等
		{
			return cursor;
		}
	}
	return -1;
}

int getAvailableVar(char* name)
{
	int found = -1;
	int foundgv = -1;	//全局变量
	int foundfuncv = -1; //函数变量
	if (currentFunc == NULL) //当前作用域为主程序,则检查是否为全局变量
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
	else //当前作用域为currentFunc指向的函数
	{
		do
		{
			found = Find_SymblItemName(found+1, name);
			if (found != -1)
			{
				if (SYMBL[found].cat == gvCAT) //若有全局变量则先记录
				{
					foundgv = found;
				}
				else if (SYMBL[found].cat == vfCAT || SYMBL[found].cat == vnCAT || SYMBL[found].cat == svCAT)
				{
					//判断是否归属于当前函数
					if (SYMBL[found].addr == currentFunc)
					{
						foundfuncv = found;
					}
				}
			}
		} while (found != -1);
	}
	//输出
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
	if (exprSP > MAX_EXPR_STACK_LEN) return 0; //栈满
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
	if (argnum == 2) //目前只做双目
	{
		//栈顶为后面的参数
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
		//检查数据类型是否相同
		if (expr_arg1.datatype != expr_arg2.datatype)
		{
			//error
		}
		char* gotten_midvarname = NULL; //生成的中间变量名
		exprProcessedType exprmidvar; //生成的中间变量的表达式项，用于栈
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
			gotten_midvarname = allocMidVar(TYPEL_INT); //逻辑结果一律使用INT形式存储
			exprmidvar.datatype = TYPEL_INT; //将exprmidvar数据类型改为INT
		}
		exprmidvar.content.strID = gotten_midvarname;
		exprPUSH(exprmidvar); //将生成的中间变量的表达式项压栈

		//填写四元式
		SEQUENCE seq = { op, {seqNONE, 0, true}, {seqNONE, 0, true} , {seqNONE, 0, true} };
		switch (expr_arg1.type) //两个参数数据类型相同,但seq类型可能不相同（一个是常数，一个是标识符）
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
		switch (expr_arg2.type) //两个参数数据类型相同,但seq类型可能不相同（一个是常数，一个是标识符）
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
		//填写中间变量表
		sprintf(MIDVL[MidvlLine].name, "_t%d", iMidVarNum); //约定临时变量前缀为‘_’
		++iMidVarNum;
		MIDVL[MidvlLine].type = TYPEL_INT;
		MIDVL[MidvlLine].OFFSET = iAvalDSegOffset; //iAvalDSegOffset在填完主表后再增加
		//填写符号表主表
		strcpy(SYMBL[SymblLine].name, MIDVL[MidvlLine].name);
		SYMBL[SymblLine].type = TYPEL_INT;
		SYMBL[SymblLine].cat = gvCAT; //临时变量默认gvCAT型
		SYMBL[SymblLine].addr = &MIDVL[MidvlLine];
		SYMBL[SymblLine].OFFSET = iAvalDSegOffset;
		iAvalDSegOffset += DEST_INT_SIZE;
		++MidvlLine;
		++SymblLine;
	}
	else if (type == TYPEL_REAL)
	{
		//填写中间变量表
		sprintf(MIDVL[MidvlLine].name, "_t%d", iMidVarNum); //约定临时变量前缀为‘_’
		++iMidVarNum;
		MIDVL[MidvlLine].type = TYPEL_REAL;
		MIDVL[MidvlLine].OFFSET = iAvalDSegOffset; //iAvalDSegOffset在填完主表后再增加
		//填写符号表主表
		strcpy(SYMBL[SymblLine].name, MIDVL[MidvlLine].name);
		SYMBL[SymblLine].type = TYPEL_REAL;
		SYMBL[SymblLine].cat = gvCAT; //临时变量默认gvCAT型
		SYMBL[SymblLine].addr = &MIDVL[MidvlLine];
		SYMBL[SymblLine].OFFSET = iAvalDSegOffset;
		iAvalDSegOffset += DEST_REAL_SIZE;
		++MidvlLine;
		++SymblLine;
	}
	else if (type == TYPEL_CHAR)
	{
		//填写中间变量表
		sprintf(MIDVL[MidvlLine].name, "_t%d", iMidVarNum); //约定临时变量前缀为‘_’
		++iMidVarNum;
		MIDVL[MidvlLine].type = TYPEL_CHAR;
		MIDVL[MidvlLine].OFFSET = iAvalDSegOffset; //iAvalDSegOffset在填完主表后再增加
		//填写符号表主表
		strcpy(SYMBL[SymblLine].name, MIDVL[MidvlLine].name);
		SYMBL[SymblLine].type = TYPEL_CHAR;
		SYMBL[SymblLine].cat = gvCAT; //临时变量默认gvCAT型
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
	//输出程序头四元式
	SEQUENCE seq = { PROG, {seqNONE, 0, false}, {seqNONE, 0, false}, {seqNONE, 0, false} };
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
		SYMBL[SymblLine].type = TYPEL_INT; //规定TYPEL中0为int,1为real,2为char
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
	//符号表填写完成
	//生成四元式
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
	//当前函数状态改变
	currentFunc = &PFINFL[PFInflLine];
	//记录至符号表主表
	strcpy(SYMBL[SymblLine].name, funcname);
	SYMBL[SymblLine].cat = fCAT;
	SYMBL[SymblLine].addr = &PFINFL[PFInflLine]; //此时该函数尚未填至PFINFL表
	int curFuncSymblLine = SymblLine; //便于回填区距
	//提前填写函数信息表中的区距，方便后续填写参数区距 
	int funcsize; //返回值占用空间
	switch (functype)
	{
	case kINT:
		funcsize = DEST_INT_SIZE;
		SYMBL[SymblLine].type = TYPEL_INT;
		//当前函数返回值类型确定
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
	//生成四元式
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
	//记录至函数信息表
	PFINFL[PFInflLine].LEVEL = 1;
	PFINFL[PFInflLine].FN = iArgNum;
	PFINFL[PFInflLine].ENTRY = 0;
	PFINFL[PFInflLine].PARAM = ptrFirstArg;
	SYMBL[curFuncSymblLine].OFFSET = PFINFL[PFInflLine].OFFSET; //回填区距
	++PFInflLine;
	//生成四元式
	SEQUENCE seq2 = { EF, {seqNONE, 0, false}, {seqNONE, 0, false}, {seqNONE, 0, false} };
	sendSequence(seq2);
	currentFunc = NULL;
	currentFuncType = NULL;
	iArgNum = 0;
	ptrFirstArg = NULL;
	iAvalFuncOffset = 0; //区距清空
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
		SYMBL[SymblLine].type = TYPEL_INT; //规定TYPEL中0为int,1为real,2为char
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
		//iArgNum在外层子程序已加过
		//填写形参表
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
		//填写形参表
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
		//生成局部变量定义四元式
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
	//检查是否符合返回值类型（未做）
	//生成四元式
	SEQUENCE seq = { RET, {seqNONE, 0, true}, {seqNONE, 0, false} , {seqNONE, 0, false} };
	fillExprSeqArg(&seq.arg1);
	sendSequence(seq);
	//使用完exprProcessed需要置空
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
	//程序结构结束四元式
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
	//判断该标识符是否合法（1.是否是变量名 2.是否可以在目前的作用域中被访问）
	int dest_var_symblID = getAvailableVar(dest_varname);
	if (dest_var_symblID == -1)
	{
		//error 非法标识符或无法访问的变量
	}
	passTOKEN = Next();
	if (!(passTOKEN.type == PTYPE && passTOKEN.id == pASSI))
	{
		//error
	}
	passTOKEN = Next();
	if (passTOKEN.type == ITYPE)
	{
		//检查该标识符是函数名还是表达式中的项（GGML不允许函数作为表达式项）
		char strNameToCheck[MAX_IDLEN];
		int iNameIdToCheck = passTOKEN.id;
		strcpy(strNameToCheck, iTable[iNameIdToCheck]);
		int check_symblID = Find_SymblItemName(0, strNameToCheck);
		if (SYMBL[check_symblID].cat == fCAT) //规定函数名与变量名不相同
		{
			int func_symblID = check_symblID;
			passTOKEN = gCodeCall(passTOKEN);
			//判断返回值类型是否与被赋值变量相同
			if (SYMBL[dest_var_symblID].type != SYMBL[func_symblID].type)
			{
				//error 返回值类型不对应
			}
			char *midvarname = allocMidVar(SYMBL[func_symblID].type);
			if (midvarname == NULL)
			{
				//栈空间申请错误
			}
			//生成函数调用四元式
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
	if (!exprProcessed.type == exprNULL) //若刚刚进行了表达式分析
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
		if (currentFunc != NULL) //在函数中
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
		if (currentFunc != NULL) //在函数中
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
		if (currentFunc != NULL) //在函数中
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
	//在赋值语句子程序中已经判断过是否为函数标识符以及生成调用四元式
	//此处只用判断语法成分是否正确
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
		//传参
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
	//填写exprProcessed
	if (!exprPOP(&exprProcessed))
	{
		printf("gExpr():表达式栈错误！\n");
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
	//判断是否为空符号
	if (passTOKEN.type == PTYPE && (passTOKEN.id == pSEMI || passTOKEN.id == pRBRACKET))
	{
		//空符号，转向识别下个语法成分
		return passTOKEN;
	}
	//不为空
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
	//检查是否为空符号
	if ((passTOKEN.type == KTYPE && (passTOKEN.id == kAND || passTOKEN.id == kOR))
		|| (passTOKEN.type == PTYPE && (passTOKEN.id == pSEMI || passTOKEN.id == pRBRACKET)))
	{
		return passTOKEN;
	}
	//不为空
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
	//判断空符号
	if ((passTOKEN.type == PTYPE && (passTOKEN.id == pGE || passTOKEN.id == pGT || passTOKEN.id == pLE || passTOKEN.id == pLT || passTOKEN.id == pEQL || passTOKEN.id == pNEQ || passTOKEN.id == pSEMI || passTOKEN.id == pRBRACKET))
		|| (passTOKEN.type == KTYPE && (passTOKEN.id == kAND || passTOKEN.id == kOR)))
	{
		return passTOKEN;
	}
	//非空
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
	//判断是否为空
	if ((passTOKEN.type == PTYPE && (passTOKEN.id == pADD || passTOKEN.id == pSUB || passTOKEN.id == pGE || passTOKEN.id == pGT || passTOKEN.id == pLE || passTOKEN.id == pLT || passTOKEN.id == pEQL || passTOKEN.id == pNEQ || passTOKEN.id == pSEMI || passTOKEN.id == pRBRACKET))
		|| (passTOKEN.type == KTYPE && (passTOKEN.id == kAND || passTOKEN.id == kOR)))
	{
		return passTOKEN;
	}
	//非空
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
	//括号内表达式
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
	//非括号内表达式
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






#include "LexicalAnalyser.h"

//自动机当前状态
pre_State = 0;
//自动机下一状态
nextState = 0;
//当前所在行数
int iCurrentSrcLine = 1;

//关键字表
char* KT[18] = { "int" , "char" , "real" , "program" , "function" , "var" , "args" , "val" ,
				 "ptr" , "body" , "return" , "if" , "else" , "while" , "endp" , "putc" , "and" , "or" };
//关键字表的长度
int KTSize = 18;

//界符表
char* PT[17] = { "<=" , ">=" , "==" , "=" , "<" , ">" , "!=" , "+" , "-" , "*" , "/" , "%" , "{" ,
				 "}" , "(" , ")"  , ";" };
//界符表的长度
int PTSize = 17;

//初始化当前单词和单词的大小
int initWord()
{
	memset(word, 0, MAX_IDLEN * sizeof(char));
	wordSize = 0;
	return 1;
}

//弹出word的最后一位字符，成功则返回1
int wordPop()
{
	if (wordSize > 0)
	{
		word[wordSize - 1] = '\0';
		wordSize--;
		return 1;
	}
	else
	{
		return 0;
	}
}

//确定自动机下一状态，参数：前一状态，读到的下一个字符，返回自动机的下一状态码
int transition(int lastState, char nextChar)
{
	assert(wordSize < MAX_IDLEN);
	if (nextChar == '\0')
	{
		nextState = 11;
	}
	else
	{
		switch (lastState)
		{
		case(0):
			nextState = next0(nextChar);
			break;
		case(1):
			nextState = next1(nextChar);
			break;
		case(2):
			nextState = next2(nextChar);
			break;
		case(3):
			nextState = next3(nextChar);
			break;
		case(4):
			nextState = next4(nextChar);
			break;
		case(5):
			nextState = next5(nextChar);
			break;
		case(6):
			nextState = next6(nextChar);
			break;
		case(7):
			nextState = next7(nextChar);
			break;
		case(8):
			nextState = next8(nextChar);
			break;
		case(10):
			nextState = next10(nextChar);
			break;
		case(11):
			exit(-1);
			break;
		case(13):
			nextState = next13(nextChar);
			break;
		case(14):
			nextState = next14(nextChar);
			break;
		case(15):
			nextState = next15(nextChar);
			break;
		case(16):
			nextState = next16(nextChar);
			break;
		case(17):
			nextState = next17(nextChar);
			break;
		case(18):
			nextState = next18(nextChar);
			break;
		default:
			printf("The File has ended!\n");
			break;
		}
	}

	return nextState;
}

//自动机状态转换，参数：前一状态，要读取的文件指针
int ToNext(int pastState, FILE* fp)
{
	if(pastState <= 10)
	{
		if ((nextChar = fgetc(fp)) != EOF)
		{
			word[wordSize] = nextChar;
			wordSize++;
		}
		else
		{
			//printf("LexicalAnalyse Completed!\n");
			return 12;
		}
	}

	nextState = transition(pastState, nextChar);
	return nextState;
}

//确定当前状态为0时的下一个状态，返回自动机的下一状态码，参数：读到的下一个字符
int next0(char nextChar)
{
	if (nextChar == '\n' || nextChar == ' ' || nextChar == '\t')
	{
		initWord();
		if (nextChar == '\n') ++iCurrentSrcLine;
		nextState = 0;
	}
	else if ((nextChar >= 'A' && nextChar <= 'Z') || (nextChar >= 'a' && nextChar <= 'z'))
	{
		nextState = 1;
	}
	else if (nextChar >= '0' && nextChar <= '9')
	{
		nextState = 2;
	}
	else if (nextChar == '\'')
	{
		nextState = 3;
	}
	else if (nextChar == '"')
	{
		nextState = 4;
	}
	else if (nextChar == '#')
	{
		nextState = 10;
	}
	else
	{
		nextState = 5;
	}
	return nextState;
}

//确定当前状态为1时的下一个状态，参数：读到的下一个字符
int next1(char nextChar)
{
	if (nextChar == '#')
	{
		wordPop();
		KTfunc();
		initWord();
		nextState = 10;
	}
	else if ((nextChar >= 'A' && nextChar <= 'Z') || (nextChar >= 'a' && nextChar <= 'z') || (nextChar >= '0' && nextChar <= '9'))
	{
		nextState = 1;
	}
	else if (nextChar == ' ' || nextChar == '\n' || nextChar == '\t')
	{
		wordPop();
		KTfunc();
		initWord();
		if (nextChar == '\n') ++iCurrentSrcLine;
		nextState = 0;
	}
	else
	{
		wordPop();
		KTfunc();
		initWord();
		nextState = 13;
	}
	return nextState;
}

//确定当前状态为2时的下一个状态，参数：读到的下一个字符
int next2(char nextChar)
{
	if (nextChar == '.')
	{
		nextState = 6;
	}
	else if (nextChar >= '0' && nextChar <= '9')
	{
		nextState = 2;
	}
	else if ((nextChar >= 'a' && nextChar <= 'z') || (nextChar >= 'A' && nextChar <= 'Z'))
	{
		printf("Expecting an number or an specific identifier after %s, line%d\n", word, iCurrentSrcLine);
		initWord();
		exit(-1);
	}
	else if (nextChar == '#')
	{
		printf("Expecting ';' after %s, line%d\n", word, iCurrentSrcLine);
		exit(-1);
	}
	else if (nextChar == ' ' || nextChar == '\n' || nextChar == '\t')
	{
		wordPop();
		dcTfunc();
		initWord();
		if (nextChar == '\n') ++iCurrentSrcLine;
		nextState = 0;
	}
	else
	{
		wordPop();
		dcTfunc();
		initWord();
		nextState = 13;
	}
	return nextState;
}

//确定当前状态为3时的下一个状态，参数：读到的下一个字符
int next3(char nextChar)
{
	if (nextChar == '\n' || nextChar == '\r')
	{
		printf("Missing ' after %s, line%d\n", word, iCurrentSrcLine);
		initWord();
		exit(-1);
	}
	else
	{
		nextState = 8;
	}
	return nextState;
}

//确定当前状态为4时的下一个状态，参数：读到的下一个字符
int next4(char nextChar)
{
	if (nextChar == '"')
	{
		sTfunc();
		initWord();
		nextState = 0;
	}
	else if (nextChar == '\n')
	{
		printf("Missing \" after %s, line%d\n", word, iCurrentSrcLine);
		initWord();
		exit(-1);
	}
	else
	{
		nextState = 4;
	}
	return nextState;
}

//确定当前状态为5时的下一个状态，参数：读到的下一个字符
int next5(char nextChar)
{
	if (nextChar == '#')
	{
		wordPop();
		PTfunc();
		nextState = 10;
	}
	if (nextChar == ' ' || nextChar == '\n' || nextChar == '\t')
	{
		wordPop();
		PTfunc();
		initWord();
		if (nextChar == '\n') ++iCurrentSrcLine;
		nextState = 0;
	}
	else if ((nextChar >= '0' && nextChar <= '9'))
	{
		wordPop();
		PTfunc();
		initWord();
		nextState = 14;
	}
	else if ((nextChar >= 'a' && nextChar <= 'z') || (nextChar >= 'A' && nextChar <= 'Z'))
	{
		wordPop();
		PTfunc();
		initWord();
		nextState = 15;
	}
	else if (nextChar == '\'')
	{
		wordPop();
		PTfunc();
		initWord();
		nextState = 16;
	}
	else if (nextChar == '"')
	{
		wordPop();
		PTfunc();
		initWord();
		nextState = 17;
	}
	else if (nextChar == '}' || nextChar == ')' || nextChar == '{' || nextChar == '(' || nextChar == ';'
		|| nextChar == '+' || nextChar == '-' || nextChar == '*' || nextChar == '/' || nextChar == '%'
		|| nextChar == '<' || nextChar == '>')
	{
		wordPop();
		PTfunc();
		initWord();
		nextState = 18;
	}
	else
	{
		nextState = 5;
	}
	return nextState;
}

//确定当前状态为6时的下一个状态，参数：读到的下一个字符
int next6(char nextChar)
{
	if (nextChar >= '0' && nextChar <= '9')
		nextState = 6;
	else if ((nextChar >= 'a' && nextChar <= 'z') || (nextChar >= 'A' && nextChar <= 'Z'))
	{
		printf("Expecting number or an specific identifier after %s, line%d\n", word, iCurrentSrcLine);
		initWord();
		exit(-1);
	}
	else if (nextChar == '#')
	{
		printf("Expecting ';' after %s, line%d", word, iCurrentSrcLine);
		exit(-1);
	}
	else if (nextChar == ' ' || nextChar == '\n' || nextChar == '\t')
	{
		wordPop();
		fcTfunc();
		initWord();
		if (nextChar == '\n') ++iCurrentSrcLine;
		nextState = 0;
	}
	else
	{
		wordPop();
		fcTfunc();
		initWord();
		nextState = 13;
	}
	return nextState;
}

//确定当前状态为7时的下一个状态，参数：读到的下一个字符
int next7(char nextChar)
{
	printf("Invalid Input after '%s', line%d\n", word, iCurrentSrcLine);
	initWord();
	exit(-1);
}

//确定当前状态为8时的下一个状态，参数：读到的下一个字符
int next8(char nextChar)
{
	if (nextChar == '\'')
	{
		cTfunc();
		initWord();
		return 0;
	}
	else
	{
		printf("Missing ' after %s, line%d\n", word, iCurrentSrcLine);
		initWord();
		exit(-1);
	}
}

//确定当前状态为9时的下一个状态，参数：读到的下一个字符
int next9(char nextChar)
{
	printf("Missing ' after %s, line%d\n", word, iCurrentSrcLine);
	initWord();
	exit(-1);
}


//确定当前状态为10时的下一个状态，返回自动机的下一状态码，参数：读到的下一个字符
int next10(char nextChar)
{
	if (nextChar == '\n')
	{
		initWord();
		if (nextChar == '\n') ++iCurrentSrcLine;
		nextState = 0;
	}
	else
	{
		initWord();
		nextState = 10;
	}
	return nextState;
}

//确定当前状态为13时的下一个状态，返回自动机的下一状态码，参数：读到的下一个字符
int next13(char nextChar)
{
	word[wordSize] = nextChar;
	wordSize++;
	return 5;
}

//确定当前状态为14时的下一个状态，返回自动机的下一状态码，参数：读到的下一个字符
int next14(char nextChar)
{
	word[wordSize] = nextChar;
	wordSize++;
	return 2;
}

//确定当前状态为15时的下一个状态，返回自动机的下一状态码，参数：读到的下一个字符
int next15(char nextChar)
{
	word[wordSize] = nextChar;
	wordSize++;
	return 1;
}

//确定当前状态为16时的下一个状态，返回自动机的下一状态码，参数：读到的下一个字符
int next16(char nextChar)
{
	word[wordSize] = nextChar;
	wordSize++;
	return 3;
}

//确定当前状态为17时的下一个状态，返回自动机的下一状态码，参数：读到的下一个字符
int next17(char nextChar)
{
	word[wordSize] = nextChar;
	wordSize++;
	return 4;
}

//确定当前状态为18时的下一个状态，返回自动机的下一状态码，参数：读到的下一个字符
int next18(char nextChar)
{
	word[wordSize] = nextChar;
	wordSize++;
	PTfunc();
	initWord();
	return 0;
}

//查询当前单词是否为关键字，是则生成对应Token项，否则运行标识符处理函数
int KTfunc()
{
	for (int i = 0; i < KTSize; i++)
	{
		if (!strcmp(word ,KT[i]))
		{
			getNext(KTYPE, i);
			return 1;
		}
	}
	iTfunc();
}

//查询当前单词是否是已有标识符，是则生成对应Token项，否则将其填入标识符表再输出对应的Token序列
int iTfunc()
{
	for (int i = 0; i < iTline; i++)
	{
		if (!strcmp(word , iTable[i]))
		{
			getNext(ITYPE, i);
			return 1;
		}
	}
	new_iTableItem(word);
	getNext(ITYPE, iTline - 1);
	return 1;
}

//将当前字符型常量置入字符表中，并生成对应的Token序列
int cTfunc()
{
	for (int i = 0; i < cTline; i++)
	{
		if (word[1] == cTable[i])
		{
			getNext(CTYPE, i);
			return 1;
		}
	}
	new_cTableItem(word[1]);
	getNext(CTYPE, cTline - 1);
	return 1;
}

//将当前字符串型常量置入字符表中，并生成对应的Token序列
int sTfunc()
{
	for (int i = 0; i < sTline; i++)
	{
		if (!strcmp(word , sTable[i]))
		{
			getNext(STYPE, i);
			return 1;
		}
	}
	new_sTableItem(word);
	getNext(STYPE, sTline - 1);
	return 1;
}

//将当前整数型常量置入字符表中，并生成对应的Token序列
int dcTfunc()
{
	int num = 0;
	for (int i = 0; i < wordSize; i++)
	{
		num += (int)(word[i] - 48) * pow(10, wordSize - i - 1);
	}
	for (int i = 0; i < dcTline; i++)
	{
		if (num == dcTable[i])
		{
			getNext(DCTYPE, i);
			return 1;
		}
	}
	new_dcTableItem(num);
	getNext(DCTYPE, dcTline - 1);
	return 1;
}

//将当前浮点数型常量置入字符表中，并生成对应的Token序列
int fcTfunc()
{
	double num = 0;
	int i = 0;
	while (word[i] != '.')
	{
		i++;
	}
	for (int t = 0; t < i; t++)
	{
		num += (double)(word[t] - 48) * pow(10, i - t - 1);
	}
	for (int j = i + 1; j < wordSize; j++)
	{
		num += (double)(word[j] - 48) * pow(10, i - j);
	}
	for (int i = 0; i < fcTline; i++)
	{
		if (num == fcTable[i])
		{
			getNext(FCTYPE, i);
			return 1;
		}
	}
	new_fcTableItem(num);
	getNext(FCTYPE, fcTline - 1);
	return 1;
}

//查询当前单词是否在界符表中，是则生成对应Token序列，否则输出错误信息
int PTfunc()
{
	for (int i = 0; i < PTSize; i++)
	{
		if (!strcmp(PT[i] , word))
		{
			getNext(PTYPE, i);
			return 1;
		}
	}
	printf("Invalid identifier '%s', line%d\n", word, iCurrentSrcLine);
	exit(-1);
}

//输出Token，参数：单词类型，单词编号
TOKEN getNext(TOKENTYPE type , int id)
{
	TOKEN token_next;
	token_next.id = id;
	token_next.type = type;
	sign = token_next;
	return token_next;
}

//读取下一个符号的Token项，参数：单词类型，单词编号
TOKEN Next()
{
	sign.type = 0;
	sign.id = 0;
	while (sign.type == 0)
	{
		pre_State = ToNext(pre_State, srcfile);
		if (pre_State == 12 && word[0] != '\0')
		{
			wordSize = 1;
			word[1] = '\0';
			PTfunc();
			word[0] = '\0';
			return sign;
		}
		else if (pre_State == 12)
		{
			sign.type = 0;
			sign.id = 0;
			return sign;
		}
	}
	return sign;
}

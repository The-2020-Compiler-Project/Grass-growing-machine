#include "LexicalAnalyser.h"
#include <windows.h>

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
	memset(word, MAX_IDLEN * sizeof(int), 0);
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
	int nextState = 0;
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
		case(9):
			nextState = next9(nextChar);
			break;
		case(10):
			nextState = next10(nextChar);
			break;
		default:
			printf("Invalid input after %s" , word);
			break;
		}
	}

	return nextState;
}

//自动机状态转换，参数：前一状态，要读取的文件指针，文件读到结尾时返回1
int ToNext(int pastState, FILE* fp)
{
	int nextState = 0;
	int nextChar;

	if ((nextChar = fgetc(fp)) != EOF)
	{
		nextChar = (char)nextChar;
		word[wordSize] = nextChar;
		wordSize++;
	}
	else
	{
		PTfunc();
		printf("LexicalAnalyse Completed!\n");
		return 12;
	}

	nextState = transition(pastState, nextChar);
	return nextState;
}

//确定当前状态为0时的下一个状态，返回自动机的下一状态码，参数：读到的下一个字符
int next0(char nextChar)
{
	int nextState = 0;

	if (nextChar == '\n' || nextChar == ' ' || nextChar == '\t')
	{
		initWord();
		nextState = 0;
	}
	else if ((nextChar >= 'A' && nextChar <= 'Z') || (nextChar >= 'a' && nextChar <= 'z') || nextChar == '_')
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
	int nextState = 0;
	if (nextChar == '#')
	{
		wordPop();
		KTfunc();
		initWord();
		nextState = 10;
	}
	else if ((nextChar >= 'A' && nextChar <= 'Z') || (nextChar >= 'a' && nextChar <= 'z') || (nextChar >= '0' && nextChar <= '9') || nextChar == '_')
	{
		nextState = 1;
	}
	else if (nextChar == ' ' || nextChar == '\n' || nextChar == '\t')
	{
		wordPop();
		KTfunc();
		initWord();
		nextState = 0;
	}
	else
	{
		wordPop();
		KTfunc();
		initWord();
		word[0] = nextChar;
		wordSize++;
		nextState = 5;
	}
	return nextState;
}

//确定当前状态为2时的下一个状态，参数：读到的下一个字符
int next2(char nextChar)
{
	int nextState = 0;
	if (nextChar == '.')
	{
		nextState = 6;
	}
	else if (nextChar >= '0' && nextChar <= '9')
	{
		nextState = 2;
	}
	else
	{
		wordPop();
		dcTfunc();
		initWord();
		word[0] = nextChar;
		wordSize++;
		nextState = 5;
	}
	return nextState;
}

//确定当前状态为3时的下一个状态，参数：读到的下一个字符
int next3(char nextChar)
{
	int nextState = 0;
	if (nextChar == '\'')
	{
		cTfunc();
		initWord();
		nextState = 0;
	}
	else if (nextChar == '\n')
	{
		wordPop();
		nextState = 8;
	}
	else
	{
		nextState = 3;
	}
	return nextState;
}

//确定当前状态为4时的下一个状态，参数：读到的下一个字符
int next4(char nextChar)
{
	int nextState = 0;
	if (nextChar == '"')
	{
		sTfunc();
		initWord();
		nextState = 0;
	}
	else if (nextChar == '\n')
	{
		wordPop();
		nextState = 9;
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
	int nextState = 0;
	if (nextChar == '#')
	{
		wordPop();
		PTfunc();
		nextState = 10;
	}
	if (nextChar == ' ' || nextChar == '\n')
	{
		wordPop();
		PTfunc();
		initWord();
		nextState = 0;
	}
	else if ((nextChar >= '0' && nextChar <= '9'))
	{
		wordPop();
		PTfunc();
		initWord();
		word[0] = nextChar;
		wordSize++;
		nextState = 2;
	}
	else if ((nextChar >= 'a' && nextChar <= 'z') || (nextChar >= 'A' && nextChar <= 'Z') || nextChar == '_')
	{
		wordPop();
		PTfunc();
		initWord();
		word[0] = nextChar;
		wordSize++;
		nextState = 1;
	}
	else if (nextChar == '\'')
	{
		wordPop();
		PTfunc();
		initWord();
		word[0] = nextChar;
		wordSize++;
		nextState = 3;
	}
	else if (nextChar == '"')
	{
		wordPop();
		PTfunc();
		initWord();
		word[0] = nextChar;
		wordSize++;
		nextState = 4;
	}
	else if (nextChar == '}' || nextChar == ')' || nextChar == '{' || nextChar == '(')
	{
		wordPop();
		PTfunc();
		initWord();
		word[0] = nextChar;
		wordSize++;
		PTfunc();
		initWord();
		nextState = 0;
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
	int nextState = 0;

	if (nextChar >= '0' && nextChar <= '9')
		nextState = 6;
	else if ((nextChar == '(') || (nextChar == '{') || (nextChar == ')') || (nextChar == '}') || (nextChar == ';'))
	{
		wordPop();
		fcTfunc();
		initWord();
		word[0] = nextChar;
		wordSize++;
		nextState = 5;
	}
	else
	{
		fcTfunc();
		initWord();
		nextState = 0;
	}
	return nextState;
}

//确定当前状态为7时的下一个状态，参数：读到的下一个字符
int next7(char nextChar)
{
	int nextState = 0;
	printf("Invalid Input after %s", word);
	initWord();
	return nextState;
}

//确定当前状态为8时的下一个状态，参数：读到的下一个字符
int next8(char nextChar)
{
	int nextState = 0;
	printf("Missing ' after %s", word);
	initWord();
	return nextState;
}

//确定当前状态为9时的下一个状态，参数：读到的下一个字符
int next9(char nextChar)
{
	int nextState = 0;
	printf("Missing \" after %s", word);
	initWord();
	return nextState;
}

//确定当前状态为10时的下一个状态，返回自动机的下一状态码，参数：读到的下一个字符
int next10(char nextChar)
{
	int nextState = 0;
	if (nextChar == '\n')
	{
		initWord();
		nextState = 0;
	}
	else
	{
		nextState = 10;
	}
	return nextState;
}

//查询当前单词是否为关键字，是则生成对应Token项，否则运行标识符处理函数
int KTfunc()
{
	for (int i = 0; i < KTSize; i++)
	{
		int j = 0;
		while (word[j] == KT[i][j])
		{
			if (j == sizeof(KT[i]) - 1)
			{
				getNext(KTYPE, i);
				return 1;
			}
			j++;
		}
	}
	iTfunc();
}

//查询当前单词是否是已有标识符，是则生成对应Token项，否则将其填入标识符表再输出对应的Token序列
int iTfunc()
{
	for (int i = 0; i < iTline; i++)
	{
		int j = 0;
		while (word[j] == iTable[i][j])
		{
			if (j == sizeof(word) - 1)
			{
				getNext(ITYPE, i);
				return 1;
			}
			j++;
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
		int j = 0;
		while (word[j] == sTable[i][j])
		{
			if (j == sizeof(word) - 1)
			{
				getNext(STYPE, i);
				return 1;
			}
			j++;
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
		num += (int)(word[i] - 48) * pow(10, wordSize - i - 1);
		i++;
	}
	for (int j = i + 1; j < wordSize; j++)
	{
		num += (int)(word[j] - 48) * pow(10, i - j);
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
	wordSize++;
	for (int i = 0; i < PTSize; i++)
	{
		int j = 0;
		while (word[j] == PT[i][j])
		{
			if (j == wordSize - 1)
			{
				getNext(PTYPE, i);
				return 1;
			}
			j++;
		}
	}
	return 0;
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
	sign.type = -1;
	while (sign.type == -1)
	{
		pre_State = ToNext(pre_State, srcfile);
		if (pre_State = 12)
		{
			sign.type = NOTYPE;
			sign.id = 0;
		}
	}
	return sign;
}

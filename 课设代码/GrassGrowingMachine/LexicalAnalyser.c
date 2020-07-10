#include "LexicalAnalyser.h"

//�Զ�����ǰ״̬
pre_State = 0;
//�Զ�����һ״̬
nextState = 0;
//��ǰ��������
int iCurrentSrcLine = 1;

//�ؼ��ֱ�
char* KT[18] = { "int" , "char" , "real" , "program" , "function" , "var" , "args" , "val" ,
				 "ptr" , "body" , "return" , "if" , "else" , "while" , "endp" , "putc" , "and" , "or" };
//�ؼ��ֱ�ĳ���
int KTSize = 18;

//�����
char* PT[17] = { "<=" , ">=" , "==" , "=" , "<" , ">" , "!=" , "+" , "-" , "*" , "/" , "%" , "{" ,
				 "}" , "(" , ")"  , ";" };
//�����ĳ���
int PTSize = 17;

//��ʼ����ǰ���ʺ͵��ʵĴ�С
int initWord()
{
	memset(word, 0, MAX_IDLEN * sizeof(char));
	wordSize = 0;
	return 1;
}

//����word�����һλ�ַ����ɹ��򷵻�1
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

//ȷ���Զ�����һ״̬��������ǰһ״̬����������һ���ַ��������Զ�������һ״̬��
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

//�Զ���״̬ת����������ǰһ״̬��Ҫ��ȡ���ļ�ָ��
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

//ȷ����ǰ״̬Ϊ0ʱ����һ��״̬�������Զ�������һ״̬�룬��������������һ���ַ�
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

//ȷ����ǰ״̬Ϊ1ʱ����һ��״̬����������������һ���ַ�
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

//ȷ����ǰ״̬Ϊ2ʱ����һ��״̬����������������һ���ַ�
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

//ȷ����ǰ״̬Ϊ3ʱ����һ��״̬����������������һ���ַ�
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

//ȷ����ǰ״̬Ϊ4ʱ����һ��״̬����������������һ���ַ�
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

//ȷ����ǰ״̬Ϊ5ʱ����һ��״̬����������������һ���ַ�
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

//ȷ����ǰ״̬Ϊ6ʱ����һ��״̬����������������һ���ַ�
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

//ȷ����ǰ״̬Ϊ7ʱ����һ��״̬����������������һ���ַ�
int next7(char nextChar)
{
	printf("Invalid Input after '%s', line%d\n", word, iCurrentSrcLine);
	initWord();
	exit(-1);
}

//ȷ����ǰ״̬Ϊ8ʱ����һ��״̬����������������һ���ַ�
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

//ȷ����ǰ״̬Ϊ9ʱ����һ��״̬����������������һ���ַ�
int next9(char nextChar)
{
	printf("Missing ' after %s, line%d\n", word, iCurrentSrcLine);
	initWord();
	exit(-1);
}


//ȷ����ǰ״̬Ϊ10ʱ����һ��״̬�������Զ�������һ״̬�룬��������������һ���ַ�
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

//ȷ����ǰ״̬Ϊ13ʱ����һ��״̬�������Զ�������һ״̬�룬��������������һ���ַ�
int next13(char nextChar)
{
	word[wordSize] = nextChar;
	wordSize++;
	return 5;
}

//ȷ����ǰ״̬Ϊ14ʱ����һ��״̬�������Զ�������һ״̬�룬��������������һ���ַ�
int next14(char nextChar)
{
	word[wordSize] = nextChar;
	wordSize++;
	return 2;
}

//ȷ����ǰ״̬Ϊ15ʱ����һ��״̬�������Զ�������һ״̬�룬��������������һ���ַ�
int next15(char nextChar)
{
	word[wordSize] = nextChar;
	wordSize++;
	return 1;
}

//ȷ����ǰ״̬Ϊ16ʱ����һ��״̬�������Զ�������һ״̬�룬��������������һ���ַ�
int next16(char nextChar)
{
	word[wordSize] = nextChar;
	wordSize++;
	return 3;
}

//ȷ����ǰ״̬Ϊ17ʱ����һ��״̬�������Զ�������һ״̬�룬��������������һ���ַ�
int next17(char nextChar)
{
	word[wordSize] = nextChar;
	wordSize++;
	return 4;
}

//ȷ����ǰ״̬Ϊ18ʱ����һ��״̬�������Զ�������һ״̬�룬��������������һ���ַ�
int next18(char nextChar)
{
	word[wordSize] = nextChar;
	wordSize++;
	PTfunc();
	initWord();
	return 0;
}

//��ѯ��ǰ�����Ƿ�Ϊ�ؼ��֣��������ɶ�ӦToken��������б�ʶ��������
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

//��ѯ��ǰ�����Ƿ������б�ʶ�����������ɶ�ӦToken������������ʶ�����������Ӧ��Token����
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

//����ǰ�ַ��ͳ��������ַ����У������ɶ�Ӧ��Token����
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

//����ǰ�ַ����ͳ��������ַ����У������ɶ�Ӧ��Token����
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

//����ǰ�����ͳ��������ַ����У������ɶ�Ӧ��Token����
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

//����ǰ�������ͳ��������ַ����У������ɶ�Ӧ��Token����
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

//��ѯ��ǰ�����Ƿ��ڽ�����У��������ɶ�ӦToken���У��������������Ϣ
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

//���Token���������������ͣ����ʱ��
TOKEN getNext(TOKENTYPE type , int id)
{
	TOKEN token_next;
	token_next.id = id;
	token_next.type = type;
	sign = token_next;
	return token_next;
}

//��ȡ��һ�����ŵ�Token��������������ͣ����ʱ��
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

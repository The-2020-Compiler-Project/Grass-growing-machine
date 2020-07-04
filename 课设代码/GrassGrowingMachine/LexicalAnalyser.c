#include "LexicalAnalyser.h"
#include <windows.h>

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
	for (int i = 0; i < MAX_IDLEN; i++)
	{
		word[i] = '\0';
	}
	wordSize = 0;
	return 1;
}

//��ʼ��Token��������
int initToken()
{
	memset(tokenFinal, 10000 * sizeof(TOKEN), '\0');
	tokenSize = 0;
	tokenPos = -1;
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
int transition(int preState, char nextChar)
{
	int nextState = 0;
	if (nextChar == '\0')
	{
		nextState = 11;
	}
	else
	{
		switch (preState)
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

//�Զ���״̬ת����������ǰһ״̬��Ҫ��ȡ���ļ�ָ�룬�ļ�������βʱ����1
int ToNext(int preState, FILE* fp)
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
		getNext(NOTYPE , 0);
		printf("LexicalAnalyse Completed!\n");
		return 1;
	}

	nextState = transition(preState, nextChar);

	ToNext(nextState, fp);
}

//ȷ����ǰ״̬Ϊ0ʱ����һ��״̬�������Զ�������һ״̬�룬��������������һ���ַ�
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

//ȷ����ǰ״̬Ϊ1ʱ����һ��״̬����������������һ���ַ�
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

//ȷ����ǰ״̬Ϊ2ʱ����һ��״̬����������������һ���ַ�
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

//ȷ����ǰ״̬Ϊ3ʱ����һ��״̬����������������һ���ַ�
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

//ȷ����ǰ״̬Ϊ4ʱ����һ��״̬����������������һ���ַ�
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

//ȷ����ǰ״̬Ϊ5ʱ����һ��״̬����������������һ���ַ�
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

//ȷ����ǰ״̬Ϊ6ʱ����һ��״̬����������������һ���ַ�
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

//ȷ����ǰ״̬Ϊ7ʱ����һ��״̬����������������һ���ַ�
int next7(char nextChar)
{
	int nextState = 0;
	printf("Invalid Input after %s", word);
	initWord();
	return nextState;
}

//ȷ����ǰ״̬Ϊ8ʱ����һ��״̬����������������һ���ַ�
int next8(char nextChar)
{
	int nextState = 0;
	printf("Missing ' after %s", word);
	initWord();
	return nextState;
}

//ȷ����ǰ״̬Ϊ9ʱ����һ��״̬����������������һ���ַ�
int next9(char nextChar)
{
	int nextState = 0;
	printf("Missing \" after %s", word);
	initWord();
	return nextState;
}

//ȷ����ǰ״̬Ϊ10ʱ����һ��״̬�������Զ�������һ״̬�룬��������������һ���ַ�
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

//��ѯ��ǰ�����Ƿ�Ϊ�ؼ��֣��������ɶ�ӦToken��������б�ʶ��������
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

//��ѯ��ǰ�����Ƿ������б�ʶ�����������ɶ�ӦToken������������ʶ�����������Ӧ��Token����
int iTfunc()
{
	for (int i = 0; i < iTline; i++)
	{
		int j = 0;
		while (word[j] == iTable[i][j])
		{
			int size = sizeof(word);
			if (j == sizeof(word) - 1)
			{
				getNext(ITYPE, i);
				return 1;
			}
			j++;
		}
	}
	new_iTableItem(word);
	getNext(ITYPE , iTline - 1);
	return 1;
}

//����ǰ�ַ��ͳ��������ַ����У������ɶ�Ӧ��Token����
int cTfunc()
{
	new_cTableItem(word[1]);
	getNext(CTYPE, cTline - 1);
	return 1;
}

//����ǰ�ַ����ͳ��������ַ����У������ɶ�Ӧ��Token����
int sTfunc()
{
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
		num += (int)(word[i] - 48) * pow(10, wordSize - i - 1);
		i++;
	}
	for (int j = i + 1; j < wordSize; j++)
	{
		num += (int)(word[j] - 48) * pow(10, i - j);
	}
	new_fcTableItem(num);
	getNext(FCTYPE, fcTline - 1);
	return 1;
}

//��ѯ��ǰ�����Ƿ��ڽ�����У��������ɶ�ӦToken���У��������������Ϣ
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

//���Token���������������ͣ����ʱ��
TOKEN getNext(TOKENTYPE type , int id)
{
	TOKEN token_next;
	token_next.id = id;
	token_next.type = type;
	tokenFinal[tokenSize] = token_next;
	tokenSize++;
	return token_next;
}

//��ȡ��һ�����ŵ�Token��������������ͣ����ʱ��
TOKEN Next()
{
	tokenPos++;
	return tokenFinal[tokenPos-1];
}

//�ʷ����������ã��������ļ�ָ��
int LexicalAnalyser(FILE* fp)
{
	initWord();
	int State = 0;
	ToNext(State, fp);

}


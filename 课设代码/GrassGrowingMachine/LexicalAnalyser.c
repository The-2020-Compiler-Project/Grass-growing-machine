#include "LexicalAnalyser.h"

//��ʼ����ǰ���ʺ͵��ʵĴ�С
int initWord()
{
	memset(word, 100 * sizeof(char), '\0');
	wordSize = 0;
	return 1;
}

//��ʼ��Token��������
int initToken()
{
	memset(tokenFinal, 1000 * sizeof(TOKEN), '\0');
	tokenSize = 0;
	return 1;
}

//����word�����һλ�ַ����ɹ��򷵻�1
int wordPop()
{
	word[wordSize - 1] = '\0';
	wordSize -= 1;
	return 1;
}

//ȷ���Զ�����һ״̬��������ǰһ״̬����������һ���ַ�����ǰ�����ĵ��ʵĳ��ȣ���ǰ�����ĵ��ʣ������Զ�������һ״̬��
int transition(int preState, char nextChar, int wordSize, char* word)
{
	int nextState = 0;
	if (nextChar = '\0')
	{
		nextState = 10;
	}
	else
	{
		switch (preState)
		{
		case(0):
			nextState = next0(nextChar, wordSize, word);
			break;
		case(1):
			nextState = next1(nextChar, wordSize, word);
			break;
		case(2):
			nextState = next2(nextChar, wordSize, word);
			break;
		case(3):
			nextState = next3(nextChar, wordSize, word);
			break;
		case(4):
			nextState = next4(nextChar, wordSize, word);
			break;
		case(5):
			nextState = next5(nextChar, wordSize, word);
			break;
		case(6):
			nextState = next6(nextChar, wordSize, word);
			break;
		case(7):
			nextState = next7(nextChar, wordSize, word);
			break;
		case(8):
			nextState = next8(nextChar, wordSize, word);
			break;
		case(9):
			nextState = next9(nextChar, wordSize, word);
			break;
		default:
			printf("Invalid input after %s", word);
			break;
		}
	}

	return nextState;
}

//�Զ���״̬ת����������ǰһ״̬��Ҫ��ȡ���ļ�ָ�룬�ļ�������βʱ����1
int ToNext(int preState, FILE* fp)
{
	int nextState = 0;
	char nextChar;

	if ((nextChar = fgetc(fp)) != EOF)
	{
		word[wordSize] = nextChar;
		wordSize++;
	}
	else
	{
		printf("LexicalAnalyse Completed!");
		return 1;
	}

	if (nextChar == ' ' || nextChar == '\n')
	{
		initWord();
		nextState = transition(preState, nextChar, wordSize, word);
		initWord();
	}
	else
	{
		nextState = transition(preState, nextChar, wordSize, word);
	}
	return ToNext(nextState, fp);
}

//ȷ����ǰ״̬Ϊ0ʱ����һ��״̬�������Զ�������һ״̬�룬��������������һ���ַ�����ǰ�������ֵĳ��ȣ���ǰ��������
int next0(char nextChar, int wordSize, char* word)
{
	int nextState = 0;

	if (nextChar == '\n' || nextChar == ' ' || nextChar == '\t')
	{
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
		nextState = 9;
	}
	else
		nextState = 5;

	return nextState;
}

//ȷ����ǰ״̬Ϊ1ʱ����һ��״̬����������������һ���ַ�����ǰ�������ֵĳ��ȣ���ǰ��������
int next1(char nextChar, int wordSize, char* word)
{
	int nextState = 0;
	if (nextChar == '#')
	{
		wordPop();
		KTfunc(word);
		initWord();
		nextState = 9;
	}
	if ((nextChar >= 'A' && nextChar <= 'Z') || (nextChar >= 'a' && nextChar <= 'z') || (nextChar >= '0' && nextChar <= '9'))
		nextState = 1;
	else if (nextChar == ' ' || nextChar == '\n')
	{
		KTfunc(word);
		initWord();
	}
	else
	{
		wordPop();
		KTfunc(word);
		initWord();
		word[0] = nextChar;
		nextState = 0;
	}
	return nextState;
}

//ȷ����ǰ״̬Ϊ2ʱ����һ��״̬����������������һ���ַ�����ǰ�������ֵĳ��ȣ���ǰ��������
int next2(char nextChar, int wordSize, char* word)
{
	int nextState = 0;
	if (nextChar >= '.')
		nextState = 6;
	else if (nextChar >= '0' && nextChar <= '9')
	{
		nextChar = 2;
	}
	else
	{
		wordPop();
		dcTfunc(word);
		initWord();
		word[0] = nextChar;
		nextState = 0;
	}
	return nextState;
}

//ȷ����ǰ״̬Ϊ3ʱ����һ��״̬����������������һ���ַ�����ǰ�������ֵĳ��ȣ���ǰ��������
int next3(char nextChar, int wordSize, char* word)
{
	int nextState = 0;
	if (nextChar == '\'')
	{
		cTfunc(word);
		initWord();
		nextState = 0;
	}
	else if (nextChar == ' ' || nextChar == '\n')
	{
		initWord();
		nextState = 7;
	}
	else
	{
		nextState = 3;
	}
	return nextState;
}

//ȷ����ǰ״̬Ϊ4ʱ����һ��״̬����������������һ���ַ�����ǰ�������ֵĳ��ȣ���ǰ��������
int next4(char nextChar, int wordSize, char* word)
{
	int nextState = 0;
	if (nextChar = '"')
	{
		sTfunc(word);
		initWord();
		nextState = 0;
	}
	else if (nextChar == ' ' || nextChar == '\n')
		nextState = 8;
	else
	{
		nextState = 4;
	}
	return nextState;
}

//ȷ����ǰ״̬Ϊ5ʱ����һ��״̬����������������һ���ַ�����ǰ�������ֵĳ��ȣ���ǰ��������
int next5(char nextChar, int wordSize, char* word)
{
	int nextState = 0;
	if (nextChar == '#')
	{
		wordPop();
		PTfunc(word);
		nextState = 9;
	}
	if (nextChar == ' ' || nextChar == '\n')
	{
		PTfunc(word);
		initWord();
		nextState = 0;
	}
	else if ((nextChar >= '0' && nextChar <= '9') || (nextChar >= 'a' && nextChar <= 'z') || (nextChar >= 'A' && nextChar <= 'Z') || nextChar == '\'' || nextChar == '"')
	{
		wordPop();
		PTfunc(word);
		initWord();
		word[0] = nextChar;
		nextState = 0;
	}
	else
	{
		nextState = 5;
	}
	return nextState;
}

//ȷ����ǰ״̬Ϊ6ʱ����һ��״̬����������������һ���ַ�����ǰ�������ֵĳ��ȣ���ǰ��������
int next6(char nextChar, int wordSize, char* word)
{
	int nextState = 0;
	if (nextChar >= '0' && nextChar <= '9')
		nextState = 6;
	else
	{
		fcTfunc(word);
		initWord();
		nextState = 0;
	}
	return nextState;
}

//ȷ����ǰ״̬Ϊ7ʱ����һ��״̬����������������һ���ַ�����ǰ�������ֵĳ��ȣ���ǰ��������
int next7(char nextChar, int wordSize, char* word)
{
	int nextState = 0;
	printf("Missing \" ' \"after %s", word);
	initWord();
	return nextState;
}

//ȷ����ǰ״̬Ϊ8ʱ����һ��״̬����������������һ���ַ�����ǰ�������ֵĳ��ȣ���ǰ��������
int next8(char nextChar, int wordSize, char* word)
{
	int nextState = 0;
	printf("Missing \" \" \"after %s", word);
	initWord();
	return nextState;
}

//ȷ����ǰ״̬Ϊ9ʱ����һ��״̬����������������һ���ַ�����ǰ�������ֵĳ��ȣ���ǰ��������
int next9(char nextChar, int wordSize, char* word)
{
	int nextState = 0;
	if (nextChar == '\n')
	{
		nextState = 0;
		initWord();
	}
	else
	{
		nextState = 9;
	}
	return nextState;
}

//��ѯ��ǰ�����Ƿ�Ϊ�ؼ��֣��������ɶ�ӦToken��������б�ʶ������������������ǰ����
int KTfunc(char* word)
{
	for (int i = 0; i < KTSize; i++)
	{
		if (word == KT[i])
		{
			next(KTYPE, i);
			return 1;
		}
	}
	return iTfunc(word);
}

//��ѯ��ǰ�����Ƿ������б�ʶ�����������ɶ�ӦToken������������ʶ�����������Ӧ��Token���У���������ǰ����
int iTfunc(char* word)
{
	new_iTableItem(word);
	next(ITYPE, iTline - 1);
	return 1;
}

//����ǰ�ַ��ͳ��������ַ����У������ɶ�Ӧ��Token���У���������ǰ����
int cTfunc(char* word)
{
	new_cTableItem(word[1]);
	next(CTYPE, cTline - 1);
	return 1;
}

//����ǰ�ַ����ͳ��������ַ����У������ɶ�Ӧ��Token���У���������ǰ����
int sTfunc(char* word)
{
	new_sTableItem(word);
	next(STYPE, sTline - 1);
	return 1;
}

//����ǰ�����ͳ��������ַ����У������ɶ�Ӧ��Token���У���������ǰ����
int dcTfunc(char* word)
{
	int num = 0;
	for (int i = 0; i < wordSize; i++)
	{
		num += (int)word[i] * pow(10, wordSize - i - 1);
	}
	new_dcTableItem(num);
	next(DCTYPE, dcTline - 1);
	return 1;
}

//����ǰ�������ͳ��������ַ����У������ɶ�Ӧ��Token���У���������ǰ����
int fcTfunc(char* word)
{
	double num = 0;
	int i = 0;
	while (word[i] != '.')
	{
		num += word[i] * pow(10, wordSize - i - 1);
		i++;
	}
	for (int j = i + 1; j < wordSize; j++)
	{
		num += word[j] * pow(10, -j);
	}
	new_fcTableItem(num);
	next(FCTYPE, fcTline - 1);
	return 1;
}

//��ѯ��ǰ�����Ƿ��ڽ�����У��������ɶ�ӦToken���У��������������Ϣ����������ǰ����
int PTfunc(char* word)
{
	for (int i = 0; i < KTSize; i++)
	{
		if (word == KT[i])
		{
			next(PTYPE, i);
			return 1;
		}
	}
	printf("Invalid input after %s", word);
	return 0;
}

//���Token���������������ͣ����ʱ��
TOKEN next(TOKENTYPE type, int id)
{
	TOKEN token_next;
	token_next.id = id;
	token_next.type = type;
	tokenFinal[tokenSize] = token_next;
	tokenSize++;
	return token_next;
}

int main()
{
	initWord();
	int State = 0;
	FILE* fp = fopen("/Hello.ggml", "r");
	ToNext(State, fp);
	for (int i = 0; i < tokenSize; i++)
	{
		printf("%d %d", tokenFinal[i].type, tokenFinal[i].id);
	}
}
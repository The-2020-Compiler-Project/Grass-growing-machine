#include "SymbolList.h"

int init_symbl()
{
	//��Ԫʽ�����
	SeqLine = 0;
	memset(SequenceList, 0, MAXSEQLISTSIZE * sizeof(SEQUENCE));
	//�������
	SymblLine = 0;
	memset(SYMBL, 0, MAX_SYMBLISTSIZE * sizeof(SYMBLITEM));
	//������Ϣ�����
	PFInflLine = 0;
	memset(PFINFL, 0, MAX_SYMBLISTSIZE * sizeof(PFINFLITEM));
	//���������
	ArglLine = 0;
	memset(ARGL, 0, MAX_SYMBLISTSIZE * sizeof(ARGLITEM));
	//���ͱ����
	TypelLine = 0;
	memset(TYPEL, 0, MAX_SYMBLISTSIZE * sizeof(TYPELITEM));
	//���������
	ConslLine = 0;
	memset(CONSL, 0, MAX_SYMBLISTSIZE * sizeof(CONSLITEM));
	//���ȱ����
	LenlLine = 0;
	memset(LENL, 0, MAX_SYMBLISTSIZE * sizeof(int));
	//�м���������
	MidvlLine = 0;
	memset(MIDVL, 0, MAX_SYMBLISTSIZE * sizeof(MIDVLITEM));
	return 1;
}
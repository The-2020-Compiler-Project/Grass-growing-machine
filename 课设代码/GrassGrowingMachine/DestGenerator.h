#pragma once
#include <stdbool.h>
#include "SymbolList.h"
#include "GrammarAnalyser.h"

//����Ԫʽ�б�����Ŀ����룬 SeqListΪ��Ԫʽ�б� SeqLineΪ��Ԫʽ����Ŀ, targetΪ������ļ����ɹ�����1�� ʧ�ܷ���0
int DestGenerator(SEQUENCE *SeqList, int SeqLine, FILE *target);
//���ɵ�����Ԫʽ��Ŀ����룬�ɹ�����1�� ʧ�ܷ���0
int SingleDestGenerate(SEQUENCE Seq);

char buff[MAX_STRLEN]; //������

int param_num; //���ڼ�¼�Ѿ����εĸ�������PARAM�����������CALL�������

int IF_SP; //IF��ŵ�ջָ��
int IF_Stack[MAX_SYMBLISTSIZE]; //IF���ջ
int WHILE_SP; //WHILE��ŵ�ջָ��
int WHILE_Stack[MAX_SYMBLISTSIZE]; //WHILE���ջ

//�Ӵ�����

//IF���ѹջ���ɹ�����1��ʧ�ܷ���0
int IF_Push(int id);
//IF��ų�ջ���ɹ�����1��ʧ�ܷ���0
int IF_Pop();
//WHILE���ѹջ���ɹ�����1��ʧ�ܷ���0
int WHILE_Push(int id);
//WHILE��ų�ջ���ɹ�����1��ʧ�ܷ���0
int WHILE_Pop();

//��AX�е�ֵ�����SYMBL[RDL_SYMBLID]������RDL_SYMBLID��Ϊ-1���ɹ�����1��ʧ�ܣ�RDL_SYMBLID����SYMBL��Χ������0��ÿ����ת���Ŀ�ͷ�����������β����������øú�����
int Reg_ST();
//��SYMBL[symbl_id]��ֵ����AX���ɹ�����1�� ʧ�ܣ�symbl_id����SYMBL��Χ������0
int Reg_LD(int symbl_id);

//�������ֲ���������, SeqΪ��Ӧ��Ԫʽ, �ɹ�����1��ʧ�ܷ���0
int sub_VT(SEQUENCE Seq);
//����ӷ�, SeqΪ��Ӧ��Ԫʽ, �ɹ�����1��ʧ�ܷ���0
int sub_ADD(SEQUENCE Seq);
//�������, SeqΪ��Ӧ��Ԫʽ, �ɹ�����1��ʧ�ܷ���0
int sub_SUB(SEQUENCE Seq);
//����˷�, SeqΪ��Ӧ��Ԫʽ, �ɹ�����1��ʧ�ܷ���0
int sub_MUL(SEQUENCE Seq);
//�������, SeqΪ��Ӧ��Ԫʽ, MODΪTrueʱ����ȡ�࣬�ɹ�����1��ʧ�ܷ���0
int sub_DIV(SEQUENCE Seq, bool isMOD);
//����ֵ, SeqΪ��Ӧ��Ԫʽ���ɹ�����1��ʧ�ܷ���0
int sub_ASSI(SEQUENCE Seq);
//�����ϵ�����, SeqΪ��Ӧ��Ԫʽ���ɹ�����1��ʧ�ܷ���0
int sub_Relation(SEQUENCE Seq);
//�����߼������, SeqΪ��Ӧ��Ԫʽ���ɹ�����1��ʧ�ܷ���0
int sub_Logic(SEQUENCE Seq);
//���������, SeqΪ��Ӧ��Ԫʽ���ɹ�����1��ʧ�ܷ���0
int sub_PARAM(SEQUENCE Seq);
//�������������, SeqΪ��Ӧ��Ԫʽ���ɹ�����1��ʧ�ܷ���0
int sub_CALL(SEQUENCE Seq);
//�������������, SeqΪ��Ӧ��Ԫʽ���ɹ�����1��ʧ�ܷ���0
int sub_RET(SEQUENCE Seq);
//����������, SeqΪ��Ӧ��Ԫʽ���ɹ�����1��ʧ�ܷ���0
int sub_FUNC(SEQUENCE Seq);
//������������β, SeqΪ��Ӧ��Ԫʽ���ɹ�����1��ʧ�ܷ���0
int sub_EF(SEQUENCE Seq);
//����IF��俪ͷ, SeqΪ��Ӧ��Ԫʽ���ɹ�����1��ʧ�ܷ���0
int sub_IF(SEQUENCE Seq);
//����ELSE���, SeqΪ��Ӧ��Ԫʽ���ɹ�����1��ʧ�ܷ���0
int sub_EL(SEQUENCE Seq);
//����IE, SeqΪ��Ӧ��Ԫʽ���ɹ�����1��ʧ�ܷ���0
int sub_IE(SEQUENCE Seq);
//����WH, SeqΪ��Ӧ��Ԫʽ���ɹ�����1��ʧ�ܷ���0
int sub_WHILE(SEQUENCE Seq);
//����DO, SeqΪ��Ӧ��Ԫʽ���ɹ�����1��ʧ�ܷ���0
int sub_DO(SEQUENCE Seq);
//����WE, SeqΪ��Ӧ��Ԫʽ���ɹ�����1��ʧ�ܷ���0
int sub_WE(SEQUENCE Seq);
//����PUTC, SeqΪ��Ӧ��Ԫʽ���ɹ�����1��ʧ�ܷ���0
int sub_PUTC(SEQUENCE Seq);
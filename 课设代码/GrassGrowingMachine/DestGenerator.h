#pragma once
#include "SymbolList.h"
#include "GrammarAnalyser.h"

//����Ԫʽ�б�����Ŀ����룬 SeqListΪ��Ԫʽ�б� SeqLineΪ��Ԫʽ����Ŀ, targetΪ������ļ����ɹ�����1�� ʧ�ܷ���0
int DestGenerator(SEQUENCE *SeqList, int SeqLine, FILE *target);
//���ɵ�����Ԫʽ��Ŀ����룬�ɹ�����1�� ʧ�ܷ���0
int SingleDestGenerate(SEQUENCE Seq);

char buff[MAX_STRLEN]; //������

int param_num; //���ڼ�¼�Ѿ����εĸ�������PARAM�����������CALL�������

//�Ӵ�����

//�������ֲ���������, SeqΪ��Ӧ��Ԫʽ, �ɹ�����1��ʧ�ܷ���0
int sub_VT(SEQUENCE Seq);
//����ӷ�, SeqΪ��Ӧ��Ԫʽ, �ɹ�����1��ʧ�ܷ���0
int sub_ADD(SEQUENCE Seq);
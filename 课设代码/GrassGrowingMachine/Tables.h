#pragma once
#include <string.h>
#define MAXTABLELEN 1024
#define MAXSTRLEN 256

int iTline;	//��ǰһ��ռ�õ�������Ҳ����˵��Ч���ݵķ�ΧΪiTable[0~iTline-1]
char iTable[MAXTABLELEN][MAXSTRLEN]; //��ʶ����

int cTline;	//��ǰһ��ռ�õ�������Ҳ����˵��Ч���ݵķ�ΧΪcTable[0~cTline-1]
char cTable[MAXTABLELEN]; //�ַ��ͳ�����

int sTline; //��ǰһ��ռ�õ�������Ҳ����˵��Ч���ݵķ�ΧΪsTable[0~sTline-1]
char sTable[MAXTABLELEN][MAXSTRLEN]; //�ַ���������

int dcTline; //��ǰһ��ռ�õ�������Ҳ����˵��Ч���ݵķ�ΧΪdcTable[0~dcTline-1]
int dcTable[MAXTABLELEN]; //����������

int fcTline; //��ǰһ��ռ�õ�������Ҳ����˵��Ч���ݵķ�ΧΪfcTable[0~fcTline-1]
double fcTable[MAXTABLELEN]; //���㳣����

//��ʼ�����Լ���Ч����
int init_tables();

//���ʶ��������������++iTline���ɹ�ʱ����1��ʧ�ܷ���0
int new_iTableItem(char* name);
//���ַ�������������++cTline���ɹ�ʱ����1��ʧ�ܷ���0
int new_cTableItem(char ch);
//���ַ���������������++sTline���ɹ�ʱ����1��ʧ�ܷ���0
int new_sTableItem(char* str);
//����������������������++dcTline���ɹ�ʱ����1��ʧ�ܷ���0
int new_dcTableItem(int dcn);
//�򸡵㳣��������������++fcTline���ɹ�ʱ����1��ʧ�ܷ���0
int new_fcTableItem(double fcn);
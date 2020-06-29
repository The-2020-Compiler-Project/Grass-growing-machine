#pragma once
#include "Tables.h"
#include "SymbolList.h"
#include "LexicalAnalyser.h"

//�﷨�����������������Ԫʽ
int GrammarAnalyse();

//���ܴ���Ų��˳�����
int SendError(int err_id);

//�ݹ��½��ӳ��򣨾��ԡ�g����ͷ��

//����
TOKEN gProgram(TOKEN preTOKEN);
//����ͷ����
TOKEN gHeadOfProgram(TOKEN preTOKEN);
//��ʶ��
TOKEN gIdentifier(TOKEN preTOKEN);
//ȫ�ֱ���������
TOKEN gDeclVars(TOKEN preTOKEN);
//��������
TOKEN gVarDef(TOKEN preTOKEN);
//��������
TOKEN gDataType(TOKEN preTOKEN);
//����������
TOKEN gDeclFuncs(TOKEN preTOKEN);
//��������
TOKEN gFuncDef(TOKEN preTOKEN);
//����������
TOKEN gDeclArgs(TOKEN preTOKEN);
//��ֵ�β�������
TOKEN gDeclVfs(TOKEN preTOKEN);
//�����β�������
TOKEN gDeclVns(TOKEN preTOKEN);
//������������
TOKEN gFuncVarDef(TOKEN preTOKEN);
//������
TOKEN gFuncBody(TOKEN preTOKEN);
//�������
TOKEN gFuncCode(TOKEN preTOKEN);
//�������
TOKEN gCodeReturn(TOKEN preTOKEN);
//������
TOKEN gProgramBody(TOKEN preTOKEN);
//�������
TOKEN gCompCode(TOKEN preTOKEN);
//��ֵ���
TOKEN gCodeAssign(TOKEN preTOKEN);
//��ӡ���
TOKEN gCodePrint(TOKEN preTOKEN);
//�������
TOKEN gCodeEnd(TOKEN preTOKEN);
//�������
TOKEN gCodeIF(TOKEN preTOKEN);
//ѭ�����
TOKEN gCodeWhile(TOKEN preTOKEN);
//�����������
TOKEN gCodeCall(TOKEN preTOKEN);
//���ʽ
TOKEN gExpr(TOKEN preTOKEN);
//�߼���׺ʽ
TOKEN gLogicSuffix(TOKEN preTOKEN);
//��ϵ���ʽ
TOKEN gRelationExpr(TOKEN preTOKEN);
//��ϵ��׺ʽ
TOKEN gRelationSuffix(TOKEN preTOKEN);
//�������ʽ
TOKEN gMathExpr(TOKEN preTOKEN);
//�ӷ���׺ʽ
TOKEN gAddSuffix(TOKEN preTOKEN);
//�˷����ʽ
TOKEN gMulExpr(TOKEN preTOKEN);
//�˷���׺ʽ
TOKEN gMulSuffix(TOKEN preTOKEN);
//��
TOKEN gTerm(TOKEN preTOKEN);
#pragma once
#include "SymbolList.h"

typedef struct BLOCK_INFO
{
	int order;
	int begin_pos;
	int end_pos;
	int kind;  //0:usual 1:if 2:while
	struct BLOCK_INFO* next;
}BLOCK_INFO;

typedef enum OPR_Graph	//DAGͼ�е������
{
	_NONE = 0, _ADD, _SUB, _MUL, _DIV, _MOD, _RET,
	_GT, _GE, _LT, _LE, _EQ, _NE, _AND, _OR, _PUTC
}OPR_Graph;

typedef struct Node  //DAGͼ�������ݽṹ
{
	OPR_Graph op;
	struct Node* first_operand;
	struct Node* second_operand;
}Node;

typedef struct NodeSet  //DAGͼ��㼯
{
	int order;
	Node* node;
	int flag;   //�����ýڵ��Ƿ�����
	struct NodeSet* next;
}NodeSet;

typedef struct ID_list_in_block  //��ű�ʶ������Ķ�Ӧ��Ϣ
{
	int order;
	char* identiter;
	int temp;
	struct ID_list_in_block* next;
}ID_list_in_block;

int optimize();
int divide(BLOCK_INFO* p);
int optimize_block(BLOCK_INFO* p);
int is_block_start(int pos);
int fill_Sequence_outside_block(BLOCK_INFO* p, int pos);
int search_block_end(int pos);
int set_active_info(BLOCK_INFO* p);
int create_DAG_graph(BLOCK_INFO* info, NodeSet* node_set, ID_list_in_block* id_set);
int rewrite_Sequence(NodeSet* set);
int in_ID_list_in_block(char* str);
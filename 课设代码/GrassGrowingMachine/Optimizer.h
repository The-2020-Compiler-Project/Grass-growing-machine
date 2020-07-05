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



typedef struct Node  //DAGͼ�������ݽṹ
{
	int order;
	OPR op;
	struct Node* first_operand;
	struct Node* second_operand;
	int flag;   //�����ýڵ��Ƿ�����
	int arg_num;
	struct Node** arg_list;
	struct Node* next;
}Node;

typedef struct info_list  //��ű�ʶ������Ķ�Ӧ��Ϣ
{
	int order; //��� /
	SEQARGTYPE type; //����
	union content content; //����
	Node* node; //��Ӧ�Ľ���ָ�� //ͷ��� ��ָ������ĺ������ý��
	struct info_list* next;
}info_list;

int optimize();
int divide(BLOCK_INFO* p);
int optimize_block(BLOCK_INFO* p);
int is_block_start(int pos);
int fill_Sequence_outside_block(BLOCK_INFO* p);
int search_block_end(int pos);
int set_active_info(int start, int end);
int create_DAG_graph(BLOCK_INFO* info, Node* node_set, info_list* list);
int rewrite_Sequence(Node* node_set, info_list* list);
int in_info_list(SEQARG arg);
info_list* arg_into_info_list(int pos, info_list* last_info);
info_list* get_arg_info(SEQARG arg, info_list* list);
Node* create_new_node(Node* last_node);
Node* search_node_in_set(Node* first, Node* second, int pos);
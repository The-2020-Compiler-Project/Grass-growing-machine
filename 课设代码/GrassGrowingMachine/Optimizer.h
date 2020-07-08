#pragma once
#include "SymbolList.h"

typedef struct Block_Info
{
	int begin_pos;		// ���������
	int end_pos;		// ���������
	struct Block_Info* next;
}Block_Info, Block_Info_List;

typedef struct Arg_Node
{
	SEQARG arg;
	struct Arg_Node* next;
} Arg_Node, Arg_List;


typedef struct Node  //DAGͼ�������ݽṹ ˫������
{
	OPR op;		// �����
	struct Node* first_operand;     // ��һ���������ڽ��
	struct Node* second_operand;	// �ڶ����������ڽ��
	int flag;   // �����ýڵ��Ƿ�����
	Arg_List* seqID_list;			// �û������ʶ����
	Arg_List* seqMIDVAR_list;		// �м������
	Arg_List* seqConstant_list;		// ������(���ֻ��һ������)
	struct Node* next;  // ��һ���
	struct Node* prev;	// ǰһ���
}Node, Node_Set;

typedef struct Arg_Info  // ��Ų���������Ķ�Ӧ��Ϣ ��������
{
	SEQARG* arg;			// ������
	Node* node;				// ��Ӧ���
	struct Arg_Info* next;
}Arg_Info, Arg_Info_List;

typedef struct Active_Info  //SYMBL
{
	SEQARG* arg;
	bool active;
	struct Active_Info* next;
}Active_Info, Active_Info_List;


int optimize();
int is_entrance(int pos);
int is_exit(int pos);
int create_DAG_graph(Node_Set* node_set, Arg_Info_List* arg_info_list, Block_Info_List* block_info);
int reorganize_DAG_graph(Node_Set* node_set, Arg_Info_List* arg_info_list);
int get_SeqList_from_DAG_graph(Node_Set* node_set, Arg_Info_List* arg_info_list);
int set_active_info(int start);
int free_list(void* p, int list_kind);
Arg_Info* get_arg_info_from_list(SEQARG* arg, Arg_Info_List* arg_info_list);
Node* add_new_node_into_set(Node* last_node);
int add_seqValue_into_node(Node* node, Arg_Info* arg_info);
Node* search_parent_node(Arg_Info* arg1_info, Arg_Info* arg2_info, OPR op, Node_Set* node_set);
int set_flag(Node_Set* last_node, Arg_Info_List* arg_info_list);
SEQARG get_value_of_node(Node* node);
int set_operand_flag_1(Node* node);
Active_Info* get_active_info_from_list(SEQARG* arg, Active_Info_List* active_info_list);
Active_Info* add_arg_into_active_info_list(SEQARG* arg, Active_Info* last_info);
int revise_active_info(SEQARG* arg, bool active, Active_Info_List* active_info_list);
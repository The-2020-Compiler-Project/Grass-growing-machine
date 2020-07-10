#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include "Optimizer.h"

//int FLAG;

int optimize() //ed
{
	//// ���ֻ����� �õ���������Ϣ��
	OptSeqLine = 0;
	// ������������Ϣ��
	Block_Info_List* block_info_list = (Block_Info_List*)malloc(sizeof(Block_Info_List));
	// ��ʼ��������Ϣ��
	block_info_list->begin_pos = -1;
	block_info_list->end_pos = -1;
	block_info_list->next = NULL;
	// ���ֻ�����
	Block_Info_List* p = block_info_list;
	for (int i = 0; i < SeqLine; i++)
	{
		// �ж���Ԫʽ�Ƿ�Ϊ���������
		if (is_entrance(i))
		{
			p->next = (Block_Info_List*)malloc(sizeof(Block_Info_List));
			p = p->next;
			p->begin_pos = i;
			// Ѱ�һ��������
			while (!is_exit(i))
				i++;
			p->end_pos = i;
			p->next = NULL;
		}
	}
	////��Ը�����������Ż�
	p = block_info_list->next;
	Block_Info_List* q = block_info_list;
	while (p)
	{
		FLAG = 0;
		// ��������������Ԫʽcopy
		for (int i = q->end_pos + 1; i < p->begin_pos; i++)
		{
			OptimizedSeqList[OptSeqLine] = SequenceList[i];
			OptSeqLine++;
		}
		int start = OptSeqLine;
		// ��������ʼ����㼯Node_Set
		Node_Set* node_set = (Node_Set*)malloc(sizeof(Node_Set));
		node_set->op = 0;
		node_set->first_operand = NULL;
		node_set->second_operand = NULL;
		node_set->flag = 0;
		node_set->seqID_list = NULL;
		node_set->seqMIDVAR_list = NULL;
		node_set->seqConstant_list = NULL;
		node_set->next = NULL;
		node_set->prev = NULL;
		// ��������ʼ��������Ϣ��Arg_Info_List
		Arg_Info_List* arg_info_list = (Arg_Info_List*)malloc(sizeof(Arg_Info_List));
		arg_info_list->arg.type = seqNONE;
		arg_info_list->node = NULL;
		arg_info_list->next = NULL;
		// ����DAGͼ
		create_DAG_graph(node_set, arg_info_list, p);
		// ����DAGͼ
		reorganize_DAG_graph(node_set, arg_info_list);
		// ������Ԫʽ
		get_SeqList_from_DAG_graph(node_set, arg_info_list);
		// ��д��Ծ��Ϣ
		set_active_info(start, arg_info_list);
		// �ͷŽ�㼯��������Ϣ��
		free_list(node_set, 2);
		free_list(arg_info_list, 3);
		q = p;
		p = p->next;
	}
	// �����һ������������Ԫʽcopy
	for (int i = q->end_pos + 1; i < SeqLine; i++)
	{
		OptimizedSeqList[OptSeqLine] = SequenceList[i];
		OptSeqLine++;
	}
	//// �ͷŻ�������Ϣ��
	free_list(block_info_list, 1);
	return 0;
}

int is_entrance(int pos)  //ed
{
	//// ���ǻ�������� ����0; ���򷵻�1
	if ((SequenceList[pos - 1].op == FUNC ||
		SequenceList[pos - 1].op == VF ||
		SequenceList[pos - 1].op == VN ||
		SequenceList[pos - 1].op == VT ||
		SequenceList[pos - 1].op == CALL ||
		SequenceList[pos - 1].op == IF ||
		SequenceList[pos - 1].op == EL ||
		SequenceList[pos - 1].op == IE ||
		SequenceList[pos - 1].op == WH ||
		SequenceList[pos - 1].op == DO ||
		SequenceList[pos - 1].op == WE ||
		SequenceList[pos - 1].op == ENT ||
		SequenceList[pos].op == DO) &&
		SequenceList[pos].op != VF &&
		SequenceList[pos].op != VN &&
		SequenceList[pos].op != VT &&
		SequenceList[pos].op != CALL &&
		SequenceList[pos].op != EL &&
		SequenceList[pos].op != IE &&
		SequenceList[pos].op != WH &&
		SequenceList[pos].op != WE &&
		SequenceList[pos].op != EXIT &&
		SequenceList[pos].op != EF)
		return 1;
	else
		return 0;
}

int is_exit(int pos)	//ed
{
	//// �ǻ�������� ����1; ���򷵻�0;
	if ((SequenceList[pos + 1].op == EXIT ||
		SequenceList[pos + 1].op == EF ||
		SequenceList[pos + 1].op == EL ||
		SequenceList[pos + 1].op == WE ||
		SequenceList[pos + 1].op == IE ||
		SequenceList[pos + 1].op == WH ||
		SequenceList[pos].op == IF ||
		SequenceList[pos].op == DO ||
		SequenceList[pos].op == CALL) &&
		SequenceList[pos].op != ENT &&
		SequenceList[pos].op != FUNC &&
		SequenceList[pos].op != EL &&
		SequenceList[pos].op != IE &&
		SequenceList[pos].op != WH &&
		SequenceList[pos].op != WE &&
		SequenceList[pos].op != VT &&
		SequenceList[pos].op != VF &&
		SequenceList[pos].op != VN &&
		SequenceList[pos].op != PARAM)
		return 1;
	else
		return 0;
}

int create_DAG_graph(Node_Set* node_set, Arg_Info_List* arg_info_list, Block_Info_List* block_info) //ed
{
	//// ����DAGͼ
	// ָ��p:���ڷ��ʵ�ǰ��㼯��βԪ��
	Node* p = node_set;
	// ����������Ԫʽ����������
	for (int i = block_info->begin_pos; i <= block_info->end_pos; i++)
	{
		// �ֲ�ͬ���������㲢���²�����Ϣ
		if (SequenceList[i].op == ASSI)
		{
			Arg_Info* arg1 = get_arg_info_from_list(SequenceList[i].arg1, arg_info_list);
			Arg_Info* target = get_arg_info_from_list(SequenceList[i].target, arg_info_list);
			int flag;
			if (arg1->node && is_seqID(arg1->arg) && !equ_arg(arg1->node->seqID_list->arg, arg1->arg))
				flag = 1;
			else
				flag = 0;
			if (flag)
			{
				p = add_new_node_into_set(p);
				p->flag = 1;
				add_seqValue_into_node(p, arg1);
			}
			if (!arg1->node)
			{
				p = add_new_node_into_set(p);
				add_seqValue_into_node(p, arg1);
				add_seqValue_into_node(p, target);
			}
			else
				add_seqValue_into_node(arg1->node, target);
			
		}
		else if (SequenceList[i].op == RET || SequenceList[i].op == PUTC ||
			SequenceList[i].op == IF || SequenceList[i].op == DO || SequenceList[i].op == PARAM)
		{
			Arg_Info* arg1 = get_arg_info_from_list(SequenceList[i].arg1, arg_info_list);
			if (!arg1->node)
			{
				p = add_new_node_into_set(p);
				add_seqValue_into_node(p, arg1);
			}
			p = add_new_node_into_set(p);
			p->op = SequenceList[i].op;
			p->flag = 1;
			p->first_operand = arg1->node;
			if (SequenceList[i].op == PARAM)
			{
				Node* q = p;
				FLAG++;
				///???
				int a = Find_SymblItemName(0, SequenceList[i].target.content.str);
				PFINFLITEM* t = (PFINFLITEM*)(SYMBL[a].addr);
				int num = t->FN;
				ARGLITEM* r = t->PARAM;
				a = 0;
				for (int i = 0; i < num; i++)
				{
					if (r[i].cat == vnCAT)
					{
						a = i + 1;
						break;
					}
				}
				///???
				if (FLAG >= a)
				{
					p = add_new_node_into_set(p);
					add_seqValue_into_node(p, arg1);
					q->second_operand = p;
				}
			}
		}
		else if (SequenceList[i].op == CALL)
		{
			Arg_Info* arg1 = get_arg_info_from_list(SequenceList[i].target, arg_info_list);
			arg_info_list->arg = SequenceList[i].arg1;
			p = add_new_node_into_set(p);
			p->op = CALL;
			p->flag = 1;
			add_seqValue_into_node(p, arg1);
		}
		else
		{
			//��Ԫʽ������Ϊ��Ԫ������
			Arg_Info* arg1 = get_arg_info_from_list(SequenceList[i].arg1, arg_info_list);
			Arg_Info* arg2 = get_arg_info_from_list(SequenceList[i].arg2, arg_info_list);
			Arg_Info* target = get_arg_info_from_list(SequenceList[i].target, arg_info_list);
			if (!arg1->node)
			{
				p = add_new_node_into_set(p);
				add_seqValue_into_node(p, arg1);
			}
			if (!arg2->node)
			{
				p = add_new_node_into_set(p);
				add_seqValue_into_node(p, arg2);
			}
			Node* parent = search_parent_node(arg1, arg2, SequenceList[i].op, node_set);
			if (!parent)
			{
				p = add_new_node_into_set(p);
				p->op = SequenceList[i].op;
				p->first_operand = arg1->node;
				p->second_operand = arg2->node;
				add_seqValue_into_node(p, target);
			}
			else
				add_seqValue_into_node(parent, target);
		}
	}
	return 0;
}

int reorganize_DAG_graph(Node_Set* node_set, Arg_Info_List* arg_info_list) //ing
{
	//// ����DAGͼ�е�ϸ�� 1�����㳣�����ʽ 2: �жϽ����Ч״��
	// 1�����㳣�����ʽ
	Node* p = node_set;
	while (p->next)
	{
		Node* p1 = p->next->first_operand;
		Node* p2 = p->next->second_operand;
		if (p1 && p2)
		{
			if (p1->seqConstant_list && p2->seqConstant_list)
			{
				if (!p->next->seqConstant_list)
				{
					p->next->seqConstant_list = (Arg_List*)malloc(sizeof(Arg_List));
					p->next->seqConstant_list->next = NULL;
					if (p1->seqConstant_list->arg.type == seqDC || p1->seqConstant_list->arg.type == seqCHAR)
					{
						p->next->seqConstant_list->arg.type = p1->seqConstant_list->arg.type;
						switch (p->next->op)
						{
						case ADD:p->next->seqConstant_list->arg.content.d = (p1->seqConstant_list->arg.content.d + p2->seqConstant_list->arg.content.d);
							p->next->seqConstant_list->arg.type = p1->seqConstant_list->arg.type; break;
						case SUB:p->next->seqConstant_list->arg.content.d = (p1->seqConstant_list->arg.content.d - p2->seqConstant_list->arg.content.d); 
							p->next->seqConstant_list->arg.type = p1->seqConstant_list->arg.type; break;
						case MUL:p->next->seqConstant_list->arg.content.d = (p1->seqConstant_list->arg.content.d * p2->seqConstant_list->arg.content.d); 
							p->next->seqConstant_list->arg.type = p1->seqConstant_list->arg.type; break;
						case DIV:p->next->seqConstant_list->arg.content.d = (p1->seqConstant_list->arg.content.d / p2->seqConstant_list->arg.content.d);
							p->next->seqConstant_list->arg.type = p1->seqConstant_list->arg.type; break;
						case MOD:p->next->seqConstant_list->arg.content.d = (p1->seqConstant_list->arg.content.d % p2->seqConstant_list->arg.content.d);
							p->next->seqConstant_list->arg.type = p1->seqConstant_list->arg.type; break;
						case GT: p->next->seqConstant_list->arg.content.d = (p1->seqConstant_list->arg.content.d > p2->seqConstant_list->arg.content.d);
							p->next->seqConstant_list->arg.type = seqDC; break;
						case GE: p->next->seqConstant_list->arg.content.d = (p1->seqConstant_list->arg.content.d >= p2->seqConstant_list->arg.content.d);
							p->next->seqConstant_list->arg.type = seqDC; break;
						case LT: p->next->seqConstant_list->arg.content.d = (p1->seqConstant_list->arg.content.d < p2->seqConstant_list->arg.content.d);
							p->next->seqConstant_list->arg.type = seqDC; break;
						case LE: p->next->seqConstant_list->arg.content.d = (p1->seqConstant_list->arg.content.d <= p2->seqConstant_list->arg.content.d);
							p->next->seqConstant_list->arg.type = seqDC; break;
						case EQ: p->next->seqConstant_list->arg.content.d = (p1->seqConstant_list->arg.content.d == p2->seqConstant_list->arg.content.d);
							p->next->seqConstant_list->arg.type = seqDC; break;
						case NE: p->next->seqConstant_list->arg.content.d = (p1->seqConstant_list->arg.content.d != p2->seqConstant_list->arg.content.d);
							p->next->seqConstant_list->arg.type = seqDC; break;
						case AND:p->next->seqConstant_list->arg.content.d = (p1->seqConstant_list->arg.content.d && p2->seqConstant_list->arg.content.d);
							p->next->seqConstant_list->arg.type = seqDC; break;
						case OR: p->next->seqConstant_list->arg.content.d = (p1->seqConstant_list->arg.content.d || p2->seqConstant_list->arg.content.d);
							p->next->seqConstant_list->arg.type = seqDC; break;
						}
					}
					if (p1->seqConstant_list->arg.type == seqFC)
					{
						p->next->seqConstant_list->arg.type = seqFC;
						switch (p->next->op)
						{
						case ADD:p->next->seqConstant_list->arg.content.f = (p1->seqConstant_list->arg.content.f + p2->seqConstant_list->arg.content.f);
							p->next->seqConstant_list->arg.type = seqFC; break;
						case SUB:p->next->seqConstant_list->arg.content.f = (p1->seqConstant_list->arg.content.f - p2->seqConstant_list->arg.content.f);
							p->next->seqConstant_list->arg.type = seqFC; break;
						case MUL:p->next->seqConstant_list->arg.content.f = (p1->seqConstant_list->arg.content.f * p2->seqConstant_list->arg.content.f);
							p->next->seqConstant_list->arg.type = seqFC; break;
						case DIV:p->next->seqConstant_list->arg.content.f = (p1->seqConstant_list->arg.content.f / p2->seqConstant_list->arg.content.f);
							p->next->seqConstant_list->arg.type = seqFC; break;
						case GT: p->next->seqConstant_list->arg.content.d = (p1->seqConstant_list->arg.content.f > p2->seqConstant_list->arg.content.f);
							p->next->seqConstant_list->arg.type = seqDC; break;
						case GE: p->next->seqConstant_list->arg.content.d = (p1->seqConstant_list->arg.content.f >= p2->seqConstant_list->arg.content.f);
							p->next->seqConstant_list->arg.type = seqDC; break;
						case LT: p->next->seqConstant_list->arg.content.d = (p1->seqConstant_list->arg.content.f < p2->seqConstant_list->arg.content.f);
							p->next->seqConstant_list->arg.type = seqDC; break;
						case LE: p->next->seqConstant_list->arg.content.d = (p1->seqConstant_list->arg.content.f <= p2->seqConstant_list->arg.content.f);
							p->next->seqConstant_list->arg.type = seqDC; break;
						case EQ: p->next->seqConstant_list->arg.content.d = (p1->seqConstant_list->arg.content.f == p2->seqConstant_list->arg.content.f);
							p->next->seqConstant_list->arg.type = seqDC; break;
						case NE: p->next->seqConstant_list->arg.content.d = (p1->seqConstant_list->arg.content.f != p2->seqConstant_list->arg.content.f);
							p->next->seqConstant_list->arg.type = seqDC; break;
						case AND:p->next->seqConstant_list->arg.content.d = (p1->seqConstant_list->arg.content.f && p2->seqConstant_list->arg.content.f);
							p->next->seqConstant_list->arg.type = seqDC; break;
						case OR: p->next->seqConstant_list->arg.content.d = (p1->seqConstant_list->arg.content.f || p2->seqConstant_list->arg.content.f);
							p->next->seqConstant_list->arg.type = seqDC; break;
						}
					}
				}
			}
		}
		p = p->next;
	}
	// ��ʱָ��pָ���㼯 βԪ��

	// ����ʶ���Ľ�����Ч
	Node* q = node_set->next;
	while (q)
	{
		if (q->seqID_list)
		{
			q->flag = 1;
		}
		q = q->next;
	}
	// 2: �жϽ����Ч״��
	set_flag(p, arg_info_list);
	return 0;
}

int get_SeqList_from_DAG_graph(Node_Set* node_set, Arg_Info_List* arg_info_list)//ed
{
	//// ������Ԫʽ
	Node* p = node_set->next;
	while (p)
	{
		if (p->flag)
		{
			if (p->op == 0)
			{
				if (p->seqID_list)
				{
					if (p->seqConstant_list)
					{
						Arg_List* q = p->seqID_list;
						while (q)
						{
							Arg_Info* arg = get_arg_info_from_list(q->arg, arg_info_list);
							if (arg->node == p)
							{
								OptimizedSeqList[OptSeqLine].op = ASSI;
								OptimizedSeqList[OptSeqLine].arg1 = p->seqConstant_list->arg;
								OptimizedSeqList[OptSeqLine].arg2.type = seqNONE;
								OptimizedSeqList[OptSeqLine].target = q->arg;
								OptSeqLine++;
							}
							q = q->next;
						}
					}
					else if (p->seqMIDVAR_list)
					{
						Arg_List* q = p->seqID_list;
						while (q)
						{
							OptimizedSeqList[OptSeqLine].op = ASSI;
							OptimizedSeqList[OptSeqLine].arg1 =  p->seqMIDVAR_list->arg;
							OptimizedSeqList[OptSeqLine].arg2.type = seqNONE;
							OptimizedSeqList[OptSeqLine].target = p->seqID_list->arg;
							OptSeqLine++;
							q = q->next;
						}
					}
					else if (p->seqID_list->next)
					{
						Arg_List* q = p->seqID_list->next;
						while (q)
						{
							OptimizedSeqList[OptSeqLine].op = ASSI;
							OptimizedSeqList[OptSeqLine].arg1 = p->seqID_list->arg;
							OptimizedSeqList[OptSeqLine].arg2.type = seqNONE;
							OptimizedSeqList[OptSeqLine].target = q->arg;
							OptSeqLine++;
							q = q->next;
						}
					}
				}
			}
			else if (p->op == PUTC || p->op == RET || p->op == IF || p->op == DO || (p->op == PARAM && !p->second_operand))
			{
				if (p->first_operand->seqConstant_list)
				{
					OptimizedSeqList[OptSeqLine].op = p->op;
					OptimizedSeqList[OptSeqLine].arg1 = p->first_operand->seqConstant_list->arg;
					OptimizedSeqList[OptSeqLine].arg2.type = seqNONE;
					OptimizedSeqList[OptSeqLine].target.type = seqNONE;
					OptSeqLine++;
				}
				else if (p->first_operand->seqID_list)
				{
					OptimizedSeqList[OptSeqLine].op = p->op;
					OptimizedSeqList[OptSeqLine].arg1 = p->first_operand->seqID_list->arg;
					OptimizedSeqList[OptSeqLine].arg2.type = seqNONE;
					OptimizedSeqList[OptSeqLine].target.type = seqNONE;
					OptSeqLine++;
				}
				else if (p->first_operand->seqMIDVAR_list)
				{
					OptimizedSeqList[OptSeqLine].op = p->op;
					OptimizedSeqList[OptSeqLine].arg1 = p->first_operand->seqMIDVAR_list->arg;
					OptimizedSeqList[OptSeqLine].arg2.type = seqNONE;
					OptimizedSeqList[OptSeqLine].target.type = seqNONE;
					OptSeqLine++;
				}
				if (p->op == PARAM)
				{
					OptSeqLine--;
					OptimizedSeqList[OptSeqLine].target = arg_info_list->arg;
					OptSeqLine++;
				}
			} //�����βθ�ֵ
			else if (p->op == PARAM && p->second_operand)
			{
				if (p->second_operand->seqID_list)
				{
					OptimizedSeqList[OptSeqLine].op = p->op;
					OptimizedSeqList[OptSeqLine].arg1 = p->second_operand->seqID_list->arg;
					OptimizedSeqList[OptSeqLine].arg2.type = seqNONE;
					OptimizedSeqList[OptSeqLine].target = arg_info_list->arg;
					OptSeqLine++;
				}
			}
			else if (p->op == CALL)
			{
				OptimizedSeqList[OptSeqLine].op = p->op;
				OptimizedSeqList[OptSeqLine].arg1 = arg_info_list->arg;
				OptimizedSeqList[OptSeqLine].arg2.type = seqNONE;
				if (p->seqID_list)
					OptimizedSeqList[OptSeqLine].target = p->seqID_list->arg;
				else if (p->seqMIDVAR_list)
					OptimizedSeqList[OptSeqLine].target = p->seqMIDVAR_list->arg;
				OptSeqLine++;
			}
			else
			{
				// ��Ԫ�����
				if (p->seqConstant_list)
				{
					Arg_List* q = p->seqID_list;
					while (q)
					{
						OptimizedSeqList[OptSeqLine].op = ASSI;
						OptimizedSeqList[OptSeqLine].arg1 = p->seqConstant_list->arg;
						OptimizedSeqList[OptSeqLine].arg2.type = seqNONE;
						OptimizedSeqList[OptSeqLine].target = q->arg;
						OptSeqLine++;
						q = q->next;
					}
				}
				else
				{
					Arg_List* q = p->seqID_list;
					if (q)
					{
						while (q)
						{
							OptimizedSeqList[OptSeqLine].op = p->op;
							OptimizedSeqList[OptSeqLine].arg1 = get_value_of_node(p->first_operand);
							OptimizedSeqList[OptSeqLine].arg2 = get_value_of_node(p->second_operand);
							OptimizedSeqList[OptSeqLine].target = q->arg;
							OptSeqLine++;
							q = q->next;
						}
					}
					else if (p->seqMIDVAR_list)
					{
						OptimizedSeqList[OptSeqLine].op = p->op;
						OptimizedSeqList[OptSeqLine].arg1 = get_value_of_node(p->first_operand);
						OptimizedSeqList[OptSeqLine].arg2 = get_value_of_node(p->second_operand);
						OptimizedSeqList[OptSeqLine].target = p->seqMIDVAR_list->arg;
						OptSeqLine++;
					}
				}
			}
		}
		p = p->next;
	}
}

int set_active_info(int start, Arg_Info_List* arg_info_list)  //ed
{
	//// ��д��Ծ��Ϣ
	// ��ʼ��
	Arg_Info* p = arg_info_list->next;
	while (p)
	{
		if (is_seqID(p->arg))
			p->arg.active = 1;
		else
			p->arg.active = 0;
		p = p->next;
	}
	for (int i = OptSeqLine; i >= start; i--)
	{
		if (OptimizedSeqList[i].op == ASSI)
		{
			OptimizedSeqList[i].arg1.active = get_active_info_from_list(OptimizedSeqList[i].arg1, arg_info_list);
			revise_active_info_list(OptimizedSeqList[i].arg1, arg_info_list, 1);
			OptimizedSeqList[i].target.active = get_active_info_from_list(OptimizedSeqList[i].target, arg_info_list);
			revise_active_info_list(OptimizedSeqList[i].target, arg_info_list, 0);
		}
		else if (OptimizedSeqList[i].op == PUTC || OptimizedSeqList[i].op == RET ||
			OptimizedSeqList[i].op == IF || OptimizedSeqList[i].op == DO || OptimizedSeqList[i].op == PARAM)
		{
			OptimizedSeqList[i].arg1.active = get_active_info_from_list(OptimizedSeqList[i].arg1, arg_info_list);
			revise_active_info_list(OptimizedSeqList[i].arg1, arg_info_list, 1);
			if (OptimizedSeqList[i].op == PARAM)
				OptimizedSeqList[i].target.active = 1;
		}
		else if (OptimizedSeqList[i].op == CALL)
		{
			OptimizedSeqList[i].arg1.active = 0;
			OptimizedSeqList[i].target.active = 1;
		}
		else
		{
			OptimizedSeqList[i].arg1.active = get_active_info_from_list(OptimizedSeqList[i].arg1, arg_info_list);
			revise_active_info_list(OptimizedSeqList[i].arg1, arg_info_list, 1);
			OptimizedSeqList[i].arg2.active = get_active_info_from_list(OptimizedSeqList[i].arg2, arg_info_list);
			revise_active_info_list(OptimizedSeqList[i].arg2, arg_info_list, 1);
			OptimizedSeqList[i].target.active = get_active_info_from_list(OptimizedSeqList[i].target, arg_info_list);
			revise_active_info_list(OptimizedSeqList[i].target, arg_info_list, 0);
		}
	}
	return 0;
}

int free_list(void* p, int list_kind)
{
	//// �ͷŻ�������Ϣ����㼯��������Ϣ��
	void* q = p;
	while (p)
	{
		q = p;
		if (list_kind == 1)
		{
			(Block_Info*)p = ((Block_Info*)p)->next;
			free((Block_Info*)q);
		}
		if (list_kind == 2)
		{
			(Node*)p = ((Node*)p)->next;
			free((Node*)q);
		}
		if (list_kind == 3)
		{
			(Arg_Info*)p = ((Arg_Info*)p)->next;
			free((Arg_Info*)q);
		}

	}
	return 0;
}

Arg_Info* get_arg_info_from_list(SEQARG arg, Arg_Info_List* arg_info_list)
{
	//// �Ӳ�����Ϣ���л�ȡ������Ӧ����Ϣ ���û�иò��� ���ڲ�����Ϣ��β���Ӹò��� ������д��������Ϣ
	Arg_Info* p = arg_info_list;
	while (p->next)
	{
		if (equ_arg(p->next->arg, arg))
		{
			return p->next;
		}
		p = p->next;
	}
	p->next = (Arg_Info*)malloc(sizeof(Arg_Info));
	p->next->arg = arg;
	p->next->node = NULL;
	p->next->next = NULL;
	return p->next;
}

Node* add_new_node_into_set(Node* last_node)
{
	//// ����һ���½�㵽��㼯��
	Node* p = last_node;
	last_node->next = (Node*)malloc(sizeof(Node));
	last_node = last_node->next;
	last_node->first_operand = NULL;
	last_node->second_operand = NULL;
	last_node->flag = 0;
	last_node->op = 0;
	last_node->seqConstant_list = NULL;
	last_node->seqID_list = NULL;
	last_node->seqMIDVAR_list = NULL;
	last_node->next = NULL;
	last_node->prev = p;
	return last_node;
}

int add_seqValue_into_node(Node* node, Arg_Info* arg_info)
{
	//// �ڽ��node�����Ӳ���arg��Ӧ���� ͬʱ�޸Ĳ�����Ϣ
		// ���ӳ�������� ͬʱ�޸Ĳ�����Ϣ
	if (arg_info->arg.type == seqDC || arg_info->arg.type == seqFC || arg_info->arg.type == seqCHAR)
	{
		if (!node->seqConstant_list)
		{
			node->seqConstant_list = (Arg_List*)malloc(sizeof(Arg_List));
			node->seqConstant_list->arg = arg_info->arg;
			node->seqConstant_list->next = NULL;
		}
	}	// �����м��������� ͬʱ�޸Ĳ�����Ϣ
	else if (is_seqMIDVAR(arg_info->arg))
	{
		if (!node->seqMIDVAR_list)
		{
			node->seqMIDVAR_list = (Arg_List*)malloc(sizeof(Arg_List));
			node->seqMIDVAR_list->arg = arg_info->arg;
			node->seqMIDVAR_list->next = NULL;
		}
		else
		{
			Arg_List* p = node->seqMIDVAR_list;
			while (p->next)
			{
				p = p->next;
			}
			p->next = (Arg_List*)malloc(sizeof(Arg_List));
			p->next->arg = arg_info->arg;
			p->next->next = NULL;
		}
	}	// ���ӱ�ʶ������� ͬʱɾ����������еĸñ�ʶ�� ͬʱ�޸Ĳ�����Ϣ
	else if (is_seqID(arg_info->arg))
	{
		/*
		if (arg_info->node)
		{
			Arg_List* p = arg_info->node->seqID_list;
			Arg_List* q = p;
			if (!strcmp(p->arg.content.str, (arg_info->arg).content.str))
			{
				arg_info->node->seqID_list = p->next;
				free(p);
				p = NULL;
			}
			else
			{
				while (p)
				{
					if (!strcmp(p->arg.content.str, (arg_info->arg).content.str))
					{
						q->next = p->next;
						free(p);
						break;
					}
					q = p;
					p = p->next;
				}
			}
		}
		*/
		arg_info->node = node;
		Arg_List* p = node->seqID_list;
		if (!p)
		{
			node->seqID_list = (Arg_List*)malloc(sizeof(Arg_List));
			node->seqID_list->arg = arg_info->arg;
			node->seqID_list->next = NULL;
		}
		else
		{
			while (p->next)
			{
				p = p->next;
			}
			p->next = (Arg_List*)malloc(sizeof(Arg_List));
			p->next->arg = arg_info->arg;
			p->next->next = NULL;
		}
	}
	arg_info->node = node;
	return 0;
}

Node* search_parent_node(Arg_Info* arg1_info, Arg_Info* arg2_info, OPR op, Node_Set* node_set)
{
	//// Ѱ��arg1��arg2����op����Ľ�� �Ҳ����򷵻�NULL
	Node* p = node_set->next;
	while (p)
	{
		if (p->op == SUB || p->op == DIV || p->op == MOD || p->op == LT || p->op == LE ||
			p->op == GT || p->op == GE)
		{
			if (p->first_operand == arg1_info->node && p->second_operand == arg2_info->node && p->op == op)
				return p;
		}
		else
		{
			if ((p->first_operand == arg1_info->node && p->second_operand == arg2_info->node && p->op == op) ||
				(p->first_operand == arg2_info->node && p->second_operand == arg1_info->node && p->op == op))
				return p;
		}
		p = p->next;
	}
	return NULL;
}

int set_flag(Node_Set* last_node, Arg_Info_List* arg_info_list) //ed
{
	// �ӽ�㼯β��ʼ �Ӻ���ǰ ���� ��Ч�Ľ��Ĳ��������� ��Ч���
	Node* p = last_node;
	while (p->prev)
	{
		if (p->flag == 1)
		{
			set_operand_flag_1(p);
		}
		p = p->prev;
	}
}

SEQARG get_value_of_node(Node* node)
{
	//�����ȼ�(����>��ʶ��>�м����)����node��������һ��ֵ
	if (node->seqConstant_list)
	{
		return node->seqConstant_list->arg;
	}
	else if (node->seqID_list)
	{
		return node->seqID_list->arg;
	}
	else if (node->seqMIDVAR_list)
	{
		return node->seqMIDVAR_list->arg;
	}
}

int set_operand_flag_1(Node* node) //ed
{
	if (!node->seqConstant_list)
	{
		if (node->first_operand)
		{
			node->first_operand->flag = 1;
			set_operand_flag_1(node->first_operand);
		}
		if (node->second_operand)
		{
			node->second_operand->flag = 1;
			set_operand_flag_1(node->second_operand);
		}
	}
	return 0;
}

bool get_active_info_from_list(SEQARG arg, Arg_Info_List* arg_info_list)
{
	Arg_Info* p = arg_info_list->next;
	while (p)
	{
		if (equ_arg(p->arg, arg))
			return p->arg.active;
		p = p->next;
	}
	return 0;
}

int revise_active_info_list(SEQARG arg, Arg_Info_List* arg_info_list, bool active)
{
	Arg_Info* p = arg_info_list->next;
	while (p)
	{
		if (equ_arg(p->arg, arg))
		{
			p->arg.active = active;
			return 0;
		}
		p = p->next;
	}
	return 0;
}

int is_seqMIDVAR(SEQARG arg)
{
	if (arg.type == seqID && arg.content.str[0] == '_')
		return 1;
	else
		return 0;
}

int is_seqID(SEQARG arg)
{
	if (arg.type == seqID && arg.content.str[0] != '_')
		return 1;
	else
		return 0;
}

int equ_arg(SEQARG arg1, SEQARG arg2)
{
	if (arg1.type == arg2.type)
	{
		switch (arg1.type)
		{
		case seqNONE:return 0;
		case seqDC: if (arg1.content.d == arg2.content.d) return 1;
				  else return 0;
		case seqFC: if (arg1.content.f == arg2.content.f) return 1;
				  else return 0;
		case seqCHAR: if (arg1.content.ch == arg2.content.ch) return 1;
					else return 0;
		case seqID: if (!strcmp(arg1.content.str, arg2.content.str)) return 1;
				  else return 0;
		}
	}
	else
		return 0;
}

/*
int Find_SymblItemName(int startpos, char* name);
*/
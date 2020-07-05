#include "Optimizer.h"

int optimize()
{
	//建立一个存放各个块的信息表
	BLOCK_INFO* info_list = (BLOCK_INFO*)malloc(sizeof(BLOCK_INFO));  
	//初始化头结点
	info_list->next = NULL;						
	info_list->order = 0;
	info_list->end_pos = 0;
	//分割基本块
	divide(info_list);			
	BLOCK_INFO* p = info_list;
	while (p->next)
	{
		//将该基本块前的 不属于任何基本块的四元式 填入新的四元式表
		fill_Sequence_outside_block(p);
		//依次优化各个代码块
		optimize_block(p->next, OptimizedSeqList);    
		p = p->next;
	}
	//将剩下的四元式入表
	//释放 info_list
	return 0;
}

int divide(BLOCK_INFO* p)
{
	int i = 0;
	while (i < SeqLine)
	{
		//判断该四元式是否为一个基本块入口
		if (is_block_start(i))
		{
			p->next = (BLOCK_INFO*)malloc(sizeof(BLOCK_INFO));
			int tmp = p->order + 1;
			p = p->next;
			p->order = tmp;
			p->begin_pos = i;
			//寻找基本块出口
			p->end_pos = search_block_end(i);
			p->next = NULL;
			//判断基本块的类型
			if (SequenceList[i].op == IF)
				p->kind = 1;
			else if (SequenceList[i].op == WH)
				p->kind = 2;
			else
				p->kind = 0;
			i = p->end_pos;
		}
		i++;
	}
}

int optimize_block(BLOCK_INFO* p)
{
	int end = 0;
	if (p->kind == 2)
	{
		////针对while的优化
	}
	else
	{
		////通常优化
		//创建各个  变量:节点 字典   以及DAG图节点集
		info_list* list = (info_list*)malloc(sizeof(info_list));
		list->order = 0;
		list->type = seqNONE;
		list->node = NULL;
		list->next = NULL;
		Node* node_set = (Node*)malloc(sizeof(Node));
		node_set->order = 0;
		node_set->op = 0;
		node_set->first_operand = NULL;
		node_set->second_operand = NULL;
		node_set->flag = 0;
		node_set->arg_num = 0;
		node_set->arg_list = NULL;
		node_set->next = NULL;
		create_DAG_graph(p, node_set, list);
		//重写四元式
		end = rewrite_Sequence(node_set, list);
		//删表
	}
	//设置临时变量的活跃信息
	set_active_info(OptSeqLine + 1, end);
	OptSeqLine = end;
	return 0;
}

int is_block_start(int pos)
{
	/*
	if((SequenceList[pos - 1].op == FUNC || 
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
		SequenceList[pos - 1].op == ENT) &&
		SequenceList[pos].op != VF &&
		SequenceList[pos].op != VN &&
		SequenceList[pos].op != VT &&
		SequenceList[pos].op != CALL &&
		SequenceList[pos].op != IF &&
		SequenceList[pos].op != EL &&
		SequenceList[pos].op != IE &&
		SequenceList[pos].op != WH &&
		SequenceList[pos].op != DO &&
		SequenceList[pos].op != WE &&
		SequenceList[pos].op != EXIT &&
		SequenceList[pos].op != EF)
	*/
	if ((SequenceList[pos - 1].op == FUNC ||
		SequenceList[pos - 1].op == VF ||
		SequenceList[pos - 1].op == VN ||
		SequenceList[pos - 1].op == VT ||
		SequenceList[pos - 1].op == IF ||
		SequenceList[pos - 1].op == EL ||
		SequenceList[pos - 1].op == IE ||
		SequenceList[pos - 1].op == WH ||
		SequenceList[pos - 1].op == DO ||
		SequenceList[pos - 1].op == WE ||
		SequenceList[pos - 1].op == ENT) &&
		SequenceList[pos].op != VF &&
		SequenceList[pos].op != VN &&
		SequenceList[pos].op != VT &&
		SequenceList[pos].op != IF &&
		SequenceList[pos].op != EL &&
		SequenceList[pos].op != IE &&
		SequenceList[pos].op != WH &&
		SequenceList[pos].op != DO &&
		SequenceList[pos].op != WE &&
		SequenceList[pos].op != EXIT &&
		SequenceList[pos].op != EF)
	return 0;
}

int fill_Sequence_outside_block(BLOCK_INFO* p)
{
	//
}

int search_block_end(int pos)
{
	while (1)
	{
		/*
		if ((SequenceList[pos + 1].op == EXIT ||
			SequenceList[pos + 1].op == EF ||
			SequenceList[pos + 1].op == IF ||
			SequenceList[pos + 1].op == EL ||
			SequenceList[pos + 1].op == IE ||
			SequenceList[pos + 1].op == WE ||
			SequenceList[pos + 1].op == DO ||
			SequenceList[pos + 1].op == WE ||
			SequenceList[pos + 1].op == CALL) &&
			SequenceList[pos].op != ENT &&
			SequenceList[pos].op != FUNC &&
			SequenceList[pos].op != IF &&
			SequenceList[pos].op != EL &&
			SequenceList[pos].op != IE &&
			SequenceList[pos].op != WH &&
			SequenceList[pos].op != DO &&
			SequenceList[pos].op != WE &&
			SequenceList[pos].op != VT &&
			SequenceList[pos].op != VF &&
			SequenceList[pos].op != VN &&
			SequenceList[pos].op != CALL)
		*/
		if ((SequenceList[pos + 1].op == EXIT ||
			SequenceList[pos + 1].op == EF ||
			SequenceList[pos + 1].op == IF ||
			SequenceList[pos + 1].op == EL ||
			SequenceList[pos + 1].op == IE ||
			SequenceList[pos + 1].op == WE ||
			SequenceList[pos + 1].op == DO ||
			SequenceList[pos + 1].op == WE) &&
			SequenceList[pos].op != ENT &&
			SequenceList[pos].op != FUNC &&
			SequenceList[pos].op != IF &&
			SequenceList[pos].op != EL &&
			SequenceList[pos].op != IE &&
			SequenceList[pos].op != WH &&
			SequenceList[pos].op != DO &&
			SequenceList[pos].op != WE &&
			SequenceList[pos].op != VT &&
			SequenceList[pos].op != VF &&
			SequenceList[pos].op != VN )
			return pos;
		pos++;
	}
}

int set_active_info(int start, int end)
{

}

int create_DAG_graph(BLOCK_INFO* info, Node* node_set, info_list* list)
{
	//用于访问当前结点集的尾元素
	Node* p = node_set;
	//用于访问当前参数信息表的尾元素
	info_list* q = list;
	//用于访问当前要用到的四元式
	int i = info->begin_pos;
	while (i <= info->end_pos)
	{
		//将该四元式中未入表info_list的arg或target入表 但不填充结点信息
		q = arg_into_info_list(i, q);
		if (SequenceList[i].op == ASSI)
		{
			//获取表中参数的信息
			info_list* info_1 = get_arg_info(SequenceList[i].target, list);
			info_list* info_2 = get_arg_info(SequenceList[i].arg1, list);
			if (info_1->node)
				info_2->node = info_1->node;
			else
			{
				//创建新节点
				p = create_new_node(p);
				info_1->node = p;
				info_2->node = p;
			}
			
		}
		else if(SequenceList[i].op == RET || 
				SequenceList[i].op == PUTC)
		{
			info_list* info_1 = get_arg_info(SequenceList[i].arg1, list);
			if (!info_1->node)
			{
				p = create_new_node(p);
				info_1->node = p;
			}
			p = create_new_node(p);
			p->op = SequenceList[i].op;
			p->first_operand = info_1->node;
		}
		else if(SequenceList[i].op == PARAM)
		{
			info_list* info_1 = get_arg_info(SequenceList[i].target, list);
			info_list* info_2 = get_arg_info(SequenceList[i].arg1, list);
			if (!info_1->node)
			{
				p = create_new_node(p);
				info_1->node = p;
				p->op = 0;
			}
			if (!list->node)
			{
				p = create_new_node(p);
				list->node = p;
				p->op = CALL;
				p->first_operand = info_1->node;
			}
			Node* pt = list->node;
			pt->arg_num++;
			Node** temp = pt->arg_list;
			pt->arg_list = (Node**)malloc(sizeof(Node*) * pt->arg_num);
			for (int j = 0; j < pt->arg_num - 1; j++)
				pt->arg_list[j] = temp[j];
			pt->arg_list[pt->arg_num - 1] = info_2->node;
		}
		else if (SequenceList[i].op == CALL)
		{
			info_list* info_1 = get_arg_info(SequenceList[i].arg1, list);
			info_list* info_2 = get_arg_info(SequenceList[i].target, list);
			if (!info_1->node)
			{
				p = create_new_node(p);
				info_1->node = p;
				p->op = 0;
			}
			if (!list->node)
			{
				p = create_new_node(p);
				list->node = p;
				p->op = CALL;
				p->first_operand = info_1->node;
			}
			info_2->node = list->node;
			list->node = NULL;
		}
		else
		{
			info_list* info_1 = get_arg_info(SequenceList[i].arg1, list);
			info_list* info_2 = get_arg_info(SequenceList[i].arg2, list);
			info_list* info_3 = get_arg_info(SequenceList[i].target, list);
			if (!info_1->node)
			{
				p = create_new_node(p);
				info_1->node = p;
				p->op = 0;
			}
			if (!info_2->node)
			{
				p = create_new_node(p);
				info_1->node = p;
				p->op = 0;
			}
			info_3->node = search_node_in_set(info_1->node, info_2->node, i);
			if (!info_3->node)
			{
				p = create_new_node(p);
				info_1->node = p;
				p->op = SequenceList[i].op;
				p->first_operand = info_1->node;
				p->second_operand = info_2->node;
			}
		}
		i++;
	}
}

int rewrite_Sequence(Node* node_set, info_list* list)
{
	
	//返回新四元式表尾位置
	return 0;
}

int in_info_list(SEQARG arg)
{
	//没有则返回0
}

info_list* arg_into_info_list(int pos, info_list* last_info)
{
	int num_1 = in_info_list(SequenceList[pos].arg1);
	int num_2 = in_info_list(SequenceList[pos].arg2);
	int num_3 = in_info_list(SequenceList[pos].target);
	//返回当前参数信息表的尾元素地址
}

info_list* get_arg_info(SEQARG arg, info_list* list)
{
	//返回arg对应的表中信息

}

Node* create_new_node(Node* last_node)
{
	//返回当前结点集的尾元素的地址
	
	last_node->next = (Node*)malloc(sizeof(Node));
	Node* p = last_node->next;
	p->order = last_node->order + 1;
	p->op = 0;
	p->first_operand = NULL;
	p->second_operand = NULL;
	p->arg_num = 0;
	p->arg_list = NULL;
	p->flag = 0;
	p->next = NULL;
	return p;
}

Node* search_node_in_set(Node* first, Node* second, int pos)
{
	Node* p = second->next;
	while (p)
	{
		if (p->op == SequenceList[pos].op &&
			p->first_operand == first &&
			p->second_operand == second)
			return p;
		p = p->next;
	}
	return NULL;
}
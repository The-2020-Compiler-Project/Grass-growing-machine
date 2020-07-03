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
	BLOCK_INFO* p = info_list->next;
	while (p)
	{
		//依次优化各个代码块
		optimize_block(p, OptimizedSeqList);    
		p = p->next;
	}
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
			//将该基本块前的 不属于任何基本块的四元式 填入新的四元式表
			fill_Sequence_outside_block(p, i - 1);
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
	if (p->kind == 2)
	{
		////针对while的优化
	}
	else
	{
		////通常优化
		//创建DAG图
		ID_list_in_block* id_set = (ID_list_in_block*)malloc(sizeof(ID_list_in_block));
		id_set->order = 0;
		id_set->identiter = NULL;
		id_set->next = NULL;
		NodeSet* node_set = = (NodeSet*)malloc(sizeof(NodeSet));
		node_set->order = 0;
		node_set->node = NULL;
		node_set->flag = 0;
		node_set->next = NULL;
		create_DAG_graph(p, node_set, id_set);
		//重写四元式
		rewrite_Sequence(set);
		//删表
	}
	//设置临时变量的活跃信息
	set_active_info(q);
	return 0;
}

int is_block_start(int pos)
{
	if ((
		SequenceList[pos - 1].op == VT ||
		SequenceList[pos - 1].op == VN ||
		SequenceList[pos - 1].op == VF ||
		SequenceList[pos - 1].op == ENT) &&
		SequenceList[pos].op != VT &&
		SequenceList[pos].op != VN &&
		SequenceList[pos].op != VF &&
		SequenceList[pos].op != IE &&
		SequenceList[pos].op != WH)
		return 1;
	if ((
		SequenceList[pos - 1].op == IF ||
		SequenceList[pos - 1].op == EL ||
		SequenceList[pos - 1].op == IE ||
		SequenceList[pos - 1].op == WH ||
		SequenceList[pos - 1].op == WE )&&
		SequenceList[pos].op != IF &&
		SequenceList[pos].op != WH &&
		SequenceList[pos].op != DO &&
		SequenceList[pos].op != EL &&
		SequenceList[pos].op != IE &&
		SequenceList[pos].op != WE)
		return 1;
	//if,while的多重嵌套 再考虑 先这样
	return 0;
}

int fill_Sequence_outside_block(BLOCK_INFO* p, int pos)
{
	int i = p->end_pos + 1;
	while (i <= pos)
	{
		OptimizedSeqList[OptSeqLine] = SequenceList[i];
		i++;
		OptSeqLine++;
	}
	return;
}

int search_block_end(int pos)
{
	while (1)
	{
		//
		if (SequenceList[pos + 1].op == EXIT ||
			SequenceList[pos + 1].op == FUNC ||
			SequenceList[pos + 1].op == ENT ||
			SequenceList[pos + 1].op == IF ||
			SequenceList[pos + 1].op == EL ||
			SequenceList[pos + 1].op == IE ||
			SequenceList[pos + 1].op == WE ||
			SequenceList[pos + 1].op == DO ||
			SequenceList[pos + 1].op == WE)
			return pos;
		pos++;
	}
}

int set_active_info(BLOCK_INFO* p)
{

}

int create_DAG_graph(BLOCK_INFO* info, NodeSet* node_set, ID_list_in_block* id_set)
{
	NodeSet* p = node_set->next;
	ID_list_in_block* q = id_set->next;
	int i = info->begin_pos;
	while (i <= info->end_pos)
	{
		p = (NodeSet*)mallco(sizeof(NodeSet));
		p->node = (Node*)malloc(sizeof(Node));
		if (Sequence[i].op == ASSI)
		{
			///////
		}
		else 
		{
			switch (SequenceList[i].op)
			{
			case ADD: p->node->op = _ADD; break;
			case SUB: p->node->op = _SUB; break;
			case MUL: p->node->op = _MUL; break;
			case DIV: p->node->op = _DIV; break;
			case MOD: p->node->op = _MOD; break;
			case RET: p->node->op = _RET; break;
			case GT: p->node->op = _GT; break;
			case GE: p->node->op = _GE; break;
			case LT: p->node->op = _LT; break;
			case LE: p->node->op = _LE; break;
			case EQ: p->node->op = _EQ; break;
			case NE: p->node->op = _NE; break;
			case AND: p->node->op = _AND; break;
			case OR: p->node->op = _OR; break;
			case PUTC: p->node->op = _PUTC; break;
			}
			/////////
		}
	}
}

int rewrite_Sequence(NodeSet* set)
{

}

int in_ID_list_in_block(char* str)
{

}
#include "SymbolList.h"

int init_symbl()
{
	SeqLine = 0;
	memset(SequenceList, MAXSEQLISTSIZE * sizeof(SEQUENCE), 0);
	return 1;
}
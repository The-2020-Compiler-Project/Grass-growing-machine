#include "Tables.h"

int init_tables()
{
	iTline = 0;
	memset(iTable, MAXTABLELEN * MAXSTRLEN * sizeof(char), 0);

	cTline = 0;
	memset(cTable, MAXTABLELEN * sizeof(char), 0);

	sTline = 0;
	memset(sTable, MAXTABLELEN * MAXSTRLEN * sizeof(char), 0);

	dcTline = 0;
	memset(dcTable, MAXTABLELEN * sizeof(int), 0);

	fcTline = 0;
	memset(fcTable, MAXTABLELEN * sizeof(double), 0);

	return 1;
}

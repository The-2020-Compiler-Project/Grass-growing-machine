#include "Tables.h"

int init_tables()
{
	iTline = 0;
	memset(iTable, 0, MAX_TABLELEN * MAX_IDLEN * sizeof(char));

	cTline = 0;
	memset(cTable, 0, MAX_TABLELEN * sizeof(char));

	sTline = 0;
	memset(sTable, 0, MAX_TABLELEN * MAX_STRLEN * sizeof(char));

	dcTline = 0;
	memset(dcTable, 0, MAX_TABLELEN * sizeof(int));

	fcTline = 0;
	memset(fcTable, 0, MAX_TABLELEN * sizeof(double));

	return 1;
}

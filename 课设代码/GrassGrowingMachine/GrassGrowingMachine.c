#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "Tables.h"
#include "SymbolList.h"

int init()
{
    init_tables();
    hello("c");
    printf("initialized!\n");
    return 1;
}

int main()
{
    init();
    printf("Hello world!");
    return 0;
}


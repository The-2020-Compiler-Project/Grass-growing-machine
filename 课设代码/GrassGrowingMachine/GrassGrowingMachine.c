#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "Tables.h"
#include "SymbolList.h"
#include "LexicalAnalyser.h"
#include "GrammarAnalyser.h"
#include "DestGenerator.h"

const int DEBUG = 1;

int init()
{
    srcfile = NULL;
    init_tables();
    init_symbl();
    printf("Tables initialized!\n");
    return 1;
}

int main(char argc, char* argv[])
{
    FILE* dstfile = NULL; //输出的asm文件
    if (!init())
    {
        printf("初始化失败！");
        return -1;
    }
    if (DEBUG)
    {
        if (!(srcfile = fopen("Hello.ggml", "r")))
        {
            printf("Hello.ggml文件不存在！");
            return -1;
        }
        if (!(dstfile = fopen("D:/Hello.asm", "w")))
        {
            printf("目标文件错误！");
            return -1;
        }
    }
    else if (argc != 2 || !(srcfile = fopen(argv[1], "r")))
    {
        printf("未输入文件或者文件不存在！");
        return -1;
    }
    printf("文件读取成功!");
    //此处以后可以进行文件的读取操作测试等

    LexicalAnalyser(srcfile);
    tokenFinal[145].type = PTYPE;
    tokenFinal[145].id = 13;
    GrammarAnalyse();
    DestGenerator(SequenceList, SeqLine, dstfile);
    return 0;
 }


#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "Tables.h"
#include "SymbolList.h"
#include "LexicalAnalyser.h"
#include "GrammarAnalyser.h"
#include "DestGenerator.h"

const int DEBUG = 0;

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
    srcfile = NULL;
    FILE* dstfile = NULL; //输出的asm文件
    printf("%d\n", argc);
    for (int i = 0; i < argc; ++i)
    {
        printf("%s\n", argv[i]);
    }
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
        if (!(dstfile = fopen("D:/C++/Practise/ASM/masm/code/Hello.asm", "w")))
        {
            printf("目标文件错误！");
            return -1;
        }
    }
    else if (argc != 3 || !(srcfile = fopen(argv[1], "r")) || !(dstfile = fopen(argv[2], "w")))
    {
        printf("未输入文件或者文件不存在！");
        return -1;
    }

    printf("文件读取成功!");
    //此处以后可以进行文件的读取操作测试等

    GrammarAnalyse();
    DestGenerator(SequenceList, SeqLine, dstfile);
    return 0;
 }


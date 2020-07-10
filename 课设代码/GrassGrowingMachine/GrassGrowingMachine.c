#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "Tables.h"
#include "SymbolList.h"
#include "LexicalAnalyser.h"
#include "GrammarAnalyser.h"
#include "DestGenerator.h"
#include "Optimizer.h"

const int DEBUG = 0;

int init()
{
    srcfile = NULL;
    init_tables();
    init_symbl();
    //printf("Tables initialized!\n");
    return 1;
}

int main(char argc, char* argv[])
{
    srcfile = NULL;
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
        if (!(dstfile = fopen("D:/C++/Practise/ASM/masm/code/Hello.asm", "w")))
        {
            printf("目标文件错误！");
            return -1;
        }
    }
    else if (argc != 3 || !(srcfile = fopen(argv[1], "r")) || !(dstfile = fopen(argv[2], "w")))
    {
        printf("未输入文件或者文件不存在！\n");
        return -1;
    }

    printf("文件读取成功\n");
    //此处以后可以进行文件的读取操作测试等

    FILE* seqfile = fopen("D:/Hello_Sequence.txt", "w");
    if (!seqfile) return -1;
    FILE* symfile = fopen("D:/Hello_SYMBL.txt", "w");
    if (!symfile) return -1;
    FILE* optseqfile = fopen("D:/Hello_OptSequence.txt", "w");
    if (!optseqfile) return -1;

    //TOKEN newtoken;
    //while (1)
    //{
    //    newtoken = Next();
    //}

    GrammarAnalyse();
    optimize();
    printf("Optimized!\n");
    Output_SeqList(SequenceList, SeqLine, seqfile);
    Output_SeqList(OptimizedSeqList, OptSeqLine, optseqfile);
   
    Output_SYMBL(symfile);
    fclose(seqfile);
    fclose(symfile);
    fclose(optseqfile);
    DestGenerator(OptimizedSeqList, OptSeqLine, dstfile);
    printf("Dest file created!\n");
    //DestGenerator(SequenceList, SeqLine, dstfile);
    return 0;
 }


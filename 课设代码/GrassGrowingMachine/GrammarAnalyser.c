#include "GrammarAnalyser.h"

int GrammarAnalyse()
{
	TOKEN passTOKEN = Next();
	passTOKEN = gProgram(passTOKEN);
	return 1;
}

TOKEN gProgram(TOKEN preTOKEN)
{

}
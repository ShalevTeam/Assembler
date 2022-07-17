#ifndef _SERVICES_
#define _SERVICES_

#include "globalDefs.h"


int reallocAndCopyBuffer(void** outFileData, int currSize);
char* readLine(char* startPos,char* line);
EFuncResSucsessFail initDataBase();
EFuncResSucsessFail addData(unsigned short val);
EFuncResSucsessFail addEntryElemet(unsigned short address, char* tagName);
EFuncResSucsessFail addExternElemet(unsigned short address, char* tagName);
EFuncResSucsessFail addDataTagElemet(unsigned short address, char* tagName);
EFuncResSucsessFail addCodeElemet(SCodeinfo codeInfo);


#endif
#ifndef _SERVICES_
#define _SERVICES_

#include "globalDefs.h"


int reallocAndCopyBuffer(char** outFileData, int currSize);
char* readLine(char* startPos,char* line);
void initDataBase();
void addEntryElemet(unsigned short address, char* tagName);
void addExternElemet(unsigned short address, char* tagName);


#endif
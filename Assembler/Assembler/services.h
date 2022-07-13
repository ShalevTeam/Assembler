#ifndef _SERVICES_
#define _SERVICES_

#include "globalDefs.h"


int reallocAndCopyBuffer(char** outFileData, int currSize);
char* readLine(char* startPos,char* line);


#endif
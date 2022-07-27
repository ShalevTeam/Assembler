#ifndef _SERVICES_
#define _SERVICES_

#include "globalDefs.h"


int reallocAndCopyBuffer(void** outFileData, int currSize);
char* readLine(char* startPos,char* line);
eSucsessFail initDataBase();
eSucsessFail addData(unsigned short val);
eSucsessFail addEntryElemet(unsigned short address, char* tagName);
eSucsessFail addExternElemet(unsigned short address, char* tagName);
eSucsessFail addDataTagElemet(unsigned short address, char* tagName, int tagLength);
eSucsessFail addCodeElemet(SCodeinfo codeInfo);

ELineType getLineType(int lineNumber, char const* line, int* additionalInfo);
eSucsessFail handleTag(char* line ,ELineType lineType);
void setCurrentLineNumber(lineNumber);

#endif
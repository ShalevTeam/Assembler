#ifndef _SERVICES_
#define _SERVICES_

#include "globalDefs.h"

/* Getters*/
SEntryElement* getEntryList();
SExternElement* getExternalList();
STagParams* getTagList();
SCodeElement* getCodeList();
unsigned short* getDataArray();
int getCurrentLineNumber();

/* Seters*/
void setCurrentLineNumber(lineNumber);
eSucsessFail addEntryElemet(unsigned short address, char const* tagName);
eSucsessFail addDataTagElemet(char const* tagName, int tagLength, EtagType);
eSucsessFail addData(unsigned short val);
eSucsessFail addExternElemet(char const* tagName);
eSucsessFail addCodeElemet(SCodeinfo codeInfo);

/* Services*/
eSucsessFail initDataBase();

#endif
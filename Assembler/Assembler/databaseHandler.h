#ifndef _DATABASE_HANDLER_
#define _DATABASE_HANDLER_

/** @databaseHandler.h
 *
 * @brief Handles the program database of elements used by multiple modules
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2022 Yam Shalev.  All rights reserved.
 */

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
eSucsessFail istagExist(char const* tag, eSucsessFail* pIsExternalTag, short* pTagAddr);

#endif
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
ScodeWord* getDataArray();
int getCurrentLineNumber();

/* Seters*/
void setCurrentLineNumber(lineNumber);
ESucsessFail addEntryElemet(unsigned short address, char const* tagName);
ESucsessFail addDataTagElemet(char const* tagName, int tagLength, EtagType);
ESucsessFail addData(short val);
ESucsessFail addExternElemet(char const* tagName);
ESucsessFail addCodeElemet(SCodeinfo codeInfo);

/* Services*/
ESucsessFail initDataBase();
ESucsessFail istagExist(char const* tag, ESucsessFail* pIsExternalTag, short* pTagAddr);
int reallocAndCopyBuffer(void** outFileData, int currSize);

#endif
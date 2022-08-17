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
SMacroElement* getMacrosList();
SEntryElement* getEntryList();
SExternElement* getExternalList();
STagParams* getTagList();
SCodeElement* getCodeList();
ScodeWord* getDataArray();
int getCurrentLineNumber();
int getDataSize();
char* getObjectFileName();
char* getEntryFileName();
char* getExternFileName();

/* Seters*/
void setCurrentLineNumber(lineNumber);
void setAsemblerFileName(char const* fileName);
ESucsessFail addMacroElemet(char const* macroName, char const* macroString);
ESucsessFail addEntryElemet(char const* tagName);
ESucsessFail addDataTagElemet(char const* tagName, int tagLength, EtagType);
ESucsessFail addData(short val);
ESucsessFail addExternElemet(char const* tagName);
ESucsessFail addCodeElemet(SCodeinfo codeInfo);
ESucsessFail addExternUsage(SCodeinfo codeInfo);

/* Services*/
ESucsessFail initDataBase();
ESucsessFail istagExist(char const* tag, ESucsessFail* pIsExternalTag, short* pTagAddr);
int reallocAndCopyBuffer(void** outFileData, int currSize);
ESucsessFail updateTagsBaseAddress();
ESucsessFail updateEntryAddress();
ESucsessFail generateCodeForTag();

#endif
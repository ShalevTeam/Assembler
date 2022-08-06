#ifndef _SERVICES_
#define _SERVICES_

/** @file service.h
 *
 * @brief General services for the code text processing.
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2022 Yam Shalev.  All rights reserved.
 */

#include "globalDefs.h"


int reallocAndCopyBuffer(void** outFileData, int currSize);
char* readLine(char* startPos,char* line);
ELineType getLineType(char const* line, int* additionalInfo);
ESucsessFail handleCodeLine(char const* line, ECodeCmnd cmnd);
ESucsessFail handleDataLine(char const* line, EDataCmnd cmnd);
ESucsessFail analizeLine(int lineNumber, char const* line);

#endif
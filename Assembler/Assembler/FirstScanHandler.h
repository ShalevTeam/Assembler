#ifndef _FIRST_SCAN_HANDLER_
#define _FIRST_SCAN_HANDLER_

/** @file FirstScanHandler.h
 *
 * @brief This module handles the first scan of the input code flie
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
ESucsessFail doFirstFileScan(char* fileData);

#endif
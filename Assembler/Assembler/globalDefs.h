#ifndef _GLOBAL_DEFS_
#define _GLOBAL_DEFS_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ASSEBLER_FILE_SIZE 5000
#define MAX_LINE_LENGTH  1000

typedef enum EFuncResSucsessFail
{
	EFuncResFail = 0,
	EFuncResSucsess = 1
}EFuncResSucsessFail;

typedef enum ECodeStatus
{
	ECodeComplete = 0,
	EWaitForTag = 1
}ECodeStatus;

typedef struct SCodeElement
{
	unsigned short address;
	unsigned short binaryCode;
	char* tagName;
	ECodeStatus Status;
	void* nextEelement;
}SCodeElement;

#endif
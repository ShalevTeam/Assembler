#ifndef _GLOBAL_DEFS_
#define _GLOBAL_DEFS_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ASSEBLER_FILE_SIZE 5000
#define MAX_LINE_LENGTH  1000
#define CODE_INITIAL_ADDR 100


typedef enum EFuncResSucsessFail
{
	EFuncResFail = 0,
	EFuncResSucsess = 1
}EFuncResSucsessFail;

typedef enum ELineType
{
	ECodeLine = 0,
	EInstructionLine = 1
}ELineType;

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

typedef struct SAddressElement
{
	unsigned short address;
	void* nextEelement;
}SAddressElement;

typedef struct STagParams
{
	char* tagName;
	unsigned short tagAddr; // Address of the TAG
	void* nextEelement;
}STagParams;

typedef struct SExternElement
{
	char* tagName;
	SAddressElement* externUseAddrList; // The adddress where the extern is used
	void* nextEelement;
}SExternElement;

typedef struct SEntryElement
{
	char* tagName;
	unsigned short address; //The Entry Tag address
	void* nextEelement;
}SEntryElement;

#endif
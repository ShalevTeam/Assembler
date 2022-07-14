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

struct SCodeElement;
typedef struct SCodeElement
{
	unsigned short address;
	unsigned short binaryCode;
	char* tagName;
	ECodeStatus Status;
	struct SCodeElement* nextEelement;
}SCodeElement;

struct SAddressElement;
typedef struct SAddressElement
{
	unsigned short address;
	struct SAddressElement* nextEelement;
}SAddressElement;

struct STagParams;
typedef struct STagParams
{
	char* tagName;
	unsigned short tagAddr; // Address of the TAG
	struct STagParams* nextEelement;
}STagParams;

struct SExternElement;
typedef struct SExternElement
{
	char* tagName;
	SAddressElement* externUseAddrList; // The adddress where the extern is used
	struct SExternElement* nextEelement;
}SExternElement;

struct SEntryElement;
typedef struct SEntryElement
{
	char* tagName;
	unsigned short address; //The Entry Tag address
	struct SEntryElement* nextEelement;
}SEntryElement;

#endif
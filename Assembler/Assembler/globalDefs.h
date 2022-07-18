#ifndef _GLOBAL_DEFS_
#define _GLOBAL_DEFS_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ASSEBLER_FILE_SIZE 5000
#define MAX_LINE_LENGTH  1000
#define CODE_INITIAL_ADDR 100


typedef enum eSucsessFail
{
	eFail = 0,
	eSucsess = 1
}eSucsessFail;

typedef enum ELineType
{
	eLineUndefine = 0,
	eCodeLine ,
	eDataLine,
	eInstructionLine
}ELineType;

typedef enum ECodeStatus
{
	eCodeComplete = 0,
	eWaitForTag = 1
}ECodeStatus;

typedef struct SCodeinfo
{
	unsigned short address;
	unsigned short binaryCode;
	ECodeStatus Status;
	char* tag; // Tag which need to be translate to code
}SCodeinfo;

struct SCodeElement;
typedef struct SCodeElement
{
	SCodeinfo codeInfo;
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
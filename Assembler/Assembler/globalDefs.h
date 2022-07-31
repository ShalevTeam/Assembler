#ifndef _GLOBAL_DEFS_
#define _GLOBAL_DEFS_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ASSEBLER_FILE_SIZE 5000
#define MAX_LINE_LENGTH  1000
#define CODE_INITIAL_ADDR 100
#define MAX_OPERAND_NUM  2


typedef enum eSucsessFail
{
	eFail = 0,
	eSucsess = 1
}eSucsessFail;

typedef enum EAddrType
{
	eEmmediateAddr, // Number
	eDirectaddr, //Tag
	eBaseRelativeAddr, //Struct
	eRegisterAddr, // Register
	eUnidentifiedAddr // Err - not identified
}EAddrType;

typedef enum ELineType
{
	eLineUndefine = 0,
	eCodeLine ,
	eDataLine,
	eInstructionLine,
	eRemarkLine
}ELineType;

typedef enum EDataCmnd
{
	eNoDataCmnd = 0,
	eStringCmnd,
	eStructCmnd,
	eDataCmnd
}EDataCmnd;

typedef enum ECodeCmnd
{
	eNoCodeCmnd = 0,
	emovCodeCmnd,
	ecmpCodeCmnd,
	eaddCodeCmnd,
	esubCodeCmnd,
	enotCodeCmnd,
	eclrCodeCmnd,
	eleaCodeCmnd,
	eincCodeCmnd,
	edecCodeCmnd,
	ejmpCodeCmnd,
	ebneCodeCmnd,
	egetCodeCmnd,
	eprnCodeCmnd,
	ejsrCodeCmnd,
	ertsCodeCmnd,
	ehltCodeCmnd,
	eMaxCodeCmnd
}ECodeCmnd;

typedef enum EInstructionCmnd
{
	eNoInstructionCmnd = 0,
	eExternInstruction,
	eEntryInstruction
}EInstructionCmnd;

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

typedef struct SEntryElement
{
	char* tagName;
	unsigned short address; //The Entry Tag address
	struct SEntryElement* nextEelement;
}SEntryElement;


typedef struct SCommandsParams
{
	char const* cmndString;
	int numOfOperands;
}SCommandsParams;


typedef struct SEmmediateAddrParams
{
	int number;
}SEmmediateAddrParams;

typedef struct SDirectaddrParams
{
	char const* tagName;
}SDirectaddrParams;

typedef struct SBaseRelativeAddrParams
{
	char const* tagName;
	int tagOffset;
}SBaseRelativeAddrParams;

typedef struct SRegisterAddrParams
{
	int refNumber;
}SRegisterAddrParams;

typedef struct SOperandAdressingParams
{
	char* operandString;
	EAddrType addrType;
	SEmmediateAddrParams emmediateAdressingParams;
	SDirectaddrParams directaddrParams;
	SBaseRelativeAddrParams baseRelativeAddrParams;
	SRegisterAddrParams registerAddrParams;
}SOperandAdressingParams;


#endif
#ifndef _GLOBAL_DEFS_
#define _GLOBAL_DEFS_

/** @globaldef.h
 *
 * @brief Structures and enums used by the progrm.
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2022 Yam Shalev.  All rights reserved.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ASSEBLER_FILE_SIZE 5000
#define MAX_LINE_LENGTH  1000
#define CODE_INITIAL_ADDR 100
#define MAX_OPERAND_NUM  2
#define MAX_FILE_NAME	100
#define MAX_OBJECTLINE_LENGTH 100
#define MAX_ENTITY_LINE_LENGTH 100
#define MAX_MACRO_LENGTH 100


typedef enum ESucsessFail
{
	eFail = 0,
	eSucsess = 1
}ESucsessFail;

typedef enum EtagType
{
	eCodeTag =0,
	eDataTag
}EtagType;

typedef enum EAddrType
{
	eEmmediateAddr, /* Number */
	eDirectaddr, /*Tag */
	eBaseRelativeAddr, /*Struct */
	eRegisterAddr, /* Register */
	eUnidentifiedAddr /* Err - not identified */
}EAddrType;


typedef enum EAreType
{
	eAreAbsulute = 0,
	eAreExternal = 1,
	eAreRelocatable = 2
}EAreType;

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
	emovCodeCmnd = 0,
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
	eMaxCodeCmnd,
	eNoCodeCmnd
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

typedef struct ScmndWordbits
{
	unsigned short are : 2;		/*EAreType*/
	unsigned short dstAdr : 2;  /*EAddrType*/
	unsigned short srcAdr : 2;	/*EAddrType*/
	unsigned short opcode : 4;	/*ECodeCmnd*/
}ScmndWordbits;

typedef struct SOperandRegWordbits
{
	unsigned short are : 2;		/*EAreType*/
	unsigned short dstRegNum : 4;  /*register number*/
	unsigned short srcRegNum : 4;	/*register number*/
}SOperandRegWordbits;

typedef struct SOperandValWordbits
{
	unsigned short are : 2;		/*EAreType*/
	short val : 8;  /* value*/
}SOperandValWordbits;

typedef union SDataVal
{
	short val : 10;
}SDataVal;

typedef struct ScmndWordVal
{
	unsigned short lsb : 5;
	unsigned short msb : 5;
}ScmndWordVal;

typedef union SAddrressVal
{
	ScmndWordVal decodedVal;
	unsigned short val : 10;
}SAddrressVal;

typedef union ScodeWord
{
	SAddrressVal        addrVal;
	SDataVal            dataVal;
	SOperandRegWordbits regBits;
	SOperandValWordbits valBits;
	ScmndWordbits cmndBits;
	ScmndWordVal decodeVal;
}ScodeWord;

typedef struct SCodeinfo
{
	SAddrressVal codeAddress;
	ScodeWord code;
	ECodeStatus Status;
	char* tag; /* Tag which need to be translate to code */
}SCodeinfo;

struct SMacroElement;
typedef struct SMacroElement
{
	char* macroName;
	char* macroData;
	struct SMacroElement* nextEelement;
}SMacroElement;

struct SCodeElement;
typedef struct SCodeElement
{
	SCodeinfo codeInfo;
	struct SCodeElement* nextEelement;
}SCodeElement;

struct STagParams;
typedef struct STagParams
{
	char* tagName;
	ScodeWord tagAddr; /* Address of the TAG */
	EtagType tagType;
	struct STagParams* nextEelement;
}STagParams;

struct SAddressElement;
typedef struct SAddressElement
{
	SAddrressVal address;
	struct SAddressElement* nextEelement;
}SAddressElement;

struct SExternElement;
typedef struct SExternElement
{
	char* tagName;
	SAddressElement* externUseAddrList; /* The adddress where the extern is used */
	struct SExternElement* nextEelement;
}SExternElement;

typedef struct SEntryElement
{
	char* tagName;
	ScodeWord address; /*The Entry Tag address */
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
	char* tagName;
}SDirectaddrParams;

typedef struct SBaseRelativeAddrParams
{
	char* tagName;
	int tagOffset;
}SBaseRelativeAddrParams;

typedef struct SRegisterAddrParams
{
	int srcRegNumber;
	int dstRegNumber;
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
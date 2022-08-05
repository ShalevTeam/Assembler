#include "services.h"
#include "databaseHandler.h"

// Private members
static char const* m_paramPtr = NULL;

// Private functions
static int isBlank(char const* line);
static EInstructionCmnd getInstructionType(char const* line);
static EDataCmnd getdataCmnd(char const* line);
static ECodeCmnd getCodeCommand(char const* line);
static int isWordExistInLine(char const* line, char const* word);
static eSucsessFail handleTag(char const* line, ELineType lineType);
static eSucsessFail getOperandAddrType(SOperandAdressingParams* pOperandAdressingParams,int operIdx);
static eSucsessFail generateCodeElement(SCodeinfo* pCodeInfo, SOperandAdressingParams* pAddrParams,int activationCount);
static eSucsessFail istagExist(char const* tag,eSucsessFail* pIsExternalTag, short* pTagAddr);
static eSucsessFail freeAndResetCodeInfo(SCodeinfo* pCodeInfo);
static eSucsessFail handleStringCmnd(char const* line);
static eSucsessFail generateCodeForTag(SCodeinfo* pCodeInfo);
static int getCmndOperandsArray(SOperandAdressingParams cmndOperandsArray[]);

int reallocAndCopyBuffer(void** allocatedBuf, int oldSize)
{
	int newSize = oldSize * 2;

	// Save the address of the old data
	void** oldBuf = allocatedBuf;

	// allocate new buf
	*allocatedBuf = malloc(newSize);

	// Copy data from old buf to the new one
	if (*allocatedBuf)
	{
		memcpy(allocatedBuf, oldBuf, oldSize);
		free(oldBuf);
	}
	else
	{
		printf("Err on line %d Allocation failed\n", getCurrentLineNumber());
		newSize = 0;
	}

	return newSize;
}

char* readLine(char* startPos, char* line)
{
	char* newStartPos = NULL;

	char* endpos = strchr(startPos, '\n');

	if (endpos == NULL)
	{
		return NULL;
	}

	int sizeToCopy = (int)(1 + endpos - startPos);

	if (sizeToCopy + 1 < MAX_LINE_LENGTH)
	{
		strncpy(line, startPos, sizeToCopy);
		line[sizeToCopy] = 0;
		newStartPos = endpos + 1;
	}
	else
	{
		printf("Err on line %d invalid line length\n",getCurrentLineNumber());
	}

	return newStartPos;
}

ELineType getLineType(int lineNumber, char const* line, int* additionalInfo)
{
	ELineType lineType = eLineUndefine;

	if (isWordExistInLine(line, ";"))
	{
		lineType = eRemarkLine;
	}
	else
	{
		EDataCmnd dataCmnd = getdataCmnd(line);

		if (dataCmnd != eNoDataCmnd)
		{
			lineType = eDataLine;
			*additionalInfo = (int)dataCmnd;
		}
		else 
		{
			EInstructionCmnd instructionType = getInstructionType(line);

			if (instructionType != eNoInstructionCmnd)
			{
				lineType = eInstructionLine;
				*additionalInfo = (int)instructionType;
			}
			else
			{
				ECodeCmnd codeCmnd = getCodeCommand(line);

				if (codeCmnd != eNoCodeCmnd)
				{
					lineType = eCodeLine;
					*additionalInfo = (int)codeCmnd;
				}
				else
				{
					// Check if line is just spaces
					if (!isBlank(line))
					{
						printf("Err on line %d: %s\n", getCurrentLineNumber(), line);
					}
				}
			}
		
		}
	}

	return lineType;
}


static int isBlank(char const* line)
{
	char const* ch;
	int is_blank = 1;

	// Iterate through each character.
	for (ch = line; *ch != '\0'; ++ch)
	{
		if (*ch != ' ')
		{
			// Found a non-whitespace character.
			is_blank = 0;
			break;
		}
	}

	return is_blank;
}

static EInstructionCmnd getInstructionType(char const* line)
{
	EInstructionCmnd cmnd = eNoInstructionCmnd;

	if (isWordExistInLine(line, ".extern"))
	{
		cmnd = eExternInstruction;
	}
	else if (isWordExistInLine(line, ".entry"))
	{
		cmnd = eEntryInstruction;
	}
	
	return cmnd;
}

static EDataCmnd getdataCmnd(char const* line)
{
	EDataCmnd dataCmnd = eNoDataCmnd;

	if (isWordExistInLine(line, ".string"))
	{
		dataCmnd = eStringCmnd;
	}
	else if (isWordExistInLine(line, ".data"))
	{
		dataCmnd = eDataCmnd;
	}
	else if (isWordExistInLine(line, ".struct"))
	{
		dataCmnd = eStructCmnd;
	}

	return dataCmnd;
}

static ECodeCmnd getCodeCommand(char const* line)
{
	ECodeCmnd cmnd = eNoCodeCmnd;

	if (isWordExistInLine(line, "mov"))
	{
		cmnd = emovCodeCmnd;
	}
	else if (isWordExistInLine(line, "cmp"))
	{
		cmnd = ecmpCodeCmnd;
	}
	else if (isWordExistInLine(line, "add"))
	{
		cmnd = eaddCodeCmnd;
	}
	else if (isWordExistInLine(line, "sub"))
	{
		cmnd = esubCodeCmnd;
	}
	else if (isWordExistInLine(line, "not"))
	{
		cmnd = enotCodeCmnd;
	}
	else if (isWordExistInLine(line, "clr"))
	{
		cmnd = eclrCodeCmnd;
	}
	else if (isWordExistInLine(line, "lea"))
	{
		cmnd = eleaCodeCmnd;
	}
	else if (isWordExistInLine(line, "inc"))
	{
		cmnd = eincCodeCmnd;
	}
	else if (isWordExistInLine(line, "dec"))
	{
		cmnd = edecCodeCmnd;
	}
	else if (isWordExistInLine(line, "jmp"))
	{
		cmnd = ejmpCodeCmnd;
	}
	else if (isWordExistInLine(line, "bne"))
	{
		cmnd = ebneCodeCmnd;
	}
	else if (isWordExistInLine(line, "get"))
	{
		cmnd = egetCodeCmnd;
	}
	else if (isWordExistInLine(line, "prn"))
	{
		cmnd = eprnCodeCmnd;
	}
	else if (isWordExistInLine(line, "jsr"))
	{
		cmnd = ejsrCodeCmnd;
	}
	else if (isWordExistInLine(line, "rts"))
	{
		cmnd = ertsCodeCmnd;
	}
	else if (isWordExistInLine(line, "hlt"))
	{
		cmnd = ehltCodeCmnd;
	}

	return cmnd;
}

static int isWordExistInLine(char const* line, char const* word)
{
	int wordExist = 0;
	char* pos;
	char* endPos = NULL;

	pos = strstr(line, word);

	if (pos != NULL)
	{
		// Check from the left side to see if its a complet word
		if (pos != line)
		{
			if (*(pos - 1) != ' ')
			{
				wordExist = 0;
				return wordExist;
			}
		}

		// Check from the right side to see if its a complet word
		endPos = (pos + strlen(word));

		if( (*endPos == ' ') ||
			(*endPos == '\n') ||
			(*endPos == 0))
		{
			wordExist = 1;
		}
	}

	if (wordExist)
	{
		m_paramPtr = endPos;
	}
	return wordExist;
}

eSucsessFail handleTag(char const* line ,ELineType lineType)
{
	eSucsessFail res = eSucsess;
	char* dotPos = strstr(line, ":");
	char* strtPos = dotPos;
	
	// find if Tag exist
	if (dotPos != NULL)
	{
		// find the tag start
		while (strtPos != line)
		{
			if (*strtPos == ' ')
			{
				break;
			}

			strtPos--;
		}

		/* Add tag entry */
		if (lineType == eCodeLine)
		{
			res = addDataTagElemet(strtPos, dotPos - strtPos,eCodeTag);
		}
		else if (lineType == eDataLine)
		{
			/* The data position will be updated after the first scan*/
			res = addDataTagElemet(strtPos, dotPos - strtPos, eDataTag);
		}
		else
		{
			res = eFail;
		}
	}

	return res;
}

eSucsessFail getOperandAddrType(SOperandAdressingParams* pOperandAdressingParams,int operIdx)
{
	eSucsessFail res = eSucsess;
	char const* pos = NULL;
	
	pos = strstr(pOperandAdressingParams->operandString, "#");
	if (NULL == pos)
	{
		pos = pOperandAdressingParams->operandString;
		if ((strlen(pos) == 2) && (*pos == 'r'))
		{
			pOperandAdressingParams->addrType = eRegisterAddr;

			if (operIdx == 0)
			{
				pOperandAdressingParams->registerAddrParams.srcRegNumber = atoi(pos + 1);
			}
			else
			{
				pOperandAdressingParams->registerAddrParams.dstRegNumber = atoi(pos + 1);
			}
			
		}
		else
		{
			pos = strstr(pOperandAdressingParams->operandString, ".");

			if (NULL == pos)
			{
				pos = pOperandAdressingParams->operandString;
				pOperandAdressingParams->addrType = eDirectaddr;
				pOperandAdressingParams->directaddrParams.tagName = malloc(strlen(pos)+1);

				if (pOperandAdressingParams->directaddrParams.tagName == NULL)
				{
					res = eFail;
					printf("Err on line %d cant alloc for tag\n", getCurrentLineNumber());
				}
				else
				{
					strncpy(pOperandAdressingParams->directaddrParams.tagName, pos, strlen(pos));
					pOperandAdressingParams->directaddrParams.tagName[strlen(pos)] = '\0';
				}
			}
			else // Struct
			{
				pOperandAdressingParams->addrType = eBaseRelativeAddr;
				pOperandAdressingParams->baseRelativeAddrParams.tagName = malloc(pos- pOperandAdressingParams->operandString);

				if (pOperandAdressingParams->baseRelativeAddrParams.tagName == NULL)
				{
					res = eFail;
					printf("Err on line %d cant alloc for tag\n", getCurrentLineNumber());
				}
				else
				{
					strncpy(pOperandAdressingParams->baseRelativeAddrParams.tagName, 
						pOperandAdressingParams->operandString, 
						pos - pOperandAdressingParams->operandString);
					pOperandAdressingParams->baseRelativeAddrParams.tagName[pos - pOperandAdressingParams->operandString] = '\0';
					pOperandAdressingParams->baseRelativeAddrParams.tagOffset = atoi(pos+1);


				}
			}
		}
	}
	else
	{
		pOperandAdressingParams->addrType = eEmmediateAddr;
		if ((pos+1) != NULL)
		{
			pOperandAdressingParams->emmediateAdressingParams.number = atoi((pos + 1));
		}	
	}
	return res;
}

eSucsessFail generateCodeElement(SCodeinfo* pCodeInfo, SOperandAdressingParams* pAddrParams,int activationCount)
{
	eSucsessFail res = eSucsess;
	eSucsessFail isExternalTag = eFail;
	short tagAddr = 0;
	char const* tagName;
	char const* tmpStrPos = NULL;

	switch (pAddrParams->addrType)
	{
	case eEmmediateAddr: // Number
		pCodeInfo->code.valBits.are = eAreAbsulute;
		pCodeInfo->code.valBits.val = pAddrParams->emmediateAdressingParams.number;
		break;
	case eDirectaddr: //Tag

		tagName = pAddrParams->directaddrParams.tagName;
		
		break;
	case eBaseRelativeAddr: //Struct

		/* Handle the struct address*/
		if (activationCount == 1)
		{
			pCodeInfo->code.valBits.are = eAreAbsulute;
			pCodeInfo->code.valBits.val = pAddrParams->baseRelativeAddrParams.tagOffset;
		}
		else // handle struct TAG
		{
			pCodeInfo->tag = malloc(strlen(pAddrParams->baseRelativeAddrParams.tagName)+1);

			if (pCodeInfo->tag == NULL)
			{
				printf("Err on line %d Allocation failed\n", getCurrentLineNumber());
			}
			else
			{
				strcpy(pCodeInfo->tag, pAddrParams->baseRelativeAddrParams.tagName);
				pCodeInfo->Status = eWaitForTag;
			}
		}
		break;
	case eRegisterAddr:// Register
		pCodeInfo->code.regBits.are = eAreAbsulute;

		if (pAddrParams->registerAddrParams.dstRegNumber)
		{
			pCodeInfo->code.regBits.dstRegNum = pAddrParams->registerAddrParams.dstRegNumber;
		}
		if (pAddrParams->registerAddrParams.srcRegNumber)
		{
			pCodeInfo->code.regBits.srcRegNum = pAddrParams->registerAddrParams.srcRegNumber;
		}
		break;
	case eUnidentifiedAddr: // Err - not identified
		printf("Err on line %d cant identify addr type\n", getCurrentLineNumber());
		res = eFail;
		break;
	}

	return res;
}

eSucsessFail istagExist(char const* tag,eSucsessFail* pIsExternalTag, short* pTagAddr)
{
	eSucsessFail res = eFail;
	SExternElement* pCurrPosExternList = getExternalList();
	STagParams* pCurrPosTagList = getTagList();
	
	// Check the external list
	while (pCurrPosExternList != NULL)
	{
		if (strcmp(pCurrPosExternList->tagName, tag) == 0)
		{
			*pIsExternalTag = eSucsess;
			*pTagAddr = 0;
			res = eSucsess;
			return res;
		}

		pCurrPosExternList = pCurrPosExternList->nextEelement;
	}

	// Check the tag list
	while (pCurrPosTagList != NULL)
	{
		if (strcmp(pCurrPosTagList->tagName, tag) == 0)
		{
			*pIsExternalTag = eFail;
			*pTagAddr = pCurrPosTagList->tagAddr.valBits.val;
			res = eSucsess;
			return res;
		}

		pCurrPosTagList = pCurrPosTagList->nextEelement;
	}

	return res;
}

eSucsessFail freeAndResetCodeInfo(SCodeinfo* pCodeInfo)
{
	eSucsessFail res = eSucsess;

	if (pCodeInfo->tag != NULL)
	{
		free(pCodeInfo->tag);
	}

	memset(pCodeInfo, 0, sizeof(*pCodeInfo));
	return res;
}

eSucsessFail handleStringCmnd(char const* line)
{
	eSucsessFail res = eSucsess;
	char const* strPos = strstr(line,".string");
	int Pos = 0;

	if (strPos != NULL)
	{
		strPos += strlen(".string");

		while (*(strPos+ Pos) != '\0')
		{
			addData(strPos[Pos]);

			Pos++;
		}

		addData(0);
	}
	else
	{
		printf("Err on line %d unexpected bug\n", getCurrentLineNumber());
		res = eFail;
	}




	return res;
}

eSucsessFail generateCodeForTag(SCodeinfo* pCodeInfo)
{
	eSucsessFail res = eSucsess;
	eSucsessFail isExternalTag = eSucsess;
	short tagAddr = 0;

	// Check if tag exist
	if (istagExist(pCodeInfo->tag, &isExternalTag, &tagAddr))
	{
		if (isExternalTag)
		{
			pCodeInfo->code.valBits.are = eAreExternal;
			pCodeInfo->code.valBits.val = 0;
			addExternElemet(pCodeInfo->tag);
		}
		else
		{
			pCodeInfo->code.valBits.are = eAreRelocatable;
			pCodeInfo->code.valBits.val = tagAddr;
		}
	}
	else /* Tag not exist*/
	{
		printf("Err tag %s used but not defined\n", pCodeInfo->tag);
		res = eFail;
	}
	
	return res;
}

eSucsessFail handleCodeLine(char const* line, ECodeCmnd cmnd)
{
	eSucsessFail res = handleTag(line, eCodeLine);
	int numOfOperands = 0;
	SOperandAdressingParams operandAdressingParams[MAX_OPERAND_NUM] = { 0, };
	int operIdx = 0;
	SCodeinfo codeinfo = { 0, };
	int activationCount = 0; /* Used for creating 2 elements for struct*/
	
	// Search for operands
	numOfOperands = getCmndOperandsArray(operandAdressingParams);
	
	
	// Process operands
	for (operIdx = 0; operIdx < numOfOperands; operIdx++)
	{
		if (!getOperandAddrType(&operandAdressingParams[operIdx], operIdx))
		{
			printf("Err on line %d invalid operands addressing\n", getCurrentLineNumber());
			res = eFail;
			return res;
		}
	}

	//Set the first code line data
	freeAndResetCodeInfo(&codeinfo);
	codeinfo.code.cmndBits.are = eAreAbsulute;
	codeinfo.code.cmndBits.opcode = cmnd;
	
	if (numOfOperands == 1)
	{
		codeinfo.code.cmndBits.srcAdr = operandAdressingParams[0].addrType;
	}
	
	if (numOfOperands == 2)
	{
		codeinfo.code.cmndBits.dstAdr = operandAdressingParams[1].addrType;
	}
	
	addCodeElemet(codeinfo);
	freeAndResetCodeInfo(&codeinfo);
	
	//Handle operands
	for (operIdx = 0; operIdx < numOfOperands; operIdx++)
	{
		/* reset on each operand index*/
		activationCount = 0;

		if (!generateCodeElement(&codeinfo, &operandAdressingParams[operIdx],activationCount))
		{
			printf("Err on line %d cant generate code for operand 1\n", getCurrentLineNumber());
		}
		else
		{
			// Handle 2 registers - special case
			if ((numOfOperands ==2) &&
				(operIdx == 0) &&
				(operandAdressingParams[0].addrType == eRegisterAddr) &&
				(operandAdressingParams[1].addrType == eRegisterAddr))
			{
				// don't add code until the second register
			}
			else /* Not 2 registers*/
			{
				if (!addCodeElemet(codeinfo))
				{
					printf("Err on line %d cant add code element 1\n", getCurrentLineNumber());
				}
				freeAndResetCodeInfo(&codeinfo);

				/* for struct we need to create another code element*/
				if (operandAdressingParams[operIdx].addrType == eBaseRelativeAddr)
				{
					activationCount++;

					if (!generateCodeElement(&codeinfo, &operandAdressingParams[operIdx], activationCount))
					{
						printf("Err on line %d cant generate code for operand 1\n", getCurrentLineNumber());
					}
					else
					{
						if (!addCodeElemet(codeinfo))
						{
							printf("Err on line %d cant add code element 1\n", getCurrentLineNumber());
						}

						freeAndResetCodeInfo(&codeinfo);
					}
				}
			}
		}
	}
	
	return res;
}

eSucsessFail handleDataLine(char const* line, EDataCmnd cmnd)
{
	// Add TAG if exist to the list of tags
	eSucsessFail res = handleTag(line, eDataLine);

	switch (cmnd)
	{
	case eStringCmnd:
		handleStringCmnd(line);
		break;
	case eStructCmnd:
		break;
	case eDataCmnd:
		break;
	case eNoDataCmnd:
		res = eFail;
		break;
	}

	return res;
}

int getCmndOperandsArray(SOperandAdressingParams operandAddrPrmsArray[])
{
	int numOperandFound = 0;
	char const* currPos = m_paramPtr;
	char const* strtPos = NULL;
	int wordLength = 0;

	if (m_paramPtr == NULL)
	{
		// Internal err
		printf("Err on line %d unexpected null pointer\n", getCurrentLineNumber());
		return 0;
	}

	// Search for operands
	while (1)
	{
		if ((*currPos == ' ') || (*currPos == ',') || (*currPos == '\n'))
		{
			if (strtPos == NULL)
			{
				if (*currPos == '\n')
				{
					// No more operands
					break;
				}
				else if (*currPos == ',')
				{
					if (numOperandFound == 0)
					{
						printf("Err on line %d invalid leading comma\n", getCurrentLineNumber());
						return 0;
					}
					else
					{
						// Skip comma between words
						currPos++;
						continue;
					}
				}
				else
				{
					// Skip leading spaces
					currPos++;
					continue;
				}
			}
			else
			{
				// End of word
				operandAddrPrmsArray[numOperandFound].operandString = malloc(wordLength+1);
				strncpy(operandAddrPrmsArray[numOperandFound].operandString, strtPos, wordLength);
				operandAddrPrmsArray[numOperandFound].operandString[wordLength] = 0;
				numOperandFound++;
				wordLength = 0;
				strtPos = NULL;

				if (numOperandFound == MAX_OPERAND_NUM)
				{
					// No more operands
					break;
				}

				if (*currPos == '\n')
				{
					// No more operands
					break;
				}
			}
		}
		else
		{
			if (strtPos == NULL)
			{
				strtPos = currPos;
			}

			wordLength++;
		}
		currPos++;
	}

	return numOperandFound;
}
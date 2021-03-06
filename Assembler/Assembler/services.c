#include "services.h"

SEntryElement* m_entryList = NULL; // List of all entry and there address
SExternElement* m_externList = NULL; // List of all extern and there use
STagParams* m_dataTagList = NULL; // list of all defined TAG in the code
SCodeElement* m_codeList = NULL; // The code that was generated
unsigned short* m_pDataSeqtion = NULL; // The data seq

// Private members
static int m_lineNumber = 0;
static int m_codePos = CODE_INITIAL_ADDR;
static int m_dataPos = CODE_INITIAL_ADDR;
static int m_maxDataLength = MAX_ASSEBLER_FILE_SIZE;
static char const* m_paramPtr = NULL;

// Private functions
static int isBlank(char const* line);
static EInstructionCmnd getInstructionType(char const* line);
static EDataCmnd getdataCmnd(char const* line);
static ECodeCmnd getCodeCommand(char const* line);
static int isWordExistInLine(char const* line, char const* word);
static eSucsessFail handleTag(char const* line, ELineType lineType);
static eSucsessFail getOperandAddrType(SOperandAdressingParams* pOperandAdressingParams);
static eSucsessFail generateCodeElement(SCodeinfo* pCodeInfo, SOperandAdressingParams* pAddrParams);
static eSucsessFail tagExist(eSucsessFail* pIsExternalTag, short* pTagAddr);
static eSucsessFail freeAndResetCodeInfo(SCodeinfo* pCodeInfo);



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
		printf("Err on line %d Allocation failed\n", m_lineNumber);
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
		printf("Err on line %d invalid line length\n",m_lineNumber);
	}

	return newStartPos;
}

void setCurrentLineNumber(lineNumber)
{
	m_lineNumber = lineNumber;
}

eSucsessFail initDataBase()
{
	eSucsessFail res = eSucsess;

	while (m_entryList != NULL)
	{
		SEntryElement* pNextEntry = m_entryList->nextEelement;
		if (m_entryList->tagName)
		{
			free(m_entryList->tagName);
			m_entryList->tagName = NULL;
		}
		free(m_entryList);

		m_entryList = pNextEntry;
	}

	while (m_externList != NULL)
	{
		SExternElement* pNextEntry = m_externList->nextEelement;

		while (m_externList->externUseAddrList != NULL)
		{
			SAddressElement* pInternalNextEntry = m_externList->externUseAddrList->nextEelement;
			
			free(m_externList->externUseAddrList);

			m_externList->externUseAddrList = pInternalNextEntry;
		}

		if (m_externList->tagName)
		{
			free(m_externList->tagName);
			m_externList->tagName = NULL;
		}

		free(m_externList);

		m_externList = pNextEntry;
	}

	while (m_dataTagList != NULL)
	{
		STagParams* pNextEntry = m_dataTagList->nextEelement;

		if (m_dataTagList->tagName)
		{
			free(m_dataTagList->tagName);
			m_dataTagList->tagName = NULL;
		}
		free(m_dataTagList);

		m_dataTagList = pNextEntry;
	}

	while (m_codeList != NULL)
	{
		SCodeElement* pNextEntry = m_codeList->nextEelement;

		if (m_codeList->codeInfo.tag)
		{
			free(m_codeList->codeInfo.tag);
			m_codeList->codeInfo.tag = NULL;
		}

		free(m_codeList);

		m_codeList = pNextEntry;
	}

	if (m_pDataSeqtion)
	{
		free(m_pDataSeqtion);
	}

	m_pDataSeqtion = malloc (sizeof(unsigned short) *m_maxDataLength);

	if (m_pDataSeqtion == NULL)
	{
		res = eFail;
	}

	m_codePos = CODE_INITIAL_ADDR;
	m_dataPos = CODE_INITIAL_ADDR;

	return res;
}

eSucsessFail addData(unsigned short val)
{
	eSucsessFail res = eSucsess;

	if (m_dataPos >= m_maxDataLength)
	{
		m_maxDataLength = reallocAndCopyBuffer(&m_pDataSeqtion, m_maxDataLength* sizeof(unsigned short));
	}

	if (m_maxDataLength == 0)
	{
		res = eFail;
	}
	else
	{
		m_dataPos++;
		*m_pDataSeqtion = val;
	}

	return res;
}

eSucsessFail addEntryElemet(unsigned short address,char const* tagName)
{
	eSucsessFail res = eSucsess;

	SEntryElement* latest = m_entryList;
	SEntryElement* prev = m_entryList;

	// Find the latest element
	while (latest != NULL)
	{
		prev = latest;
		latest = latest->nextEelement;
	}

	SEntryElement* newElem = malloc(sizeof(SEntryElement));

	if (m_entryList == NULL)
	{
		m_entryList = newElem;
	}

	if (newElem)
	{
		if (prev != NULL)
		{
			prev->nextEelement = newElem;
		}

		newElem->address.valBits.val = address;

		if (tagName)
		{
			newElem->tagName = malloc(strlen(tagName) + 1);
			if (newElem->tagName)
			{
				strcpy(newElem->tagName, tagName);
				newElem->nextEelement = NULL;
			}
			else
			{
				printf("Err on line %d memory alloc fail\n",m_lineNumber);
				res = eFail;
			}
		}
		else
		{
			newElem->tagName = NULL;
		}
	}
	else
	{
		printf("Err on line %d memory alloc fail\n",m_lineNumber);
		res = eFail;
	}

	return res;
}

eSucsessFail addDataTagElemet(unsigned short address, char const* tagName,int tagLength)
{
	eSucsessFail res = eSucsess;

	STagParams* latest = m_dataTagList;
	STagParams* prev = m_dataTagList;

	// Validate tag
	if (*tagName == ' ')
	{
		printf("Err on line %d Tag containe spaces\n", m_lineNumber);
	}

	// Find the latest element
	while (latest != NULL) 
	{
		if (strncmp(latest->tagName, tagName, tagLength) == 0)
		{
			printf("Err on line %d Tag define twice: %s",m_lineNumber, tagName);

			return eFail;
		}
		prev = latest;
		latest = latest->nextEelement;
	}

	STagParams* newElem = malloc(sizeof(STagParams));

	if (m_dataTagList == NULL)
	{
		m_dataTagList = newElem;
	}

	if (newElem)
	{
		if (prev != NULL)
		{
			prev->nextEelement = newElem;
		}

		newElem->tagAddr.valBits.val = address;

		if (tagName)
		{
			newElem->tagName = malloc(tagLength + 1);
			if (newElem->tagName)
			{
				strncpy(newElem->tagName, tagName, tagLength);
				newElem->tagName[tagLength] = 0;
				newElem->nextEelement = NULL;
			}
			else
			{
				printf("Err on line %d memory alloc fail\n",m_lineNumber);
				res = eFail;
			}
		}
		else
		{
			newElem->tagName = NULL;
		}
	}
	else
	{
		printf("Err on line %d memory alloc fail\n",m_lineNumber);
		res = eFail;
	}

	return res;
}

eSucsessFail addCodeElemet(SCodeinfo codeInfo)
{
	eSucsessFail res = eSucsess;
	SCodeElement* latest = m_codeList;
	SCodeElement* prev = m_codeList;

	// Find the latest element
	while (latest != NULL)
	{
		prev = latest;
		latest = latest->nextEelement;
	}

	SCodeElement* newElem = malloc(sizeof(SCodeElement));

	if (m_entryList == NULL)
	{
		m_codeList = newElem;
	}

	if (newElem)
	{
		if (prev != NULL)
		{
			prev->nextEelement = newElem;
		}

		newElem->codeInfo = codeInfo;

		if (codeInfo.tag)
		{
			newElem->codeInfo.tag = malloc(strlen(codeInfo.tag) + 1);
			if (newElem->codeInfo.tag)
			{
				strcpy(newElem->codeInfo.tag, codeInfo.tag);
				newElem->nextEelement = NULL;
				m_codePos++;
			}
			else
			{
				printf("Err on line %d memory alloc fail\n",m_lineNumber);
				res = eFail;
			}
		}
	}
	else
	{
		printf("Err on line %d memory alloc fail\n",m_lineNumber);
		res = eFail;
	}

	return res;
}

eSucsessFail addExternElemet(unsigned short address, char const* tagName)
{
	eSucsessFail res = eSucsess;
	SExternElement* latest = m_externList;
	SExternElement* prev = m_externList;
	eSucsessFail tagFound = eFail;

	// Check if the tag exist
	while (latest != NULL)
	{
		if (strcmp(latest->tagName, tagName) == 0)
		{
			// Tag found
			tagFound = eSucsess;
			
			// Find the latest at the use list
			SAddressElement* addrcurr = latest->externUseAddrList;
			SAddressElement* addrprev = latest->externUseAddrList;
			while (addrcurr != NULL)
			{
				addrprev = addrcurr;
				addrcurr = addrcurr->nextEelement;
			}

			if (addrprev == NULL)
			{
				printf("Err on line %d unexpected null pointer\n",m_lineNumber);
				res = eFail;
				return res;
			}

			// Set the new element
			addrprev->nextEelement = malloc(sizeof(SAddressElement));

			if (addrprev->nextEelement)
			{
				((SAddressElement*)addrprev->nextEelement)->address.valBits.val = address;
				((SAddressElement*)addrprev->nextEelement)->nextEelement = NULL;
				return res;
			}
			else
			{
				printf("Err on line %d memory alloc fail\n",m_lineNumber);
				res = eFail;
				return res;
			}
			
		}

		latest = latest->nextEelement;
	}

	SExternElement* newElem = malloc(sizeof(SExternElement));

	if (m_externList == NULL)
	{
		m_externList = newElem;
	}

	if (newElem)
	{
		if (prev != NULL)
		{
			prev->nextEelement = newElem;
		}

		// If we got here -> Generate new SExternElement element
		newElem->tagName = malloc(strlen(tagName) + 1);
		if (newElem->tagName)
		{
			strcpy(newElem->tagName, tagName);
			newElem->nextEelement = NULL;
		}
		else
		{
			printf("Err on line %d memory alloc fail\n",m_lineNumber);
			res = eFail;
			return res;
		}

		newElem->externUseAddrList = malloc(sizeof(SAddressElement));
		if (newElem->externUseAddrList)
		{
			((SAddressElement*)newElem->externUseAddrList)->address.valBits.val = address;
			((SAddressElement*)newElem->externUseAddrList)->nextEelement = NULL;
			
		}
		else
		{
			printf("Err on line %d memory alloc fail\n",m_lineNumber);
			res = eFail;
			return res;
		}
	}
	else
	{
		printf("Err on line %d memory alloc fail\n",m_lineNumber);
		res = eFail;
	}

	return res;
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
						printf("Err on line %d: %s\n", m_lineNumber, line);
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

		if (lineType == eCodeLine)
		{
			res = addDataTagElemet(m_codePos, strtPos, dotPos - strtPos);
		}
		else if (lineType == eDataLine)
		{
			res = addDataTagElemet(m_dataPos, strtPos, dotPos - strtPos);
		}
		else
		{
			res = eFail;
		}
	}

	return res;
}

eSucsessFail getOperandAddrType(SOperandAdressingParams* pOperandAdressingParams)
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
			pOperandAdressingParams->registerAddrParams.regNumber = atoi(pos + 1);
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
					printf("Err on line %d cant alloc for tag\n", m_lineNumber);
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
					printf("Err on line %d cant alloc for tag\n", m_lineNumber);
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

eSucsessFail generateCodeElement(SCodeinfo* pCodeInfo, SOperandAdressingParams* pAddrParams)
{
	eSucsessFail res = eSucsess;
	eSucsessFail isExternalTag = eFail;
	short tagAddr = 0;
	char const* tagName;

	switch (pAddrParams->addrType)
	{
	case eEmmediateAddr: // Number
		pCodeInfo->code.valBits.are = eAreAbsulute;
		pCodeInfo->code.valBits.val = pAddrParams->emmediateAdressingParams.number;
		break;
	case eDirectaddr: //Tag
		tagName = pAddrParams->directaddrParams.tagName;
		// Check if tag exist
		if (tagExist(&isExternalTag, &tagAddr))
		{
			if (isExternalTag)
			{
				pCodeInfo->code.valBits.are = eAreExternal;
				pCodeInfo->code.valBits.val = 0;
				addExternElemet(m_codePos, tagName);
			}
			else
			{
				pCodeInfo->code.valBits.are = eAreRelocatable;
				pCodeInfo->code.valBits.val = tagAddr;
			}
		}
		else
		{
			pCodeInfo->tag = malloc(strlen(tagName) + 1);

			if (pCodeInfo->tag != NULL)
			{
				strcpy(pCodeInfo->tag, tagName);
				pCodeInfo->Status = eWaitForTag;
			}
			else
			{
				printf("Err on line %d cant alloc\n", m_lineNumber);
				res = eFail;
				return res;
			}

		}
	case eBaseRelativeAddr: //Struct
	case eRegisterAddr:// Register
	case eUnidentifiedAddr: // Err - not identified
		break;
	}

	return res;
}

eSucsessFail tagExist(eSucsessFail* pIsExternalTag, short* pTagAddr)
{
	eSucsessFail res = eSucsess;

	return res;
}

eSucsessFail freeAndResetCodeInfo(SCodeinfo* pCodeInfo)
{
	eSucsessFail res = eSucsess;
	return res;
}

eSucsessFail handleCodeLine(char const* line, ECodeCmnd cmnd)
{
	eSucsessFail res = handleTag(line, eCodeLine);
	int numOfOperands = 0;
	SOperandAdressingParams operandAdressingParams[MAX_OPERAND_NUM];
	int operIdx = 0;
	SCodeinfo codeinfo;
	
	// Search for operands
	numOfOperands = getCmndOperandsArray(operandAdressingParams);
	
	
	// Process operands
	for (operIdx = 0; operIdx < numOfOperands; operIdx++)
	{
		if (!getOperandAddrType(&operandAdressingParams[operIdx]))
		{
			printf("Err on line %d invalid operands addressing\n", m_lineNumber);
			res = eFail;
			return res;
		}
	}

	//Set the first code line data
	memset(&codeinfo, 0, sizeof(codeinfo));
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
	if (numOfOperands > 0)
	{
		if (numOfOperands == 1)
		{
			//Set the first code line code
			if (!generateCodeElement(&codeinfo, &operandAdressingParams[0]))
			{
				printf("Err on line %d cant generate code for operand 1\n", m_lineNumber);
			}
			else
			{
				if (!addCodeElemet(codeinfo))
				{
					printf("Err on line %d cant add code element 1\n", m_lineNumber);
				}
				freeAndResetCodeInfo(&codeinfo);
			}

		}
	}
	
	return res;
}

eSucsessFail handleDataLine(char const* line, EDataCmnd cmnd)
{
	eSucsessFail res = handleTag(line, eCodeLine);

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
		printf("Err on line %d unexpected null pointer\n", m_lineNumber);
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
						printf("Err on line %d invalid leading comma\n", m_lineNumber);
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
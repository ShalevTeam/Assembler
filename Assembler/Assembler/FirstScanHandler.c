#include "FirstScanHandler.h"
#include "databaseHandler.h"

/* Private members*/
static char const* m_operandPosPtr = NULL; // Pointer to the operands of a command

/* Private functions*/
static ESucsessFail isBlank(char const* line);
static EInstructionCmnd getInstructionType(char const* line);
static EDataCmnd getdataCmnd(char const* line);
static ECodeCmnd getCodeCommand(char const* line);
static ESucsessFail isWordExistInLine(char const* line, char const* word);
static ESucsessFail handleTag(char const* line, EtagType tagType);
static ESucsessFail setOperandAddrParams(SOperandAdressingParams* pOperandAdressingParams,int operIdx);
static ESucsessFail generateCodeForOperand(SCodeinfo* pCodeInfo, SOperandAdressingParams* pAddrParams,int activationCount);
static ESucsessFail freeAndResetCodeInfo(SCodeinfo* pCodeInfo);
static ESucsessFail handleStringCmnd(char const* line);
static ESucsessFail generateCodeForTag(SCodeinfo* pCodeInfo);
static ESucsessFail isNumberOfOperandsValid(int operandNum,ECodeCmnd cmnd);
static ESucsessFail handleCodeLine(char const* line, ECodeCmnd cmnd);
static ESucsessFail handleDataLine(char const* line, EDataCmnd cmnd);
static char* readLine(char* startPos, char* line);
static ELineType getLineType(char const* line, int* additionalInfo);
static ESucsessFail analizeLine(int lineNumber, char const* line);
static int setOperandsString(SOperandAdressingParams cmndOperandsArray[]);

/******************************************************************************
* Function : readLine()
*
*  This function is used to read a line from the input start position
*  and return this line to the user
*
* \param
*  char* startPos:	INPUT:  pointer to the start position in which to search for the line
*  int line:		INPUT/OUTPUT: pointer to the line data
*
* \return
*  char*: pointer to the next line start position or NULL if no more lines
*
*******************************************************************************/
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

/******************************************************************************
* Function : getLineType()
*
*  This function is used to get the line type:Instruction,Code or data.
*  and return the type to the user
*
* \param
*  char const* line:	INPUT:  pointer to the line
*  int* additionalInfo:	OUTPUT: additional info relevant to the line type
*  EDataCmnd* for data line
*  EInstructionCmnd* for instruction line
*  ECodeCmnd* for code line
*
* \return
*  ELineType: The line type
*
*******************************************************************************/
ELineType getLineType(char const* line, int* additionalInfo)
{
	ELineType lineType = eLineUndefine;

	/* Reset the operands pointer before analizing a line */
	m_operandPosPtr = NULL;

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
						printf("syntax err on line %d: %s", getCurrentLineNumber(), line);
					}
				}
			}
		
		}
	}

	return lineType;
}

/******************************************************************************
* Function : isBlank()
*
*  This function is used to check if a line is just white spaces
*  and return eSucsess if its all white spaces
*
* \param
*  char const* line:	INPUT:  pointer to the line
* 
*
* \return
*  ESucsessFail: eSucsess if line is all white spaces or eFail if its contains other characters
*
*******************************************************************************/
static ESucsessFail isBlank(char const* line)
{
	char const* ch;
	ESucsessFail is_blank = eSucsess;

	// Iterate through each character.
	for (ch = line; (*ch != '\0') && (*ch != '\n'); ++ch)
	{
		if (*ch != ' ')
		{
			// Found a non-whitespace character.
			is_blank = eFail;
			break;
		}
	}

	return is_blank;
}

/******************************************************************************
* Function : getInstructionType()
*
*  This function is used to get the instruction type: external or entry
*  
*
* \param
*  char const* line:	INPUT:  pointer to the line
*
*
* \return
*  EInstructionCmnd: eExternInstruction  or eEntryInstruction and eNoInstructionCmnd on failure
*
*******************************************************************************/
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

/******************************************************************************
* Function : getdataCmnd()
*
*  This function is used to get the data command type: structure,string or data
*
*
* \param
*  char const* line:	INPUT:  pointer to the line
*
*
* \return
*  EDataCmnd: the type of data or eNoDataCmnd on failure
*
*******************************************************************************/
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

/******************************************************************************
* Function : getCodeCommand()
*
*  This function is used to get the command opcode
*
*
* \param
*  char const* line:	INPUT:  pointer to the line
*
*
* \return
*  ECodeCmnd: the opcode of the command or eNoCodeCmnd on failure
*
*******************************************************************************/
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

/******************************************************************************
* Function : isWordExistInLine()
*
*  This function is used to check if a word exist in line in full: 
*  at the begining or end of the line
*  or with spaces or ',' after
*
* \param
*  char const* line:	INPUT: pointer to the line
*  char const* word:	INPUT: pointer to the word we looking for
*
*
* \return
*  ESucsessFail: eSucsess if word exist
*
*******************************************************************************/
static ESucsessFail isWordExistInLine(char const* line, char const* word)
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

		if((*endPos == ',')	 ||
			(*endPos == ' ') ||
			(*endPos == '\n')||
			(*endPos == 0))
		{
			wordExist = 1;
		}
	}

	if (wordExist)
	{
		// Assume a command word (if not the m_operandPosPtr will not be used)
		m_operandPosPtr = endPos;
	}
	return wordExist;
}

/******************************************************************************
* Function : handleTag()
*
*  This function checks if a TAG exist in a line and add it to the TAG list
*
* \param
*  char const* line: INPUT: pointer to the line
*  EtagType		     INPUT: the tag type: Code or data
*
*
* \return
*  ESucsessFail: eSucsess if added sucseesfuly or tag not exist in the line
*
*******************************************************************************/
ESucsessFail handleTag(char const* line , EtagType tagType)
{
	ESucsessFail res = eSucsess;
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
		res = addDataTagElemet(strtPos, dotPos - strtPos, tagType);
		
	}

	return res;
}

/******************************************************************************
* Function : setOperandAddrParams()
*
*  This function sets the addressing type for an operand input which contains the operand string
*
* \param
*  SOperandAdressingParams* pOperandAdressingParams: INPUT/OUTPUT: pointer to operand data to update
*  int operIdx		     INPUT: the operand index first=0,second=1
*
*
* \return
*  ESucsessFail: eSucsess if the operand params were set correctly
*
*******************************************************************************/
ESucsessFail setOperandAddrParams(SOperandAdressingParams* pOperandAdressingParams,int operIdx)
{
	ESucsessFail res = eSucsess;
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

/******************************************************************************
* Function : generateCodeForOperand()
*
*  This function sets all the code information which is needed for writing a code line for an operand
*
* \param
*  SCodeinfo* pCodeInfo: INPUT/OUTPUT: a pointer to the code information to be set
*  SOperandAdressingParams* pOperandAdressingParams: INPUT: pointer to operand data
*  int callCount:	INPUT: the call count for this operand ('structs' operand coding needs 2 calls for this function)
*
*
* \return
*  ESucsessFail: eSucsess if the code information was set correctly
*
*******************************************************************************/
ESucsessFail generateCodeForOperand(SCodeinfo* pCodeInfo, SOperandAdressingParams* pAddrParams,int callCount)
{
	ESucsessFail res = eSucsess;
	ESucsessFail isExternalTag = eFail;
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

		/* Set the struct offset*/
		if (callCount == 1)
		{
			pCodeInfo->code.valBits.are = eAreAbsulute;
			pCodeInfo->code.valBits.val = pAddrParams->baseRelativeAddrParams.tagOffset;
		}
		else // Set the struct TAG
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

/******************************************************************************
* Function : freeAndResetCodeInfo()
*
*  This function reset fields and free all alocated data in SCodeinfo
*
* \param
*  SCodeinfo* pCodeInfo: INPUT: a pointer to the code information to be free
*  
*
* \return
*  ESucsessFail: eSucsess if the code was free correctly
*
*******************************************************************************/
ESucsessFail freeAndResetCodeInfo(SCodeinfo* pCodeInfo)
{
	ESucsessFail res = eSucsess;

	if (pCodeInfo->tag != NULL)
	{
		free(pCodeInfo->tag);
	}

	memset(pCodeInfo, 0, sizeof(*pCodeInfo));
	return res;
}

/******************************************************************************
* Function : handleStringCmnd()
*
*  This function handles ".string" instruction and alocate it
*
* \param
*  char const* line, INPUT: a pointer to line which contains the ".string" instruction
*
*
* \return
*  ESucsessFail: eSucsess if the ".string" was allocated correctly
*
*******************************************************************************/
ESucsessFail handleStringCmnd(char const* line)
{
	ESucsessFail res = eSucsess;
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

/******************************************************************************
* Function : generateCodeForTag()
*
*  This function generate code information for a TAG
*
* \param
*  SCodeinfo* pCodeInfo, INPUT/OUPUT: a pointer to the code info to be set
*
*
* \return
*  ESucsessFail: eSucsess if the code was set correctly
*
*******************************************************************************/
ESucsessFail generateCodeForTag(SCodeinfo* pCodeInfo)
{
	ESucsessFail res = eSucsess;
	ESucsessFail isExternalTag = eSucsess;
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
			addCodeElemet(*pCodeInfo);
		}
	}
	else /* Tag not exist*/
	{
		printf("Err tag %s used but not defined\n", pCodeInfo->tag);
		res = eFail;
	}
	
	return res;
}

/******************************************************************************
* Function : isNumberOfOperandsValid()
*
*  This function checks if the command has the correct number of opperands
*
* \param
*  int operandNum, INPUT: The number of operands founds on the command line
*  ECodeCmnd cmnd, INPUT: The command founds on the command line
*
* \return
*  ESucsessFail: eSucsess if the number of params is valid
*
*******************************************************************************/
ESucsessFail isNumberOfOperandsValid(int operandNum, ECodeCmnd cmnd)
{
	ESucsessFail res = eSucsess;
	int expectedParamsNumber[] = 
	{
		2,/*emovCodeCmnd*/
		2,/*ecmpCodeCmnd*/
		2,/*eaddCodeCmnd*/
		2,/*esubCodeCmnd*/
		1,/*enotCodeCmnd*/
		1,/*eclrCodeCmnd*/
		2,/*eleaCodeCmnd*/
		1,/*eincCodeCmnd*/
		1,/*edecCodeCmnd*/
		1,/*ejmpCodeCmnd*/
		1,/*ebneCodeCmnd*/
		1,/*egetCodeCmnd*/
		1,/*eprnCodeCmnd*/
		1,/*ejsrCodeCmnd*/
		0,/*ertsCodeCmnd*/
		0 /*ehltCodeCmnd*/
	};

	if (cmnd < sizeof(expectedParamsNumber) / sizeof(expectedParamsNumber[0]) && (cmnd >= 0))
	{
		res = (operandNum == expectedParamsNumber[cmnd]);

		if (!res)
		{
			printf("Invalid number of operands, expected %d got %d\n",expectedParamsNumber[cmnd],operandNum);
		}
	}
	else
	{
		printf("Internal bug, unexpected behaviour\n");
		res = eFail;
	}
	

	return res;
}

/******************************************************************************
* Function : handleCodeLine()
*
*  This function handles a code line and writes all its data to the code list
*
* \param
*  char const* line, INPUT: the code line to be handled
*  ECodeCmnd cmnd, INPUT: the opcode of the command
*
*
* \return
*  ESucsessFail: eSucsess if the line was handled correctly
*
*******************************************************************************/
ESucsessFail handleCodeLine(char const* line, ECodeCmnd cmnd)
{
	ESucsessFail res = handleTag(line, eCodeTag);
	int numOfOperands = 0;
	SOperandAdressingParams operandAdressingParams[MAX_OPERAND_NUM] = { 0, };
	int operIdx = 0;
	SCodeinfo codeinfo = { 0, };
	int activationCount = 0; /* Used for creating 2 elements for struct*/

	// Search for operands
	numOfOperands = setOperandsString(operandAdressingParams);
	
	// Check if the number of operands is valid
	if (!isNumberOfOperandsValid(numOfOperands,cmnd))
	{
		printf("Err on line %d invalid number of operands\n", getCurrentLineNumber());
		res = eFail;
		return res;
	}
	
	// Process operands
	for (operIdx = 0; operIdx < numOfOperands; operIdx++)
	{
		if (!setOperandAddrParams(&operandAdressingParams[operIdx], operIdx))
		{
			printf("Err on line %d invalid operands addressing\n", getCurrentLineNumber());
			res = eFail;
			return res;
		}
	}

	//Set the code opcode line data
	freeAndResetCodeInfo(&codeinfo);
	codeinfo.code.cmndBits.are = eAreAbsulute;
	codeinfo.code.cmndBits.opcode = cmnd;
	
	/* Set the addrresing type*/
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
	
	// Set the code operands
	for (operIdx = 0; operIdx < numOfOperands; operIdx++)
	{
		/* reset on each operand index*/
		activationCount = 0;

		if (!generateCodeForOperand(&codeinfo, &operandAdressingParams[operIdx],activationCount))
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

					if (!generateCodeForOperand(&codeinfo, &operandAdressingParams[operIdx], activationCount))
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

/******************************************************************************
* Function : handleDataLine()
*
*  This function handles a data line and writes all its data to the data array
*
* \param
*  char const* line, INPUT: the code line to be handled
*  EDataCmnd cmnd, INPUT: the data type
*
*
* \return
*  ESucsessFail: eSucsess if the line was handled correctly
*
*******************************************************************************/
ESucsessFail handleDataLine(char const* line, EDataCmnd cmnd)
{
	// Add TAG if exist to the list of tags
	ESucsessFail res = handleTag(line, eDataTag);

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

/******************************************************************************
* Function : setOperandsString()
*
*  This function finds and sets all the operand strings found in a code line
*  the function uses the m_operandPosPtr member to get the starting point of the search
* 
* \param
*  SOperandAdressingParams operandAddrPrmsArray[], INPUT/OUTPUT: The array of params to be set
*  
*
*
* \return
*  int: The number of operands found
*
*******************************************************************************/
int setOperandsString(SOperandAdressingParams operandAddrPrmsArray[])
{
	int numOperandFound = 0;
	char const* currPos = m_operandPosPtr;
	char const* strtPos = NULL;
	int wordLength = 0;

	if (m_operandPosPtr == NULL)
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

/******************************************************************************
*Function : analizeLine()
*
* This function is analizing a code line and generates its info
*
* \param
* int lineNumber, INPUT: The line number to analize
* char* line, INPUT: The line data
*
* \return
* ESucsessFail: eSucsess if the lines is valid
*
*******************************************************************************/
ESucsessFail analizeLine(int lineNumber, char const* line)
{
	ESucsessFail res = eSucsess;
	int additionalInfo = 0;

	setCurrentLineNumber(lineNumber);

	// First we chek the line type
	ELineType lineType = getLineType(line, &additionalInfo);

	if (lineType == eCodeLine)
	{
		ECodeCmnd cmnd = (ECodeCmnd)additionalInfo;

		res = handleCodeLine(line, cmnd);
	}
	else if (lineType == eDataLine)
	{
		EDataCmnd cmnd = (EDataCmnd)additionalInfo;
		res = handleDataLine(line, cmnd);
	}

	return res;
}

/******************************************************************************
*Function : doFirstFileScan()
*
* This function is doing the first file scan by analizing line by line
*
* \param
* char* fileData, INPUT: The file data as a string
*
*
* \return
* ESucsessFail: eSucsess if all the lines were valid
*
*******************************************************************************/
ESucsessFail doFirstFileScan(char* fileData)
{
	ESucsessFail scanStatus = eSucsess;
	char line[MAX_LINE_LENGTH];
	int lineLength = 0;

	// read line
	char* startPos = fileData;
	int lineNumber = 0;

	while (1)
	{
		lineNumber++;

		startPos = readLine(startPos, line);

		if (startPos != NULL)
		{
			printf("Handle line %d: %s", lineNumber, line);

			if (!analizeLine(lineNumber, line))
			{
				scanStatus = eFail;
				printf("Handle line failed: %d", lineNumber);
			}
		}
		else
		{
			break;
		}

	}

	return scanStatus;
}

#include "services.h"

SEntryElement* m_entryList = NULL; // List of all entry and there address
SExternElement* m_externList = NULL; // List of all extern and there use
STagParams* m_dataTagList = NULL; // list of all defined TAG in the code
SCodeElement* m_codeList = NULL; // The code that was generated
unsigned short* m_pDataSeqtion = NULL; // The data seq
int m_codePos = 0;
int m_dataPos = 0;
int m_maxDataLength = MAX_ASSEBLER_FILE_SIZE;

// Private functions
static int isBlank(char const* line);
static EInstructionCmnd getInstructionType(char const* line);
static EDataCmnd getdataCmnd(char const* line);
static ECodeCmnd getCodeCommand(char const* line);
static int isWordExistInLine(char const* line, char const* word);

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
		printf("Allocation failed\n");
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

	int sizeToCopy = 1 + endpos - startPos;

	if (sizeToCopy + 1 < MAX_LINE_LENGTH)
	{
		strncpy(line, startPos, sizeToCopy);
		line[sizeToCopy] = 0;
		newStartPos = endpos + 1;
	}
	else
	{
		printf("invalid line length\n");
	}

	return newStartPos;
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

	m_codePos = 0;
	m_dataPos = 0;

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

eSucsessFail addEntryElemet(unsigned short address,char* tagName)
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

		newElem->address = address;

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
				printf("memory alloc fail\n");
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
		printf("memory alloc fail\n");
		res = eFail;
	}

	return res;
}

eSucsessFail addDataTagElemet(unsigned short address, char* tagName)
{
	eSucsessFail res = eSucsess;

	STagParams* latest = m_dataTagList;
	STagParams* prev = m_dataTagList;

	// Find the latest element
	while (latest != NULL)
	{
		prev = latest;
		latest = latest->nextEelement;
	}

	STagParams* newElem = malloc(sizeof(STagParams));

	if (m_entryList == NULL)
	{
		m_dataTagList = newElem;
	}

	if (newElem)
	{
		if (prev != NULL)
		{
			prev->nextEelement = newElem;
		}

		newElem->tagAddr = address;

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
				printf("memory alloc fail\n");
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
		printf("memory alloc fail\n");
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
				printf("memory alloc fail\n");
				res = eFail;
			}
		}
	}
	else
	{
		printf("memory alloc fail\n");
		res = eFail;
	}

	return res;
}

eSucsessFail addExternElemet(unsigned short address, char* tagName)
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
				printf("unexpected null pointer\n");
				res = eFail;
				return res;
			}

			// Set the new element
			addrprev->nextEelement = malloc(sizeof(SAddressElement));

			if (addrprev->nextEelement)
			{
				((SAddressElement*)addrprev->nextEelement)->address = address;
				((SAddressElement*)addrprev->nextEelement)->nextEelement = NULL;
				return res;
			}
			else
			{
				printf("memory alloc fail\n");
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
			printf("memory alloc fail\n");
			res = eFail;
			return res;
		}

		newElem->externUseAddrList = malloc(sizeof(SAddressElement));
		if (newElem->externUseAddrList)
		{
			((SAddressElement*)newElem->externUseAddrList)->address = address;
			((SAddressElement*)newElem->externUseAddrList)->nextEelement = NULL;
			
		}
		else
		{
			printf("memory alloc fail\n");
			res = eFail;
			return res;
		}
	}
	else
	{
		printf("memory alloc fail\n");
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
						printf("Syntact err on line %d: %s\n", lineNumber, line);
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

	return cmnd;
}

static int isWordExistInLine(char const* line, char const* word)
{
	int wordExist = 0;
	char* pos;

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
		char* endPos = (pos + strlen(word));

		if( (*endPos == ' ') ||
			(*endPos == '\n') ||
			(*endPos == 0))
		{
			wordExist = 1;
		}
	}

	return wordExist;
}
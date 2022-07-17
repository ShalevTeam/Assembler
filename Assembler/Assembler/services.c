#include "services.h"

SEntryElement* m_entryList = NULL; // List of all entry and there address
SExternElement* m_externList = NULL; // List of all extern and there use
STagParams* m_dataTagList = NULL; // list of all defined TAG in the code
SCodeElement* m_codeList = NULL; // The code that was generated
unsigned short DataSeqtion[MAX_ASSEBLER_FILE_SIZE]; // The data seq
int codePos = 0;
int dataPos = 0;

int reallocAndCopyBuffer(char** allocatedBuf, int oldSize)
{
	int newSize = oldSize * 2;

	// Save the address of the old data
	char** oldBuf = allocatedBuf;

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

void initDataBase()
{
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

	memset(DataSeqtion, 0, sizeof(DataSeqtion));
	codePos = CODE_INITIAL_ADDR;
	dataPos = CODE_INITIAL_ADDR;
}

void addEntryElemet(unsigned short address,char* tagName)
{
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
	}
}

void addDataTagElemet(unsigned short address, char* tagName)
{
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
	}
}

void addCodeElemet(SCodeinfo codeInfo)
{
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
			}
			else
			{
				printf("memory alloc fail\n");
			}
		}
	}
	else
	{
		printf("memory alloc fail\n");
	}
}

void addExternElemet(unsigned short address, char* tagName)
{
	SExternElement* latest = m_externList;
	SExternElement* prev = m_externList;
	EFuncResSucsessFail tagFound = EFuncResFail;

	// Check if the tag exist
	while (latest != NULL)
	{
		if (strcmp(latest->tagName, tagName) == 0)
		{
			// Tag found
			tagFound = EFuncResSucsess;
			
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
				return;
			}

			// Set the new element
			addrprev->nextEelement = malloc(sizeof(SAddressElement));

			if (addrprev->nextEelement)
			{
				((SAddressElement*)addrprev->nextEelement)->address = address;
				((SAddressElement*)addrprev->nextEelement)->nextEelement = NULL;
				return;
			}
			else
			{
				printf("memory alloc fail\n");
				return;
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
		}
	}
	else
	{
		printf("memory alloc fail\n");
	}
}
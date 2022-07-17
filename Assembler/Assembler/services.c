#include "services.h"

SEntryElement* m_entryList = NULL; // List of all entry and there address
SExternElement* m_externList = NULL; // List of all extern and there use
STagParams* m_dataTagList = NULL; // list of all defined TAG in the code
SCodeElement* m_codeList = NULL; // The code that was generated
unsigned short* m_pDataSeqtion = NULL; // The data seq
int m_codePos = 0;
int m_dataPos = 0;
int m_maxDataLength = MAX_ASSEBLER_FILE_SIZE;

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

EFuncResSucsessFail initDataBase()
{
	EFuncResSucsessFail res = EFuncResSucsess;

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
		res = EFuncResFail;
	}

	m_codePos = CODE_INITIAL_ADDR;
	m_dataPos = CODE_INITIAL_ADDR;

	return res;
}

EFuncResSucsessFail addData(unsigned short val)
{
	EFuncResSucsessFail res = EFuncResSucsess;

	if (m_dataPos >= m_maxDataLength)
	{
		m_maxDataLength = reallocAndCopyBuffer(&m_pDataSeqtion, m_maxDataLength* sizeof(unsigned short));
	}

	if (m_maxDataLength == 0)
	{
		res = EFuncResFail;
	}
	else
	{
		m_dataPos++;
		*m_pDataSeqtion = val;
	}

	return res;
}

EFuncResSucsessFail addEntryElemet(unsigned short address,char* tagName)
{
	EFuncResSucsessFail res = EFuncResSucsess;

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
				res = EFuncResFail;
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
		res = EFuncResFail;
	}

	return res;
}

EFuncResSucsessFail addDataTagElemet(unsigned short address, char* tagName)
{
	EFuncResSucsessFail res = EFuncResSucsess;

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
				res = EFuncResFail;
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
		res = EFuncResFail;
	}

	return res;
}

EFuncResSucsessFail addCodeElemet(SCodeinfo codeInfo)
{
	EFuncResSucsessFail res = EFuncResSucsess;
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
				res = EFuncResFail;
			}
		}
	}
	else
	{
		printf("memory alloc fail\n");
		res = EFuncResFail;
	}

	return res;
}

EFuncResSucsessFail addExternElemet(unsigned short address, char* tagName)
{
	EFuncResSucsessFail res = EFuncResSucsess;
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
				res = EFuncResFail;
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
				res = EFuncResFail;
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
			res = EFuncResFail;
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
			res = EFuncResFail;
			return res;
		}
	}
	else
	{
		printf("memory alloc fail\n");
		res = EFuncResFail;
	}

	return res;
}
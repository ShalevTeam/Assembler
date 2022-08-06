
#include "databaseHandler.h"

/* Private memebers */
static SEntryElement* m_entryList = NULL; // List of all entry and there address
static int m_lineNumber = 0;
SExternElement* m_externList = NULL; // List of all extern and there use
STagParams* m_dataTagList = NULL; // list of all defined TAG in the code
SCodeElement* m_codeList = NULL; // The code that was generated
unsigned short* m_pDataSeqtion = NULL; // The data seq
static int m_codePos = CODE_INITIAL_ADDR;
static int m_dataPos = CODE_INITIAL_ADDR;
static int m_maxDataLength = MAX_ASSEBLER_FILE_SIZE;

/******************************************************************************
* Function : initDataBase()
*
*  This function inits all the database objects. needs to be call before handling a new code file
*
*
* \param
*  None.
*
*
*
* \return
*  None.
*
*******************************************************************************/
ESucsessFail initDataBase()
{
	ESucsessFail res = eSucsess;

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

	m_pDataSeqtion = malloc(sizeof(unsigned short) * m_maxDataLength);

	if (m_pDataSeqtion == NULL)
	{
		res = eFail;
	}

	m_codePos = CODE_INITIAL_ADDR;
	m_dataPos = CODE_INITIAL_ADDR;

	return res;
}

/******************************************************************************
* Function : getEntryList()
*
*  This function returns a pointer to the entry list
*  
*
* \param
*  None.
*
*
*
* \return
*  SEntryElement* : A pointer to the entry list
*
*******************************************************************************/
SEntryElement* getEntryList()
{
	return m_entryList;
}

/******************************************************************************
* Function : getExternalList()
*
*  This function returns a pointer to the externals list
*
*
* \param
*  None.
*
*
*
* \return
*  SExternElement*: A pointer to the externals list
*
*******************************************************************************/
SExternElement* getExternalList()
{
	return m_externList;
}

/******************************************************************************
* Function : getTagList()
*
*  This function returns a pointer to the TAG list
*
*
* \param
*  None.
*
*
*
* \return
*  STagParams*: A pointer to the TAG list
*
*******************************************************************************/
STagParams* getTagList()
{
	return m_dataTagList;
}

/******************************************************************************
* Function : getCodeList()
*
*  This function returns a pointer to the code list
*
*
* \param
*  None.
*
*
*
* \return
*  SCodeElement*: A pointer to the code list
*
*******************************************************************************/
SCodeElement* getCodeList()
{
	return m_codeList;
}

/******************************************************************************
* Function : getDataArray()
*
*  This function returns a pointer to the data segment array
*
*
* \param
*  None.
*
*
*
* \return
*  unsigned short*: A pointer to the data segment array
*
*******************************************************************************/
unsigned short* getDataArray()
{
	return m_pDataSeqtion;
}

/******************************************************************************
* Function : getCurrentLineNumber()
*
*  This function returns the current line number which is handled
*
*
* \param
*  None.
*
*
*
* \return
*  int: the line number
*
*******************************************************************************/
int getCurrentLineNumber()
{
	return m_lineNumber;
}

/******************************************************************************
* Function : setCurrentLineNumber()
*
*  This function sets the current line number
*
*
* \param
*  int lineNumber, INPUT: The line number to set
*
*
*
* \return
*  None.
*
*******************************************************************************/
void setCurrentLineNumber(int lineNumber)
{
	m_lineNumber = lineNumber;
}

/******************************************************************************
* Function : addEntryElemet()
*
*  This function adds an element to the Entry list
*
*
* \param
*  unsigned short address, INPUT: The addrress of the Entry
*  char const* tagName, INPUT: The Entry TAG
*
*
* \return
*  ESucsessFail: eSucsess if added OK
*
*******************************************************************************/
ESucsessFail addEntryElemet(unsigned short address, char const* tagName)
{
	ESucsessFail res = eSucsess;

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
		newElem->nextEelement = NULL;

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
				printf("Err on line %d memory alloc fail\n", m_lineNumber);
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
		printf("Err on line %d memory alloc fail\n", m_lineNumber);
		res = eFail;
	}

	return res;
}

/******************************************************************************
* Function : addDataTagElemet()
*
*  This function adds an element to the Tags list
*
*
* \param
*  char const* tagName, INPUT: The Entry TAG
*  int tagLength, INPUT: The tag length
*  EtagType tagType, INPUT: The tad type
*
* \return
*  ESucsessFail: eSucsess if added OK
*
*******************************************************************************/
ESucsessFail addDataTagElemet(char const* tagName, int tagLength, EtagType tagType)
{
	ESucsessFail res = eSucsess;

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
			printf("Err on line %d Tag define twice: %s", m_lineNumber, tagName);

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

		if (tagType == eCodeTag)
		{
			newElem->tagAddr.valBits.val = m_codePos;
		}
		else
		{
			newElem->tagAddr.valBits.val = m_dataPos;
		}
		
		newElem->tagType = tagType;
		newElem->nextEelement = NULL;

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
				printf("Err on line %d memory alloc fail\n", m_lineNumber);
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
		printf("Err on line %d memory alloc fail\n", m_lineNumber);
		res = eFail;
	}

	return res;
}

/******************************************************************************
* Function : addDataTagElemet()
*
*  This function adds an element to the data segmnet array
*
*
* \param
*  unsigned short val, INPUT: The value to add
*  
*
* \return
*  ESucsessFail: eSucsess if added OK
*
*******************************************************************************/
ESucsessFail addData(unsigned short val)
{
	ESucsessFail res = eSucsess;

	if (m_dataPos >= m_maxDataLength)
	{
		m_maxDataLength = reallocAndCopyBuffer(&m_pDataSeqtion, m_maxDataLength * sizeof(unsigned short));
	}

	if (m_maxDataLength == 0)
	{
		res = eFail;
	}
	else
	{
		m_pDataSeqtion[m_dataPos] = val;
		m_dataPos++;
	}

	return res;
}

/******************************************************************************
* Function : addDataTagElemet()
*
*  This function adds an element to the externals list
*
*
* \param
*  char const* tagName, INPUT: The tag name
*
*
* \return
*  ESucsessFail: eSucsess if added OK
*
*******************************************************************************/
ESucsessFail addExternElemet(char const* tagName)
{
	ESucsessFail res = eSucsess;
	SExternElement* latest = m_externList;
	SExternElement* prev = m_externList;
	ESucsessFail tagFound = eFail;

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
				printf("Err on line %d unexpected null pointer\n", m_lineNumber);
				res = eFail;
				return res;
			}

			// Set the new element
			addrprev->nextEelement = malloc(sizeof(SAddressElement));

			if (addrprev->nextEelement)
			{
				((SAddressElement*)addrprev->nextEelement)->address.valBits.val = m_codePos;
				((SAddressElement*)addrprev->nextEelement)->nextEelement = NULL;
				return res;
			}
			else
			{
				printf("Err on line %d memory alloc fail\n", m_lineNumber);
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

		newElem->nextEelement = NULL;
		// If we got here -> Generate new SExternElement element
		newElem->tagName = malloc(strlen(tagName) + 1);
		if (newElem->tagName)
		{
			strcpy(newElem->tagName, tagName);
			newElem->nextEelement = NULL;
		}
		else
		{
			printf("Err on line %d memory alloc fail\n", m_lineNumber);
			res = eFail;
			return res;
		}

		newElem->externUseAddrList = malloc(sizeof(SAddressElement));
		if (newElem->externUseAddrList)
		{
			((SAddressElement*)newElem->externUseAddrList)->address.valBits.val = m_codePos;
			((SAddressElement*)newElem->externUseAddrList)->nextEelement = NULL;

		}
		else
		{
			printf("Err on line %d memory alloc fail\n", m_lineNumber);
			res = eFail;
			return res;
		}
	}
	else
	{
		printf("Err on line %d memory alloc fail\n", m_lineNumber);
		res = eFail;
	}

	return res;
}

/******************************************************************************
* Function : addCodeElemet()
*
*  This function adds an element to the code list
*
*
* \param
*  SCodeinfo codeInfo, INPUT: The code data
*
*
* \return
*  ESucsessFail: eSucsess if added OK
*
*******************************************************************************/
ESucsessFail addCodeElemet(SCodeinfo codeInfo)
{
	ESucsessFail res = eSucsess;
	SCodeElement* latest = m_codeList;
	SCodeElement* prev = m_codeList;

	// Find the latest element
	while (latest != NULL)
	{
		prev = latest;
		latest = latest->nextEelement;
	}

	SCodeElement* newElem = malloc(sizeof(SCodeElement));

	if (m_codeList == NULL)
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
		newElem->nextEelement = NULL;

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
				printf("Err on line %d memory alloc fail\n", m_lineNumber);
				res = eFail;
			}
		}
	}
	else
	{
		printf("Err on line %d memory alloc fail\n", m_lineNumber);
		res = eFail;
	}

	if (res)
	{
		/* Increas the positions of code and data*/
		m_codePos++;
	}

	return res;
}

/******************************************************************************
* Function : istagExist()
*
*  This function checks if a tag was already defined
*
*
* \param
*  char const* tag, INPUT: The tags name
*  ESucsessFail* pIsExternalTag, INPUT/OUTPUT: sets eSucsess if the tag is external
*  short* pTagAddr, INPUT/OUTPUT: sets the tags addrress
*
* \return
*  ESucsessFail: eSucsess if added OK
*
*******************************************************************************/
ESucsessFail istagExist(char const* tag, ESucsessFail* pIsExternalTag, short* pTagAddr)
{
	ESucsessFail res = eFail;
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

/******************************************************************************
* Function : reallocAndCopyBuffer()
*
*  This function is used to allocate a buffer to twice the size of the original size and
*  copy all the old data to the new buffer
*
* \param
*  void** allocatedBuf: INPUT/OUTPUT:pointer to the old/new buffer
*  int oldSize:			INPUT:the size of the old buffer
*
* \return
*  int: the new size or 0 if fail
*
*******************************************************************************/
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


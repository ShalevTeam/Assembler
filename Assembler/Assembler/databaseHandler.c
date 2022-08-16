
#include "databaseHandler.h"

/* Private memebers */
static SEntryElement* m_entryList = NULL; // List of all entry and there address
static int m_lineNumber = 0;
SExternElement* m_externList = NULL; // List of all extern and there use
STagParams* m_dataTagList = NULL; // list of all defined TAG in the code
SCodeElement* m_codeList = NULL; // The code that was generated
ScodeWord* m_pDataSeqtion = NULL; // The data seq
static int m_codePos = CODE_INITIAL_ADDR;
static int m_dataPos = 0;
static int m_maxDataLength = MAX_ASSEBLER_FILE_SIZE;
char m_asemblerFileName[MAX_FILE_NAME];
char m_objectFileName[MAX_FILE_NAME];
char m_externFileName[MAX_FILE_NAME];
char m_entryFileName[MAX_FILE_NAME];

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

	m_pDataSeqtion = malloc(sizeof(ScodeWord) * m_maxDataLength);

	if (m_pDataSeqtion == NULL)
	{
		res = eFail;
	}

	m_codePos = CODE_INITIAL_ADDR;
	m_dataPos = 0;

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
* Function : getObjectFileName()
*
*  This function returns a pointer to a string containing the object file name
*
*
* \param
*  None.
*
*
*
* \return
*  char*: A pointer to the object file name
*
*******************************************************************************/
char* getObjectFileName()
{
	return m_objectFileName;
}

char* getEntryFileName()
{
	return m_entryFileName;
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
ScodeWord* getDataArray()
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

int getDataSize()
{
	return m_dataPos;
}

/******************************************************************************
* Function : setAsemblerFileName()
*
*  This function sets the currently handled assembly file name
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
void setAsemblerFileName(char const* fileName)
{
	if (strlen(fileName) + strlen(".ext") < MAX_FILE_NAME)
	{
		strcpy(m_objectFileName, fileName);
		strcat(m_objectFileName,".ob");

		strcpy(m_externFileName, fileName);
		strcat(m_externFileName, ".ext");

		strcpy(m_entryFileName, fileName);
		strcat(m_entryFileName, ".ent");
	}
	else
	{
		printf("internal Error - Assembler file name to long");
	}
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
ESucsessFail addEntryElemet(char const* tagName)
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
			printf("Err on line %d internal err invalid entry\n", m_lineNumber);
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
			newElem->tagAddr.addrVal.val = m_codePos;
		}
		else
		{
			newElem->tagAddr.addrVal.val = m_dataPos;
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
*  short val, INPUT: The value to add
*  
*
* \return
*  ESucsessFail: eSucsess if added OK
*
*******************************************************************************/
ESucsessFail addData(short val)
{
	ESucsessFail res = eSucsess;

	if (m_dataPos >= m_maxDataLength)
	{
		m_maxDataLength = reallocAndCopyBuffer(&m_pDataSeqtion, m_maxDataLength * sizeof(ScodeWord));
	}

	if (m_maxDataLength == 0)
	{
		res = eFail;
	}
	else
	{
		m_pDataSeqtion[m_dataPos].dataVal.val = val;
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
			break;
		}

		prev = latest;
		latest = latest->nextEelement;
	}

	if (!tagFound)
	{
		latest = malloc(sizeof(SExternElement));

		if (latest)
		{
			latest->externUseAddrList = NULL;

			latest->tagName = malloc(strlen(tagName)+1);

			if (latest->tagName)
			{
				strcpy(latest->tagName, tagName);

				if (m_externList == NULL)
				{
					m_externList = latest;
				}

				if (prev != NULL)
				{
					prev->nextEelement = latest;
				}

				latest->nextEelement = NULL;
			}
			else
			{
				printf("Err on line %d memory alloc fail\n", m_lineNumber);
				res = eFail;
			}
			
		}
		else
		{
			printf("Err on line %d memory alloc fail\n", m_lineNumber);
			res = eFail;
		}
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

	codeInfo.codeAddress.val = m_codePos;

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
* Function : addExternUsage()
*
*  This function sets usage addrress for an external tag
*
*
* \param
*  SCodeinfo codeInfo, INPUT: The code addrress and tag
*
* \return
*  ESucsessFail: eSucsess if sets OK
*
*******************************************************************************/
ESucsessFail addExternUsage(SCodeinfo codeInfo)
{
	ESucsessFail res = eFail;
	SExternElement* curPosOnExternalList = m_externList;
	SAddressElement* addrPtr = NULL;
	SAddressElement* prevAddrPtr = NULL;
	
	if (m_externList == NULL)
	{
		printf("Err on line %d Using external which is not defined\n", getCurrentLineNumber());
	}

	/* Go ever the external list*/
	while(curPosOnExternalList != NULL)
	{
		/*Search for the tag in the external list*/
		if (strcmp(codeInfo.tag, curPosOnExternalList->tagName) == 0)
		{
			/* get the addrress list*/
			addrPtr = curPosOnExternalList->externUseAddrList;

			/* Handle the first element in the addrress list*/
			if (addrPtr == NULL)
			{
				/* Alloc the addrress list */
				curPosOnExternalList->externUseAddrList = malloc(sizeof(curPosOnExternalList->externUseAddrList));

				/* Set the addrress element fields*/
				if (curPosOnExternalList->externUseAddrList)
				{
					curPosOnExternalList->externUseAddrList->address = codeInfo.codeAddress;
					curPosOnExternalList->externUseAddrList->nextEelement = NULL;
					res = eSucsess;
				}
				else
				{
					printf("internal err Cant alloc external adress\n");
				}
			}
			else /* Not first element in the addrress list*/
			{
				/* Find the last addrress*/
				while (addrPtr != NULL)
				{
					prevAddrPtr = addrPtr;
					addrPtr = addrPtr->nextEelement;
				}

				addrPtr = malloc(sizeof(*addrPtr));

				/* Set fields to the new addrress element*/
				if (addrPtr)
				{
					addrPtr->nextEelement = NULL;
					addrPtr->address = codeInfo.codeAddress;
					prevAddrPtr->nextEelement = addrPtr;
					res = eSucsess;
				}
				else
				{
					printf("internal err Cant alloc external adress\n");
				}
			}
		}

		curPosOnExternalList = curPosOnExternalList->nextEelement;
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
			*pTagAddr = pCurrPosTagList->tagAddr.addrVal.val;
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

/******************************************************************************
* Function : updateEntryAddress()
*
*  This function scans the entry list and update the addrress of the tags
*
*
* \param
*  None.
*
* \return
*  ESucsessFail eSucsess if update Sucsessfully
*
*******************************************************************************/
ESucsessFail updateEntryAddress()
{
	ESucsessFail res = eSucsess;
	ESucsessFail tagFound = eFail;
	SEntryElement* entryPos = m_entryList;
	STagParams* tagPos = m_dataTagList;

	/* Update the base addrress of data tags */
	while (entryPos != NULL)
	{
		/* Find the tag in the tag list */
		while (tagPos != NULL)
		{
			/* Asume tag not found */
			tagFound = eFail;

			if (strcmp(tagPos->tagName, entryPos->tagName) == 0)
			{
				tagFound = eSucsess;
				entryPos->address.addrVal.val = tagPos->tagAddr.addrVal.val;
				break;
			}

			tagPos = tagPos->nextEelement;
		}

		if (!tagFound)
		{
			res = eFail;
			entryPos->address.addrVal.val = 0;
			printf("Tag not found for entry %s", entryPos->tagName);
		}

		entryPos = entryPos->nextEelement;
	}
}

/******************************************************************************
* Function : updateTagsBaseAddress()
*
*  This function scans the tags list and update the base addrress of data tags
*  
*
* \param
*  None.
*
* \return
*  ESucsessFail eSucsess if update Sucsessfully
*
*******************************************************************************/
ESucsessFail updateTagsBaseAddress()
{
	ESucsessFail res = eSucsess;

	STagParams* pos = m_dataTagList;

	/* Update the base addrress of data tags */
	while (pos != NULL)
	{
		if (pos->tagType == eDataTag)
		{
			pos->tagAddr.addrVal.val += m_codePos;
		}
		pos = pos->nextEelement;
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
ESucsessFail generateCodeForTag()
{
	ESucsessFail res = eSucsess;
	ESucsessFail isExternalTag = eSucsess;
	SCodeElement* currPos = m_codeList;
	short tagAddr = 0;

	while (currPos != NULL)
	{
		/* Check if this code needs tag update */
		if (currPos->codeInfo.tag == NULL)
		{
			/* Do nothing*/
		}
		else
		{
			// Check if tag exist
			if (istagExist(currPos->codeInfo.tag, &isExternalTag, &tagAddr))
			{
				if (isExternalTag)
				{
					currPos->codeInfo.code.valBits.are = eAreExternal;
					currPos->codeInfo.code.valBits.val = 0;
					addExternUsage(currPos->codeInfo);
				}
				else
				{
					currPos->codeInfo.code.valBits.are = eAreRelocatable;
					currPos->codeInfo.code.valBits.val = tagAddr;
				}
			}
			else /* Tag not exist*/
			{
				printf("Err tag %s used but not defined\n", currPos->codeInfo.tag);
				res = eFail;
			}
		}

		currPos = currPos->nextEelement;
	}
	return res;
}



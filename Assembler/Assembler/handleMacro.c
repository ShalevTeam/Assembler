#include "handleMacro.h"
#include "databaseHandler.h"
#include "FirstScanHandler.h"

/*Local macros */
#define FILE_SIZE_FACTOR  5
#define CELL_BEFORE -1

/* members */
static size_t m_maxAllocForOutput = MAX_ASSEBLER_FILE_SIZE * FILE_SIZE_FACTOR;
static size_t m_outputlength = 0; /*length of output file */

/* Private functions*/
static char* macro_replace(char const* orig, char const* rep, char const* with);
static unsigned long addMacrosToList(char** curPos, int currLineNumber);

/******************************************************************************
*Function : handleMacros()
*
* This function handle macros in the input buffer and build an aoutput buffer with all macros
* handled
*
*
* \param
* char* inFileData, INPUT: The file data to proccess
* char* outFileData, INPUT/Output: The file data after proccessing
*
* \return
* ESucsessFail eSucsess if proccessed ok
*
*******************************************************************************/
ESucsessFail handleMacros(char* inFileData, char** outFileData)
{
	ESucsessFail parsOk = eSucsess;
	char* curPos = inFileData;
	char const* macroString = NULL;
	int lineNumber = 0;
	char line[MAX_LINE_LENGTH];
	int outPos = 0;
	char* tmpFileData;
	SMacroElement* macroList = NULL;
	
	if (curPos == NULL)
	{
		printf("Internal err - input buffer is null when handle macro\n");
		parsOk = eFail;
	}
	else
	{

		*outFileData = malloc(m_maxAllocForOutput);

		m_outputlength = strlen(inFileData);

		while (m_outputlength > m_maxAllocForOutput)
		{
			m_maxAllocForOutput = doubleSizeRealloc(outFileData, m_maxAllocForOutput);

			if (m_maxAllocForOutput == 0)
			{
				printf("ReAllocation output failed\n");
				parsOk = eFail;
				return parsOk;
			}
		}


		if (*outFileData)
		{
			/* Scan the file once to handle macro definitions */
			while (curPos && *curPos != '\0')
			{
				lineNumber++;

				curPos = readLine(curPos, line);

				if (curPos != NULL)
				{
					/* Check for macro definition and handle it*/
					lineNumber = addMacrosToList(&curPos, lineNumber);
				}
			}

			/* Build  the initial output */
			strcpy(*outFileData, inFileData);

			/* Replace macro usage in the output file */
			macroList = getMacrosList();
			while (macroList != NULL)
			{
				tmpFileData = macro_replace(*outFileData, macroList->macroName, macroList->macroData);

				if (tmpFileData != NULL)
				{
					if (strlen(tmpFileData) > m_maxAllocForOutput)
					{
						m_maxAllocForOutput = doubleSizeRealloc(outFileData, m_maxAllocForOutput);

						if (m_maxAllocForOutput == 0)
						{
							printf("ReAllocation output failed\n");
							parsOk = eFail;
							return parsOk;
						}
					}

					strcpy(*outFileData, tmpFileData);

					free(tmpFileData);
				}

				macroList = macroList->nextEelement;
			
			}
			
		}
		else
		{
			printf("Allocation failed\n");
			parsOk = eFail;
		}
	}

	return parsOk;
}

/******************************************************************************
*Function : addMacrosToList()
*
* This function search for macros in the current position and if found add them to the macro list.
* handled
*
*
* \param
* char** curPos, INPUT/OUTPUT: The current position in the input file
* int currLineNumber, INPUT: The current line number
*
* \return
* unsigned long the next line number in the input file after the macro
*
*******************************************************************************/
unsigned long addMacrosToList(char** curPos, int currLineNumber)
{
	/* Save the position before macro definition */
	char* startPos = *curPos;
	char* endPos = *curPos;
	char* tmpPos = NULL;
	char macroName[MAX_MACRO_LENGTH] = {0,};
	char macroData[MAX_MACRO_LENGTH] = { 0, };
	char line[MAX_LINE_LENGTH] = { 0, };
	unsigned long nextLineNumber = currLineNumber;

	if (endPos != NULL)
	{
		/* Look for macro definition*/
		readLine(*curPos, line);
		tmpPos = strstr(line, "macro ");

		if (tmpPos != NULL)
		{
			currLineNumber++;
			tmpPos = *curPos + strlen("macro ");

			endPos = tmpPos;

			/*Get The macro name */
			while (*endPos == ' ')
			{
				endPos++;
			}

			while ((*endPos != ' ') &&
				   (*endPos != '\n'))
			{
				endPos++;
			}

			strncpy(macroName, tmpPos, endPos - tmpPos);
			macroName[endPos - tmpPos] = '\0';

			/* Skeep spaces after the macro*/
			while (*endPos != '\n')
			{
				endPos++;
			}

			/* Skip the line end*/
			endPos++;

			/* Replac the macro definition in the input file with empty line */
			while (startPos < endPos + CELL_BEFORE)
			{
				*startPos = ' ';
				startPos++;
			}

			/* Get the macro string */
			while (TRUE)
			{
				/* read line */
				startPos = endPos;
				endPos = readLine(endPos, line);
				
				currLineNumber++;

				/* Check if line ends the macro*/
				if (strstr(line, "endmacro") != NULL)
				{
					/* Replac the macro definition in the input file with spaces to keep the line number */
					while (startPos < endPos + CELL_BEFORE)
					{
						*startPos = ' ';
						startPos++;
					}

					/* update the current position  and line number in the input file*/
					*curPos = endPos;
					nextLineNumber = ++currLineNumber;
					break;
				}
				else
				{
					/* Add the line to the macro string */
					strcat(macroData, line);
				}

				/* Replac the macro definition in the input file with spaces */
				while (startPos < endPos + CELL_BEFORE)
				{
					*startPos = ' ';
					startPos++;
				}
			}

			addMacroElemet(macroName, macroData);
		}

	}
	else
	{
		printf("Internal error on addMacrosToList()\n");
	}

	return nextLineNumber;
}
/******************************************************************************
*Function : macro_replace()
*
* This function finds a macro in a string and replace it with its definition
*
* \param
* char* orig, INPUT: The file data containing the macro
* char* rep, INPUT: The macro name
* char* with, INPUT: The macro string
*
* \return
* char* : result -  The output file data after the macro replacment
*  You must free the result if result is non-NULL!
*******************************************************************************/
char* macro_replace(char const* orig, char const* rep, char const* with)
{
	char* result; /* the return string */
	char const* ins;    /* the next insert point */
	char* tmp;    /* varies */
	size_t len_rep;  /* length of rep (the string to remove) */
	size_t len_with; /* length of with (the string to replace rep with) */
	size_t len_front; /* distance between rep and end of last rep */
	int count;    /* number of replacements */

	/* sanity checks and initialization */
	if (!orig || !rep)
		return NULL;
	len_rep = strlen(rep);
	if (len_rep == 0)
		return NULL; /* empty rep causes infinite loop during count */
	if (!with)
		with = "";
	len_with = strlen(with);

	/* count the number of replacements needed */
	ins = orig;
	for (count = 0; tmp = strstr(ins, rep); ++count) {
		ins = tmp + len_rep;
	}

	if (count == 0)
	{
		return NULL;
	}

	tmp = result = malloc(strlen(orig) + (len_with - len_rep) * count + CELL_FOR_NULL);

	if (!result)
		return NULL;

	/* first time through the loop, all the variable are set correctly*/
	/* from here on,												  */
	/*    tmp points to the end of the result string				  */
	/*    ins points to the next occurrence of rep in orig			  */
	/*    orig points to the remainder of orig after "end of rep"	  */
	while (count--) {
		ins = strstr(orig, rep);
		len_front = ins - orig;
		tmp = strncpy(tmp, orig, len_front) + len_front;
		tmp = strcpy(tmp, with) + len_with;
		orig += len_front + len_rep; /* move to next "end of rep" */
	}
	strcpy(tmp, orig);
	return result;
}




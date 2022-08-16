#include "generator.h"
#include "databaseHandler.h"

/* Private memebers */
char symbolTable[] =
{
	'!', /*0*/
	'@', /*1*/
	'#', /*2*/
	'$', /*3*/
	'%', /*4*/
	'^', /*5*/
	'&', /*6*/
	'*', /*7*/
	'<', /*8*/
	'>', /*9*/
	'a', /*10*/
	'b', /*11*/
	'c', /*12*/
	'd', /*13*/
	'e', /*14*/
	'f', /*15*/
	'g', /*16*/
	'h', /*17*/
	'i', /*18*/
	'j', /*19*/
	'k', /*20*/
	'l', /*21*/
	'm', /*22*/
	'n', /*23*/
	'o', /*24*/
	'p', /*25*/
	'q', /*26*/
	'r', /*27*/
	's', /*28*/
	't', /*29*/
	'u', /*30*/
	'v'	 /*31*/
};

/* Private functions*/
static ESucsessFail generateObjectsFile();

/******************************************************************************
* Function : generateCodeFile()
*
*  This function generates the output files:
*  1. file.ob -  The object file
*  2. file.ent - The entry file
*  3. file.ext - The extern symbols file
*
*
* \param
*  None.
*
*
*
* \return
*  ESucsessFail - eSucsess if all files were generated Sucsessfuly
*
*******************************************************************************/
ESucsessFail generateCodeFile()
{
	ESucsessFail res = eSucsess;

	if (generateObjectsFile())
	{
		if (generateEntryFile())
		{
	
		}
		else
		{
			res = eFail;
			printf("Internal err : entity file generation failed\n");
		}
	}
	else
	{
		res = eFail;
		printf("Internal err : object file generation failed\n");
	}
	return res;
}

/******************************************************************************
* Function : generateEntryFile()
*
*  This function generates the entity file
*
*
* \param
*  None.
*
*
*
* \return
*  ESucsessFail - eSucsess if all files were generated Sucsessfuly
*
*******************************************************************************/
ESucsessFail generateEntryFile()
{
	ESucsessFail res = eSucsess;
	FILE* file = NULL;
	char* fileName = getEntryFileName();
	SEntryElement* listPos = getEntryList();
	char line[MAX_ENTITY_LINE_LENGTH] = { 0, };
	int linePos = 0;
	char symbol = 0;

	if (listPos == NULL)
	{
		printf("Internal err:  Entry list is empty\n");
	}
	else
	{
		/* Generate the output file*/
		file = fopen(fileName, "w");

		if (file != NULL)
		{
			/* Go over the code list*/
			while (listPos != NULL)
			{
				linePos = 0;

				strcpy(line, listPos->tagName);
				
				linePos = strlen(listPos->tagName);
				line[linePos++] = ' ';
				line[linePos++] = ' ';
				line[linePos++] = ' ';
				line[linePos++] = ' ';

				if (listPos->address.decodeVal.msb < sizeof(symbolTable) / sizeof(symbolTable[0]))
				{
					symbol = symbolTable[listPos->address.decodeVal.msb];
					line[linePos++] = symbol;

					if (listPos->address.decodeVal.lsb < sizeof(symbolTable) / sizeof(symbolTable[0]))
					{
						symbol = symbolTable[listPos->address.decodeVal.lsb];
						line[linePos++] = symbol;
						line[linePos++] = '\n';
						line[linePos++] = '\0';
						fwrite(line, strlen(line), 1, file);

					}
					else
					{
						res = eFail;
						printf("Internal err - Symbol index is to big\n");
					}
				}
				else
				{
					res = eFail;
					printf("Internal err - Symbol index is to big\n");
				}

				listPos = listPos->nextEelement;
			}

			fclose(file);
		}
	}

	return res;
}
/******************************************************************************
* Function : generateObjectsFile()
*
*  This function generates the object file
*
*
* \param
*  None.
*
*
*
* \return
*  ESucsessFail - eSucsess if all files were generated Sucsessfuly
*
*******************************************************************************/
ESucsessFail generateObjectsFile()
{
	ESucsessFail res = eSucsess;
	FILE* file = NULL;
	int dataIdx = 0;
	int dataSize = getDataSize();
	char* fileName = getObjectFileName();
	char symbol = 0;
	char objectline[MAX_OBJECTLINE_LENGTH] = { 0, };
	int objectLinePos = 0;
	ScodeWord* dataArray = getDataArray();
	SCodeElement* codelistPos = getCodeList();
	SAddrressVal lastAddr = { 0, };

	if (codelistPos == NULL)
	{
		res = eFail;
		printf("Internal err:  Code list is empty\n");
	}
	else
	{
		/* Generate the output file*/
		file = fopen(fileName, "w");

		if (file != NULL)
		{
			/* Go over the code list*/
			while (codelistPos != NULL)
			{
				lastAddr.val = codelistPos->codeInfo.codeAddress.val;
				objectLinePos = 0;
				if (codelistPos->codeInfo.codeAddress.decodedVal.msb < sizeof(symbolTable) / sizeof(symbolTable[0]))
				{
					symbol = symbolTable[codelistPos->codeInfo.codeAddress.decodedVal.msb];
					objectline[objectLinePos++] = symbol;

					if (codelistPos->codeInfo.codeAddress.decodedVal.lsb < sizeof(symbolTable) / sizeof(symbolTable[0]))
					{
						symbol = symbolTable[codelistPos->codeInfo.codeAddress.decodedVal.lsb];
						objectline[objectLinePos++] = symbol;
						objectline[objectLinePos++] = ' ';
						objectline[objectLinePos++] = ' ';
						objectline[objectLinePos++] = ' ';
						objectline[objectLinePos++] = ' ';

						if (codelistPos->codeInfo.code.decodeVal.msb < sizeof(symbolTable) / sizeof(symbolTable[0]))
						{
							symbol = symbolTable[codelistPos->codeInfo.code.decodeVal.msb];
							objectline[objectLinePos++] = symbol;

							if (codelistPos->codeInfo.code.decodeVal.lsb < sizeof(symbolTable) / sizeof(symbolTable[0]))
							{
								symbol = symbolTable[codelistPos->codeInfo.code.decodeVal.lsb];
								objectline[objectLinePos++] = symbol;
								objectline[objectLinePos++] = '\n';
								objectline[objectLinePos++] = '\0';
								fwrite(objectline, strlen(objectline), 1, file);
							}
							else
							{
								res = eFail;
								printf("Internal err - Symbol index is to big\n");
							}
						}
						else
						{
							res = eFail;
							printf("Internal err - Symbol index is to big\n");
						}
					}
					else
					{
						res = eFail;
						printf("Internal err - Symbol index is to big\n");
					}
				}
				else
				{
					res = eFail;
					printf("Internal err - Symbol index is to big\n");
				}
				codelistPos = codelistPos->nextEelement;
			}

			/* Go over the data Array*/
			for (dataIdx =0; dataIdx < dataSize; dataIdx++)
			{
				objectLinePos = 0;
				lastAddr.val++;
				if (lastAddr.decodedVal.msb < sizeof(symbolTable) / sizeof(symbolTable[0]))
				{
					symbol = symbolTable[lastAddr.decodedVal.msb];
					objectline[objectLinePos++] = symbol;

					if (lastAddr.decodedVal.lsb < sizeof(symbolTable) / sizeof(symbolTable[0]))
					{
						symbol = symbolTable[lastAddr.decodedVal.lsb];
						objectline[objectLinePos++] = symbol;
						objectline[objectLinePos++] = ' ';
						objectline[objectLinePos++] = ' ';
						objectline[objectLinePos++] = ' ';
						objectline[objectLinePos++] = ' ';

						if (dataArray[dataIdx].decodeVal.msb < sizeof(symbolTable) / sizeof(symbolTable[0]))
						{
							symbol = symbolTable[dataArray[dataIdx].decodeVal.msb];
							objectline[objectLinePos++] = symbol;

							if (dataArray[dataIdx].decodeVal.lsb < sizeof(symbolTable) / sizeof(symbolTable[0]))
							{
								symbol = symbolTable[dataArray[dataIdx].decodeVal.lsb];
								objectline[objectLinePos++] = symbol;
								objectline[objectLinePos++] = '\n';
								objectline[objectLinePos++] = '\0';
								fwrite(objectline, strlen(objectline), 1, file);
							}
							else
							{
								res = eFail;
								printf("Internal err - Symbol index is to big\n");
							}
						}
						else
						{
							res = eFail;
							printf("Internal err - Symbol index is to big\n");
						}
					}
					else
					{
						res = eFail;
						printf("Internal err - Symbol index is to big\n");
					}
				}
				else
				{
					res = eFail;
					printf("Internal err - Symbol index is to big\n");
				}
			}

			fclose(file);
		}
	}

	return res;
}

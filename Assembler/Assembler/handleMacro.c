#include "handleMacro.h"

//Local macros
#define FILE_SIZE_FACTOR  5

// members
static int m_maxAllocForOutput = MAX_ASSEBLER_FILE_SIZE * FILE_SIZE_FACTOR;
int m_outputlength = 0; //length of output file

EFuncResSucsessFail handleMacros(char* inFileData, char** outFileData)
{
	EFuncResSucsessFail parsOk = EFuncResSucsess;

	*outFileData = malloc(m_maxAllocForOutput);

	if (*outFileData)
	{
		// Buld the output buffer - dont use more then the allocated memory!!;
		strcpy(*outFileData, inFileData); // TODO - replce with implementation

		// Check if outputlength is bigger then the allocated memory
		if (m_outputlength > m_maxAllocForOutput)
		{
			m_maxAllocForOutput = reallocAndCopyBuffer(outFileData, m_maxAllocForOutput);

			if (m_maxAllocForOutput == 0)
			{
				printf("ReAllocation output failed\n");
				parsOk = EFuncResFail;
			}
		}
	}
	else
	{
		printf("Allocation failed\n");
		parsOk = EFuncResFail;
	}

	return parsOk;
}


#include "services.h"

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
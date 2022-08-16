#include "SecondScanHandler.h"
#include "databaseHandler.h"


/******************************************************************************
* Function : doSecondFileScan()
*
*  This function do the final processing of the code data
*  1. it Updates the TAGs base addrress to the code end addrress
*  2. it Update the code list which uses tags which were not set yet.
*
*
* \param
*  None
*
*
* \return
*  ESucsessFail: eSucsess if complete without errors
*
*******************************************************************************/
ESucsessFail doSecondFileScan()
{
	ESucsessFail res = eSucsess;

	/* update the code Tags base address*/
	updateTagsBaseAddress();

	/* update the entry Tags address*/
	updateEntryAddress();

	/* Update code which uses tags*/
	generateCodeForTag();


	return res;
}
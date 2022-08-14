#include "generator.h"

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
}

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

	//generateObjectsFile();
	return res;
}

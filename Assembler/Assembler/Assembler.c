

#include "databaseHandler.h"
#include "handleMacro.h"
#include "FirstScanHandler.h"
#include "SecondScanHandler.h"
#include "generator.h"

// private function declerations
static void processFile(char* fileName);

/******************************************************************************
* Function : main()
*
*  This is the main function. it receives files of code to proccess and process them
*  
*
* \param
*  int argc, INPUT: input params count
*  char** argv, INPUT: input params data
*  
* \return
*  int: 0 on succsess and -1 on fail
*
*******************************************************************************/
int main(int argc, char** argv)
{
    int i;    

    for (i = 1; i < argc; i++)
    {
        char* fullFileName = NULL;
        

        if ((fullFileName = malloc(strlen(argv[i]) + strlen(".as") + 1)) != NULL)
        {
            fullFileName[0] = '\0';   // ensures the memory is an empty string
            strcat(fullFileName, argv[i]);

            /* Save the name of the currently handled file */
            setAsemblerFileName(fullFileName);

            strcat(fullFileName, ".as");

            processFile(fullFileName);

            free(fullFileName);
        }
        else
        {
            printf("malloc failed!\n");
        }
    }


    return 0;
}

/******************************************************************************
*Function : processFile()
*
* This function proceess a single code file and generates its outputs
* 1. Open the file
* 2. Handle macro
* 3. Do first scan
* 4. Do Second scan
* 5. Generate the output files
*
* \param
* char* fileName, INPUT: The file name to proccess
* 
*
* \return
* None.
*
*******************************************************************************/
void processFile(char* fileName)
{
    int fileSize = 0;
    int filePos = 0;
    int fileData = 0;
    char* postProccesData = NULL;
    FILE* file = NULL;
    char* rawCodeData;
    

    file = fopen(fileName, "r");

    if (file)
    {
        printf("handle file %s\n", fileName);

        fseek(file, 0L, SEEK_END);
        fileSize = ftell(file);

        fseek(file, 0L, SEEK_SET);

        if (fileSize > 0)
        {
            /* Allocate buffer for data read*/
            rawCodeData = malloc(fileSize);

            if (rawCodeData)
            {
                /* Read all the file content*/
                while (1)
                {
                    fileData = fgetc(file);

                    if (fileData == EOF)
                    {
                        break;
                    }
                    else
                    {
                        if (filePos < fileSize)
                        {
                            rawCodeData[filePos] = (char)fileData;
                        }
                    }

                    filePos++;
                }

                // End the string
                rawCodeData[filePos] = '\0';

                // Init all data structure
                initDataBase();

                if (handleMacros(rawCodeData, &postProccesData))
                {
                    printf("\ndone handle macros for %s\n", fileName);

                    if (doFirstFileScan(postProccesData))
                    {
                        printf("done first file scan for %s", fileName);

                        if (doSecondFileScan())
                        {
                            printf("\ndone second file scan for %s\n", fileName);

                            if (generateCodeFile())
                            {

                            }
                            else
                            {
                                printf("fail on second file scan for %s\n", fileName);
                            }
                        }
                        else
                        {
                            printf("fail on second file scan for %s\n", fileName);
                        }
                    }
                    else
                    {
                        printf("fail on first file scan for %s\n", fileName);
                    }

                    free(postProccesData);

                }// if macro handled ok
                else
                {
                    printf("fail handle macros for %s\n", fileName);
                }

                free(rawCodeData);

            } // if raw code
        }
        else
        {
            printf("Input file is empty: %s\n", fileName);
        }

        fclose(file);
    }
    else
    {
        printf("can't open input file: %s\n", fileName);
    }
}



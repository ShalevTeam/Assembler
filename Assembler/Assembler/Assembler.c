

#include "globalDefs.h"
#include "handleMacro.h"
#include "FirstScanHandler.h"

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
    printf("handle file %s\n", fileName);

    // Read all file data to string
    FILE* file = NULL;
    file = fopen(fileName, "r");

    if (file)
    {
        // Get file size
        int fileSize = 0;
        char* postProccesData = NULL;

        fseek(file, 0L, SEEK_END);
        fileSize = ftell(file);

        fseek(file, 0L, SEEK_SET);

        char* rawCodeData = malloc(fileSize+1);

        if (rawCodeData)
        {
            rawCodeData[0] = '\0';
            rawCodeData[fileSize] = '\0';

            int generatedCodeLength = 0;

            // read all file data to struct SCodeElement
            fread(rawCodeData, fileSize+1, 1, file);

            // Init all data structure
            initDataBase();

            if (handleMacros(rawCodeData, &postProccesData))
            {
                printf("done handle macros for %s\n", fileName);

                if (doFirstFileScan(postProccesData))
                {
                    printf("done first file scan for %s\n", fileName); 
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
        fclose(file);
    }
    else
    {
        printf("can't open input file: %s\n", fileName);
    }
}



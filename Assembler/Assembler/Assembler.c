

#include "globalDefs.h"
#include "handleMacro.h"

// function declerations
void processFile(char* fileName);
void freeCodeList(SCodeElement* elemToFree);
void analizeLineOfCode(int lineNumber,char* line);
EFuncResSucsessFail doFirstFileScan(char* fileData);

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

void processFile(char* fileName)
{
    printf("handle file %s\n", fileName);

    // Read all file data to string
    FILE* file = NULL;
    file = fopen(fileName, "r");

    if (file)
    {
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

EFuncResSucsessFail doFirstFileScan(char* fileData)
{
    EFuncResSucsessFail scanStatus = EFuncResSucsess;
    char line[MAX_LINE_LENGTH];
    int lineLength = 0;

    // read line
    char* startPos = fileData;
    int lineNumber = 0;

    while (1)
    {
        lineNumber++;

        startPos = readLine(startPos,line);

        if (startPos != NULL)
        {
            printf("Handle line: %s", line);

            analizeLineOfCode(lineNumber,line);
        }
        else
        {
            break;
        }
        
    }

    return scanStatus;
}

void freeCodeList(SCodeElement* elemToFree)
{
    while (elemToFree)
    {

        SCodeElement* next = elemToFree->nextEelement;
        free(elemToFree);
        elemToFree = next;
    }
}

void analizeLineOfCode(int lineNumber, char* line)
{
    // First we chek the line type
   // ELineType lineType = getLineType(lineNumber, line);

    //if (lineType == ECodeLine)
    {
        //handleCodeLine(lineNumber, line);
    }
    //else
    {
        //handleInstructionLine(lineNumber, line);
    }
}

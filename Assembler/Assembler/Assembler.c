#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "globalDefs.h"
#include "handleMacro.h"

int main(int argc, char** argv)
{
    int i;


    for (i = 1; i < argc; i++)
    {
        char* fullFileName;

        if ((fullFileName = malloc(strlen(argv[i]) + strlen(".as") + 1)) != NULL)
        {
            fullFileName[0] = '\0';   // ensures the memory is an empty string
            strcat(fullFileName, argv[i]);
            strcat(fullFileName, ".as");

            printf("handle file %s\n", fullFileName);

            // Read all file data to string
            char* srcFile = NULL;
            char* dstFile = NULL;
            FILE* file = NULL;
            int fileSize = 0;
            file = fopen(fullFileName, "r");

            fseek(file, 0L, SEEK_END);
            fileSize = ftell(file);

            fseek(file, 0L, SEEK_SET);

            srcFile = malloc(fileSize);
            dstFile = malloc(fileSize);


            if (handleMacros(srcFile, dstFile))
            {
                printf("done handle macros for %s\n", fullFileName);
            }
            else
            {
                printf("fail handle macros for %s\n", fullFileName);
            }

            free(srcFile);
            free(dstFile);
        }
        else
        {
            printf("malloc failed!\n");
        }

        
    }


    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

            printf("open file %s\n", fullFileName);
        }
        else
        {
            printf("malloc failed!\n");
        }

        
    }


    return 0;
}

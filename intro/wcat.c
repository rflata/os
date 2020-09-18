#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE (1024)

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        exit(0);
    }
    for (size_t i = 1; i < argc; i++)
    {
        const char *filename = argv[i];
        FILE *fp = fopen(filename, "r");
        if (fp == NULL)
        {
            printf("wcat: cannot open file\n");
            exit(1);
        }

        char buffer[BUFFER_SIZE];
        while (fgets(buffer, BUFFER_SIZE, fp) != NULL)
        {
            printf("%s", buffer);
        }
        fclose(fp);
    }
    return (0);
}
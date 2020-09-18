#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    size_t len = 0;
    char *line = NULL;

    if (argc == 1)
    {
        printf("wgrep: searchterm [file ...]\n");
        exit(1);
    }
    char *grep = argv[1];
    if (argc == 2)
    {
        while (getline(&line, &len, stdin) != -1)
        {
            if (strstr(line, grep))
            {
                printf("%s", line);
            }
        }
    }
    for (size_t i = 2; i < argc; i++)
    {
        const char *filename = argv[i];
        FILE *fp = fopen(filename, "r");
        if (fp == NULL)
        {
            printf("wgrep: cannot open file\n");
            exit(1);
        }

        while (getline(&line, &len, fp) != -1)
        {
            if (strstr(line, grep))
            {
                printf("%s", line);
            }
        }
        fclose(fp);
    }
    return (0);
}
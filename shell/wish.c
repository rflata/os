#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define EXIT ("exit")
#define CHDIR ("cd")
#define PATH ("path")
#define ERROR ("An error has occurred\n")

char **get_input(char *input)
{
    char **command = malloc(8 * sizeof(char *));
    char *value;
    int args = 0;

    while ((value = strsep(&input, " ")) != NULL)
    {
        if (strcmp(value, "") != 0)
        {
            command[args] = value;
            args++;
        }
    }
    command[args] = NULL;
    return command;
}

char *execute(char **command, char *env, int redirect)
{
    pid_t child_pid;
    int stat_loc;
    char *file;
    if (command[0] != NULL)
    {
        if (redirect)
        {
            if (command[redirect + 1] == NULL)
            {
                char *value;
                char *direct;
                int i = 0;
                while ((value = strsep(&command[redirect], ">")) != NULL)
                {
                    if (i == 0)
                    {
                        direct = value;
                    }
                    else if (i == 1)
                    {
                        file = value;
                    }
                    i++;
                }
                command[redirect] = direct;
            }
            else
            {
                file = command[redirect + 1];
                command[redirect] = NULL;
            }
        }
        if (strcmp(command[0], EXIT) == 0)
        {
            if (command[1] != NULL)
            {
                write(STDERR_FILENO, ERROR, strlen(ERROR));
            }
            else
            {
                exit(0);
            }
        }
        else if (strcmp(command[0], CHDIR) == 0)
        {
            if (command[1] == NULL || command[2] != NULL)
            {
                write(STDERR_FILENO, ERROR, strlen(ERROR));
            }
            else
            {
                chdir(command[1]);
            }
        }
        else if (strcmp(command[0], PATH) == 0)
        {
            if (command[1] == NULL)
            {
                env = "";
            }
            else
            {
                for (int i = 1; i < sizeof(command); i++)
                {
                    if (command[i] != NULL)
                    {
                        env = strcat(env, ":");
                        env = strcat(env, command[i]);
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }
        else
        {
            child_pid = fork();
            if (child_pid == 0)
            {
                if (redirect != 0)
                {
                    int fd = creat(file, 0644);
                    dup2(fd, STDOUT_FILENO);
                    dup2(fd, STDERR_FILENO);
                    close(fd);
                }
                /* Never returns if the call is successful */
                setenv("PATH", env, 1);
                execvp(command[0], command);
                write(STDERR_FILENO, ERROR, strlen(ERROR));
                exit(1);
            }
            else
            {
                waitpid(child_pid, &stat_loc, WUNTRACED);
            }
        }
    }
    return env;
}

int checkRedirect(char **command)
{
    int redirect = 0;
    int i = 0;
    while (command[i] != NULL)
    {
        if (strstr(command[i], ">") != NULL)
        {
            if ((command[i + 1] == NULL && (strcmp(command[i], ">") == 0)) || command[i + 2] != NULL || redirect != 0 || i == 0)
            {
                redirect = -1;
                break;
            }
            else
            {
                redirect = i;
            }
        }
        i++;
    }
    return redirect;
}

int main(int argc, char *argv[])
{
    size_t len = 0;
    char *input;
    char **command;
    char *env = strdup(getenv("PATH"));
    int redirect = 0;

    if (argc == 1)
    {
        while (1)
        {
            printf("wish> ");
            getline(&input, &len, stdin);
            input[strcspn(input, "\n")] = 0;
            command = get_input(input);
            redirect = checkRedirect(command);
            if (redirect == -1)
            {
                write(STDERR_FILENO, ERROR, strlen(ERROR));
            }
            else
            {
                env = execute(command, env, redirect);
            }
        }
    }
    else if (argc == 2)
    {
        const char *filename = argv[1];
        FILE *fp = fopen(filename, "r");
        if (fp == NULL)
        {
            write(STDERR_FILENO, ERROR, strlen(ERROR));
            exit(1);
        }

        while (getline(&input, &len, fp) != -1)
        {
            input[strcspn(input, "\n")] = 0;
            command = get_input(input);
            redirect = checkRedirect(command);
            if (redirect == -1)
            {
                write(STDERR_FILENO, ERROR, strlen(ERROR));
            }
            else
            {
                env = execute(command, env, redirect);
            }
        }
        fclose(fp);
    }
    else
    {
        exit(1);
    }
}

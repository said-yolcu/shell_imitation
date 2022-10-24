#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_LINE 80
#define MAX_ARGS 4 // There will be at most 4 arguments

int getArgs(char argums[][MAX_LINE + 1], int, int);
int processArgs(char argums[][MAX_LINE + 1], int numArgs);
void whoami(char *);

int main(void)
{
    char name[MAX_LINE];
    whoami(name);
    strcpy(name, "said"); // CHANGE THAT

    printf("\n%s >>> ", name);

    char argums[MAX_ARGS][MAX_LINE + 1]; // 4 by 80
    int numArgs;                         // Number of arguments

    while ((numArgs = getArgs(argums, MAX_LINE, MAX_ARGS)) != 0)
    {
        processArgs(argums, numArgs);
        printf("\n%s >>> ", name);
    }
}

int processArgs(char argums[][MAX_LINE + 1], int numArgs)
{
    pid_t pid;
    pid = fork();

    if (pid < 0) // Fork failure
    {
        fprintf(stderr, "Fork failed");
        return 1;
    }
    else if (pid == 0) // Child process
    {

        int argIn = 0;
        if (strcmp(argums[argIn], "listdir") == 0)
        {
            execlp("/bin/ls", "ls", NULL);
        }
        else if (strcmp(argums[argIn], "mycomputername") == 0)
        {
            execlp("/bin/hostname", "hostname", NULL);
        }
        else if (strcmp(argums[argIn], "whatsmyip") == 0)
        {
            execlp("/bin/hostname", "hostname", "-I", NULL);
        }
        else if (strcmp(argums[argIn], "printfile") == 0)
        {
            if (numArgs == 2)
            {
                execlp("/bin/gcc", "gcc", "./file_reader.c", "-o",
                       "file_reader", NULL);
                execlp("./file_reader", "./file_reader", argums[1],
                       NULL);
            }
        }
        else if (strcmp(argums[argIn], "dididothat") == 0)
        {
        }
        else if (strcmp(argums[argIn], "hellotext") == 0)
        {
        }
        else if (strcmp(argums[argIn], "exit") == 0)
        {
        }
    }
    else // Parent process
    {
        wait(NULL); // Wait for child to execute
    }
    return 0;
}

int getArgs(char argums[][MAX_LINE + 1], int lineLim, int argLim)
{
    int c;
    int strIn = 0;   // String index
    int argIn = 0;   // Argument index
    int numArgs = 0; // Number of arguments

    while ((c = getc(stdin)) != '\n') // Proceed until newline character
    {
        if (strIn >= lineLim || argIn >= argLim)
        {
            printf("Too long a command\n");
            return -1;
        }
        if (c != ' ')
        {
            argums[argIn][strIn++] = c; // Store the char into string
            numArgs++;
        }
        else
        {
            argums[argIn][strIn] = '\0'; // End the string
            argIn++;                     // Move to the next argument
            strIn = 0;                   // Point to its first char
        }
    }
    numArgs++; // We count empty line as one-argument also

    return 1;
}

void whoami(char *name)
{
}
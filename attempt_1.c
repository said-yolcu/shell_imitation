
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "char_io.h"

#define MAX_LINE 80
#define MAX_ARGS 4 // There will be at most 4 arguments

#define BUF_SIZE 25
#define READ_END 0
#define WRITE_END 1

int getArgs(char argums[][MAX_LINE + 1], int, int);
int processArgs(char argums[][MAX_LINE + 1], int numArgs);
int whoami(char *);

int main(void)
{
    char name[MAX_LINE];
    if (whoami(name) == 1)
    {
        fprintf(stderr, "Whoami failed");
        return 1;
    }
    // strcpy(name, "said"); // CHANGE THAT

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
    int strIn = 0;    // String index
    int argIn = 0;    // Argument index
    int word = false; // Whether the cha pointer currently points to a word,
                      // or a non-word char such as newline or blank space

    // Skip leading blank spaces
    while (true)
    {
        if ((c = getch(stdin)) == ' ')
            continue;
        else
        {
            ungetch(c);
            break;
        }
    }

    while ((c = getch(stdin)) != '\n') // Proceed until newline character
    {
        if (strIn >= lineLim || argIn >= argLim)
        {
            printf("Too long a command\n");
            return -1;
        }
        if (c != ' ')
        {
            word = true;
            argums[argIn][strIn++] = c; // Store the char into string
        }
        else
        {
            word = false;                // Exit the word
            argums[argIn][strIn] = '\0'; // End the string
            argIn++;                     // Move to the next argument
            strIn = 0;                   // Point to its first char
            while (true)                 // Pass the blank spaces
            {
                if ((c = getch(stdin)) == '\n') // Line ended
                {
                    return argIn;
                }

                if (c == ' ') // Skip blank spaces
                {
                    continue;
                }

                ungetch(c); // If the char is non-blank, ungetch it and
                break;      // break back to outer loop
            }
        }
    }

    if (word) // The word ended with newline
    {
        argums[argIn][strIn] = '\0';
        argIn++;
    }

    return argIn;
}

/*
Store the current user into the argument string
Return 1 if error occurs, else return 0
*/
int whoami(char *name)
{
    char read_msg[BUF_SIZE];
    int fd[2];
    pid_t pid;

    if (pipe(fd) == -1) // Create the pipe
    {
        fprintf(stderr, "Pipe failed");
        return 1;
    }

    pid = fork();

    if (pid < 0)
    {
        fprintf(stderr, "Fork failed");
        return 1;
    }
    else if (pid == 0) // Child process
    {
        close(fd[READ_END]);        // Close unused end
        dup2(fd[1], STDOUT_FILENO); // Duplicate the stdout to pipe
        execlp("/bin/whoami", "whoami", NULL);
        close(fd[WRITE_END]);
    }
    else // Parent process
    {
        close(fd[WRITE_END]);
        // Read from the pipe
        int nBytes = read(fd[READ_END], read_msg, sizeof(read_msg));
        for (int i = 0; read_msg[i] != '\0'; i++) // Terminate the string before
        {                                         // the newline
            if (read_msg[i] == '\n')
            {
                read_msg[i] = '\0';
                break;
            }
        }
        strcpy(name, read_msg); // Store in argument
        wait(NULL);
        close(fd[READ_END]);
    }
    return 0;
}

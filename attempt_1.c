
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "char_io.h"
#include "history.h"

#define MAX_LINE 80
#define MAX_ARGS 4 // There will be at most 4 arguments

#define BUF_SIZE 25
#define READ_END 0
#define WRITE_END 1

int getArgs(char argums[][MAX_LINE + 1], int, int, int);
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

    while ((numArgs = getArgs(argums, MAX_LINE, MAX_ARGS, 0)) != 0)
    {
        processArgs(argums, numArgs);
        printf("\n%s >>> ", name);
    }
}

int processArgs(char argums[][MAX_LINE + 1], int numArgs)
{
    pid_t pid;
    pid = fork();

    int argIn = 0;

    if (pid < 0) // Fork failure
    {
        fprintf(stderr, "Fork failed");
        return 1;
    }
    else if (pid == 0) // Child process
    {

        if (strcmp(argums[argIn], "listdir") == 0)
        {
            if (numArgs > 1)
            {
                printf("%s takes no arguments", argums[0]);
                exit(-1);
            }
            execlp("/bin/ls", "ls", NULL);
        }
        else if (strcmp(argums[argIn], "mycomputername") == 0)
        {
            if (numArgs > 1)
            {
                printf("%s takes no arguments", argums[0]);
                exit(-1);
            }
            execlp("/bin/hostname", "hostname", NULL);
        }
        else if (strcmp(argums[argIn], "whatsmyip") == 0)
        {
            if (numArgs > 1)
            {
                printf("%s takes no arguments", argums[0]);
                exit(-1);
            }
            execlp("/bin/hostname", "hostname", "-I", NULL);
        }
        else if (strcmp(argums[argIn], "printfile") == 0)
        {
            if (numArgs == 2)
            {
                // Create separate processes for each exc call
                pid_t pid_2;
                pid_2 = fork();
                if (pid_2 < 0)
                {
                    fprintf(stderr, "Fork Failed");
                    return 1;
                }
                else if (pid_2 == 0)
                {
                    execlp("/bin/gcc", "gcc", "./file_reader.c", "-o",
                           "file_reader", NULL);
                }
                else
                {
                    wait(NULL);
                    execlp("./file_reader", "./file_reader", argums[1],
                           NULL);
                }
            }
            else if (numArgs == 4)
            {
                if (strcmp(argums[2], ">"))
                {
                    printf("Correct usage:\n");
                    printf("printfile <file_1> > <file_2>\n");
                    exit(-1); // With using exit instead of return, we ensure
                              // that the command is recorded to the history
                              // via not disturbing the execurion of parent
                }

                // Create separate processes for each exc call
                pid_t pid_2;
                pid_2 = fork();
                if (pid_2 < 0)
                {
                    fprintf(stderr, "Fork Failed");
                    return 1;
                }
                else if (pid_2 == 0)
                {
                    execlp("/bin/gcc", "gcc", "./file_copier.c", "-o",
                           "file_copier", NULL);
                }
                else
                {
                    wait(NULL);
                    execlp("./file_copier", "./file_copier", argums[1], argums[3],
                           NULL);
                }
            }
            else
            {
                printf("Invalid number of arguments\n");
                exit(-1);
            }
        }
        else if (strcmp(argums[argIn], "dididothat") == 0)
        {
            char command[MAX_LINE];

            if (numArgs == 4) // dididothat "  <command>  "
            {
                if (strcmp(argums[1], "\"") != 0 || strcmp(argums[3], "\""))
                {
                    printf("Usage:\n");
                    printf("dididothat \"<command>\"");
                    exit(-1);
                }
                strcpy(command, argums[2]);
            }
            else if (numArgs == 3)
            {
                // dididothat "  <command>"
                if (strcmp(argums[1], "\"") == 0 &&           // Check if the first arg is \"
                    argums[2][strlen(argums[2]) - 1] == '\"') // and second arg ends with \"
                {
                    // Copy the actual command to the command variable
                    strncpy(command, argums[2], strlen(argums[2]) - 1);
                    command[strlen(argums[2]) - 1] = '\0';
                    for (int i = 0; i <= strlen(command); i++)
                    {
                        printf("%c ", command[i]);
                    }
                    printf("Commmand is <%s>\n", command);
                    printf("\n");
                }
                // dididothat "<command>   "
                else if (strcmp(argums[2], "\"") == 0 && // Check if the second arg is \"
                         argums[1][0] == '\"')           // and first arg end with \"
                {
                    // Copy the actual command to the command variable
                    strncpy(command, &argums[1][1], strlen(argums[1]) - 1);
                    command[strlen(argums[1]) - 1] = '\0';
                }
                else
                {
                    printf("Correct usage:\n");
                    printf("dididothat \"<command>\"");
                    exit(-1);
                }
            }
            else if (numArgs == 2) // dididothat "<command>"
            {
                if (argums[1][0] == '\"' && argums[1][strlen(argums[1]) - 1] == '\"')
                {
                    // Copy the actual command to the command variable
                    strncpy(command, &argums[1][1], strlen(argums[1]) - 2);
                    command[strlen(argums[1]) - 2] = '\0';
                }
                else if (strcmp(argums[1], "-allhistory") == 0) // Print last 15 records of history
                {
                    printHist();
                    exit(-1); // Do not process to the normal execution
                }
                else
                {
                    printf("Correct usage:\n");
                    printf("dididothat \"<command>\"");
                    exit(-1);
                }
            }
            else
            {
                printf("Invalid number of arguments. Correct usage:\n");
                printf("dididothat \"<command>\"");
                exit(-1);
            }

            if (checkHist(command))
            {
                printf("Yes");
            }
            else
            {
                printf("No");
            }
            exit(0);
        }
        else if (strcmp(argums[argIn], "hellotext") == 0)
        {
        }
        else if (strcmp(argums[argIn], "exit") == 0)
        {
        }
        else
        {
            printf("There is no such command: %s", argums[argIn]);
            exit(-1);
        }
        /*
        recordHist(argums[argIn]); // Record the command to the history
                                   // after executing the command
        */
    }
    else // Parent process
    {
        wait(NULL);                // Wait for child to execute
        recordHist(argums[argIn]); // Record the command to the history
                                   // after executing the command
    }
    return 0;
}

int getArgs(char argums[][MAX_LINE + 1], int lineLim, int argLim, int argSta)
{
    int c;
    int strIn = 0;      // String index
    int argIn = argSta; // First argument index
    int word = false;   // Whether the cha pointer currently points to a word,
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

    while ((c = getch(stdin)) != '\n' && c != '\0')
    // Proceed until newline character
    // Or until the string's end
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
                if ((c = getch(stdin)) == '\n' || c == '\0') // Line or string ended
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

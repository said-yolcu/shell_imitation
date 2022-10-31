
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
#define MAX_ARGS 5 // There will be at most 4 arguments

#define BUF_SIZE 25
#define READ_END 0
#define WRITE_END 1

#define INV_USE 2  // Invalid usage exit status
#define EXIT_COM 3 // Exit command exit status

int getArgs(char argums[][MAX_LINE + 1], int, int, int, char (*line)[MAX_LINE + 1]);
int processArgs(char argums[][MAX_LINE + 1], int numArgs, char *);
int whoami(char *);

int main(void)
{
    char name[MAX_LINE];
    if (whoami(name) == 1)
    {
        fprintf(stderr, "Whoami failed");
        return 1;
    }

    printf("%s >>> ", name);

    char argums[MAX_ARGS][MAX_LINE + 1]; // 4 by 80
    int numArgs;                         // Number of arguments
    int processStatus;                   // Return status of the process
    char line[MAX_LINE + 1];

    while ((numArgs = getArgs(argums, MAX_LINE, MAX_ARGS, 0, &line)) != 0)
    {
        processStatus = processArgs(argums, numArgs, line);
        if (processStatus == -2)
        {
            return 0;
        }
        printf("%s >>> ", name);
    }
}

int processArgs(char argums[][MAX_LINE + 1], int numArgs, char *line)
{
    pid_t pid;
    pid = fork();
    static int childStatus;

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
                printf("%s takes no arguments\n", argums[0]);
                exit(INV_USE);
            }
            execlp("/bin/ls", "ls", NULL);
        }
        else if (strcmp(argums[argIn], "mycomputername") == 0)
        {
            if (numArgs > 1)
            {
                printf("%s takes no arguments\n", argums[0]);
                exit(INV_USE);
            }
            execlp("/bin/hostname", "hostname", NULL);
        }
        else if (strcmp(argums[argIn], "whatsmyip") == 0)
        {
            if (numArgs > 1)
            {
                printf("%s takes no arguments\n", argums[0]);
                exit(INV_USE);
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
                    exit(INV_USE); // With using exit instead of return, we ensure
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
                exit(INV_USE);
            }
        }
        else if (strcmp(argums[argIn], "dididothat") == 0)
        {
            char command[MAX_LINE];

            // A quote block is a single word
            if (argums[1][0] == '\"' && argums[1][strlen(argums[1]) - 1] == '\"')
            {
                // Copy the actual command to the command variable
                strncpy(command, &argums[1][1], strlen(argums[1]) - 2);
                command[strlen(argums[1]) - 2] = '\0';
            }
            else if (strcmp(argums[1], "-allhistory") == 0) // Print last 15 records of history
            {
                printHist();
                exit(INV_USE); // Do not process to the normal execution
            }
            else
            {

                printf("Correct usage:\n");
                printf("dididothat \"<command>\"\n");
                exit(INV_USE);
            }

            if (checkHist(command))
            {
                printf("Yes\n");
            }
            else
            {
                printf("No\n");
            }
            exit(0);
        }
        else if (strcmp(argums[argIn], "hellotext") == 0)
        {
            if (numArgs == 2)
            {
                execlp("/bin/gedit", "gedit", argums[1], NULL);
            }
            else if (numArgs == 1)
            {
                execlp("/bin/gedit", "gedit", NULL);
            }
            else
            {
                printf("Invalid number of arguments\n");
                exit(INV_USE);
            }
        }
        else if (strcmp(argums[argIn], "exit") == 0)
        {
            exit(EXIT_COM); // Exit the shell
        }
        else
        {
            printf("There is no such command: %s\n", argums[argIn]);
            exit(INV_USE);
        }
    }
    else // Parent process
    {
        wait(&childStatus); // Wait for child to execute
        // printf("Child exit status: %d\n", WEXITSTATUS(childStatus));
        if (WEXITSTATUS(childStatus) == 3) // Exit the shell
        {
            return -2;
        }

        recordHist(line); // Record the command to the history
                          // after executing the command
    }
    return 0;
}

int getArgs(char argums[][MAX_LINE + 1], int lineLim, int argLim, int argSta, char (*linePtr)[MAX_LINE + 1])
{
    int c;
    int lineIn = 0;     // Current index on the line
    int strIn = 0;      // String index
    int argIn = argSta; // First argument index
    int word = false;   // Whether the cha pointer currently points to a word,
                        // or a non-word char such as newline or blank space

    // Skip leading blank spaces
    while (true)
    {
        if ((c = getch(stdin)) == ' ')
        {
            (*linePtr)[lineIn++] = c; // Put the char to the line and increment the index
            continue;
        }
        else
        {
            ungetch(c);
            break;
        }
    }

    int quote = false; // Whether  we are in a quote block

    while ((c = getch(stdin)) != '\n' && c != '\0')
    // Proceed until newline character
    // Or until the string's end
    {
        (*linePtr)[lineIn++] = c; // Put char to the line and increment index

        if (strIn >= lineLim || argIn >= argLim)
        {
            printf("Too long a command\n");
            return -1;
        }

        if (quote) // If we are inside a quote block
        {
            word = true;
            argums[argIn][strIn++] = c; // Store the char into string

            if (c == '\"')
            {
                quote = false; // End quote
            }
        }
        else if (c != ' ') // If we are outside a quote
        {
            word = true;
            argums[argIn][strIn++] = c; // Store the char into string

            if (c == '\"')
            {
                quote = true; // Enter quote
            }
        }
        else // c== ' '
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
        (*linePtr)[lineIn] = '\0';
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

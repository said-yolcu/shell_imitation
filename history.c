
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define MAX_LINE 80
#define MAX_HISTORY 15

static char history[MAX_HISTORY][MAX_LINE]; // Where we store the history

int last = 0;     // Index of the last line in history
                  // Last points to the line where the command WILL be written
int full = false; // Whether the history is full

void recordHist(char *command)
{
    strcpy(history[last], command); // Copy the command to history
    history[last][strlen(history[last])] = '\0';

    last += 1; // Update the index of the last line

    if (!full && last == MAX_HISTORY) // Update full
    {
        full = true;
    }

    last %= MAX_HISTORY; // History array is used circularly
}

int checkHist(char *command)
{
    int terminus = full ? 15 : last;

    for (int i = 0; i < terminus; i++)
    {

        if (strcmp(history[i], command) == 0)
        {
            return true;
        }
    }
    return false;
}

void printHist()
{
    if (full)
    {
        int i = last;
        for (int t = MAX_HISTORY - 1; t >= 0; t--)
        {
            i--;
            if (i < 0)
            {
                i += MAX_HISTORY;
            }
            printf("%d: %s\n", t, history[i]);
        }
    }
    else
    {
        for (int i = last - 1; i >= 0; i--)
        {
            printf("%d: %s\n", i, history[i]);
        }
    }
}

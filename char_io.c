#include <stdio.h>

#define BUFSIZE 1

char buf[BUFSIZE];
int bufP = 0;
int lineP = 0;

/*
If there is a char in buffer, get it. Else get a char from the file.
*/
int getch(const FILE *fp)
{
    if (bufP)
    {
        return buf[--bufP];
    }
    return getc(fp);
}

/*
Unget a char to the buffer. It is used when you read too many chars by accident
*/
void ungetch(int c)
{
    if (bufP)
    {
        fprintf(stderr, "Cannot unget the char, buffer is full\n");
        return;
    }
    buf[bufP++] = c;
}
#ifndef CHAR_IO_H
#define CHAR_IO_H

#include <stdio.h>

int getch(const FILE *fp);
void ungetch(int c);

#endif
#ifndef HISTORY_H
#define HISTORY_H

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

void recordHist(char *command);
int checkHist(char *command);
void printHist();

#endif
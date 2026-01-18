#ifndef UTILS_H
#define UTILS_H

#include <ctype.h>
#include <string.h>

// Trim leading and trailing whitespace from a string
void trimWhitespace(char *str);

// Extract the first word from a string
char *getFirstWord(char *str, char *word);

// Check if a variable name is valid (exactly 3 letters)
int isValidVarName(const char *name);

#endif // UTILS_H

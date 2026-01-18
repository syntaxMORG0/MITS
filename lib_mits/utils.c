#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

void trimWhitespace(char *str) {
    int start = 0, end = strlen(str) - 1;
    while (start <= end && isspace(str[start])) start++;
    while (end >= start && isspace(str[end])) end--;
    if (start > 0) {
        memmove(str, str + start, end - start + 2);
        str[end - start + 1] = '\0';
    } else {
        str[end + 1] = '\0';
    }
}

char *getFirstWord(char *str, char *word) {
    int i = 0;
    while (*str && !isspace(*str) && i < 63) {
        word[i++] = *str++;
    }
    word[i] = '\0';
    while (*str && isspace(*str)) str++;
    return str;
}

int isValidVarName(const char *name) {
    if (strlen(name) != 3) {
        fprintf(stderr, "Error: Variable names must be exactly 3 letters, got '%s' (%zu letters)\n", name, strlen(name));
        return 0;
    }
    for (int i = 0; i < 3; i++) {
        if (!isalpha(name[i])) {
            fprintf(stderr, "Error: Variable names must contain only letters, got '%s'\n", name);
            return 0;
        }
    }
    return 1;
}

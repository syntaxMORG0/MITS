#include "rom.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int addROMEntry(const char *key, const char *type, const char *value) {
    if (state.romCount < MAX_ROM_ENTRIES) {
        strcpy(state.romEntries[state.romCount].key, key);
        strcpy(state.romEntries[state.romCount].type, type);
        strcpy(state.romEntries[state.romCount].value, value);
        state.romCount++;
        return 0;
    }
    return -1;
}

int getROMEntry(const char *key, char *type, char *value) {
    for (int i = 0; i < state.romCount; i++) {
        if (strcmp(state.romEntries[i].key, key) == 0) {
            strcpy(type, state.romEntries[i].type);
            strcpy(value, state.romEntries[i].value);
            return 0;
        }
    }
    return -1;
}

int isFileImported(const char *filename) {
    for (int i = 0; i < importedFileCount; i++) {
        if (strcmp(importedFiles[i], filename) == 0) {
            return 1;
        }
    }
    return 0;
}

void markFileImported(const char *filename) {
    if (importedFileCount < MAX_IMPORTED_FILES) {
        strcpy(importedFiles[importedFileCount++], filename);
    }
}

void parseROMFile(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return;

    char line[MAX_LINE_LENGTH];
    while (fgets(line, MAX_LINE_LENGTH, f)) {
        trimWhitespace(line);
        if (strlen(line) == 0 || line[0] == '#') continue;

        // Parse key="value" or key=number format
        char *eq = strchr(line, '=');
        if (eq) {
            *eq = '\0';
            char key[64];
            strcpy(key, line);
            trimWhitespace(key);

            char *val = eq + 1;
            trimWhitespace(val);

            if (val[0] == '"') {
                // String value
                val++;
                char *endQuote = strchr(val, '"');
                if (endQuote) *endQuote = '\0';
                addROMEntry(key, "string", val);
            } else {
                // Numeric value
                addROMEntry(key, "number", val);
            }
        }
    }
    fclose(f);
}

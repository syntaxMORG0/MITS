#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "utils.h"
#include "rom.h"
#include "register.h"
#include "compiler.h"

// Global state and imported files
State state;
char importedFiles[MAX_IMPORTED_FILES][256];
int importedFileCount = 0;

void compile(const char *inputFile, const char *outputFile) {
    // Parse the ROM file first
    parseROMFile("main.rom");

    FILE *in = fopen(inputFile, "r");
    if (!in) {
        fprintf(stderr, "Error: Cannot open input file '%s'\n", inputFile);
        return;
    }

    char lines[MAX_LINES][MAX_LINE_LENGTH];
    int lineCount = 0;

    while (lineCount < MAX_LINES && fgets(lines[lineCount], MAX_LINE_LENGTH, in)) {
        trimWhitespace(lines[lineCount]);
        lineCount++;
    }
    fclose(in);

    // Check for _start label
    int hasStart = 0;
    for (int i = 0; i < lineCount; i++) {
        if (strcmp(lines[i], "_start:") == 0) {
            hasStart = 1;
            break;
        }
    }

    if (!hasStart) {
        fprintf(stderr, "Error: Missing _start: label\n");
        return;
    }

    // First pass: collect functions
    for (int i = 0; i < lineCount; i++) {
        char *line = lines[i];
        if (strlen(line) == 0 || line[0] == '#') continue;

        char word[64];
        getFirstWord(line, word);

        if (strcmp(word, "def") == 0) {
            char lineCopy[MAX_LINE_LENGTH];
            strcpy(lineCopy, line + 3);
            char funcName[64];
            getFirstWord(lineCopy, funcName);
            
            if (state.funcCount < MAX_FUNCTIONS) {
                strcpy(state.functions[state.funcCount].name, funcName);
                state.functions[state.funcCount].startLine = i;
                state.funcCount++;
            }
        }
    }

    printf("Compilation successful: Assembly parsed with %d ROM entries and %d functions\n", state.romCount, state.funcCount);
}

void printUsage(const char *progName) {
    fprintf(stderr, "Usage: %s build -f <input.s> -rom <output.rom>\n", progName);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "build") == 0) {
        const char *inputFile = NULL;
        const char *outputFile = NULL;

        for (int i = 2; i < argc; i++) {
            if (strcmp(argv[i], "-f") == 0 && i + 1 < argc) {
                inputFile = argv[i + 1];
                i++;
            } else if (strcmp(argv[i], "-rom") == 0 && i + 1 < argc) {
                outputFile = argv[i + 1];
                i++;
            }
        }

        if (!inputFile || !outputFile) {
            fprintf(stderr, "Error: Missing -f or -rom argument\n");
            printUsage(argv[0]);
            return 1;
        }

        compile(inputFile, outputFile);
        return 0;
    } else {
        fprintf(stderr, "Error: Unknown command '%s'\n", argv[1]);
        printUsage(argv[0]);
        return 1;
    }

    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINES 1024
#define MAX_LINE_LENGTH 256
#define MAX_REGISTERS 256
#define MAX_FUNCTIONS 128
#define MAX_ROM_ENTRIES 512
#define MAX_IMPORTED_FILES 64

typedef struct {
    char name[64];
    char type[32]; // "number", "string", "hex"
    char value[512];
} Register;

typedef struct {
    char key[64];
    char type[32]; // "string" or "number"
    char value[256];
} ROMEntry;

typedef struct {
    char name[64];
    int startLine;
    int endLine;
} Function;

typedef struct {
    Register registers[MAX_REGISTERS];
    int regCount;
    ROMEntry romEntries[MAX_ROM_ENTRIES];
    int romCount;
    Function functions[MAX_FUNCTIONS];
    int funcCount;
} State;

State state;

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

char importedFiles[MAX_IMPORTED_FILES][256];
int importedFileCount = 0;

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
        if (strlen(line) == 0 || line[0] == ';') continue;

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

void addRegister(const char *name, const char *type, const char *value) {
    if (!isValidVarName(name)) return;
    
    // Check if register exists and update it
    for (int i = 0; i < state.regCount; i++) {
        if (strcmp(state.registers[i].name, name) == 0) {
            strcpy(state.registers[i].type, type);
            strcpy(state.registers[i].value, value);
            return;
        }
    }
    
    // Add new register
    if (state.regCount < MAX_REGISTERS) {
        strcpy(state.registers[state.regCount].name, name);
        strcpy(state.registers[state.regCount].type, type);
        strcpy(state.registers[state.regCount].value, value);
        state.regCount++;
    }
}

Register *getRegister(const char *name) {
    for (int i = 0; i < state.regCount; i++) {
        if (strcmp(state.registers[i].name, name) == 0) {
            return &state.registers[i];
        }
    }
    return NULL;
}

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
        if (strlen(line) == 0 || line[0] == ';') continue;

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

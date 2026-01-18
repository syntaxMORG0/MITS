#ifndef TYPES_H
#define TYPES_H

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

extern State state;
extern char importedFiles[MAX_IMPORTED_FILES][256];
extern int importedFileCount;

#endif // TYPES_H

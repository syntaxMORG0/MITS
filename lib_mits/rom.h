#ifndef ROM_H
#define ROM_H

#include "types.h"

// Add a ROM entry to the state
int addROMEntry(const char *key, const char *type, const char *value);

// Get a ROM entry from the state
int getROMEntry(const char *key, char *type, char *value);

// Parse a ROM file and populate ROM entries
void parseROMFile(const char *filename);

// Check if a file has already been imported
int isFileImported(const char *filename);

// Mark a file as imported
void markFileImported(const char *filename);

#endif // ROM_H

#ifndef REGISTER_H
#define REGISTER_H

#include "types.h"

// Add or update a register in the state
void addRegister(const char *name, const char *type, const char *value);

// Get a register from the state by name
Register *getRegister(const char *name);

#endif // REGISTER_H

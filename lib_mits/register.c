#include "register.h"
#include "utils.h"
#include <string.h>

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

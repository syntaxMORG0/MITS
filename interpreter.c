#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

#define MAX_LINES 1024
#define MAX_LINE_LENGTH 512
#define MAX_REGISTERS 256
#define MAX_FUNCTIONS 128
#define MAX_ROM_ENTRIES 512
#define MAX_ARGS 64
#define MAX_IMPORTED_FILES 64

typedef enum {
    TYPE_NUMBER,
    TYPE_STRING,
    TYPE_HEX
} ValueType;

typedef struct {
    ValueType type;
    union {
        long long numValue;
        char strValue[512];
        unsigned char hexValue[256];
    } data;
    int hexLen; // for HEX type
} Value;

typedef struct {
    char name[64];
    Value value;
} Register;

typedef struct {
    char key[64];
    Value value;
} ROMEntry;

typedef struct {
    char name[64];
    int startLine;
    int endLine;
    int argCount;
} Function;

typedef struct {
    Register registers[MAX_REGISTERS];
    int regCount;
    ROMEntry romEntries[MAX_ROM_ENTRIES];
    int romCount;
    Function functions[MAX_FUNCTIONS];
    int funcCount;
    long long arguments[MAX_ARGS];
    int argCount;
    int exitCode;
    int shouldExit;
} State;

State state;
char lines[MAX_LINES][MAX_LINE_LENGTH];
int lineCount = 0;

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

void stripComments(char *str) {
    char *comment = strchr(str, ';');
    if (comment) {
        *comment = '\0';
    }
    trimWhitespace(str);
}

char *getFirstWord(char *str, char *word) {
    int i = 0;
    while (*str && !isspace(*str) && *str != ',' && i < 63) {
        word[i++] = *str++;
    }
    word[i] = '\0';
    while (*str && (isspace(*str) || *str == ',')) str++;
    return str;
}

void addRegister(const char *name, Value value) {
    for (int i = 0; i < state.regCount; i++) {
        if (strcmp(state.registers[i].name, name) == 0) {
            state.registers[i].value = value;
            return;
        }
    }
    if (state.regCount < MAX_REGISTERS) {
        strcpy(state.registers[state.regCount].name, name);
        state.registers[state.regCount].value = value;
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

int addROMEntry(const char *key, Value value) {
    if (state.romCount < MAX_ROM_ENTRIES) {
        strcpy(state.romEntries[state.romCount].key, key);
        state.romEntries[state.romCount].value = value;
        state.romCount++;
        return 0;
    }
    return -1;
}

ROMEntry *getROMEntry(const char *key) {
    for (int i = 0; i < state.romCount; i++) {
        if (strcmp(state.romEntries[i].key, key) == 0) {
            return &state.romEntries[i];
        }
    }
    return NULL;
}

void parseROMFile(const char *filename);

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

        char *eq = strchr(line, '=');
        if (eq) {
            *eq = '\0';
            char key[64];
            strcpy(key, line);
            trimWhitespace(key);

            char *val = eq + 1;
            trimWhitespace(val);

            Value v;
            if (val[0] == '"') {
                v.type = TYPE_STRING;
                val++;
                char *endQuote = strchr(val, '"');
                if (endQuote) *endQuote = '\0';
                strncpy(v.data.strValue, val, 511);
            } else {
                v.type = TYPE_NUMBER;
                v.data.numValue = strtoll(val, NULL, 10);
            }
            addROMEntry(key, v);
        }
    }
    fclose(f);
}

Value parseValue(const char *str);

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

Value stringToHex(const char *str) {
    Value v;
    v.type = TYPE_HEX;
    v.hexLen = 0;
    while (*str && v.hexLen < 256) {
        v.data.hexValue[v.hexLen++] = (unsigned char)*str++;
    }
    return v;
}

Value hexToString(const Value *hex) {
    Value v;
    v.type = TYPE_STRING;
    int len = hex->hexLen < 511 ? hex->hexLen : 511;
    memcpy(v.data.strValue, hex->data.hexValue, len);
    v.data.strValue[len] = '\0';
    return v;
}

Value parseValue(const char *str) {
    Value v;
    char word[64];
    strcpy(word, str);
    trimWhitespace(word);

    // Check for rom=key
    if (strncmp(word, "rom=", 4) == 0) {
        ROMEntry *entry = getROMEntry(word + 4);
        if (entry) return entry->value;
        v.type = TYPE_NUMBER;
        v.data.numValue = 0;
        return v;
    }

    // Check for code=N
    if (strncmp(word, "code=", 5) == 0) {
        v.type = TYPE_NUMBER;
        v.data.numValue = strtoll(word + 5, NULL, 10);
        return v;
    }

    // Check for hex=register
    if (strncmp(word, "hex=", 4) == 0) {
        Register *reg = getRegister(word + 4);
        if (reg && reg->value.type == TYPE_STRING) {
            return stringToHex(reg->value.data.strValue);
        }
        v.type = TYPE_HEX;
        v.hexLen = 0;
        return v;
    }

    // Check for b31 register (hex to string)
    if (strncmp(word, "b31 ", 4) == 0) {
        Register *reg = getRegister(word + 4);
        if (reg && reg->value.type == TYPE_HEX) {
            return hexToString(&reg->value);
        }
        v.type = TYPE_STRING;
        strcpy(v.data.strValue, "");
        return v;
    }

    // Check if it's a register name
    Register *reg = getRegister(word);
    if (reg) return reg->value;

    // Check for special values
    if (strcmp(word, "ARGUMENTS") == 0) {
        v.type = TYPE_NUMBER;
        long long sum = 0;
        for (int i = 0; i < state.argCount; i++) sum += state.arguments[i];
        v.data.numValue = sum;
        return v;
    }

    // Try to parse as number
    v.type = TYPE_NUMBER;
    v.data.numValue = strtoll(word, NULL, 10);
    return v;
}

long long evaluateExpression(const char *expr) {
    char *plus = strchr(expr, '+');
    char *minus = strchr(expr, '-');
    
    // Find the operator
    char *op = NULL;
    char opChar = 0;
    
    if (plus && (!minus || plus < minus)) {
        op = plus;
        opChar = '+';
    } else if (minus && (!plus || minus < plus)) {
        op = minus;
        opChar = '-';
    }

    if (!op) {
        Value v = parseValue(expr);
        return v.data.numValue;
    }

    char left[256], right[256];
    strncpy(left, expr, op - expr);
    left[op - expr] = '\0';
    strcpy(right, op + 1);

    trimWhitespace(left);
    trimWhitespace(right);

    long long lval = evaluateExpression(left);
    long long rval = evaluateExpression(right);

    return opChar == '+' ? lval + rval : lval - rval;
}

int findMatchingEnd(int startLine) {
    int depth = 1;
    for (int i = startLine + 1; i < lineCount; i++) {
        char word[64];
        getFirstWord(lines[i], word);
        if (strcmp(word, "for") == 0 || strcmp(word, "def") == 0 || strcmp(word, "cond") == 0) {
            depth++;
        } else if (strcmp(word, "end") == 0) {
            depth--;
            if (depth == 0) return i;
        }
    }
    return -1;
}

void executeProgram(int startLine, int endLine);

void executeInstruction(const char *line) {
    if (state.shouldExit) return;
    if (strlen(line) == 0 || line[0] == ';') return;

    char lineCopy[MAX_LINE_LENGTH];
    strcpy(lineCopy, line);
    stripComments(lineCopy);
    
    if (strlen(lineCopy) == 0) return;

    char instruction[64];
    char *remaining = getFirstWord(lineCopy, instruction);

    if (strcmp(instruction, "mov") == 0) {
        char dest[64], temp[256];
        remaining = getFirstWord(remaining, dest);
        if (!isValidVarName(dest)) return;
        strcpy(temp, remaining);
        Value val = parseValue(temp);
        addRegister(dest, val);
    }

    else if (strcmp(instruction, "rdl") == 0) {
        char dest[64];
        remaining = getFirstWord(remaining, dest);
        if (!isValidVarName(dest)) return;
        
        char buffer[512];
        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            // Remove newline if present
            int len = strlen(buffer);
            if (len > 0 && buffer[len-1] == '\n') {
                buffer[len-1] = '\0';
            }
            Value v;
            v.type = TYPE_STRING;
            strncpy(v.data.strValue, buffer, 511);
            addRegister(dest, v);
        }
    }

    else if (strcmp(instruction, "char") == 0) {
        char dest[64], temp[256];
        remaining = getFirstWord(remaining, dest);
        if (!isValidVarName(dest)) return;
        
        char *start = strchr(remaining, '"');
        if (start) {
            start++;
            char *end = strchr(start, '"');
            if (end) {
                *end = '\0';
                Value v;
                v.type = TYPE_STRING;
                strncpy(v.data.strValue, start, 511);
                addRegister(dest, v);
            }
        }
    }

    else if (strcmp(instruction, "addr") == 0) {
        char dest[64], expr[256];
        remaining = getFirstWord(remaining, dest);
        if (!isValidVarName(dest)) return;
        strcpy(expr, remaining);

        Register *destReg = getRegister(dest);
        
        // Check if we're concatenating hex
        if (destReg && destReg->value.type == TYPE_HEX) {
            // Hex concatenation
            Value result;
            result.type = TYPE_HEX;
            result.hexLen = 0;
            
            char *plus = strchr(expr, '+');
            if (plus) {
                char left[256], right[256];
                strncpy(left, expr, plus - expr);
                left[plus - expr] = '\0';
                strcpy(right, plus + 1);
                
                trimWhitespace(left);
                trimWhitespace(right);
                
                Value lval = parseValue(left);
                Value rval = parseValue(right);
                
                if (lval.type == TYPE_HEX) {
                    memcpy(result.data.hexValue, lval.data.hexValue, lval.hexLen);
                    result.hexLen = lval.hexLen;
                }
                if (rval.type == TYPE_HEX) {
                    memcpy(result.data.hexValue + result.hexLen, rval.data.hexValue, rval.hexLen);
                    result.hexLen += rval.hexLen;
                }
            }
            addRegister(dest, result);
        } else {
            // Numeric addition
            long long val = evaluateExpression(expr);
            Value v;
            v.type = TYPE_NUMBER;
            v.data.numValue = val;
            addRegister(dest, v);
        }
    }

    else if (strcmp(instruction, "sda") == 0) {
        char dest[64];
        remaining = getFirstWord(remaining, dest);
        if (!isValidVarName(dest)) return;

        long long sum = 0;
        if (strncmp(remaining, "ARGUMENTS", 9) == 0) {
            for (int i = 0; i < state.argCount; i++) sum += state.arguments[i];
        } else {
            while (*remaining) {
                char word[64];
                remaining = getFirstWord(remaining, word);
                if (strlen(word) == 0) break;
                Value v = parseValue(word);
                sum += v.data.numValue;
            }
        }
        
        Value v;
        v.type = TYPE_NUMBER;
        v.data.numValue = sum;
        addRegister(dest, v);
    }

    else if (strcmp(instruction, "subr") == 0) {
        char dest[64], expr[256];
        remaining = getFirstWord(remaining, dest);
        strcpy(expr, remaining);

        long long val = evaluateExpression(expr);
        Value v;
        v.type = TYPE_NUMBER;
        v.data.numValue = val;
        addRegister(dest, v);
    }

    else if (strcmp(instruction, "mul") == 0) {
        char dest[64], expr[256];
        remaining = getFirstWord(remaining, dest);
        if (!isValidVarName(dest)) return;
        strcpy(expr, remaining);

        char *mul = strchr(expr, '*');
        if (mul) {
            char left[256], right[256];
            strncpy(left, expr, mul - expr);
            left[mul - expr] = '\0';
            strcpy(right, mul + 1);

            trimWhitespace(left);
            trimWhitespace(right);

            Value lval = parseValue(left);
            Value rval = parseValue(right);

            Value v;
            v.type = TYPE_NUMBER;
            v.data.numValue = lval.data.numValue * rval.data.numValue;
            addRegister(dest, v);
        }
    }

    else if (strcmp(instruction, "div") == 0) {
        char dest[64], expr[256];
        remaining = getFirstWord(remaining, dest);
        if (!isValidVarName(dest)) return;
        strcpy(expr, remaining);

        char *div = strchr(expr, '/');
        if (div) {
            char left[256], right[256];
            strncpy(left, expr, div - expr);
            left[div - expr] = '\0';
            strcpy(right, div + 1);

            trimWhitespace(left);
            trimWhitespace(right);

            Value lval = parseValue(left);
            Value rval = parseValue(right);

            Value v;
            v.type = TYPE_NUMBER;
            v.data.numValue = rval.data.numValue != 0 ? lval.data.numValue / rval.data.numValue : 0;
            addRegister(dest, v);
        }
    }

    else if (strcmp(instruction, "mod") == 0) {
        char dest[64], expr[256];
        remaining = getFirstWord(remaining, dest);
        if (!isValidVarName(dest)) return;
        strcpy(expr, remaining);

        char *mod = strchr(expr, '%');
        if (mod) {
            char left[256], right[256];
            strncpy(left, expr, mod - expr);
            left[mod - expr] = '\0';
            strcpy(right, mod + 1);

            trimWhitespace(left);
            trimWhitespace(right);

            Value lval = parseValue(left);
            Value rval = parseValue(right);

            Value v;
            v.type = TYPE_NUMBER;
            v.data.numValue = rval.data.numValue != 0 ? lval.data.numValue % rval.data.numValue : 0;
            addRegister(dest, v);
        }
    }

    else if (strcmp(instruction, "vga") == 0) {
        Value val = parseValue(remaining);
        
        if (val.type == TYPE_NUMBER) {
            printf("%lld\n", val.data.numValue);
        } else if (val.type == TYPE_STRING) {
            printf("%s\n", val.data.strValue);
        } else if (val.type == TYPE_HEX) {
            for (int i = 0; i < val.hexLen; i++) {
                printf("%02x", val.data.hexValue[i]);
                if (i < val.hexLen - 1) printf(" ");
            }
            printf("\n");
        }
    }

    else if (strcmp(instruction, "exec") == 0) {
        if (strncmp(remaining, "sys=help", 8) == 0) {
            printf("mov, char, hex, addr, subr, mul, div, mod, vga, exec, cond, for, sda, def, req\n");
        } else {
            Value val = parseValue(remaining);
            if (val.type == TYPE_NUMBER && val.data.numValue <= 255) {
                state.exitCode = val.data.numValue;
                state.shouldExit = 1;
            }
        }
    }

    else if (strcmp(instruction, "req") == 0) {
        // Parse: req ftype="rom"|"asm", "filepath"
        char ftype[64] = "";
        char filepath[256] = "";
        
        // Extract ftype
        char *ftypeStart = strchr(remaining, '"');
        if (ftypeStart) {
            ftypeStart++;
            char *ftypeEnd = strchr(ftypeStart, '"');
            if (ftypeEnd) {
                strncpy(ftype, ftypeStart, ftypeEnd - ftypeStart);
                ftype[ftypeEnd - ftypeStart] = '\0';
                remaining = ftypeEnd + 1;
            }
        }
        
        // Extract filepath
        char *pathStart = strchr(remaining, '"');
        if (pathStart) {
            pathStart++;
            char *pathEnd = strchr(pathStart, '"');
            if (pathEnd) {
                strncpy(filepath, pathStart, pathEnd - pathStart);
                filepath[pathEnd - pathStart] = '\0';
            }
        }
        
        // Process import
        if (strlen(ftype) > 0 && strlen(filepath) > 0) {
            if (strcmp(ftype, "rom") == 0) {
                if (!isFileImported(filepath)) {
                    parseROMFile(filepath);
                    markFileImported(filepath);
                }
            } else if (strcmp(ftype, "asm") == 0) {
                if (!isFileImported(filepath)) {
                    // Load and execute imported assembly
                    FILE *f = fopen(filepath, "r");
                    if (f) {
                        char tmpLines[MAX_LINES][MAX_LINE_LENGTH];
                        int tmpLineCount = 0;
                        while (tmpLineCount < MAX_LINES && fgets(tmpLines[tmpLineCount], MAX_LINE_LENGTH, f)) {
                            trimWhitespace(tmpLines[tmpLineCount]);
                            tmpLineCount++;
                        }
                        fclose(f);
                        
                        // Execute imported assembly (skip _start if present)
                        for (int j = 0; j < tmpLineCount; j++) {
                            if (strcmp(tmpLines[j], "_start:") == 0) continue;
                            if (strlen(tmpLines[j]) == 0 || tmpLines[j][0] == ';') continue;
                            executeInstruction(tmpLines[j]);
                        }
                        markFileImported(filepath);
                    }
                }
            }
        }
    }
}

void executeProgram(int startLine, int endLine) {
    for (int i = startLine; i <= endLine && !state.shouldExit; i++) {
        char *line = lines[i];
        if (strlen(line) == 0 || line[0] == ';') continue;
        
        // Skip label lines (label: format, but not "exec:" or "something: ...")
        if (line[strlen(line) - 1] == ':') {
            // Only skip if it's a label (starts right after whitespace with text then :)
            char temp[512];
            strcpy(temp, line);
            trimWhitespace(temp);
            if (strchr(temp, ' ') == NULL) {
                // No space, so it's just "label:"
                continue;
            }
        }

        char instruction[64];
        getFirstWord(line, instruction);

        if (strcmp(instruction, "for") == 0) {
            // Parse: for mov index, start, end, exec: ... end
            char line_copy[512];
            strcpy(line_copy, line);
            
            // Skip "for " to get "mov index, start, end, exec:"
            char *for_part = line_copy + 4;
            
            // Find exec: to separate the declaration from exec:
            char *exec_marker = strstr(for_part, ", exec:");
            if (exec_marker) {
                *exec_marker = '\0'; // Terminate at ", exec:"
                trimWhitespace(for_part);
                
                // Now for_part contains "mov index, start, end"
                // Need to parse: mov VAR, START, END
                char for_copy[256];
                strcpy(for_copy, for_part);
                
                // Replace commas with spaces for tokenization
                for (int c = 0; c < (int)strlen(for_copy); c++) {
                    if (for_copy[c] == ',') for_copy[c] = ' ';
                }
                
                char *mov_word = strtok(for_copy, " ");
                char *var_name = strtok(NULL, " ");
                char *start_str = strtok(NULL, " ");
                char *end_str = strtok(NULL, " ");
                
                if (mov_word && var_name && start_str && end_str && strcmp(mov_word, "mov") == 0) {
                    if (isValidVarName(var_name)) {
                        Value start_val = parseValue(start_str);
                        Value end_val = parseValue(end_str);
                        
                        if (start_val.type == TYPE_NUMBER && end_val.type == TYPE_NUMBER) {
                            long long start = start_val.data.numValue;
                            long long end = end_val.data.numValue;
                            
                            int for_end = findMatchingEnd(i);
                            
                            // Execute for loop
                            for (long long loop_val = start; loop_val <= end; loop_val++) {
                                // Set loop variable
                                Value loop_value;
                                loop_value.type = TYPE_NUMBER;
                                loop_value.data.numValue = loop_val;
                                addRegister(var_name, loop_value);
                                
                                // Execute body using executeProgram for nested construct support
                                executeProgram(i + 1, for_end - 1);
                                
                                if (state.shouldExit) break;
                            }
                            
                            i = for_end;
                        }
                    }
                }
            }
        } else if (strcmp(instruction, "cond") == 0) {
            // Parse conditional: cond a OP b, exec: ... end
            // Supports: <, >, <=, >=, ==, !=
            char line_copy[512];
            strcpy(line_copy, line);
            
            // Remove leading "cond "
            char *cond_part = line_copy + 5;
            
            // Find comma
            char *comma = strchr(cond_part, ',');
            if (comma) {
                *comma = '\0'; // Terminate at comma
                
                // Now cond_part contains "a OP b"
                trimWhitespace(cond_part);
                
                // Tokenize to get left, op, right
                char cond_copy[256];
                strcpy(cond_copy, cond_part);
                
                char *left_str = strtok(cond_copy, " ");
                char *op = strtok(NULL, " ");
                char *right_str = strtok(NULL, " ");
                
                if (left_str && op && right_str) {
                    Value left = parseValue(left_str);
                    Value right = parseValue(right_str);
                    
                    int cond_true = 0;
                    if (left.type == TYPE_NUMBER && right.type == TYPE_NUMBER) {
                        long long l = left.data.numValue;
                        long long r = right.data.numValue;
                        
                        if (strcmp(op, "<") == 0) cond_true = (l < r);
                        else if (strcmp(op, ">") == 0) cond_true = (l > r);
                        else if (strcmp(op, "<=") == 0) cond_true = (l <= r);
                        else if (strcmp(op, ">=") == 0) cond_true = (l >= r);
                        else if (strcmp(op, "==") == 0) cond_true = (l == r);
                        else if (strcmp(op, "!=") == 0) cond_true = (l != r);
                    }
                    
                    int end_line = findMatchingEnd(i);
                    int else_line = -1;
                    
                    // Look for else block (line must be exactly "else")
                    for (int j = i + 1; j < end_line; j++) {
                        if (strcmp(lines[j], "else") == 0) {
                            else_line = j;
                            break;
                        }
                    }
                    
                    if (cond_true) {
                        // Execute lines from i+1 until "else" or "end"
                        for (int j = i + 1; j < end_line; j++) {
                            if (strcmp(lines[j], "else") == 0) break;
                            if (strcmp(lines[j], "exec:") == 0) continue;
                            if (lines[j][0] == '\0') continue;
                            
                            executeInstruction(lines[j]);
                            if (state.shouldExit) break;
                        }
                    } else if (else_line != -1) {
                        // Execute lines from else_line+1 until "end"
                        for (int j = else_line + 1; j < end_line; j++) {
                            if (strcmp(lines[j], "end") == 0) break;
                            if (lines[j][0] == '\0') continue;
                            
                            executeInstruction(lines[j]);
                            if (state.shouldExit) break;
                        }
                    }
                    
                    i = end_line;





                }
            }

        } else if (strcmp(instruction, "def") == 0) {
            // Skip function definitions
            i = findMatchingEnd(i);
        } else {
            executeInstruction(line);
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input.s> [rom.data]\n", argv[0]);
        return 1;
    }

    // Parse ROM file (optional)
    if (argc >= 3) {
        parseROMFile(argv[2]);
    }

    // Read assembly file
    FILE *in = fopen(argv[1], "r");
    if (!in) {
        fprintf(stderr, "Error: Cannot open input file '%s'\n", argv[1]);
        return 1;
    }

    while (lineCount < MAX_LINES && fgets(lines[lineCount], MAX_LINE_LENGTH, in)) {
        trimWhitespace(lines[lineCount]);
        lineCount++;
    }
    fclose(in);

    // Find _start label
    int startIdx = -1;
    for (int i = 0; i < lineCount; i++) {
        if (strcmp(lines[i], "_start:") == 0) {
            startIdx = i + 1;
            break;
        }
    }

    if (startIdx == -1) {
        fprintf(stderr, "Error: Missing _start: label\n");
        return 1;
    }

    // Execute program
    executeProgram(startIdx, lineCount - 1);

    if (state.exitCode != 0) {
        printf("program finished with: code %d\n", state.exitCode);
    }

    return state.exitCode;
}

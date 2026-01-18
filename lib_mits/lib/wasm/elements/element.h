#ifndef WASM_ELEMENT_H
#define WASM_ELEMENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *type;     // h1, h2, span, div, button, etc.
    char *id;       // unique identifier
    char *class;    // CSS class
    char *txt;      // text content
} WasmElement;

typedef struct {
    char *name;
    WasmElement *elements;
    int element_count;
    int capacity;
} WasmPage;

// Element creation
WasmElement* element_create(const char *type, const char *id, const char *class, const char *txt);
void element_free(WasmElement *elem);

// Page management
WasmPage* page_create(const char *name);
void page_add_element(WasmPage *page, WasmElement *elem);
void page_free(WasmPage *page);

// HTML generation
void element_to_html(FILE *out, WasmElement *elem);
void page_to_html(FILE *out, WasmPage *page);

#endif

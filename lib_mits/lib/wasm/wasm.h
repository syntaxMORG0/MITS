#ifndef WASM_INSTRUCTION_H
#define WASM_INSTRUCTION_H

#include "elements/element.h"

typedef struct {
    WasmPage **pages;
    int page_count;
    int page_capacity;
    
    void **scripts;      // JavaScript/C code blocks
    char **script_types;  // ftype: "javascript" or "clang"
    char **script_ids;    // id for script
    int script_count;
    int script_capacity;
    
    char **css_files;    // External CSS file paths
    char **js_files;     // External JavaScript file paths
    char *inline_css;    // Inline CSS styles
    int css_file_count;
    int js_file_count;
    
    int web_port;        // Active web server port
    char *active_page;   // Currently active page
} WasmState;

// Initialize WebAssembly state
WasmState* wasm_init(void);
void wasm_free(WasmState *state);

// Instruction handlers
// wasm -np page="name"
WasmPage* wasm_new_page(WasmState *state, const char *page_name);

// wasm -ne type="h1" txt="..." class="..." id="..."
WasmElement* wasm_new_element(const char *type, const char *id, const char *class, const char *txt);

// wasm -ns ftype="javascript|clang" id="..." exec => { ... }
void wasm_new_script(WasmState *state, const char *ftype, const char *id, const char *code);

// wasm -ae id="element_or_script" page="page_name"
void wasm_attach_element(WasmState *state, const char *id, const char *page_name);

// wasm -op port="8000" page="home"
void wasm_open_server(WasmState *state, int port, const char *page_name);

// Generate HTML for all pages
char* wasm_generate_html(WasmState *state);

// Utility Functions
WasmPage* wasm_find_page(WasmState *state, const char *name);
WasmElement* wasm_find_element(WasmPage *page, const char *id);
void wasm_add_css(WasmState *state, const char *css_content);
void wasm_add_js_file(WasmState *state, const char *filepath);
void wasm_add_css_file(WasmState *state, const char *filepath);
void wasm_list_pages(WasmState *state);
void wasm_list_elements(WasmPage *page);

#endif

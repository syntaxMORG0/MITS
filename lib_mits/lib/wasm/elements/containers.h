#ifndef WASM_CONTAINERS_H
#define WASM_CONTAINERS_H

#include "element.h"

// Container and layout elements
// div - Generic container/block element
// section - Semantic content section
// article - Self-contained article content
// header - Page/section header
// footer - Page/section footer
// nav - Navigation container
// main - Main content area
// aside - Sidebar/supplementary content

WasmElement* wasm_div(const char *id, const char *class);
WasmElement* wasm_section(const char *id, const char *class);
WasmElement* wasm_article(const char *id, const char *class);
WasmElement* wasm_header(const char *id, const char *class);
WasmElement* wasm_footer(const char *id, const char *class);
WasmElement* wasm_nav(const char *id, const char *class);
WasmElement* wasm_main(const char *id, const char *class);
WasmElement* wasm_aside(const char *id, const char *class);

#endif

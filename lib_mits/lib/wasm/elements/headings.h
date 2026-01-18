#ifndef WASM_HEADINGS_H
#define WASM_HEADINGS_H

#include "element.h"

// Heading elements h1-h6 for semantic structure
// h1 - Main page heading (typically one per page)
// h2 - Section headings
// h3 - Subsection headings
// h4-h6 - Further hierarchy levels

WasmElement* wasm_h1(const char *txt, const char *id, const char *class);
WasmElement* wasm_h2(const char *txt, const char *id, const char *class);
WasmElement* wasm_h3(const char *txt, const char *id, const char *class);
WasmElement* wasm_h4(const char *txt, const char *id, const char *class);
WasmElement* wasm_h5(const char *txt, const char *id, const char *class);
WasmElement* wasm_h6(const char *txt, const char *id, const char *class);

#endif

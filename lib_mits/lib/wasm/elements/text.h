#ifndef WASM_TEXT_H
#define WASM_TEXT_H

#include "element.h"

// Text and content elements
// p - Paragraph text
// span - Inline text wrapper
// strong - Bold/important text
// em - Emphasized/italic text
// code - Inline code
// pre - Preformatted text

WasmElement* wasm_paragraph(const char *txt, const char *id, const char *class);
WasmElement* wasm_span(const char *txt, const char *id, const char *class);
WasmElement* wasm_strong(const char *txt, const char *id, const char *class);
WasmElement* wasm_em(const char *txt, const char *id, const char *class);
WasmElement* wasm_code(const char *txt, const char *id, const char *class);
WasmElement* wasm_pre(const char *txt, const char *id, const char *class);

#endif

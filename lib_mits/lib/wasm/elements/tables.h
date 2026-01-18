#ifndef WASM_TABLES_H
#define WASM_TABLES_H

#include "element.h"

// Table and list elements
// table - Data table
// thead - Table header
// tbody - Table body
// tr - Table row
// th - Table header cell
// td - Table data cell
// ul - Unordered list
// ol - Ordered list
// li - List item
// dl - Description list

WasmElement* wasm_table(const char *id, const char *class);
WasmElement* wasm_thead(const char *id, const char *class);
WasmElement* wasm_tbody(const char *id, const char *class);
WasmElement* wasm_tr(const char *id, const char *class);
WasmElement* wasm_th(const char *txt, const char *id, const char *class);
WasmElement* wasm_td(const char *txt, const char *id, const char *class);
WasmElement* wasm_ul(const char *id, const char *class);
WasmElement* wasm_ol(const char *id, const char *class);
WasmElement* wasm_li(const char *txt, const char *id, const char *class);
WasmElement* wasm_dl(const char *id, const char *class);

#endif

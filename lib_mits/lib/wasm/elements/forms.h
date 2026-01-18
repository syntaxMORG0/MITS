#ifndef WASM_FORMS_H
#define WASM_FORMS_H

#include "element.h"

// Form and input elements
// form - Form container
// input - Text, password, checkbox, radio, file inputs
// button - Clickable button
// select - Dropdown select
// textarea - Multi-line text input
// label - Form labels
// fieldset - Group related form fields

WasmElement* wasm_form(const char *id, const char *class);
WasmElement* wasm_input(const char *type, const char *placeholder, const char *id, const char *class);
WasmElement* wasm_button(const char *txt, const char *id, const char *class);
WasmElement* wasm_textarea(const char *placeholder, const char *id, const char *class);
WasmElement* wasm_label(const char *txt, const char *for_id, const char *class);
WasmElement* wasm_select(const char *id, const char *class);
WasmElement* wasm_fieldset(const char *legend, const char *id, const char *class);

#endif

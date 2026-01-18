#include "forms.h"
#include <stdlib.h>
#include <string.h>

WasmElement* wasm_form(const char *id, const char *class) {
    WasmElement *form = element_create("form", id, class, NULL);
    return form;
}

WasmElement* wasm_input(const char *type, const char *placeholder, const char *id, const char *class) {
    // Note: input type and placeholder should be stored as attributes
    // For now, combine them in txt as a workaround
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "type=%s placeholder=%s", type ? type : "text", placeholder ? placeholder : "");
    WasmElement *input = element_create("input", id, class, buffer);
    return input;
}

WasmElement* wasm_button(const char *txt, const char *id, const char *class) {
    return element_create("button", id, class, txt);
}

WasmElement* wasm_textarea(const char *placeholder, const char *id, const char *class) {
    return element_create("textarea", id, class, placeholder);
}

WasmElement* wasm_label(const char *txt, const char *for_id, const char *class) {
    // for_id would be used as data attribute for now
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "for=%s", for_id ? for_id : "");
    WasmElement *label = element_create("label", NULL, class, txt);
    return label;
}

WasmElement* wasm_select(const char *id, const char *class) {
    return element_create("select", id, class, NULL);
}

WasmElement* wasm_fieldset(const char *legend, const char *id, const char *class) {
    return element_create("fieldset", id, class, legend);
}

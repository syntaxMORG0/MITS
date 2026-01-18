#include "text.h"

WasmElement* wasm_paragraph(const char *txt, const char *id, const char *class) {
    return element_create("p", id, class, txt);
}

WasmElement* wasm_span(const char *txt, const char *id, const char *class) {
    return element_create("span", id, class, txt);
}

WasmElement* wasm_strong(const char *txt, const char *id, const char *class) {
    return element_create("strong", id, class, txt);
}

WasmElement* wasm_em(const char *txt, const char *id, const char *class) {
    return element_create("em", id, class, txt);
}

WasmElement* wasm_code(const char *txt, const char *id, const char *class) {
    return element_create("code", id, class, txt);
}

WasmElement* wasm_pre(const char *txt, const char *id, const char *class) {
    return element_create("pre", id, class, txt);
}

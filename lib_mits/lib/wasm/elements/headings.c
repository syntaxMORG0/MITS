#include "headings.h"

WasmElement* wasm_h1(const char *txt, const char *id, const char *class) {
    return element_create("h1", id, class, txt);
}

WasmElement* wasm_h2(const char *txt, const char *id, const char *class) {
    return element_create("h2", id, class, txt);
}

WasmElement* wasm_h3(const char *txt, const char *id, const char *class) {
    return element_create("h3", id, class, txt);
}

WasmElement* wasm_h4(const char *txt, const char *id, const char *class) {
    return element_create("h4", id, class, txt);
}

WasmElement* wasm_h5(const char *txt, const char *id, const char *class) {
    return element_create("h5", id, class, txt);
}

WasmElement* wasm_h6(const char *txt, const char *id, const char *class) {
    return element_create("h6", id, class, txt);
}

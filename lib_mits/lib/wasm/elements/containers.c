#include "containers.h"

WasmElement* wasm_div(const char *id, const char *class) {
    return element_create("div", id, class, NULL);
}

WasmElement* wasm_section(const char *id, const char *class) {
    return element_create("section", id, class, NULL);
}

WasmElement* wasm_article(const char *id, const char *class) {
    return element_create("article", id, class, NULL);
}

WasmElement* wasm_header(const char *id, const char *class) {
    return element_create("header", id, class, NULL);
}

WasmElement* wasm_footer(const char *id, const char *class) {
    return element_create("footer", id, class, NULL);
}

WasmElement* wasm_nav(const char *id, const char *class) {
    return element_create("nav", id, class, NULL);
}

WasmElement* wasm_main(const char *id, const char *class) {
    return element_create("main", id, class, NULL);
}

WasmElement* wasm_aside(const char *id, const char *class) {
    return element_create("aside", id, class, NULL);
}

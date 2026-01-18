#include "tables.h"

WasmElement* wasm_table(const char *id, const char *class) {
    return element_create("table", id, class, NULL);
}

WasmElement* wasm_thead(const char *id, const char *class) {
    return element_create("thead", id, class, NULL);
}

WasmElement* wasm_tbody(const char *id, const char *class) {
    return element_create("tbody", id, class, NULL);
}

WasmElement* wasm_tr(const char *id, const char *class) {
    return element_create("tr", id, class, NULL);
}

WasmElement* wasm_th(const char *txt, const char *id, const char *class) {
    return element_create("th", id, class, txt);
}

WasmElement* wasm_td(const char *txt, const char *id, const char *class) {
    return element_create("td", id, class, txt);
}

WasmElement* wasm_ul(const char *id, const char *class) {
    return element_create("ul", id, class, NULL);
}

WasmElement* wasm_ol(const char *id, const char *class) {
    return element_create("ol", id, class, NULL);
}

WasmElement* wasm_li(const char *txt, const char *id, const char *class) {
    return element_create("li", id, class, txt);
}

WasmElement* wasm_dl(const char *id, const char *class) {
    return element_create("dl", id, class, NULL);
}

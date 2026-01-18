#include "media.h"
#include <stdlib.h>
#include <string.h>

WasmElement* wasm_img(const char *src, const char *alt, const char *id, const char *class) {
    char buffer[512];
    snprintf(buffer, sizeof(buffer), "src=%s alt=%s", src ? src : "", alt ? alt : "");
    return element_create("img", id, class, buffer);
}

WasmElement* wasm_link(const char *href, const char *text, const char *id, const char *class) {
    char buffer[512];
    snprintf(buffer, sizeof(buffer), "href=%s", href ? href : "#");
    WasmElement *link = element_create("a", id, class, text);
    return link;
}

WasmElement* wasm_link_resource(const char *rel, const char *href) {
    char buffer[512];
    snprintf(buffer, sizeof(buffer), "rel=%s href=%s", rel ? rel : "", href ? href : "");
    return element_create("link", NULL, NULL, buffer);
}

WasmElement* wasm_video(const char *src, const char *id, const char *class) {
    char buffer[512];
    snprintf(buffer, sizeof(buffer), "src=%s controls", src ? src : "");
    return element_create("video", id, class, buffer);
}

WasmElement* wasm_audio(const char *src, const char *id, const char *class) {
    char buffer[512];
    snprintf(buffer, sizeof(buffer), "src=%s controls", src ? src : "");
    return element_create("audio", id, class, buffer);
}

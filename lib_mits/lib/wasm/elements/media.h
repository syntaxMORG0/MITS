#ifndef WASM_MEDIA_H
#define WASM_MEDIA_H

#include "element.h"

// Media and link elements
// img - Image element
// a - Hyperlink
// link - External resource link (CSS, favicon, etc.)
// picture - Responsive image container
// video - Video player
// audio - Audio player
// source - Media source specification

WasmElement* wasm_img(const char *src, const char *alt, const char *id, const char *class);
WasmElement* wasm_link(const char *href, const char *text, const char *id, const char *class);
WasmElement* wasm_link_resource(const char *rel, const char *href);
WasmElement* wasm_video(const char *src, const char *id, const char *class);
WasmElement* wasm_audio(const char *src, const char *id, const char *class);

#endif

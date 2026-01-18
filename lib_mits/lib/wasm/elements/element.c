#include "element.h"

WasmElement* element_create(const char *type, const char *id, const char *class, const char *txt) {
    WasmElement *elem = malloc(sizeof(WasmElement));
    elem->type = type ? malloc(strlen(type) + 1) : NULL;
    elem->id = id ? malloc(strlen(id) + 1) : NULL;
    elem->class = class ? malloc(strlen(class) + 1) : NULL;
    elem->txt = txt ? malloc(strlen(txt) + 1) : NULL;
    
    if (elem->type) strcpy(elem->type, type);
    if (elem->id) strcpy(elem->id, id);
    if (elem->class) strcpy(elem->class, class);
    if (elem->txt) strcpy(elem->txt, txt);
    
    return elem;
}

void element_free(WasmElement *elem) {
    if (!elem) return;
    free(elem->type);
    free(elem->id);
    free(elem->class);
    free(elem->txt);
    free(elem);
}

WasmPage* page_create(const char *name) {
    WasmPage *page = malloc(sizeof(WasmPage));
    page->name = malloc(strlen(name) + 1);
    strcpy(page->name, name);
    page->elements = malloc(sizeof(WasmElement) * 10);
    page->element_count = 0;
    page->capacity = 10;
    return page;
}

void page_add_element(WasmPage *page, WasmElement *elem) {
    if (page->element_count >= page->capacity) {
        page->capacity *= 2;
        page->elements = realloc(page->elements, sizeof(WasmElement) * page->capacity);
    }
    page->elements[page->element_count++] = *elem;
}

void page_free(WasmPage *page) {
    if (!page) return;
    free(page->name);
    for (int i = 0; i < page->element_count; i++) {
        element_free(&page->elements[i]);
    }
    free(page->elements);
    free(page);
}

void element_to_html(FILE *out, WasmElement *elem) {
    fprintf(out, "<%s", elem->type);
    if (elem->id) fprintf(out, " id=\"%s\"", elem->id);
    if (elem->class) fprintf(out, " class=\"%s\"", elem->class);
    fprintf(out, ">");
    if (elem->txt) fprintf(out, "%s", elem->txt);
    fprintf(out, "</%s>\n", elem->type);
}

void page_to_html(FILE *out, WasmPage *page) {
    fprintf(out, "<!-- Page: %s -->\n", page->name);
    for (int i = 0; i < page->element_count; i++) {
        element_to_html(out, &page->elements[i]);
    }
}

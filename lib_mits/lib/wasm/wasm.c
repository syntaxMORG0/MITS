#include "wasm.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

WasmState* wasm_init(void) {
    WasmState *state = malloc(sizeof(WasmState));
    state->pages = malloc(sizeof(WasmPage*) * 10);
    state->page_count = 0;
    state->page_capacity = 10;
    
    state->scripts = malloc(sizeof(void*) * 10);
    state->script_types = malloc(sizeof(char*) * 10);
    state->script_ids = malloc(sizeof(char*) * 10);
    state->script_count = 0;
    state->script_capacity = 10;
    
    state->css_files = malloc(sizeof(char*) * 10);
    state->js_files = malloc(sizeof(char*) * 10);
    state->inline_css = malloc(4096);  // 4KB for inline CSS
    state->inline_css[0] = '\0';
    state->css_file_count = 0;
    state->js_file_count = 0;
    
    state->web_port = 0;
    state->active_page = NULL;
    
    return state;
}

void wasm_free(WasmState *state) {
    if (!state) return;
    
    for (int i = 0; i < state->page_count; i++) {
        page_free(state->pages[i]);
    }
    free(state->pages);
    
    for (int i = 0; i < state->script_count; i++) {
        free(state->script_types[i]);
        free(state->script_ids[i]);
        free(state->scripts[i]);
    }
    free(state->scripts);
    free(state->script_types);
    free(state->script_ids);
    
    for (int i = 0; i < state->css_file_count; i++) {
        free(state->css_files[i]);
    }
    free(state->css_files);
    
    for (int i = 0; i < state->js_file_count; i++) {
        free(state->js_files[i]);
    }
    free(state->js_files);
    
    free(state->inline_css);
    if (state->active_page) free(state->active_page);
    free(state);
}

WasmPage* wasm_new_page(WasmState *state, const char *page_name) {
    if (state->page_count >= state->page_capacity) {
        state->page_capacity *= 2;
        state->pages = realloc(state->pages, sizeof(WasmPage*) * state->page_capacity);
    }
    
    WasmPage *page = page_create(page_name);
    state->pages[state->page_count++] = page;
    return page;
}

WasmElement* wasm_new_element(const char *type, const char *id, const char *class, const char *txt) {
    return element_create(type, id, class, txt);
}

void wasm_new_script(WasmState *state, const char *ftype, const char *id, const char *code) {
    if (state->script_count >= state->script_capacity) {
        state->script_capacity *= 2;
        state->scripts = realloc(state->scripts, sizeof(void*) * state->script_capacity);
        state->script_types = realloc(state->script_types, sizeof(char*) * state->script_capacity);
        state->script_ids = realloc(state->script_ids, sizeof(char*) * state->script_capacity);
    }
    
    state->script_types[state->script_count] = malloc(strlen(ftype) + 1);
    strcpy(state->script_types[state->script_count], ftype);
    
    state->script_ids[state->script_count] = malloc(strlen(id) + 1);
    strcpy(state->script_ids[state->script_count], id);
    
    state->scripts[state->script_count] = malloc(strlen(code) + 1);
    strcpy((char*)state->scripts[state->script_count], code);
    
    state->script_count++;
}

void wasm_attach_element(WasmState *state, const char *id, const char *page_name) {
    // Find the page
    WasmPage *page = NULL;
    for (int i = 0; i < state->page_count; i++) {
        if (strcmp(state->pages[i]->name, page_name) == 0) {
            page = state->pages[i];
            break;
        }
    }
    
    if (!page) return;
    
    // TODO: Implement element attachment logic
    // This would attach elements or scripts to pages
}

void wasm_open_server(WasmState *state, int port, const char *page_name) {
    state->web_port = port;
    if (state->active_page) free(state->active_page);
    state->active_page = malloc(strlen(page_name) + 1);
    strcpy(state->active_page, page_name);
}

char* wasm_generate_html(WasmState *state) {
    char *html = malloc(65536);  // Max 64KB HTML
    html[0] = '\0';
    
    strcat(html, "<!DOCTYPE html>\n<html>\n<head>\n<meta charset=\"UTF-8\">\n</head>\n<body>\n");
    
    for (int i = 0; i < state->page_count; i++) {
        char page_section[1024];
        sprintf(page_section, "<div id=\"page-%s\" class=\"page\">\n", state->pages[i]->name);
        strcat(html, page_section);
        
        for (int j = 0; j < state->pages[i]->element_count; j++) {
            char elem_html[512];
            WasmElement *elem = &state->pages[i]->elements[j];
            sprintf(elem_html, "  <%s", elem->type);
            if (elem->id) sprintf(elem_html + strlen(elem_html), " id=\"%s\"", elem->id);
            if (elem->class) sprintf(elem_html + strlen(elem_html), " class=\"%s\"", elem->class);
            sprintf(elem_html + strlen(elem_html), ">");
            if (elem->txt) sprintf(elem_html + strlen(elem_html), "%s", elem->txt);
            sprintf(elem_html + strlen(elem_html), "</%s>\n", elem->type);
            strcat(html, elem_html);
        }
        
        strcat(html, "</div>\n");
    }
    
    // Add scripts
    for (int i = 0; i < state->script_count; i++) {
        if (strcmp(state->script_types[i], "javascript") == 0) {
            strcat(html, "<script>\n");
            strcat(html, (char*)state->scripts[i]);
            strcat(html, "\n</script>\n");
        }
    }
    
    strcat(html, "</body>\n</html>");
    return html;
}

WasmPage* wasm_find_page(WasmState *state, const char *name) {
    if (!state || !name) return NULL;
    for (int i = 0; i < state->page_count; i++) {
        if (strcmp(state->pages[i]->name, name) == 0) {
            return state->pages[i];
        }
    }
    return NULL;
}

WasmElement* wasm_find_element(WasmPage *page, const char *id) {
    if (!page || !id) return NULL;
    for (int i = 0; i < page->element_count; i++) {
        if (page->elements[i].id && strcmp(page->elements[i].id, id) == 0) {
            return &page->elements[i];
        }
    }
    return NULL;
}

void wasm_add_css(WasmState *state, const char *css_content) {
    if (!state || !css_content) return;
    // TODO: Store CSS and inject into HTML generation
    // For now, this is a placeholder for future stylesheet support
}

void wasm_add_js_file(WasmState *state, const char *filepath) {
    if (!state || !filepath) return;
    // TODO: Read and include JavaScript file
}

void wasm_add_css_file(WasmState *state, const char *filepath) {
    if (!state || !filepath) return;
    // TODO: Read and include CSS file
}

void wasm_list_pages(WasmState *state) {
    if (!state) return;
    printf("Pages (%d):\n", state->page_count);
    for (int i = 0; i < state->page_count; i++) {
        printf("  - %s (%d elements)\n", state->pages[i]->name, state->pages[i]->element_count);
    }
}

void wasm_list_elements(WasmPage *page) {
    if (!page) return;
    printf("Elements in '%s' (%d):\n", page->name, page->element_count);
    for (int i = 0; i < page->element_count; i++) {
        WasmElement *elem = &page->elements[i];
        printf("  - <%s>", elem->type);
        if (elem->id) printf(" id=\"%s\"", elem->id);
        if (elem->class) printf(" class=\"%s\"", elem->class);
        printf("\n");
    }
}

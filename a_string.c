/*
 * a_string/a_vector: a scuffed dynamic vector/string implementation.
 *
 * Copyright (c) Eason Qin, 2025.
 *
 * This source code form is licensed under the MIT/Expat license.
 * Visit the OSI website for a digital version.
 */
#define _POSIX_C_SOURCE 200809L
#define _GNU_SOURCE

#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "a_common.h"
#include "a_string.h"

a_string a_string_new(void) {
    a_string res = {
        .len = 0,
        .cap = 8,
    };

    res.data = calloc(res.cap, 1); // sizeof(char)
    check_alloc(res.data);

    return res;
}

a_string a_string_with_capacity(size_t cap) {
    a_string res = {.len = 0, .cap = cap};

    res.data = calloc(res.cap, 1); // sizeof(char)
    check_alloc(res.data);

    return res;
}

void a_string_clear(a_string* s) { memset(s->data, '\0', s->cap); }

void a_string_free(a_string* s) {
    if (!a_string_valid(s)) {
        panic("the string is invalid");
    }

    free(s->data);

    s->len = -1;
    s->cap = -1;
}

void a_string_copy(a_string* dest, const a_string* src) {
    if (src->len > dest->cap) {
        a_string_reserve(dest, src->cap);
    }

    strcpy(dest->data, src->data);
    dest->len = strlen(src->data);
}

void a_string_copy_cstr(a_string* dest, const char* src) {
    size_t len = strlen(src);
    if (len + 1 > dest->cap) {
        a_string_reserve(dest, len);
    }

    strcpy(dest->data, src);
    dest->len = len;
    if (dest->data[dest->len] != '\0') {
        dest->data[dest->len] = '\0'; // always nullterm
    }
}

void a_string_ncopy(a_string* dest, const a_string* src, size_t chars) {
    if (chars > dest->cap) {
        a_string_reserve(dest, chars);
    }

    strncpy(dest->data, src->data, chars);
    dest->len = chars;
}

void a_string_reserve(a_string* s, size_t cap) {
    if (!a_string_valid(s)) {
        panic("the string is invalid");
    }

    if (s->cap == cap)
        return;

    s->data = realloc(s->data, cap);
    check_alloc(s->data);
    s->cap = cap;
}

a_string a_string_from_cstr(const char* cstr) {
    a_string res = {
        .data = NULL,
        .cap = strlen(cstr) + 1,
        .len = strlen(cstr),
    };

    res.data = calloc(res.cap, 1); // sizeof(char)
    strcpy(res.data, cstr);

    return res;
}

a_string astr(const char* cstr) { return a_string_from_cstr(cstr); }

a_string a_string_dupe(const a_string* s) {
    a_string res = a_string_with_capacity(s->cap);
    res.len = s->len;
    strncpy(res.data, s->data, s->len);
    return res;
}

a_string a_string_sprintf(const char* restrict format, ...) {
    va_list args;
    va_start(args, format);
    char* dest = NULL;
    vasprintf(&dest, format, args);
    va_end(args);
    a_string res = a_string_from_cstr(dest);
    free(dest);
    return res;
}

char* a_string_fgets(a_string* buf, size_t cap, FILE* restrict stream) {
    size_t actual_cap = (cap == 0) ? 8192 : cap;
    if (a_string_valid(buf)) {
        a_string_reserve(buf, actual_cap);
    } else {
        *buf = a_string_with_capacity(actual_cap);
    }
    char* fgets_res = fgets(buf->data, actual_cap, stream);
    if (fgets_res == NULL)
        return NULL;
    buf->len = strlen(buf->data);
    return buf->data;
}

a_string a_string_read_file(const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        if (errno == ENOENT) {
            panic("file %s: No such file or directory", filename);
        } else {
            panic("file %s: Error while opening file", filename);
            // TODO: better errno handling?
        }
    }

    a_string res = a_string_new();
    a_string buf = a_string_with_capacity(8192);

    while (a_string_fgets(&buf, 0, fp)) {
        a_string_append_astr(&res, &buf);
    }

    a_string_free(&buf);

    return res;
}

a_string a_string_input(const char* prompt) {
    if (prompt) {
        printf("%s", prompt);
        fflush(stdout);
    }

    a_string raw = {0};
    a_string_fgets(&raw, 0, stdin);
    a_string_reserve(&raw, raw.len + 1);
    if (raw.data[raw.len - 1] == '\n')
        raw.data[--raw.len] = '\0';
    return raw;
}

bool a_string_valid(const a_string* s) {
    return !(s->len == (size_t)-1 || s->cap == (size_t)-1 || s->data == NULL);
}

a_string a_string_new_uninitialized(void) {
    return (a_string){
        .len = -1,
        .cap = -1,
        .data = NULL,
    };
}

void a_string_append_char(a_string* s, char c) {
    if (s->len + 2 > s->cap) { // +c +'\0'
        a_string_reserve(s, s->cap * 2);
    }

    s->data[s->len++] = c;
}

void a_string_append_cstr(a_string* s, const char* new) {
    if (s->data == new) {
        panic("identical pointers passed to function");
    }

    size_t required_cap = s->len + strlen(new) + 1;
    if (required_cap > s->cap) {
        while (s->cap < required_cap) {
            a_string_reserve(s, s->cap * 2);
        }
    }

    strcat(s->data, new);
    s->len += strlen(new);
}

void a_string_append_astr(a_string* s, const a_string* new) {
    a_string_append_cstr(s, new->data);
}

void a_string_append(a_string* s, const char* new) {
    a_string_append_cstr(s, new);
}

char a_string_pop(a_string* s) {
    char last = s->data[--s->len];
    s->data[s->len] = '\0';
    return last;
}

a_string a_string_trim_left(const a_string* s) {
    size_t i = 0;
    while (i < s->len) { // must check this first, else segfault
        if (strchr(" \n\t\r", s->data[i])) {
            i++;
        } else {
            break;
        }
    }

    return a_string_from_cstr(&s->data[i]);
}

a_string a_string_trim_right(const a_string* s) {
    size_t end = s->len - 1;
    while (strchr(" \n\t\r", s->data[end])) { // end >= 0 is always true
        end--;
    }
    end++;

    a_string res = a_string_with_capacity(end + 1);
    for (size_t i = 0; i < end; i++) {
        res.data[i] = s->data[i];
    }

    return res;
}

a_string a_string_trim(const a_string* s) {
    size_t begin = 0;
    size_t end = s->len - 1;

    while (begin < s->len) { // must check this first, else segfault
        if (strchr(" \n\t\r", s->data[begin]) != NULL) {
            begin++;
        } else {
            break;
        }
    }

    if (end == s->len)
        end--;

    while (strchr(" \n\t\r", s->data[end])) { // end >= 0 is always true
        end--;
    }

    end++;

    a_string res = a_string_with_capacity(end - begin + 1);
    res.len = end - begin;

    size_t i = 0;
    for (size_t j = begin; j < end; j++) {
        res.data[i++] = s->data[j];
    }

    return res;
}

void a_string_inplace_trim_left(a_string* s) {
    size_t i = 0;
    while (i < s->len) { // must check this first, else segfault
        if (strchr(" \n\t\r", s->data[i])) {
            i++;
        } else {
            break;
        }
    }

    memmove(&s->data[0], &s->data[i], s->len - i);
}

void a_string_inplace_trim_right(a_string* s) {
    size_t end = s->len - 1;
    while (strchr(" \n\t\r", s->data[end])) { // end >= 0 is always true
        end--;
    }
    end++;

    size_t oldlen = s->len;
    s->len = end + 1;

    memset(&s->data[end + 1], 0, oldlen - end + 1);
}

void a_string_inplace_trim(a_string* s) {
    size_t begin = 0;
    size_t end = s->len - 1;

    while (begin < s->len) { // must check this first, else segfault
        if (strchr(" \n\t\r", s->data[begin]) != NULL) {
            begin++;
        } else {
            break;
        }
    }

    if (end == s->len)
        end--;

    while (strchr(" \n\t\r", s->data[end])) { // end >= 0 is always true
        end--;
    }

    end++;

    size_t oldlen = s->len;
    s->len = end - begin;
    memmove(&s->data[0], &s->data[begin], s->len);
    memset(&s->data[s->len + 1], 0, oldlen - s->len + 1);
}

a_string a_string_toupper(const a_string* s) {
    a_string res = a_string_with_capacity(s->cap);
    for (size_t i = 0; i < s->len; i++) {
        res.data[i] = toupper(s->data[i]);
    }
    return res;
}

a_string a_string_tolower(const a_string* s) {
    a_string res = a_string_with_capacity(s->cap);
    for (size_t i = 0; i < s->len; i++) {
        res.data[i] = tolower(s->data[i]);
    }
    return res;
}

void a_string_inplace_toupper(a_string* s) {
    for (size_t i = 0; i < s->len; i++) {
        s->data[i] = toupper(s->data[i]);
    }
}

void a_string_inplace_tolower(a_string* s) {
    for (size_t i = 0; i < s->len; i++) {
        s->data[i] = tolower(s->data[i]);
    }
}

bool a_string_equal(const a_string* lhs, const a_string* rhs) {
    if (lhs->len != rhs->len) {
        return false;
    }

    for (size_t i = 0; i < lhs->len; i++) {
        if (lhs->data[i] != rhs->data[i]) {
            return false;
        }
    }

    return true;
}

bool a_string_equal_case_insensitive(const a_string* lhs, const a_string* rhs) {
    if (lhs->len != rhs->len) {
        return false;
    }

    for (size_t i = 0; i < lhs->len; i++) {
        if (tolower(lhs->data[i]) != tolower(rhs->data[i])) {
            return false;
        }
    }

    return true;
}

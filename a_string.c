/*
 * a_string/a_vector: a scuffed dynamic vector/string implementation.
 *
 * Copyright (c) Eason Qin, 2025.
 *
 * This source code form is licensed under the MIT/Expat license.
 * Visit the OSI website for a digital version.
 */
#define _POSIX_C_SOURCE 200809L

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
    if (res.data == NULL)
        return a_string_new_invalid();

    return res;
}

a_string a_string_with_capacity(size_t cap) {
    a_string res = {.len = 0, .cap = cap};

    res.data = calloc(res.cap, 1); // sizeof(char)
    if (res.data == NULL)
        return a_string_new_invalid();

    return res;
}

void a_string_clear(a_string* s) { memset(s->data, '\0', s->cap); }

void a_string_free(a_string* s) {
    if (!a_string_valid(s)) {
        return;
    }

    free(s->data);

    s->len = -1;
    s->cap = -1;
}

void a_string_copy(a_string* dest, const a_string* src) {
    if (!a_string_valid(dest))
        panic("cannot operate on invalid a_string!");
    if (!a_string_valid(src))
        panic("source string is invalid!");

    if (src->len > dest->cap) {
        a_string_reserve(dest, src->cap);
    }

    strcpy(dest->data, src->data);
    dest->len = strlen(src->data);
}

void a_string_copy_cstr(a_string* dest, const char* src) {
    if (!a_string_valid(dest))
        panic("cannot operate on invalid a_string!");
    if (src == NULL)
        panic("source C string is null!");

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
    if (!a_string_valid(dest))
        panic("cannot operate on invalid a_string!");
    if (!a_string_valid(src))
        panic("source string is invalid!");

    if (chars > dest->cap) {
        a_string_reserve(dest, chars);
    }

    strncpy(dest->data, src->data, chars);
    dest->len = chars;
}

void a_string_ncopy_cstr(a_string* dest, const char* src, size_t chars) {
    if (!a_string_valid(dest))
        panic("cannot operate on invalid a_string!");
    if (src == NULL)
        panic("source C string is null!");

    if (chars > dest->cap) {
        a_string_reserve(dest, chars);
    }

    strncpy(dest->data, src, chars);
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
    if (cstr == NULL)
        panic("source C string is null!");

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
    if (!a_string_valid(s))
        panic("cannot operate on invalid a_string!");

    a_string res = a_string_with_capacity(s->cap);
    res.len = s->len;
    strncpy(res.data, s->data, s->len);
    return res;
}

a_string a_string_asprintf(const char* restrict format, ...) {
    va_list args;
    va_start(args, format);

    va_list argscopy;
    va_copy(argscopy, args);

    size_t len = vsnprintf(NULL, 0, format, argscopy);
    a_string res = a_string_with_capacity(len + 1);
    vsnprintf(res.data, res.cap, format, args);

    va_end(args);

    res.len = len;
    return res;
}

size_t a_string_sprintf(a_string* dest, const char* restrict format, ...) {
    va_list args;
    va_start(args, format);

    va_list argscopy;
    va_copy(argscopy, args);
    size_t len = vsnprintf(NULL, 0, format, argscopy);

    if (a_string_valid(dest)) {
        a_string_reserve(dest, len + 1);
    } else {
        *dest = a_string_with_capacity(len + 1);
    }

    size_t res = vsnprintf(dest->data, dest->cap, format, args);

    va_end(args);

    return res;
}

int a_string_fprint(const a_string* s, FILE* restrict stream) {
    return fprintf(stream, "%.*s", (int)s->len, s->data);
}

int a_string_fprintln(const a_string* s, FILE* restrict stream) {
    return fprintf(stream, "%.*s\n", (int)s->len, s->data);
}

int a_string_print(const a_string* s) { return a_string_fprint(s, stdout); }

int a_string_println(const a_string* s) { return a_string_fprintln(s, stdout); }

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

bool a_string_read_line(a_string* buf, FILE* restrict stream) {
    char* res = a_string_fgets(buf, 0, stream);
    if (res == NULL)
        return false;

    // trim newline off
    if (a_string_get_last(buf) == '\n')
        a_string_pop(buf);

    // resize buf to appropriate size.
    a_string_reserve(buf, buf->len + 1);
    return buf;
}

a_string a_string_read_file(const char* filename) {
    if (filename == NULL)
        panic("source file name C string is null!");

    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        return a_string_new_invalid();
    }

    fseek(fp, 0, SEEK_END);
    size_t sz = ftell(fp);
    rewind(fp);
    a_string res = a_string_with_capacity(sz);
    if (fread(res.data, 1, sz, fp) != sz) {
        return a_string_new_invalid();
    }
    res.len = sz;
    fclose(fp);
    return res;
}

a_string a_string_input(const char* prompt) {
    if (prompt) {
        printf("%s", prompt);
        fflush(stdout);
    }

    a_string raw = a_string_new();
    if (!a_string_read_line(&raw, stdin))
        return a_string_new_invalid();
    else
        return raw;
}

bool a_string_valid(const a_string* s) {
    return !(s->len == (size_t)-1 || s->cap == (size_t)-1 || s->data == NULL);
}

a_string a_string_new_invalid(void) {
    return (a_string){
        .len = -1,
        .cap = -1,
        .data = NULL,
    };
}

void a_string_append_char(a_string* s, char c) {
    if (!a_string_valid(s))
        panic("cannot operate on an invalid a_string!");

    if (s->len + 1 > s->cap) {
        a_string_reserve(s, s->cap * 2);
    }

    s->data[s->len++] = c;
}

void a_string_append_cstr(a_string* s, const char* new) {
    if (!a_string_valid(s))
        panic("cannot operate on an invalid a_string!");
    if (new == NULL)
        panic("null string passed to append operation!");

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
    if (!a_string_valid(new))
        panic("a_string to be appended cannot be NULL!");

    a_string_append_cstr(s, new->data);
}

void a_string_append(a_string* s, const char* new) {
    a_string_append_cstr(s, new);
}

char a_string_pop(a_string* s) {
    if (!a_string_valid(s))
        panic("cannot operate on an invalid a_string!");

    char last = s->data[--s->len];
    s->data[s->len] = '\0';
    return last;
}

char a_string_get_last(const a_string* s) {
    if (!a_string_valid(s))
        panic("cannot operate on an invalid a_string!");

    char last = s->data[s->len - 1];
    return last;
}

a_string a_string_trim_left(const a_string* s) {
    if (!a_string_valid(s))
        panic("cannot operate on an invalid a_string!");

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
    if (!a_string_valid(s))
        panic("cannot operate on an invalid a_string!");

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
    if (!a_string_valid(s))
        panic("cannot operate on an invalid a_string!");

    size_t begin = 0;
    size_t end = s->len - 1;

    while (begin < s->len) { // must check this first, else segfault
        if (strchr(" \n\t\r", s->data[begin]) != NULL) {
            begin++;
        } else {
            break;
        }
    }

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
    if (!a_string_valid(s))
        panic("cannot operate on an invalid a_string!");

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
    if (!a_string_valid(s))
        panic("cannot operate on an invalid a_string!");

    size_t end = s->len - 1;
    while (strchr(" \n\t\r", s->data[end])) { // end >= 0 is always true
        end--;
    }
    end++;

    size_t oldlen = s->len;
    s->len = end + 1;

    if (end > s->len)
        memset(&s->data[end + 1], 0, oldlen - end + 1);
}

void a_string_inplace_trim(a_string* s) {
    if (!a_string_valid(s))
        panic("cannot operate on an invalid a_string!");

    size_t begin = 0;
    size_t end = s->len - 1;

    while (begin < s->len) { // must check this first, else segfault
        if (isspace(s->data[begin])) {
            begin++;
        } else {
            break;
        }
    }

    while (isspace(s->data[end])) { // end >= 0 is always true
        end--;
    }

    end++;

    size_t oldlen = s->len;
    s->len = end - begin;
    memmove(&s->data[0], &s->data[begin], s->len);
    if (end > s->len)
        memset(&s->data[end], 0, oldlen - s->len + 1);
}

a_string a_string_toupper(const a_string* s) {
    if (!a_string_valid(s))
        panic("cannot operate on an invalid a_string!");

    a_string res = a_string_with_capacity(s->cap);
    for (size_t i = 0; i < s->len; i++) {
        res.data[i] = toupper(s->data[i]);
    }
    return res;
}

a_string a_string_tolower(const a_string* s) {
    if (!a_string_valid(s))
        panic("cannot operate on an invalid a_string!");

    a_string res = a_string_with_capacity(s->cap);
    for (size_t i = 0; i < s->len; i++) {
        res.data[i] = tolower(s->data[i]);
    }
    return res;
}

void a_string_inplace_toupper(a_string* s) {
    if (!a_string_valid(s))
        panic("cannot operate on an invalid a_string!");

    for (size_t i = 0; i < s->len; i++) {
        s->data[i] = toupper(s->data[i]);
    }
}

void a_string_inplace_tolower(a_string* s) {
    if (!a_string_valid(s))
        panic("cannot operate on an invalid a_string!");

    for (size_t i = 0; i < s->len; i++) {
        s->data[i] = tolower(s->data[i]);
    }
}

bool a_string_equal(const a_string* lhs, const a_string* rhs) {
    if (!a_string_valid(lhs))
        panic("cannot compare an invalid a_string!");

    if (!a_string_valid(rhs))
        panic("cannot compare an invalid a_string!");

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
    if (!a_string_valid(lhs))
        panic("cannot compare an invalid a_string!");

    if (!a_string_valid(rhs))
        panic("cannot compare an invalid a_string!");

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

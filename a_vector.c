/*
 * a_string/a_vector: a scuffed dynamic vector/string implementation.
 *
 * Copyright (c) Eason Qin, 2025.
 *
 * This source code form is licensed under the Mozilla Public License version
 * 2.0. Visit the OSI website for a digital version.
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "a_common.h"
#include "a_vector.h"

#define A_VECTOR_GROWTH_FACTOR 3

a_vector a_vector_new(void) {
    return a_vector_with_capacity(5);
}

a_vector a_vector_with_capacity(size_t cap) {
    a_vector res = {.len = 0, .cap = cap};

    res.data = calloc(res.cap, sizeof(void*));
    check_alloc(res.data);

    return res;
}

a_vector a_vector_from_ints(const int* ints, size_t len) {
    a_vector res = a_vector_with_capacity(len);

    for (size_t i = 0; i < len; i++) {
        int* heap_int = calloc(1, sizeof(int));
        check_alloc(heap_int);
        *heap_int = ints[i];
        res.data[i] = (void*)heap_int;
    }

    res.len = len;

    return res;
}

void a_vector_free(a_vector* v) {
    free(v->data);
    v->len = -1;
    v->cap = -1;
}

void a_vector_free_with_items(a_vector* v) {
    for (size_t i = 0; i < v->len; i++) {
        void* e = *(((*v).data) + i);
        free(e);
    }

    a_vector_free(v);
}

bool a_vector_invalid(a_vector* v) {
    return (v->len == (size_t)-1 || v->cap == (size_t)-1 || v->data == NULL);
}

void a_vector_reserve(a_vector* v, size_t cap) {
    if (a_vector_invalid(v)) {
        panic("the vector is invalid");
    }

    v->data = realloc(v->data, sizeof(void*) * cap);
    check_alloc(v->data);
    v->cap = cap;
}

void a_vector_append(a_vector* v, void* new) {
    if (a_vector_invalid(v)) {
        panic("the vector is invalid");
    }

    if (v->len + 1 > v->cap) {
        a_vector_reserve(v, v->cap * A_VECTOR_GROWTH_FACTOR);
    }

    v->data[v->len++] = new;
}

void a_vector_append_vector(a_vector* v, const a_vector* other) {
    if (a_vector_invalid(v)) {
        panic("the vector is invalid");
    }

    size_t required_len = v->len + other->len;
    if (required_len > v->cap) {
        size_t sz = v->cap;
        while (sz < required_len)
            sz *= A_VECTOR_GROWTH_FACTOR;
        a_vector_reserve(v, sz);
    }

    // copy other->len number of elements into the end of v->data.
    memcpy(&v->data[v->len], other->data, sizeof(void*) * other->len);
    v->len += other->len;
}

void* a_vector_pop(a_vector* v) {
    if (a_vector_invalid(v)) {
        panic("the vector is invalid");
    }

    void* res = v->data[--v->len];

    if (v->len < (size_t)(v->cap / A_VECTOR_GROWTH_FACTOR)) {
        a_vector_reserve(v, (int)(v->cap / A_VECTOR_GROWTH_FACTOR));
    }

    return res;
}

void* a_vector_pop_at(a_vector* v, size_t pos) {
    if (a_vector_invalid(v)) {
        panic("the vector is invalid");
    }

    if (pos >= v->len) {
        panic("array index %zu out of range", pos);
    }

    void* res = v->data[pos];

    size_t items =
        (v->len - pos + 1); // items between pos+1 and the end of the list

    memmove(&v->data[pos], &v->data[pos + 1], items * sizeof(void*));

    v->len--;

    return res;
}

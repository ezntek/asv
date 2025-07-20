/*
 * a_string/a_vector: a scuffed dynamic vector/string implementation.
 *
 * Copyright (c) Eason Qin, 2025.
 *
 * This source code form is licensed under the MIT/Expat license.
 * Visit the OSI website for a digital version.
 */
#ifndef _A_VECTOR_H
#define _A_VECTOR_H

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#define A_VECTOR_DECL(T)                                                       \
    typedef struct {                                                           \
        T* data;                                                               \
        size_t len;                                                            \
        size_t cap;                                                            \
    } a_vector_##T;                                                            \
    a_vector_##T a_vector_##T##_new(void);                                     \
    a_vector_##T a_vector_##T##_with_capacity(size_t cap);                     \
    a_vector_##T a_vector_##T##_from_slice(const T* slice, size_t nitems);     \
    void a_vector_##T##_free(a_vector_##T* v);                                 \
    bool a_vector_##T##_valid(a_vector_##T* v);                                \
    void a_vector_##T##_reserve(a_vector_##T* v, size_t cap);                  \
    void a_vector_##T##_append(a_vector_##T* v, T new_elem);                   \
    void a_vector_##T##_append_vector(a_vector_##T* v,                         \
                                      const a_vector_##T* other);              \
    void a_vector_##T##_append_slice(a_vector_##T* v, const T* ptr,            \
                                     size_t nitems);                           \
    T a_vector_##T##_pop(a_vector_##T* v);                                     \
    T a_vector_##T##_pop_at(a_vector_##T* v, size_t pos);
#define A_VECTOR_GROWTH_FACTOR 3
#define A_VECTOR_IMPL(T)                                                       \
    a_vector_##T a_vector_##T##_new(void) {                                    \
        return a_vector_##T##_with_capacity(5);                                \
    }                                                                          \
    a_vector_##T a_vector_##T##_with_capacity(size_t cap) {                    \
        a_vector_##T res = {.len = 0, .cap = cap};                             \
        res.data = calloc(res.cap, sizeof(T));                                 \
        check_alloc(res.data);                                                 \
        return res;                                                            \
    }                                                                          \
    a_vector_##T a_vector_##T##_from_slice(const T* slice, size_t nitems) {    \
        a_vector_##T res = a_vector_##T##_with_capacity(nitems);               \
        memcpy(res.data, slice, nitems * sizeof(T));                           \
        res.len = nitems;                                                      \
        return res;                                                            \
    }                                                                          \
    void a_vector_##T##_free(a_vector_##T* v) {                                \
        free(v->data);                                                         \
        v->len = (size_t)-1;                                                   \
        v->cap = (size_t)-1;                                                   \
    }                                                                          \
    bool a_vector_##T##_valid(a_vector_##T* v) {                               \
        return !(v->len == (size_t)-1 || v->cap == (size_t)-1 ||               \
                 v->data == NULL);                                             \
    }                                                                          \
    void a_vector_##T##_reserve(a_vector_##T* v, size_t cap) {                 \
        if (!a_vector_##T##_valid(v)) {                                        \
            panic("the vector is invalid");                                    \
        }                                                                      \
        v->data = realloc(v->data, sizeof(T) * cap);                           \
        check_alloc(v->data);                                                  \
        v->cap = cap;                                                          \
    }                                                                          \
    void a_vector_##T##_append(a_vector_##T* v, T new_elem) {                  \
        if (!a_vector_##T##_valid(v)) {                                        \
            panic("the vector is invalid");                                    \
        }                                                                      \
        if (v->len + 1 > v->cap) {                                             \
            a_vector_##T##_reserve(v, v->cap * A_VECTOR_GROWTH_FACTOR);        \
        }                                                                      \
        v->data[v->len++] = new_elem;                                          \
    }                                                                          \
    void a_vector_##T##_append_vector(a_vector_##T* v,                         \
                                      const a_vector_##T* other) {             \
        if (!a_vector_##T##_valid(v)) {                                        \
            panic("the vector is invalid");                                    \
        }                                                                      \
        size_t len = v->len + other->len;                                      \
        if (len > v->cap) {                                                    \
            size_t sz = v->cap;                                                \
            while (sz < len)                                                   \
                sz *= A_VECTOR_GROWTH_FACTOR;                                  \
            a_vector_##T##_reserve(v, sz);                                     \
        }                                                                      \
        memcpy(&v->data[v->len], other->data, sizeof(T) * other->len);         \
        v->len += other->len;                                                  \
    }                                                                          \
    void a_vector_##T##_append_slice(a_vector_##T* v, const T* data,           \
                                     size_t nitems) {                          \
        if (!a_vector_##T##_valid(v)) {                                        \
            panic("the vector is invalid");                                    \
        }                                                                      \
        size_t len = v->len + nitems;                                          \
        if (len > v->cap) {                                                    \
            size_t sz = v->cap;                                                \
            while (sz < len)                                                   \
                sz *= A_VECTOR_GROWTH_FACTOR;                                  \
            a_vector_##T##_reserve(v, sz);                                     \
        }                                                                      \
        memcpy(&v->data[v->len], data, sizeof(T) * nitems);                    \
        v->len += nitems;                                                      \
    }                                                                          \
    T a_vector_##T##_pop(a_vector_##T* v) {                                    \
        if (!a_vector_##T##_valid(v)) {                                        \
            panic("the vector is invalid");                                    \
        }                                                                      \
        T res = v->data[--v->len];                                             \
        if (v->len < (size_t)(v->cap / A_VECTOR_GROWTH_FACTOR)) {              \
            a_vector_##T##_reserve(v, v->cap / A_VECTOR_GROWTH_FACTOR);        \
        }                                                                      \
        return res;                                                            \
    }                                                                          \
    T a_vector_##T##_pop_at(a_vector_##T* v, size_t pos) {                     \
        if (!a_vector_##T##_valid(v)) {                                        \
            panic("the vector is invalid");                                    \
        }                                                                      \
        if (pos >= v->len) {                                                   \
            panic("array index %zu out of range", pos);                        \
        }                                                                      \
        T res = v->data[pos];                                                  \
        size_t items = (v->len - pos - 1);                                     \
        memmove(&v->data[pos], &v->data[pos + 1], items * sizeof(T));          \
        v->len--;                                                              \
        return res;                                                            \
    }

#endif // _A_VECTOR_H

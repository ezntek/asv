/*
 * a_string/a_vector: a scuffed dynamic vector/string implementation.
 *
 * Copyright (c) Eason Qin, 2025.
 *
 * This source code form is licensed under the MIT/Expat license.
 * Visit the OSI website for a digital version.
 */

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

/**
 * a non-null-terminated dynamic array of void*.
 */
typedef struct {
    // the raw data.
    void** data;

    // length of the vector.
    size_t len;

    // capacity of the vector.
    size_t cap;
} a_vector;

/**
 * creates an empty a_vector.
 */
a_vector a_vector_new(void);

/**
 * creates an a_vector with a specified capacity.
 *
 * @param cap the capacity of the string.
 */
a_vector a_vector_with_capacity(size_t cap);

/**
 * creates an a_vector from integers.
 *
 * @param ints a list of ints
 * @param len the length of the ints
 */
a_vector a_vector_from_ints(const int* ints, size_t len);

/**
 * destroys the heap-allocated buffer in which the data exists.
 * the data within the pointers themselves will be left untouched.
 *
 * @param v the vector
 */
void a_vector_free(a_vector* v);

/**
 * similar to a_vector_free, but both the buffers and the pointers
 * are freed. should be used with a_vector_from_ variants.
 *
 * @param v the vector
 */
void a_vector_free_with_items(a_vector* v);

/**
 * checks if the a_vector is invalid.
 *
 * @param v the vector
 */
bool a_vector_invalid(a_vector* v);

/**
 * reserves a specific capacity on an a_vector.
 *
 * @param v the vector
 * @param cap the new capacity to reserve
 */
void a_vector_reserve(a_vector* v, size_t cap);

/**
 * appends a new element to the end of the vector.
 *
 * @param v the vector
 * @param new the new element
 */
void a_vector_append(a_vector* v, void* new);

/**
 * appends an a_vector to the end of the current a_vector.
 * the data within the other list will not be touched nor freed.
 *
 * @param v the vector
 * @param other the vector to append from
 */
void a_vector_append_list(a_vector* v, const a_vector* other);

/**
 * pops an element off the end of the a_vector.
 *
 * @param v the vector
 */
void* a_vector_pop(a_vector* v);

/**
 * pops an element off the end of the a_vector, at a given index.
 *
 * @param v the vector
 * @param pos the position
 */
void* a_vector_pop_at(a_vector* v, size_t pos);

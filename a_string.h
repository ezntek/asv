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
#include <stdio.h>
#include <stdlib.h>

/**
 * null terminated, heap-allocated string slice.
 */
typedef struct {
    // The raw string slice allocated on the heap.
    char* data;

    // length of the string.
    size_t len;

    // capacity of the string. includes the null terminator.
    size_t cap;
} a_string;

/**
 * creates an empty a_string.
 */
a_string a_string_new(void);

/**
 * creates an empty a_string with a specified capacity.
 *
 * @param cap the capacity of the string.
 */
a_string a_string_with_capacity(size_t cap);

/**
 * clears the string with null terminators, keeping the capacity.
 *
 * @param s the string
 */
void a_string_clear(a_string* s);

/**
 * destroys the heap-allocated data in an a_string. Do not read from the
 * a_string after it is destroyed!
 *
 * @param s the string to be destroyed
 */
void a_string_free(a_string* s);

/**
 * Copies one a_string to another.
 *
 * @param dest the dest string
 * @param src the source string
 */
void a_string_copy(a_string* dest, const a_string* src);

/**
 * Copies the entirety of a C string into an a_string.
 *
 * Passing a null-terminated string causes undefined behavior.
 *
 * @param dest the dest string
 * @param src the source string
 */
void a_string_copy_cstr(a_string* dest, const char* src);

/**
 * Copies N bytes of one a_string to another.
 *
 * @param dest the dest string
 * @param src the source string
 * @param chars the number of chars
 */
void a_string_ncopy(a_string* dest, const a_string* src, size_t chars);

/**
 * reserves a specific capacity on an a_string.
 *
 * @param s the string to be modified
 * @param cap the new capacity of the string
 */
void a_string_reserve(a_string* s, size_t cap);

/**
 * creates an a_string from a C string.
 *
 * @param cstr the C string to be converted. This function does not free the
 * string if it is heap-allocated, the string is instead duplicated.
 */
a_string a_string_from_cstr(const char* cstr);

/**
 * creates an a_string from a C string.
 *
 * shorthand of `a_string_from_cstr()`
 *
 * @param cstr the C string to be converted. This function does not free the
 * string if it is heap-allocated, the string is instead duplicated.
 */
a_string astr(const char* cstr);

/**
 * duplicates an a_string.
 *
 * the exact string, including the capacity it holds will be duplicated.
 *
 * @param s the string to duplicate
 */
a_string a_string_duplicate(const a_string* s);

/**
 * similar to asprintf, but for an a_string.
 *
 * @param format the format
 * @param ... format args
 */
a_string a_string_sprintf(const char* restrict format, ...);

/**
 * similar to fgets, but reads into an a_string. it returns the
 * buffer at buf.data or NULL.
 *
 * the string is guaranteed to be null-terminated. passing in a valid a_string
 * will result in its buffer being overwritten with a buffer of capacity cap.
 * passing in a capacity of 0 will result in a default capacity of 8192.
 *
 * @param cap the maximum capacity of the string to be entered.
 * @param stream the file stream.
 */
char* a_string_fgets(a_string* buf, size_t cap, FILE* restrict stream);

/**
 * reads the entirety of a file into an a_string.
 *
 * the capacity of the string will be equal to the length of string read from
 * the file. the maximum supported capacity for a single line is 8192 chars.
 *
 * @param filename the name of the file.
 */
a_string a_string_read_file(const char* filename);

/**
 * gets a string input from stdin into an a_string with a non-formatted prompt.
 *
 * only up to 8192 characters may be read.
 *
 * @param prompt a C string to be printed as the prompt. leaving it as null will
 *               print no prompt.
 */
a_string a_string_input(const char* prompt);

/**
 * checks if an a_string is valid
 *
 * @param s the string to be checked
 */
bool a_string_valid(const a_string* s);

/**
 * creates an empty, invalid a_string.
 */
a_string a_string_empty(void);

/**
 * adds 1 character to an a_string
 *
 * @param s the target string to be concatenated
 * @param c the character to be added.
 */
void a_string_append_char(a_string* s, char c);

/**
 * concatenates 2 a_strings together.
 *
 * @param s the target string to be concatenated
 * @param new the string to add on. This string will be kept intact and will not
 * be freed.
 */
void a_string_append_astr(a_string* s, const a_string* new);

/**
 * concatenates a C string to an a_string.
 *
 * @param s the target string to be concatenated
 * @param new the string to add on. This is a C string that will be kept intact.
 */
void a_string_append_cstr(a_string* s, const char* new);

/**
 * concatenates a C string to an a_string.
 *
 * shorthand form of `a_string_append_cstr()`.
 *
 * @param s the target string to be concatenated
 * @param new the string to add on. This string will be kept intact and will not
 * be freed.
 */
void a_string_append(a_string* s, const char* new);

/**
 * removes the last character from an a_string.
 *
 * @param s the target string
 * @return the last character
 */
char a_string_pop(a_string* s);

/**
 * removes all whitespace characters from the left side of an a_string.
 *
 * @param s the string
 * @return a new a_string.
 */
a_string a_string_trim_left(const a_string* s);

/**
 * removes all whitespace characters from the right side of an a_string.
 *
 * @param s the string
 * @return a new a_string.
 */
a_string a_string_trim_right(const a_string* s);

/**
 * removes all whitespace characters from the left and right sides of an
 * a_string.
 *
 * @param s the string
 * @return a new a_string.
 */
a_string a_string_trim(const a_string* s);

/**
 * (IN PLACE) removes all whitespace characters from the left side of an
 * a_string.
 *
 * @param s the string
 * @return a new a_string.
 */
void a_string_inplace_trim_left(a_string* s);

/**
 * (IN PLACE) removes all whitespace characters from the right side of an
 * a_string.
 *
 * @param s the string
 * @return a new a_string.
 */
void a_string_inplace_trim_right(a_string* s);

/**
 * (IN PLACE) removes all whitespace characters from the left and right sides of
 * an a_string.
 *
 * @param s the string
 * @return a new a_string.
 */
void a_string_inplace_trim(a_string* s);

/**
 * converts all the characters in the a_string to uppercase.
 *
 * @param s the string
 */
a_string a_string_toupper(const a_string* s);

/**
 * converts all the characters in the a_string to lowercase.
 *
 * @param s the string
 */
a_string a_string_tolower(const a_string* s);

/**
 * (IN PLACE) converts all the characters in the a_string to uppercase.
 *
 * @param s the string
 */
void a_string_inplace_toupper(a_string* s);

/**
 * (IN PLACE) converts all the characters in the a_string to lowercase.
 *
 * @param s the string
 */
void a_string_inplace_tolower(a_string* s);

/**
 * checks if 2 a_strings are the same.
 *
 * @param lhs the first string
 * @param rhs the other string
 */
bool a_string_equal(const a_string* lhs, const a_string* rhs);

/**
 * checks if 2 a_strings are the same, case insensitive.
 *
 * @param lhs the first string
 * @param rhs the other string
 */

bool a_string_equal_case_insensitive(const a_string* lhs, const a_string* rhs);

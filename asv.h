/*
 * a_common: common typedefs and DEFINEs to make life easier
 *           (used by a_string and a_vector)
 *
 * Copyright (c) Eason Qin, 2025.
 *
 * This source code form is licensed under the MIT/Expat license.
 * Visit the OSI website for a digital version.
 */

#pragma once

#include <stddef.h>
#include <stdio.h>

#define S_BOLD "\033[1m"
#define S_DIM  "\033[2m"
#define S_END  "\033[0m"

#define S_BLACK   "\033[30m"
#define S_RED     "\033[31m"
#define S_GREEN   "\033[32m"
#define S_YELLOW  "\033[33m"
#define S_BLUE    "\033[34m"
#define S_MAGENTA "\033[35m"
#define S_CYAN    "\033[36m"
#define S_WHITE   "\033[37m"

#define S_BGBLACK   "\033[40m"
#define S_BGRED     "\033[41m"
#define S_BGGREEN   "\033[42m"
#define S_BGYELLOW  "\033[43m"
#define S_BGBLUE    "\033[44m"
#define S_BGMAGENTA "\033[45m"
#define S_BGCYAN    "\033[46m"
#define S_BGWHITE   "\033[47m"

#define S_CLEAR_SCREEN "\033[2J\033[H"
#define S_CLEAR_LINE   "\r\033[K"

#define S_ENTER_ALT "\033[?1049h"
#define S_LEAVE_ALT "\033[?1049l"

#define S_SHOWCURSOR "\033[?25h"
#define S_HIDECURSOR "\033[?25l"

#define check_alloc(ptr)                                                       \
                                                                               \
    if (ptr == NULL) {                                                         \
        panic("allocation of `%s` failed", #ptr);                              \
        perror("perror");                                                      \
        exit(1);                                                               \
    }

#define eprintf(...) fprintf(stderr, __VA_ARGS__);

#define panic(...)                                                             \
    {                                                                          \
        eprintf("\033[31;1mpanic:\033[0m line `%d`, func `%s` in file `%s`: "  \
                "`",                                                           \
                __LINE__, __func__, __FILE__);                                 \
        eprintf(__VA_ARGS__);                                                  \
        eprintf(S_DIM                                                          \
                "`\n       please report this to the developer.\n" S_END);     \
        exit(1);                                                               \
    }

#define fatal_noexit(...)                                                      \
    {                                                                          \
        eprintf(S_RED S_BOLD "[fatal] " S_END);                                \
        eprintf(S_DIM);                                                        \
        eprintf(__VA_ARGS__);                                                  \
        eprintf(S_END "\n");                                                   \
    }

#define fatal(...)                                                             \
    {                                                                          \
        fatal_noexit(__VA_ARGS__);                                             \
        exit(1);                                                               \
    }

#define warn(...)                                                              \
    {                                                                          \
        eprintf(S_MAGENTA S_BOLD "[warn] " S_END);                             \
        eprintf(S_DIM);                                                        \
        eprintf(__VA_ARGS__);                                                  \
        eprintf(S_END "\n");                                                   \
    }

#define info(...)                                                              \
    {                                                                          \
        eprintf(S_CYAN S_BOLD "[info] " S_END);                                \
        eprintf(S_DIM);                                                        \
        eprintf(__VA_ARGS__);                                                  \
        eprintf(S_END "\n");                                                   \
    }

typedef size_t usize;
typedef ssize_t isize;
typedef unsigned char u8;
typedef signed char i8;
typedef unsigned short int u16;
typedef signed short int i16;
typedef unsigned int u32;
typedef signed int i32;
typedef unsigned long long int u64;
typedef signed long long int i64;
typedef float f32;
typedef double f64;
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
 * similar to fgets, but reads into an a_string.
 *
 * the string is guaranteed to be null-terminated.
 *
 * @param cap the maximum capacity of the string to be entered.
 * @param stream the file stream.
 */
a_string a_string_fgets(size_t cap, FILE* restrict stream);

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
 * checks if an a_string is invalid
 *
 * @param s the string to be checked
 */
bool a_string_invalid(const a_string* s);

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

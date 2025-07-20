#include "a_common.h"
#include "a_vector.h"
#include <stdio.h>
#include <string.h>

A_VECTOR_DECL(int);

A_VECTOR_IMPL(int);

void printvec(a_vector_int* v) {
    for (size_t i = 0; i < v->len; i++) {
        printf("%d, ", v->data[i]);
    }
    putchar('\n');
}

void printint(int x) { printf("%d\n", x); }

int main(void) {
    a_vector_int v = a_vector_int_new();
    a_vector_int_append(&v, 5);
    a_vector_int_append(&v, 3);
    a_vector_int_append(&v, 1);
    a_vector_int_append(&v, 2);
    a_vector_int_append(&v, 4);
    printvec(&v);

    // basic operations
    int res = a_vector_int_pop(&v);
    printint(res);
    res = a_vector_int_pop_at(&v, 1);
    printint(res);
    printvec(&v); // should be 5, 1, 2,

    // complex operations
    const int slc[] = {9, 13, 5};
    a_vector_int other =
        a_vector_int_from_slice(slc, (int)(sizeof(slc) / sizeof(int)));
    a_vector_int_append_vector(&v, &other);
    a_vector_int_append_slice(&v, &other.data[1], 2);
    printvec(&v); // should be 5, 1, 2, 9, 13, 5, 13, 5,

    a_vector_int_free(&other);
    a_vector_int_free(&v);
    return 0;
}

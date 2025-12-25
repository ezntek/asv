#include "a_common.h"
#include "a_vector.h"
#include <stdio.h>
#include <string.h>

AV_DECL(i32, Numbers)

void printvec(Numbers* v) {
    if (!v->len) {
        eprintf("no items\n");
        return;
    }

    printf("L(%d) C(%d): ", v->len, v->cap);
    for (size_t i = 0; i < v->len; i++) {
        printf("%d, ", av_at(v, i));
    }
    putchar('\n');
}

void printint(int x) {
    printf("%d\n", x);
}

int main(void) {
    Numbers nums = {0};
    av_append(&nums, 5);
    av_append(&nums, 8);
    av_append(&nums, 7);
    av_append(&nums, 4);
    printvec(&nums);
    int manynums[] = {34, 35, 1};
    av_append_many(&nums, manynums, 3);
    printvec(&nums);
    av_clear(&nums);
    printvec(&nums);
    av_free(&nums);
}

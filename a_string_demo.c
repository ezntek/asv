#include "a_common.h"
#include "a_string.h"
#include <stdio.h>

#define FILENAME "./demo.txt"

int main(void) {
    // reading an entire file
    a_string file_content = as_read_file(FILENAME);
    if (!as_valid(&file_content)) {
        panic("failed to read file %s", FILENAME);
    }

    printf("file contents:\n %s\n", file_content.data);
    as_free(&file_content);

    // reading line-by-line
    a_string tmp;
    FILE* fp = fopen(FILENAME, "r");
    if (fp == NULL)
        panic("failed to open file");

    while (as_read_line(&tmp, fp)) {
        a_string line = as_dupe(&tmp);
        eprintf("pre: %.*s\n", as_fmt(line));
        as_sprintf(&tmp, "got: \"%.*s\"", as_fmt(line));
        as_println(&tmp);
        as_free(&line);
    }

    fclose(fp);
    as_free(&tmp);
}

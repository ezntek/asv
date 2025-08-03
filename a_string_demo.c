#include "a_common.h"
#include "a_string.h"
#include <stdio.h>

#define FILENAME "./demo.txt"

int main(void) {
    // reading an entire file
    a_string file_content = a_string_read_file(FILENAME);
    if (!a_string_valid(&file_content)) {
        panic("failed to read file %s", FILENAME);
    }

    printf("file contents:\n %s\n", file_content.data);
    a_string_free(&file_content);

    // reading line-by-line
    a_string tmp;
    FILE* fp = fopen(FILENAME, "r");
    if (fp == NULL)
        panic("failed to open file");

    while (a_string_read_line(&tmp, fp)) {
        a_string line = a_string_dupe(&tmp);
        a_string_sprintf(&tmp, "got: \"%s\"", line.data);
        a_string_println(&tmp);
        a_string_free(&line);
    }

    fclose(fp);
    a_string_free(&tmp);
}

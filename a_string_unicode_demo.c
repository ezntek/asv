#include <stdio.h>

#include "a_string.h"

static const char* UTF8_VALID[] = {
    "\x00",
    "\x09\x0A\x0D",
    "\x7F",
    "Hello, world!",
    "\xC2\x80",         // U+0080
    "\xC2\xA2",         // ¢
    "\xDF\xBF",         // U+07FF
    "\xE0\xA0\x80",     // U+0800
    "\xE2\x82\xAC",     // €
    "\xEF\xBF\xBF",     // U+FFFF
    "\xF0\x90\x80\x80", // U+10000
    "\xF0\x9F\x92\xA9", // 💩
    "\xF4\x8F\xBF\xBF", // U+10FFFF
    "ASCII \xC2\xA2 \xE2\x82\xAC \xF0\x9F\x92\xA9",
    "\x61\xE2\x82\xAC\x62",
    NULL};

static const char* UTF8_INVALID[] = {
    //
    "\x80",
    "\xBF",
    "\x80\x80",
    "\xC2",
    "\xE2\x82",
    "\xF0\x9F\x92",
    "\xC2\x20",
    "\xE2\x28\xA1",
    "\xF0\x28\x8C\xBC",
    "\xC0\x80",
    "\xC1\xBF",
    "\xE0\x80\x80",
    "\xF0\x80\x80\x80",
    "\xED\xA0\x80",
    "\xED\xBF\xBF",
    "\xF4\x90\x80\x80",
    "\xF7\xBF\xBF\xBF",
    "\xF5\x80\x80\x80",
    "\xF8\x80\x80\x80\x80",
    "\xFE",
    "\xFF",
    "OK\xE2\x82 BAD",
    "\x61\xF0\x9F\x92",
    NULL};

void test_valid(void) {
    puts("=== validation ===");
    a_string buf = as_with_capacity(64);
    int i = 0;
    bool test_pass = false;
    const char** ptr = NULL;
    for (ptr = UTF8_VALID; *ptr; ptr++) {
        as_copy_cstr(&buf, *ptr);
        test_pass = au_valid(&buf);
        printf("test %d: ", i + 1);
        if (test_pass) {
            printf("\x1b[32;1mpassed\x1b[0m (string: `%.*s`)", as_fmt(buf));
        } else {
            printf("\x1b[31;1mfailed\x1b[0m");
        }
        printf("\n");
        i++;
    }

    i = 0;
    for (ptr = UTF8_INVALID; *ptr; ptr++) {
        as_copy_cstr(&buf, *ptr);
        test_pass = !au_valid(&buf);
        printf("test %d: ", i + 1);
        if (test_pass) {
            printf("\x1b[32;1mpassed\x1b[0m");
        } else {
            printf("\x1b[31;1mfailed\x1b[0m");
        }
        printf("\n");
        i++;
    }
    as_free(&buf);
}

void test_basic(void) {
    puts("=== basic ===");
    a_string s = astr("🔥a_string is kööl 真好");
    printf("very kool string: %.*s\n", as_fmt(s));
    printf("bytes: %zu, chars: %zu\n", s.len, au_len(&s));
    as_free(&s);
}

void test_iter(void) {
    puts("=== iter ===");
    a_string s = astr("🔥C is a 🅱️eautifüł language💐🌸");
    as_println(&s);

    au_iter(&s, ptr) {
        dchar ch = au_decode(ptr);
        if (ch < 128) {
            printf("`%c`, ", ch);
        } else {
            printf("%d, ", ch);
        }
        fflush(stdout);
    }

    putchar('\n');
    as_free(&s);
}

int main(void) {
    test_basic();
    test_valid();
    test_iter();
    return 0;
}

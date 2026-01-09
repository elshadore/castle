#ifndef SHARED_HEADER_GUARD__
#define SHARED_HEADER_GUARD__
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

typedef struct String_ {
    char *c;
    size_t length;
} String;

typedef struct Writer_ {
    String string;
    size_t capacity;
} Writer;

#define strlit(text_) (String){.c = text_, .length = ((sizeof(text_) / sizeof(char)) - 1)}

size_t djb2_hash(String string);
bool string_eq(String a, String b);

#endif

#ifdef SHARED_IMPLEMENTATION

size_t djb2_hash(String string) {
    size_t result = 5381;
    for (size_t i = 0; i < string.length; ++i) {
        result = ((result << 5) + result) + (size_t)string.c[i];
    }
    return result;
}

bool string_eq(String a, String b) {
    if (a.length != b.length) return false;
    for (size_t i = 0; i < a.length; ++i) {
        if (a.c[i] != b.c[i]) return false;
    }
    return true;
}

void writer_put(Writer *writer, char c) {
    if ((writer->count + 1) >= string->capacity) {
        size_t new_capacity = new_capacity(string->capacity);
        char *new_c = realloc(string->c, sizeof(char) * new_capacity);
        string->c = new_c;
        string->capacity = new_capacity;
    }
    string->c[string->count++] = c;
    string->c[string->count] = '\0';
}

#endif

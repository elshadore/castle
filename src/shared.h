#ifndef SHARED_HEADER_GUARD__
#define SHARED_HEADER_GUARD__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <stdarg.h>

typedef struct String_ {
    char *c;
    size_t length;
} String;

typedef struct Bytes_ {
    uint8_t *b;
    size_t length;
} Bytes;

typedef struct Writer_ {
    String string;
    size_t capacity;
} Writer;

#define strlit(text_) (String){.c = text_, .length = ((sizeof(text_) / sizeof(char)) - 1)}

#define lengthof(array_) (sizeof(array_) / sizeof(*array_))

#define bcast(array_) (Bytes){.b = (uint8_t *)array_, .length = lengthof(array_)}

void string_reverse_mut(String string);
size_t grow_capacity(size_t old_capacity);
size_t djb2_hash(String string);

bool string_eq(String a, String b);
void string_stdout(String string);
void string_stderr(String string);

char char_to_digit(char c);
bool char_is_digit(char c);
bool char_is_alphabet(char c);
bool char_is_whitespace(char c);
char char_from_digit(size_t digit);

Writer writer_new(void);
void writer_deinit(Writer *writer);
size_t writer_curr(Writer *writer);
void writer_put(Writer *writer, char c);
void writer_puts(Writer *writer, String string);
void writer_number(Writer *writer, size_t number);
void writer_hex(Writer *writer, Bytes bytes, size_t padding);
void writer_int(Writer *writer, int integer);

void writer_stdout(Writer *writer);
void writer_stderr(Writer *writer);

#define talloc(type_, count_) (type_ *)malloc(sizeof(type_) * (count_))
#define trealloc(type_, ptr_, count_) (type_ *)realloc((ptr_), sizeof(type_) * (count_))

#endif

#ifdef SHARED_IMPLEMENTATION

void string_reverse_mut(String string) {
    char *a = string.c;
    char *b = string.c + (string.length - 1);
    while (a < b) {
        char temp = *a;
        *a = *b;
        *b = temp;
        a += 1;
        b -= 1;
    }
}

size_t grow_capacity(size_t old_capacity) {
    if (old_capacity == 0) {
        return 8;
    } else {
        return old_capacity * 2;
    }
}

size_t djb2_hash(String string) {
    size_t result = 5381;
    for (size_t i = 0; i < string.length; ++i) {
        result = ((result << 5) + result) + (size_t)string.c[i];
    }
    return result;
}

bool string_eq(String a, String b) {
    if (a.length != b.length) {
        return false;
    }
    for (size_t i = 0; i < a.length; ++i) {
        if (a.c[i] != b.c[i]) {
            return false;
        }
    }
    return true;
}

void string_stdout(String string) {
    fprintf(stdout, "%.*s", (int)string.length, string.c);
}

void string_stderr(String string) {
    fprintf(stderr, "%.*s", (int)string.length, string.c);
}

char char_to_digit(char c) {
    switch (c) {
        case '0': return 0;
        case '1': return 1;
        case '2': return 2;
        case '3': return 3;
        case '4': return 4;
        case '5': return 5;
        case '6': return 6;
        case '7': return 7;
        case '8': return 8;
        case '9': return 9;
        default: return '\0';
    }
}

bool char_is_digit(char c) {
    switch (c) {
        case '0': return true;
        case '1': return true;
        case '2': return true;
        case '3': return true;
        case '4': return true;
        case '5': return true;
        case '6': return true;
        case '7': return true;
        case '8': return true;
        case '9': return true;
        default: return false;
    }
}

bool char_is_alphabet(char c) {
    if (c >= 'A' && c <= 'Z') return true;
    if (c >= 'a' && c <= 'z') return true;
    return false;
}

bool char_is_whitespace(char c) {
    switch (c) {
        case ' ': return true;
        case '\n': return true;
        case '\f': return true;
        case '\r': return true;
        case '\t': return true;
        case '\v': return true;
        default: return false;
    }
}

char char_from_digit(size_t digit) {
    switch (digit) {
        case 0: return '0';
        case 1: return '1';
        case 2: return '2';
        case 3: return '3';
        case 4: return '4';
        case 5: return '5';
        case 6: return '6';
        case 7: return '7';
        case 8: return '8';
        case 9: return '9';
        default: return '\0';
    }
}

char char_from_hex(size_t hex) {
    switch (hex) {
        case 0: return '0';
        case 1: return '1';
        case 2: return '2';
        case 3: return '3';
        case 4: return '4';
        case 5: return '5';
        case 6: return '6';
        case 7: return '7';
        case 8: return '8';
        case 9: return '9';
        case 10: return 'A';
        case 11: return 'B';
        case 12: return 'C';
        case 13: return 'D';
        case 14: return 'E';
        case 15: return 'F';
        default: return '\0';
    }
}

Writer writer_new(void) {
    return (Writer){0};
}

void writer_deinit(Writer *writer) {
    if (writer->capacity > 0) {
        free(writer->string.c);
    }
    *writer = (Writer){0};
}

size_t writer_curr(Writer *writer) {
    return writer->string.length;
}

void writer_put(Writer *writer, char c) {
    if ((writer->string.length + 1) >= writer->capacity) {
        size_t new_capacity = grow_capacity(writer->capacity);
        char *new_c = trealloc(char, writer->string.c, new_capacity);
        writer->string.c = new_c;
        writer->capacity = new_capacity;
    }
    writer->string.c[writer->string.length++] = c;
    writer->string.c[writer->string.length] = '\0';
}

void writer_puts(Writer *writer, String string) {
    for(size_t i = 0; i < string.length; ++i) {
        writer_put(writer, string.c[i]);
    }
}

void __attribute__ ((format (printf, 2, 3))) writer_printf(Writer *writer, const char *format, ...) {
    va_list ap;
    va_start(ap, format);
    size_t length = vsnprintf(NULL, 0, format, ap);
    va_end(ap);
    size_t curr = writer_curr(writer);
    for (size_t i = 0; i < length; ++i) {
        writer_put(writer, '\0');
    }
    va_start(ap, format);
    (void)vsnprintf(writer->string.c + curr, length, format, ap);
    va_end(ap);
}

void writer_number(Writer *writer, size_t number) {
    if (number == 0) {
        writer_put(writer, '0');
        return;
    } 
    size_t curr = writer_curr(writer);
    size_t dec = number;

    while (dec != 0) {
        size_t a = dec;
        dec /= 10;
        size_t b = dec * 10;
        size_t c = a - b;
        char digit = char_from_digit(c);
        writer_put(writer, digit);
    }
    String rev = (String){.c = writer->string.c + curr, .length = writer->string.length - curr};
    string_reverse_mut(rev); 
}

void writer_hex(Writer *writer, Bytes bytes, size_t padding) {
    writer_puts(writer, strlit("0x"));
    for (size_t i = 0; i < bytes.length; ++i) {
        uint8_t byte = bytes.b[i];
        uint8_t hi = (byte >> 4) & 0x0F;
        uint8_t lo = byte & 0xF;
        writer_put(writer, char_from_hex(hi));
        writer_put(writer, char_from_hex(lo));
    }
    if (padding > bytes.length) {
        size_t diff = padding - bytes.length;
        for (size_t i = 0; i < diff; ++i) {
            writer_puts(writer, strlit("00"));
        }
    }
}

void writer_int(Writer *writer, int integer) {
    if (integer == 0) {
        writer_put(writer, '0');
        return;
    } 
    size_t curr = writer_curr(writer);
    int dec = 0;
    bool negetive = false;
    if (integer < 0) {
        dec = -integer;
        negetive = true;
    } else {
        dec = integer;
    }

    while (dec != 0) {
        int a = dec;
        dec /= 10;
        int b = dec * 10;
        int c = a - b;
        char digit = char_from_digit((size_t)c);
        writer_put(writer, digit);
    }
    if (negetive) {
        writer_put(writer, '-');
    }
    String rev = (String){.c = writer->string.c + curr, .length = writer->string.length - curr};
    string_reverse_mut(rev); 
}

void writer_stdout(Writer *writer) {
    string_stdout(writer->string);
}

void writer_stderr(Writer *writer) {
    string_stderr(writer->string);
}

#endif

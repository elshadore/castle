#ifndef SHARED_H
#define SHARED_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <stdarg.h>
#include <string.h>

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

typedef enum Indianness {
    Little_Indian,
    Big_Indian,
} Indianness;

typedef struct WriterMemOpt_ {
    Indianness indianness;
    size_t padding;
    bool prefix;
} WriterMemOpt;

#define DEFAULT_WRITER_MEMOPT (WriterMemOpt){.indianness = Little_Indian, .padding = 0, .prefix = true}

#define strlit(text_) (String){.c = text_, .length = ((sizeof(text_) / sizeof(char)) - 1)}

#define lengthof(array_) (sizeof(array_) / sizeof(*array_))

#define bcast(value_) (Bytes){.b = (uint8_t *)(&value_), .length = sizeof(value_)}

#define dlog(message_, ...) fprintf(stderr, message_ "\n" __VA_OPT__(,) __VA_ARGS__)

#define dpanic(message_, ...) do { \
    fprintf(stderr, message_ "\n" __VA_OPT__(,) __VA_ARGS__); \
    exit(69); \
} while(0)

float compute_fillpc(size_t count, size_t capacity);

void string_reverse_mut(String string);
size_t grow_capacity(size_t old_capacity);
size_t djb2_hash(String string);
size_t index_invert(size_t index, size_t length);

bool string_eq(String a, String b);
String string_shift(String string);
String string_shiftn(String string, size_t shift);
void string_stdout(String string);
void string_stderr(String string);
Bytes string_to_bytes(String string);

char char_to_digit(char c);
bool char_is_digit(char c);
bool char_is_alphabet(char c);
bool char_is_whitespace(char c);
char char_from_digit(size_t digit);

Writer writer_new(void);
void writer_deinit(Writer *writer);
void writer_clear(Writer *writer);
size_t writer_curr(Writer *writer);
void writer_put(Writer *writer, char c);
void writer_puts(Writer *writer, String string);
void writer_number(Writer *writer, size_t number);
void writer_hex(Writer *writer, Bytes bytes);
void writer_binary(Writer *writer, Bytes bytes);
void writer_hex_ex(Writer *writer, Bytes bytes, WriterMemOpt opt);
void writer_binary_ex(Writer *writer, Bytes bytes, WriterMemOpt opt);
void writer_newline(Writer *writer);
void writer_tab(Writer *writer);
void writer_tabn(Writer *writer, size_t count);
void writer_int(Writer *writer, int integer);
void __attribute__ ((format (printf, 2, 3))) writer_printf(Writer *writer, const char *format, ...);

void writer_stdout(Writer *writer);
void writer_stderr(Writer *writer);

#define talloc(type_, count_) (type_ *)malloc(sizeof(type_) * (count_))
#define trealloc(type_, ptr_, count_) (type_ *)realloc((ptr_), sizeof(type_) * (count_))

#define dynamic_array_grow(type_, array_) do { \
    if ((array_)->count >= (array_)->capacity) { \
        size_t new_capacity = grow_capacity((array_)->capacity); \
        type_ *new_e = (type_ *)realloc((array_)->e, sizeof(type_) * new_capacity); \
        (array_)->capacity = new_capacity; \
        (array_)->e = new_e; \
    } \
} while(0)

#define dynamic_array_append(type_, array_, item_) do { \
    dynamic_array_grow(type_, array_);\
    (array_)->e[(array_->count++)] = item_; \
} while(0)

#endif


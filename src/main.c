#define FLOPPY_IMPLEMENTATION
#include "floppy.h"

#define CHAIN_TABLE_IMPLEMENTATION
#include "chain_table.h"

#define SHARED_IMPLEMENTATION
#include "shared.h"

// typedef struct CoolSlot_ {
//     String key;
//     int value;
//     bool occupied;
// } CoolSlot;

// typedef struct CoolTable_ {
//     CoolSlot *e;
//     size_t count;
//     size_t capacity;
// } CoolTable;

// CoolTable cool_init(void) {
//     return (CoolTable){0};
// }

// void cool_put(CoolTable *table) {
    
// }

// typedef struct UTF8String_ {
//     char *c;
//     size_t chars;
//     size_t length;
// } UTF8String;

// void bytedump(String string) {
//     for (size_t i = 0; i < string.length; ++i) {
//         char c = string.c[i];
//         printf("[%zu] ", i);
//         for (size_t j = 0; j < 8; ++j) {
//             // printf()
//         }
//         printf(" '%c'", c);
//     }
//     printf("\n");
// }

// bool utf8_codepoint(char c) {
//     return (c & 0xC0) == 0x80;
// }

// void utf8_read(String string) {
    
// }

// void utf8_testicle(void) {
//     String a = strlit("λ");
// }

void writer_testicle(Writer *writer) {
    writer_printf(writer, "henlo word => %s, %d, %f\n", "foobar", 69420, 1.0f);
}

int main(void) {
    printf("henlo word!\n");
    // chain_testicle();
    // floppy_testicle();
    Writer writer = writer_new();
    writer_testicle(&writer);
    writer_stdout(&writer);
    writer_deinit(&writer);

    return EXIT_SUCCESS;
}

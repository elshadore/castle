#include "shared.h"

#define EXP_MASK ((uint32_t)0x7F800000)
// #define EXP_MASK ((uint32_t)0b01111111100000000000000000000000)

#define SIGN_MASK ((uint32_t)0x80000000)
// #define SIGN_MASK ((uint32_t)0b10000000000000000000000000000000)

#define FRAC_MASK ((uint32_t)0x007FFFFF)
// #define FRAC_MASK ((uint32_t)0b00000000011111111111111111111111)

void writer_floppy_info(Writer *writer, float floater) {
    uint32_t bits = *((uint32_t *)&floater);
    writer_puts(writer, strlit("float => \n"));
    writer_puts(writer, strlit("\tsign => "));
    if ((bits & SIGN_MASK) != 0) {
        writer_puts(writer, strlit("[-]\n"));
    } else {
        writer_puts(writer, strlit("[+]\n"));
    }

    uint32_t exponent = bits & EXP_MASK;
    uint32_t exp_value = exponent >> 23;
    int32_t exp = ((int32_t)exp_value) - 127;
    writer_puts(writer, strlit("\texponent => 2^"));
    writer_int(writer, (int)exp);
    writer_newline(writer);
    writer_puts(writer, strlit("\tfraction => "));
    uint32_t frac = bits & FRAC_MASK;

    size_t acc = 1;
    size_t dec = 1;
    for (size_t i = 0; i < 23; ++i) {
        size_t inv = index_invert(i, 23);
        dec *= 10;
        dec /= 2;
        if (frac & (1 << inv)) {
            acc *= 10;
            acc += dec;
            dlog("dec => %zu, acc => %zu", dec, acc);
        }
    }
    writer_number(writer, (size_t)acc);
    writer_newline(writer);
}

void writer_floppy(Writer *writer, float floater) {
    uint32_t bits = *((uint32_t *)&floater);
    if ((bits & 0x80000000) != 0) {
        writer_put(writer, '-');
    } else {
        writer_put(writer, '+');
    }
}

void floppy_foop(Writer *writer, float floater) {
    writer_puts(writer, strlit("FLOAT => "));
    writer_printf(writer, "%f", floater);
    writer_newline(writer);
    Bytes bytes = bcast(floater);
    writer_hex(writer, bytes);
    writer_newline(writer);
    writer_binary(writer, bytes);
    writer_newline(writer);
    writer_floppy_info(writer, floater);
    writer_newline(writer);
    writer_newline(writer);
}

void floppy_testicle(Writer *writer) {
    writer_puts(writer, strlit("*~FLOPPY-TESTICLE~*\n"));
    floppy_foop(writer, 1.2f);
    // {
    //     float floaters[] = {
    //         1.0f,
    //         -1.0f,
    //         2.0f,
    //         1.75f,
    //     };
    //     size_t length = lengthof(floaters);
    //     for (size_t i = 0; i < length; ++i) {
    //         floppy_foop(writer, floaters[i]);
    //     }
    // }
    // {
    //     uint32_t datas[] = {
    //         0x3F800000,  // 1.0
    //         0x40000000,  // 2.0
    //         0x40490FDB,  // 3.1415927 (pi approximation)
    //         0x3F99999A,  // 1.2
    //         0x00000000,  // +0.0
    //         0x80000000,  // -0.0
    //         0x7F800000,  // +inf
    //         0xFF800000,  // -inf
    //     };
    //     size_t length = lengthof(datas);
    //     for (size_t i = 0; i < length; ++i) {
    //         float floater = *((float *)&datas[i]);
    //         floppy_foop(writer, floater);
    //     }
    // }
}

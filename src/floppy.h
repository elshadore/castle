#ifndef FLOPPY_HEADER_GUARD__
#define FLOPPY_HEADER_GUARD__

#include "shared.h"

void floppy_testicle(void);

#endif


#ifdef FLOPPY_IMPLEMENTATION

void writer_floppy(Writer *writer, float floater) {
    uint32_t bits = *((uint32_t *)&floater);
    uint32_t sign = (bits >> 31) & 0x1;
    uint32_t exponent = (bits >> 23) & 0xFF;
    uint32_t mantissa = bits & 0x7FFFFF;

    if (exponent == 0xFF) {
        if (mantissa == 0) {
            if (sign) {
                writer_put(writer, '-');
            }
            writer_puts(writer, strlit("inf"));
        } else {
            writer_puts(writer, strlit("nan"));
            // if (mantissa = & 0x400000) {
            //     writer_puts(writer, strlit("qNaN"));
            // } else {
            //     writer_puts(writer, strlit("sNaN"));
            // }
        }
        return;
    } 
    if (sign) {
        writer_put(writer, '-');
    }
    if ((exponent == 0) && (mantissa == 0)) {
        writer_puts(writer, strlit("0.0"));
        return;
    } 
    int32_t exp_value = 0;
    if (exponent == 0) {
        exp_value = -126;
    } else {
        exp_value = (int32_t)exponent - 127;
        mantissa |= 0x800000;
    }
    if ((exp_value >= 0) && (exp_value <= 10)) {
        uint32_t integer_part = 0;
        if (exponent == 0) {
            integer_part = 0;
        } else {
            integer_part = 1;
        }
        for (int i = 0; i < exp_value; i++) {
            integer_part = integer_part << 1;
            if (mantissa & 0x800000) {
                integer_part |= 1;
            }
            mantissa = mantissa << 1;
        }
        float fraction_part = 0.0;
        uint32_t temp_mantissa = mantissa;
        for (int i = 0; i < 23; i++) {
            fraction_part /= 2.0;
            if (temp_mantissa & 0x800000) {
                fraction_part += 0.5;
            }
            temp_mantissa <<= 1;
        }
        writer_number(writer, (size_t)integer_part);
        writer_put(writer, '.');

        int frac_int = (int)(fraction_part * 1000000);

        if (frac_int == 0) {
            writer_put(writer, '0');
        } else {
            writer_int(writer, frac_int);
        }
    } else {
        writer_puts(writer, strlit("1."));
    }
}

void floppy_testicle(void) {
    Writer writer = writer_new();
    writer_puts(&writer, strlit("*~FLOPPY-TESTICLE~*\n"));
    uint32_t test_values[] = {
        0x3F800000,  // 1.0
        0x40000000,  // 2.0
        0x40490FDB,  // 3.1415927 (pi approximation)
        0x3F99999A,  // 1.2
        0x00000000,  // +0.0
        0x80000000,  // -0.0
        0x7F800000,  // +inf
        0xFF800000,  // -inf
    };
    size_t length = lengthof(test_values);
    for (size_t i = 0; i < length; ++i) {
        float flop = *((float *)&test_values[i]);
        writer_puts(&writer, strlit("["));
        writer_number(&writer, i);
        writer_puts(&writer, strlit("] => "));
        writer_floppy(&writer, flop);
        writer_put(&writer, '\n');
    }
    writer_stdout(&writer);
    writer_deinit(&writer);
}

#endif

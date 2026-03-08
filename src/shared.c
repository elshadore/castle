#include "shared.h"

uint64_t next_pow2_u64(uint64_t value) {
    uint64_t temp = value;
    temp--;
	temp |= temp >> 1;
	temp |= temp >> 2;
	temp |= temp >> 4;
	temp |= temp >> 8;
	temp |= temp >> 16;
	temp |= temp >> 32;
	temp++;
	return temp;
}

uint32_t next_pow2_u32(uint32_t value) {
    uint32_t temp = value;
    temp--;
	temp |= temp >> 1;
	temp |= temp >> 2;
	temp |= temp >> 4;
	temp |= temp >> 8;
	temp |= temp >> 16;
	temp++;
	return temp;
}

size_t next_pow2_uword(size_t value) {
    if (sizeof(size_t) == sizeof(uint64_t)) {
        return (size_t)next_pow2_u64((uint64_t)value);
    }
    if (sizeof(size_t) == sizeof(uint32_t)) {
        return (size_t)next_pow2_u32((uint32_t)value);
    }
    dpanic("unsupported word size of %zu", sizeof(size_t));
    return 0;
}

float compute_fillpc(size_t count, size_t capacity) {
    return (float)count / (float)capacity;
}

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

size_t index_invert(size_t index, size_t length) {
    return length - index - 1;
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

String string_shift(String string) {
    return (String) {
        .c = string.c + 1,
        .length = string.length - 1,
    };
}

String string_shiftn(String string, size_t shift) {
    return (String) {
        .c = string.c + shift,
        .length = string.length - shift,
    };
}

void string_stdout(String string) {
    fprintf(stdout, "%.*s", (int)string.length, string.c);
}

void string_stderr(String string) {
    fprintf(stderr, "%.*s", (int)string.length, string.c);
}

Bytes string_to_bytes(String string) {
    return (Bytes) {
        .b = (uint8_t *)string.c,
        .length = string.length,
    };
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

void writer_clear(Writer *writer) {
    writer->string.length = 0;
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
    if (length == 0) {
        return;
    }
    size_t curr = writer_curr(writer);
    for (size_t i = 0; i < length; ++i) {
        writer_put(writer, '\0');
    }
    va_start(ap, format);
    (void)vsnprintf(writer->string.c + curr, (length + 1), format, ap);
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

void writer_hex_ex(Writer *writer, Bytes bytes, WriterMemOpt opt) {
    if (opt.prefix) {
        writer_puts(writer, strlit("0x"));
    }
    for (size_t i = 0; i < bytes.length; ++i) {
        size_t index = i;
        if (opt.indianness == Little_Indian) {
            index = index_invert(i, bytes.length);
        }
        uint8_t byte = bytes.b[index];
        uint8_t hi = (byte >> 4) & 0x0F;
        uint8_t lo = byte & 0xF;
        writer_put(writer, char_from_hex(hi));
        writer_put(writer, char_from_hex(lo));
    }
    if (opt.padding > bytes.length) {
        size_t diff = opt.padding - bytes.length;
        for (size_t i = 0; i < diff; ++i) {
            writer_puts(writer, strlit("00"));
        }
    }
}

void writer_hex(Writer *writer, Bytes bytes) {
    writer_hex_ex(writer, bytes, DEFAULT_WRITER_MEMOPT);
}

void writer_binary_ex(Writer *writer, Bytes bytes, WriterMemOpt opt) {
    if (opt.prefix) {
        writer_puts(writer, strlit("0b"));
    }
    bool flag = false;
    for (size_t i = 0; i < bytes.length; ++i) {
        size_t index = i;
        if (opt.indianness == Little_Indian) {
            index = index_invert(i, bytes.length);
        }
        uint8_t byte = bytes.b[index];
        if (flag) {
            writer_put(writer, '_');
        }
        for (size_t j = 0; j < 8; ++j) {
            size_t inv = index_invert(j, 8);
            uint8_t bit = (byte >> inv) & 1;
            if (bit) {
                writer_put(writer, '1');
            } else {
                writer_put(writer, '0');
            }
        }
        flag = true;
    }
    if (opt.padding > bytes.length) {
        size_t diff = opt.padding - bytes.length;
        for (size_t i = 0; i < diff; ++i) {
            if (flag) {
                writer_put(writer, '_');
            }
            writer_puts(writer, strlit("00000000"));
            flag = true;
        }
    }
}

void writer_binary(Writer *writer, Bytes bytes) {
    writer_binary_ex(writer, bytes, DEFAULT_WRITER_MEMOPT);
}

void writer_newline(Writer *writer) {
    writer_put(writer, '\n');
}

void writer_tab(Writer *writer) {
    writer_put(writer, '\t');
}

void writer_tabn(Writer *writer, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        writer_put(writer, '\t');
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

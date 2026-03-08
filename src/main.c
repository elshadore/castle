#include "all.h"

#define UTF8_BIT (1 << 7)

typedef struct UTF8Code_ {
    uint32_t codepoint;
    size_t length;
    String select;
    String rest;
} UTF8Code;

bool utf8_is_valid_token(char c) {
    return (c & 0xC0) == 0x80;
}

bool utf8_codepoint_parse(String string, UTF8Code *output) {
    if (string.length == 0) {
        return false;
    }
    char first = string.c[0];
    uint32_t code = 0;
    size_t length = 0;
    if ((first & 0x80) == 0) {
        code = (uint32_t)first;
        length = 1;
    } else if ((first & 0xE0) == 0xC0) {
        if (string.length < 2) {
            return false;
        }
        char a = string.c[1];
        if (!utf8_is_valid_token(a)) {
            return false;
        }
        code = ((first & 0xF0) << 6) | (a & 0x3F);
        length = 2;
    } else if ((first & 0xF0) == 0xE0) {
        if (string.length < 3) {
            return false;
        }
        char a = string.c[1];
        if (!utf8_is_valid_token(a)) {
            return false;
        }
        char b = string.c[2];
        if (!utf8_is_valid_token(b)) {
            return false;
        }
        code = ((first & 0xF0) << 12) | ((a & 0x3F) << 6) | (b & 0x3F);
        length = 3;
    } else if ((first & 0xF8) == 0xF0) {
        if (string.length < 4) {
            return false;
        }
        char a = string.c[1];
        if (!utf8_is_valid_token(a)) {
            return false;
        }
        char b = string.c[2];
        if (!utf8_is_valid_token(b)) {
            return false;
        }
        char c = string.c[3];
        if (!utf8_is_valid_token(c)) {
            return false;
        }
        code = ((first & 0xF0) << 18) | ((a & 0x3F) << 12) | ((b & 0x3F) << 6) | (c & 0x3F);
        length = 4;
    } else {
        return false;
    }
    *output = (UTF8Code) {
        .codepoint = code,
        .length = length,
        .select = (String) {
            .c = string.c,
            .length = length,
        },
        .rest = string_shiftn(string, length),
    };
    return true;
}

void utf8_copy(String string, Writer *output) {
    String iter = string;
    while (iter.length > 0) {
        UTF8Code code = {0};
        if (utf8_codepoint_parse(iter, &code)) {
            writer_puts(output, code.select);
            iter = code.rest;
        } else {
            writer_puts(output, strlit("�"));
            iter = string_shift(iter);
        }
    }
}

bool utf8_is_valid(String string) {
    UTF8Code trash = (UTF8Code){0};
    return utf8_codepoint_parse(string, &trash);
}

void utf8_testicle(Writer *w) {
    writer_puts(w, strlit("*~UTF8-TESTICLE~*\n"));
    // uint8_t byte = 0b10000000;
    // // 0x80
    // // uint8_t byte = 0b11000000;
    // // 0xC0
    // // uint8_t byte = 0b00111111;
    // // 0x0000003F
    // Bytes bytes = bcast(byte);
    // writer_hex(w, bytes);
    // writer_newline(w);
    String strings[] = {
        // Valid
        strlit("A"),
        strlit("α"),
        strlit("世"),
        strlit("😊"),
        strlit("Hello"),
        strlit("café"),
        strlit("日本語"),
        strlit("Aα😊世"),
        // Maformed
        // 1. Overlong encodings (encoding ASCII in more bytes than needed)
        strlit("\xC0\xAF"),           // Overlong '/' (should be 0x2F)
        strlit("\xE0\x80\xAF"),       // Another overlong '/'
        strlit("\xF0\x80\x80\xAF"),   // 4-byte overlong '/'

        // 2. Invalid continuation bytes
        strlit("\xC0"),               // Missing continuation byte
        strlit("\xE0\x80"),           // Missing second continuation
        strlit("\xF0\x80\x80"),       // Missing third continuation
        strlit("\xC0\xC0"),           // Invalid continuation (0xC0 not 10xxxxxx)
        strlit("\xE0\xFF\x80"),       // Middle byte invalid (0xFF)

        // 3. Invalid first bytes
        strlit("\xFF"),               // 0xFF never valid in UTF-8
        strlit("\xFE"),               // 0xFE never valid
        strlit("\xC1\xBF"),           // 0xC1 invalid (would encode 0x7F in 2 bytes)
        strlit("\xF5\x80\x80\x80"),   // 0xF5 invalid (would encode > U+10FFFF)

        // 4. Surrogate halves (U+D800 to U+DFFF)
        strlit("\xED\xA0\x80"),       // U+D800 (high surrogate)
        strlit("\xED\xBF\xBF"),       // U+DFFF (low surrogate)
        strlit("\xED\xA0\x80\xED\xB0\x80"),  // Paired surrogates (U+D800 U+DC00)

        // 5. Beyond Unicode range (> U+10FFFF)
        strlit("\xF4\x90\x80\x80"),   // U+110000 (too high)
        strlit("\xF7\xBF\xBF\xBF"),   // U+1FFFFF

        // 6. Mixed valid and invalid
        strlit("Hello\xC0\xAFWorld"),  // Valid + overlong + valid
        strlit("caf\xC3"),            // Truncated é (é = C3 A9, missing A9)
        strlit("日\xE6\x9C"),          // Truncated 本 (本 = E6 9C AC)

        // 7. Non-shortest form zeros
        strlit("\xC0\x80"),           // Overlong NULL (should be 0x00)
        strlit("\xE0\x80\x80"),       // 3-byte NULL
        strlit("\xF0\x80\x80\x80"),   // 4-byte NULL

        // 8. Invalid byte sequences
        strlit("\x80"),               // Continuation byte without start
        strlit("\xBF"),               // Another lone continuation
        strlit("\xFE\xFE\xFF\xFF"),   // Common invalid pattern

        // 9. Edge cases with BOM
        strlit("\xEF\xBB"),           // Truncated BOM (EF BB BF)
        strlit("\xEF\xBB\xBF\xC0"),   // BOM followed by invalid
    };
    size_t length = lengthof(strings);
    for (size_t i = 0; i < length; ++i) {
        String string = strings[i];
        writer_put(w, '[');
        writer_number(w, i);
        writer_puts(w, strlit("] => "));
        writer_puts(w, string);
        writer_newline(w);
        Bytes b = string_to_bytes(string);
        WriterMemOpt opt = {
            .indianness = Big_Indian,
            .padding = 0,
            .prefix = true,
        };
        writer_tab(w);
        // writer_binary_ex(w, b, opt);
        writer_hex_ex(w, b, opt);
        writer_newline(w);
        size_t codes = 0;
        String iter = string;
        while (iter.length > 0) {
            UTF8Code code = {0};
            if (utf8_codepoint_parse(iter, &code)) {
                writer_tabn(w, 2);
                writer_puts(w, strlit("char: "));
                writer_puts(w, code.select);
                writer_newline(w);
                writer_tabn(w, 2);
                writer_puts(w, strlit("code: "));
                writer_number(w, code.codepoint);
                writer_puts(w, strlit(", => U+"));
                Bytes b = bcast(code.codepoint);
                WriterMemOpt opt = {
                    .indianness = Little_Indian,
                    .padding = 0,
                    .prefix = false,
                };
                writer_hex_ex(w, b, opt);
                writer_newline(w);
                writer_tabn(w, 2);
                writer_puts(w, strlit("length: "));
                writer_number(w, code.length);
                writer_newline(w);
                iter = code.rest;
                codes += 1;
            } else {
                writer_tabn(w, 2);
                writer_puts(w, strlit("malformed codepoint"));
                writer_newline(w);
                writer_tabn(w, 2);
                writer_puts(w, strlit("�"));
                writer_newline(w);
                iter = string_shift(iter);
            }
        }
        writer_tab(w);
        writer_puts(w, strlit("codepoints: "));
        writer_number(w, codes);
        writer_newline(w);
    }
}

int main(void) {
    printf("henlo word!\n");
    Writer w = writer_new();
    utf8_testicle(&w);
    writer_stdout(&w);
    writer_deinit(&w);
    return EXIT_SUCCESS;
}

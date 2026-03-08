#define NOB_IMPLEMENTATION
#include "testicles.h"

int main(void) {
    printf("henlo word!\n");
    Writer w = writer_new();
    utf8_testicle(&w);
    writer_stdout(&w);
    writer_deinit(&w);
    return EXIT_SUCCESS;
}

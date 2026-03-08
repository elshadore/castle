#define NOB_IMPLEMENTATION
#include "lib/nob.h"

int main(int argc, char **argv) {
    NOB_GO_REBUILD_URSELF(argc, argv);
    if (!nob_mkdir_if_not_exists("./out/")) return EXIT_FAILURE;
    Nob_Cmd cmd = {0};
    nob_cmd_append(&cmd,
        "cc", "-std=c99", "-Wall", "-Wextra",
        "./src/main.c",
        "./src/shared.c",
        "./src/floppy.c",
        "./src/skip_table.c",
        "./src/chain_table.c",
        "-o", "./out/castle"
    );
    if (!nob_cmd_run(&cmd)) return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

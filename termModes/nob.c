#define NOB_IMPLEMENTATION
#include "nob.h"

// Some folder paths that we use throughout the build process.
#define BUILD_FOLDER "build/"
#define SRC_FOLDER   "src/"

int main(int argc, char **argv)
{
    NOB_GO_REBUILD_URSELF(argc, argv);
    if (!nob_mkdir_if_not_exists(BUILD_FOLDER)) return 1;
    Nob_Cmd cmd = {0};
#if !defined(_MSC_VER)
    // On POSIX
    nob_cmd_append(&cmd, "cc", "-Wall", "-Wextra", "-o", BUILD_FOLDER"term", SRC_FOLDER"main.c");
    // nob_cmd_append(&cmd, "cc", "-o", BUILD_FOLDER"term", SRC_FOLDER"main.c");
#else
    // On MSVC
    nob_cmd_append(&cmd, "cl", "-I.", "/Fe:"BUILD_FOLDER"term", SRC_FOLDER"main.c");
#endif // _MSC_VER
    // Let's execute the command.
    if (!nob_cmd_run(&cmd)) return 1;
    nob_cc(&cmd);
    // nob_cc_flags(&cmd);
    // if (!nob_cmd_run(&cmd)) return 1;

    return 0;
}

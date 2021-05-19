#include "CmdArgs.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif


namespace cmd {

void show_windows_console()
{
#ifdef _WIN32
    AllocConsole();
    freopen("CONIN$", "r", stdin);
    freopen("CONOUT$", "w", stderr);
    freopen("CONOUT$", "w", stdout);
#endif
}


void print_help(const char* title, const char* version)
{
    printf("%s %s\n\n", title, version);
    printf(
        "Super Mario War is a Super Mario multiplayer game. The goal is to stomp\n"
        "as many other Marios as possible to win the game. It's a tribute to Nintendo\n"
        "and the game Mario War by Samuele Poletto.\n\n");
    printf("Options:\n");
    printf("  -h, --help              Prints this help\n");
    printf("      --datadir <DIR>     Sets the data directory to DIR (default: ./data)\n");
    printf("      --debug             Shows the debug console on Windows\n");
}


Args parse_args(int argc, char* argv[])
{
    Args result;

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];

        if (arg == "-h" || arg == "--help") {
            result.success = true;
            result.show_help = true;
            return result;
        }

        if (arg == "--debug") {
            result.debug = true;
            continue;
        }

        if (arg == "--datadir") {
            if (++i >= argc) {
                fprintf(stderr, "Error: `--datadir` requires a parameter, see `--help`\n");
                return result;
            }
            result.data_root = argv[i];
            continue;
        }
    }

    result.success = true;
    return result;
}

} //namespace cmd

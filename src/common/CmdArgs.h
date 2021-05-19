#pragma once

#include <string>


namespace cmd {

struct Args {
    bool success = false;
    bool show_help = false;
    bool debug = false;
    std::string data_root;
};
Args parse_args(int argc, char* argv[]);


void print_help(const char* title, const char* version);
void show_windows_console();

} // namespace cmd

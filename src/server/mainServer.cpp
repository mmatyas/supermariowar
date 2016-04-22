#include "server.h"

#include <cstdio>
#include <cstdlib>
#include <string>
#include <ctime>

#ifdef __unix__
#include <csignal>
#include <unistd.h>
#endif

bool running = true;

void cleanup()
{
    //server cleans up in its destructor
}

void interrupt(int code)
{
    printf("  Goodbye!\n");
    running = false;
}

int main(int argc, char* argv[])
{
    printf("SMW Server alpha\n");

    SMWServer server;
    if (!server.init()) {
        cleanup();
        return 1;
    }

    //
    // Interrupt handling
    //
    #ifdef __unix__
        struct sigaction interruptHandler;
            interruptHandler.sa_handler = interrupt;
            sigemptyset(&interruptHandler.sa_mask);
            interruptHandler.sa_flags = 0;
        sigaction(SIGINT, &interruptHandler, NULL);
    #endif

    printf("Ready!\n");

    //
    // Main loop
    //
    clock_t frameStart = clock();
    clock_t frameEnd;
    while (running)
    {
        server.update(running);

        // Do not use 100% CPU if not necessary
        frameEnd = clock();
        if (frameEnd - frameStart < 30)
            #ifdef __unix__
                usleep( (frameStart + 30 - frameEnd) * 1000); // microseconds!
            #endif
        frameStart = clock();
    }

    cleanup();
    return 0;
}

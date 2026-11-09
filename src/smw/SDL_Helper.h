#pragma once

#define SDL_TrialAndError(expression) [&]() { \
        auto result = (expression); \
        if (!result) { \
            SDL_Log("SDL Error: %s", SDL_GetError()); \
        } \
        return result; \
    }()
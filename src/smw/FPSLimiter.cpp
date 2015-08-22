#include "FPSLimiter.h"

#include "GameValues.h"
#include "ResourceManager.h"

#include "SDL.h"


extern CGameValues game_values;
extern CResourceManager* rm;


FPSLimiter::FPSLimiter()
    : framestart(0)
    , ticks(0)
    , realfps(0)
    , flipfps(0)
{}

FPSLimiter& FPSLimiter::instance()
{
    static FPSLimiter instance;
    return instance;
}


#ifdef __EMSCRIPTEN__
// the browser limits the frame rate
void FPSLimiter::frameStart() {}
void FPSLimiter::beforeFlip() {}
void FPSLimiter::afterFlip() {}

#else

void FPSLimiter::frameStart() {
    framestart = SDL_GetTicks();
}

void FPSLimiter::beforeFlip()
{
    ticks = SDL_GetTicks() - framestart;
    if (ticks == 0)
        ticks = 1;

#if !_DEBUG
    if (game_values.showfps)
#endif
    {
        float potentialFps = 1000.0f / (float)(game_values.framelimiter == 0 ? 1 : game_values.framelimiter);
        rm->menu_font_large.drawf(0, 480 - rm->menu_font_large.getHeight(), "Actual:%.1f/%.1f, Flip:%.1f, Potential:%.1f", realfps, potentialFps, flipfps, 1000.0f / (float)ticks);
    }
}

void FPSLimiter::afterFlip()
{
    flipfps = 1000.0f / (float)ticks;

    //Sleep for time just under what we need
    short delay = (short)(game_values.framelimiter - SDL_GetTicks() + framestart - 2);

    if (delay > 0) {
        if (delay > game_values.framelimiter)
            delay = game_values.framelimiter;

        SDL_Delay(delay);
    }

    //Fine tune wait here
    while (SDL_GetTicks() - framestart < (unsigned short)game_values.framelimiter)
        SDL_Delay(0);   //keep framerate constant at 1000/game_values.framelimiter fps

    //Debug code to slow framerate down to 1 fps to see exact movement
#ifdef _DEBUG
    if (game_values.frameadvance) {
        delay = (short)(1000 - SDL_GetTicks() + framestart);

        if (delay > 0) {
            if (delay > 1000)
                delay = 1000;

            SDL_Delay(delay);
        }

        while (SDL_GetTicks() - framestart < 1000)
            SDL_Delay(0);
    }
#endif

    ticks = SDL_GetTicks() - framestart;
    if (ticks == 0)
        ticks = game_values.framelimiter;

    realfps = 1000.0f / (float)ticks;

    /*
    static float avgFPS = 0.0f;
    static short outputtimer = 0;

    avgFPS += realfps;

    if (++outputtimer == 1)
    {
        FILE * out = fopen("fps.txt", "a+");

        fprintf(out, "%.2f\n", avgFPS / (float)outputtimer);

        fclose(out);

        avgFPS = 0.0f;
        outputtimer = 0;
    }*/
}
#endif

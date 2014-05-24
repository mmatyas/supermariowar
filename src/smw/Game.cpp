
#include "global.h"
#include "Game.h"

#if	_WIN32
#include <windows.h>
#endif

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "path.h"
#include "FPSLimiter.h"
#include "GSSplashScreen.h"

#ifdef __EMSCRIPTEN__
void game_frame();
#endif

CGame::CGame(char *rootDirectory)
{
    // make sure that the .smw directory is created
    std::string smwHome = GetHomeDirectory();

#if	_WIN32
    if (CreateDirectory(smwHome .c_str(), NULL) ||
            ERROR_ALREADY_EXISTS == GetLastError()) {
    	//TODO: print that directory already exists

    } else {
    	//TODO: print that we created the directory
    }
#else
    //TODO: create directory in MacOSX/Linux systems
#endif

#ifdef _XBOX
    	//TODO: make this configurable!
    	//TV's seem to need extra shade
    	MenuTransparency = 96;
#else
    	MenuTransparency = 72;
#endif

    ScreenWidth = 640;
    ScreenHeight = 480;

	// a supposedly faster RNG
	rng = new Well512RandomNumberGenerator();
}

CGame::~CGame(void)
{
    delete rng;
}

void CGame::Go()
{
    SplashScreenState::instance().init();
    GameStateManager::instance().currentState = &SplashScreenState::instance();


#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(game_frame, 60, 1);
}

void game_frame()
#else
    while(game_values.gamestate != GS_QUIT)
#endif
    {
        FPSLimiter::get().frameStart();

        GameStateManager::instance().currentState->update();

        FPSLimiter::get().beforeFlip();

#ifdef USE_SDL2
        SDL_UpdateTexture(screenAsTexture, NULL, screen->pixels, screen->pitch);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, screenAsTexture, NULL, NULL);
        SDL_RenderPresent(renderer);
#else
        SDL_Flip(screen);
#endif

        FPSLimiter::get().afterFlip();
    }

#ifndef __EMSCRIPTEN__
}
#endif

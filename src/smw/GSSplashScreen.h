#ifndef GAMESTATE_SPLASHSCREENSTATE_H
#define GAMESTATE_SPLASHSCREENSTATE_H

#include "GameState.h"

#include "SDL.h"
#include "gfx.h"

class SplashScreenState : public GameState
{
    public:
        bool init();
        void update();

        static SplashScreenState& instance();

    private:
        void onEnterState() {}
        void onLeaveState();

        //bool LoadStartGraphics();
        //bool LoadGameSounds();

        SDL_Event loop_event;
        gfxSprite* menu_dpi_logo;
        gfxSprite* menu_contest_winners;
        gfxSprite* menu_credits;
        int alpha;
        int state;
        int timer;

        SplashScreenState();
        ~SplashScreenState() {}
        SplashScreenState(SplashScreenState const&);
        void operator=(SplashScreenState const&);
};

#endif // GAMESTATE_SPLASHSCREENSTATE_H

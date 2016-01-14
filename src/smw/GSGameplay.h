#ifndef GAMESTATE_GAMEPLAY_H
#define GAMESTATE_GAMEPLAY_H

#include "GameState.h"
#include "input.h"

class GameplayState : public GameState
{
    public:
        void update();

        static GameplayState& instance();

    private:
        GameplayState();
        ~GameplayState() {}
        GameplayState(GameplayState const&);
        void operator=(GameplayState const&);

        void onEnterState();

        void createPlayers();
        void initScoreDisplayPosition();
        void initEyeCandy();
        void initRunGame();
        void CleanDeadPlayers();

        void handleInput();

        void drawFrontLayer();
        void drawBackLayer();
        void drawMiddleLayer();

        void spinScreen();
        void drawScreenFade();
        void drawPlayerSwap();
        void drawScreenShakeBackground();
        void drawScoreboard(short iScoreTextOffset[4]);
        void drawWindMeter();
        void drawCountdown();
        void drawSpotlights();
        void drawOutOfScreenIndicators();
        void drawEverything(short iCountDownState, short iScoreTextOffset[4]);
        //.
        //..
        //...
        //TODO
        void read_network();
        void update_countdown_timer();
        void update_world();

        short iCountDownState;
        short iCountDownTimer;
        short iWindTimer;
        float dNextWind;
        short iScoreTextOffset[4];

        short respawnCount[4];
        short respawnanimationtimer[4];
        short respawnanimationframe[4];

        COutputControl* current_playerKeys;
        COutputControl previous_playerKeys;

        static short scoreoffsets[3];
        static short g_iPowerupToIcon[8];

        //Vars that keep track of spinning the screen
        float spinangle;
        float spinspeed;
        short spindirection;
        short spintimer;

};

#endif // GAMESTATE_GAMEPLAY_H

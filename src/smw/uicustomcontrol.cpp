#include "uicustomcontrol.h"

#include "SDL_image.h"

#include "FileList.h"
#include "Game.h"
#include "GameMode.h"
#include "gamemodes.h"
#include "GameValues.h"
#include "map.h"
#include "MapList.h"
#include "RandomNumberGenerator.h"
#include "ResourceManager.h"
#include "Score.h"
#include "ui/MI_Image.h"
#include "ui/MI_ScoreText.h"
#include "ui/MI_Text.h"

#include <cmath>
#include <cstdlib> // abs()
#include <cstring>
#include <string>

#ifndef __EMSCRIPTEN__
    inline void smallDelay() { SDL_Delay(10); }
#else
    inline void smallDelay() {}
#endif

extern SDL_Surface* screen;
extern SDL_Surface* blitdest;

extern CScore* score[4];

extern CMap* g_map;

extern short g_iDefaultPowerupPresets[NUM_POWERUP_PRESETS][NUM_POWERUPS];
extern short g_iCurrentPowerupPresets[NUM_POWERUP_PRESETS][NUM_POWERUPS];
extern short iScoreboardPlayerOffsetsX[3][3];
extern WorldMap g_worldmap;
extern short LookupTeamID(short id);

extern CGameMode * gamemodes[GAMEMODE_LAST];
extern CGM_Pipe_MiniGame * pipegamemode;
extern CGM_Boss_MiniGame * bossgamemode;
extern CGM_Boxes_MiniGame * boxesgamemode;

extern MapList *maplist;
extern MusicList *musiclist;
extern WorldMusicList *worldmusiclist;

extern CGameValues game_values;
extern CResourceManager* rm;
extern CGame* smw;


/**************************************
 * MI_ScreenResize Class
 **************************************/

#ifdef _XBOX
MI_ScreenResize::MI_ScreenResize() :
    UI_Control(0, 0)
{
    miText[0] = new MI_Text("Use left joystick to set", 320, 110, 0, 2, 1);
    miText[1] = new MI_Text("upper left corner.", 320, 135, 0, 2, 1);
    miText[2] = new MI_Text("Use right joystick to set", 320, 190, 0, 2, 1);
    miText[3] = new MI_Text("lower right corner.", 320, 215, 0, 2, 1);
    miText[4] = new MI_Text("Press random to snap", 320, 270, 0, 2, 1);
    miText[5] = new MI_Text("to preset screen size.", 320, 295, 0, 2, 1);
    miText[6] = new MI_Text("Press key to exit.", 320, 350, 0, 2, 1);

    fAutoModify = true;
    dPreset = 0.0f;
}

MI_ScreenResize::~MI_ScreenResize()
{
    for (int iText = 0; iText < 7; iText++)
        delete miText[iText];
}

MenuCodeEnum MI_ScreenResize::Modify(bool modify)
{
    fModifying = modify;
    return MENU_CODE_MODIFY_ACCEPTED;
}

MenuCodeEnum MI_ScreenResize::SendInput(CPlayerInput * playerInput)
{
    SDL_Event event;
    bool done = false;

    short iLastJoy1X = 0, iLastJoy1Y = 0, iLastJoy2X = 0, iLastJoy2Y = 0;

    bool resized = false;
    while (!done) {
        game_values.playerInput.ClearPressedKeys(1);

        while (SDL_PollEvent(&event)) {
            game_values.playerInput.Update(event, 1);

            for (int iPlayer = 0; iPlayer < 4; iPlayer++) {
                if (playerInput->outputControls[iPlayer].menu_cancel.fPressed ||
                        playerInput->outputControls[iPlayer].menu_select.fPressed) {
                    done = true;
                    break;
                } else if (playerInput->outputControls[iPlayer].menu_random.fPressed) {
                    if (dPreset * 4.0f == game_values.screenResizeX && dPreset * 3.0f == game_values.screenResizeY &&
                            dPreset * -8.0f == game_values.screenResizeW && dPreset * -6.0f == game_values.screenResizeH) {
                        dPreset += 1.0f;
                        if (dPreset > 21.0f)
                            dPreset = 0.0f;
                    }

                    game_values.screenResizeX = dPreset * 4.0f;
                    game_values.screenResizeY = dPreset * 3.0f;
                    game_values.screenResizeW = dPreset * -8.0f;
                    game_values.screenResizeH = dPreset * -6.0f;
                    resized = true;

                    break;
                }
            }

            if (event.type == SDL_JOYAXISMOTION) {
                if (event.jaxis.axis == 0)
                    iLastJoy1X = event.jaxis.value;
                else if (event.jaxis.axis == 1)
                    iLastJoy1Y = event.jaxis.value;
                else if (event.jaxis.axis == 2)
                    iLastJoy2X = event.jaxis.value;
                else if (event.jaxis.axis == 3)
                    iLastJoy2Y = event.jaxis.value;
            }
        }

        if (iLastJoy1X > JOYSTICK_DEAD_ZONE || iLastJoy1X < -JOYSTICK_DEAD_ZONE) {
            game_values.screenResizeX += (float)iLastJoy1X / 50000.0f;
            resized = true;
        }

        if (iLastJoy1Y > JOYSTICK_DEAD_ZONE || iLastJoy1Y < -JOYSTICK_DEAD_ZONE) {
            game_values.screenResizeY += (float)iLastJoy1Y / 50000.0f;
            resized = true;
        }

        if (iLastJoy2X > JOYSTICK_DEAD_ZONE || iLastJoy2X < -JOYSTICK_DEAD_ZONE) {
            game_values.screenResizeW += (float)iLastJoy2X / 50000.0f;
            resized = true;
        }

        if (iLastJoy2Y > JOYSTICK_DEAD_ZONE || iLastJoy2Y < -JOYSTICK_DEAD_ZONE) {
            game_values.screenResizeH += (float)iLastJoy2Y / 50000.0f;
            resized = true;
        }

        if (resized) {
            resized = false;
            SDL_XBOX_SetScreenStretch(game_values.screenResizeW, game_values.screenResizeH);
            SDL_XBOX_SetScreenPosition(game_values.screenResizeX, game_values.screenResizeY);
        }
    }

    game_values.playerInput.ResetKeys();

    fModifying = false;
    return MENU_CODE_BACK_TO_SCREEN_SETTINGS_MENU;
}

void MI_ScreenResize::Draw()
{
    if (!fShow)
        return;

    for (int iText = 0; iText < 7; iText++)
        miText[iText]->Draw();
}
#endif //_XBOX

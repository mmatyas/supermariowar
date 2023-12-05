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

#ifndef GLOBAL_H
#define GLOBAL_H

#define FILEBUFSIZE 1024

//----------------------------------------------------------------
// this file contains stuff that's needed by every part of smw
//----------------------------------------------------------------

#define TITLESTRING "Super Mario War"
#define VERSIONNUMBER "2.0"

#include "sfx.h"
#include "gfx.h"
#include "path.h"


//----------------- game options all parts of the game need -----------

#include "GlobalConstants.h"

#define ifSoundOnPlay(x) if (game_values.sound) x.play()
#define ifsoundonandreadyplay(x) (x.isready() && game_values.sound ? x.play() : -1)
#define ifSoundOnPlayLoop(x,y) (game_values.sound ? x.playloop(y) : -1)
#define ifsoundonstop(x) (game_values.sound ? x.stop() : void(NULL))
#define ifsoundonpause(x) (game_values.sound ? x.sfx_pause() : void(NULL))
#define ifmusiconplay(x) (game_values.music ? x.play() : -1)

void GetNameFromFileName(char * szName, const char * szFileName, bool fStripAuthor = false);
std::string stripCreatorAndDotMap(const std::string &filename);


//------------- data structures / selfmade include files -------------
#include "GameValues.h" // TODO: include only when necessary
#include "uimenu.h"
#include "Game.h"
#include "uicontrol.h"
#include "MapList.h"
#include "FileList.h"
#include "movingplatform.h"
#include "map.h"
#include "player.h"
#include "object.h"
#include "objectgame.h"
#include "objecthazard.h"
#include "eyecandy.h"
#include "gamemodes.h"
#include "world.h"
#include "TilesetManager.h"
#include "ResourceManager.h"

//------------- global variables / etc -------------
//i know that using so much global variables is ugly, this will change somewhen
//neagix: that somewhen is *NOW*

//gfx stuff
extern SDL_Surface		*screen;		//for gfx
extern SDL_Surface		*blitdest;		//for gfx
extern short			x_shake;
extern short			y_shake;

extern CMap				*g_map;
extern CTilesetManager	*g_tilesetmanager;
extern CPlayer			*list_players[4];
extern short			list_players_cnt;

extern SDL_Joystick		**joysticks;
extern short			joystickcount;

extern CScore			*score[4];

extern short			score_cnt;

extern CObjectContainer noncolcontainer;
extern CObjectContainer objectcontainer[3];

extern CEyecandyContainer eyecandy[3];

extern bool fResumeMusic;


extern FiltersList *filterslist;
extern MapList *maplist;
extern SkinList *skinlist;
extern AnnouncerList *announcerlist;
extern MusicList *musiclist;
extern WorldMusicList *worldmusiclist;
extern GraphicsList *menugraphicspacklist;
extern GraphicsList *worldgraphicspacklist;
extern GraphicsList *gamegraphicspacklist;
extern SoundsList *soundpacklist;
extern TourList *tourlist;
extern WorldList *worldlist;

extern CGameMode * gamemodes[GAMEMODE_LAST];
extern CGM_Bonus * bonushousemode;
extern CGM_Pipe_MiniGame * pipegamemode;
extern CGM_Boss_MiniGame * bossgamemode;
extern CGM_Boxes_MiniGame * boxesgamemode;
extern short currentgamemode;

extern float CapFallingVelocity(float vel);
extern float CapSideVelocity(float vel);

extern int g_iNextNetworkID;
extern int g_iNextMessageID;
extern char szIPString[32];
//extern NetServer netServer;
//extern NetClient netClient;

extern short g_iDefaultPowerupPresets[NUM_POWERUP_PRESETS][NUM_POWERUPS];
extern short g_iCurrentPowerupPresets[NUM_POWERUP_PRESETS][NUM_POWERUPS];

void _load_drawmsg(const std::string& f);
void _load_waitforkey();

void DrawPlatform(short pathtype, TilesetTile ** tiles, short startX, short startY, short endX, short endY, float angle, float radiusX, float radiusY, short iSize, short iPlatformWidth, short iPlatformHeight, bool fDrawPlatform, bool fDrawShadow);
void DrawMapHazard(MapHazard * hazard, short iSize, bool fDrawCenter);

// main game object - over time shall be used to
extern CGame	*smw;
extern CResourceManager *rm;


//TODO:: Move menu settings from game_values to global menu context structure
//menu context (like game_values, but for menu)
/*
struct MenuContext
{

};

extern MenuContext menu_context;
*/

struct MapMusicOverride {
    std::string mapname;
    std::vector<std::string> songs;
};

struct WorldMusicOverride {
    std::string worldname;
    std::string song;
};

extern std::vector<MapMusicOverride*> mapmusicoverrides;
extern std::vector<WorldMusicOverride*> worldmusicoverrides;

extern gv game_values;


#endif // GLOBAL_H

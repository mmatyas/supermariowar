#include "GSGameplay.h"

#include "eyecandy.h"
#include "FileList.h"
#include "GameMode.h"
#include "GameValues.h"
#include "GSMenu.h"
#include "net.h"
#include "map.h"
#include "MapList.h"
#include "ObjectContainer.h"
#include "objecthazard.h"
#include "player.h"
#include "RandomNumberGenerator.h"
#include "ResourceManager.h"
#include "Score.h"
#include "sfx.h"
#include "WorldTourStop.h"
#include "gamemodes/Chicken.h"
#include "gamemodes/MiniBoss.h"
#include "gamemodes/Tag.h"
#include "objects/MysteryMushroomTempPlayer.h"
#include "objects/carriable/CO_KuriboShoe.h"
#include "objects/carriable/CO_Shell.h"
#include "objects/carriable/CO_Spike.h"
#include "objects/carriable/CO_ThrowBlock.h"
#include "objects/carriable/CO_ThrowBox.h"
#include "objects/blocks/BounceBlock.h"
#include "objects/blocks/BreakableBlock.h"
#include "objects/blocks/DonutBlock.h"
#include "objects/blocks/FlipBlock.h"
#include "objects/blocks/NoteBlock.h"
#include "objects/blocks/OnOffSwitchBlock.h"
#include "objects/blocks/PowerupBlock.h"
#include "objects/blocks/SwitchBlock.h"
#include "objects/blocks/ThrowBlock.h"
#include "objects/blocks/ViewBlock.h"
#include "objects/blocks/WeaponBreakableBlock.h"
#include "objects/moving/MO_BulletBill.h"
#include "objects/moving/MO_Coin.h"
#include "objects/moving/MO_PirhanaPlant.h"
#include "objects/powerup/PU_BobombPowerup.h"
#include "objects/powerup/PU_BombPowerup.h"
#include "objects/powerup/PU_BoomerangPowerup.h"
#include "objects/powerup/PU_BulletBillPowerup.h"
#include "objects/powerup/PU_ClockPowerup.h"
#include "objects/powerup/PU_ExtraGuyPowerup.h"
#include "objects/powerup/PU_FeatherPowerup.h"
#include "objects/powerup/PU_FirePowerup.h"
#include "objects/powerup/PU_HammerPowerup.h"
#include "objects/powerup/PU_IceWandPowerup.h"
#include "objects/powerup/PU_LeafPowerup.h"
#include "objects/powerup/PU_ModPowerup.h"
#include "objects/powerup/PU_MysteryMushroomPowerup.h"
#include "objects/powerup/PU_PodoboPowerup.h"
#include "objects/powerup/PU_PoisonPowerup.h"
#include "objects/powerup/PU_PowPowerup.h"
#include "objects/powerup/PU_PWingsPowerup.h"
#include "objects/powerup/PU_StarPowerup.h"
#include "objects/powerup/PU_Tanooki.h"
#include "objects/walkingenemy/WalkingEnemy.h"

#include <algorithm>
#include <cmath>
#include <cassert>
#include <cstring>

#define COUNTDOWN_START_INDEX 4

extern SDL_Surface* screen;
extern SDL_Surface* blitdest;

extern bool fResumeMusic;
extern short g_iCurrentDrawIndex;
extern CPlayer * GetPlayerFromGlobalID(short iGlobalID);

extern CGM_Boss_MiniGame * bossgamemode;
extern short currentgamemode;

extern CMap* g_map;

extern std::vector<CPlayer*> players;
extern CScore* score[4];
extern short score_cnt;

extern MapList *maplist;
extern MusicList *musiclist;

extern CGameValues game_values;
extern CResourceManager* rm;

extern short x_shake;
extern short y_shake;

CObjectContainer noncolcontainer;
CObjectContainer objectcontainer[3];

CEyecandyContainer eyecandy[3];
SpotlightManager spotlightManager;

short g_iWinningPlayer;
short scorepowerupoffsets[3][3] = { {  37,  0,    0 },
                                    {  71,  89,   0 },
                                    { 105, 123, 141 } };

short GameplayState::scoreoffsets[3] = {2, 36, 70};
short GameplayState::g_iPowerupToIcon[8] = {80, 176, 272, 304, 336, 368, 384, 400};

GameplayState::GameplayState()
{
    for (short p = 0; p < 4; p++) {
        respawnCount[p] = 0;
        respawnanimationtimer[p] = 0;
        respawnanimationframe[p] = 0;
    }


    //Vars that keep track of spinning the screen
    spinangle = 0.0f;
    spinspeed = 0.0f;
    spindirection = 1;
    spintimer = 0;
}

GameplayState& GameplayState::instance() {
    static GameplayState gps;
    return gps;
}

//-----------------------------------------------------------------------------
// THE GAME LOOP
//-----------------------------------------------------------------------------

extern void SetupScoreBoard(bool fOrderMatters);
extern void ShowScoreBoard();

constexpr SDL_Rect iCountDownNumbers[4][4][2] =
    {   {{{0, 0, 64, 64},{288, 208, 64, 64}},
        {{0, 64, 48, 48},{296, 216, 48, 48}},
        {{192, 64, 32, 32},{304, 224, 32, 32}},
        {{0, 112, 16, 16},{312, 232, 16, 16}}
    },

    {   {{64, 0, 64, 64},{288, 208, 64, 64}},
        {{48, 64, 48, 48},{296, 216, 48, 48}},
        {{224, 64, 32, 32},{304, 224, 32, 32}},
        {{16, 112, 16, 16},{312, 232, 16, 16}}
    },

    {   {{128, 0, 64, 64},{288, 208, 64, 64}},
        {{96, 64, 48, 48},{296, 216, 48, 48}},
        {{192, 96, 32, 32},{304, 224, 32, 32}},
        {{32, 112, 16, 16},{312, 232, 16, 16}}
    },

    {   {{192, 0, 64, 64},{288, 208, 64, 64}},
        {{144, 64, 48, 48},{296, 216, 48, 48}},
        {{224, 96, 32, 32},{304, 224, 32, 32}},
        {{48, 112, 16, 16},{312, 232, 16, 16}}
    }
};

constexpr short iCountDownTimes[28] = {3, 3, 3, 15, 3, 3, 3, 3, 3, 3, 15, 3, 3, 3, 3, 3, 3, 15, 3, 3, 3, 3, 3, 3, 45, 3, 3, 3};
constexpr short iCountDownRectSize[28] = {3, 2, 1, 0, 1, 2, 3, 3, 2, 1, 0, 1, 2, 3, 3, 2, 1, 0, 1, 2, 3, 3, 2, 1, 0, 1, 2, 3};
constexpr short iCountDownRectGroup[28] = {0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3};
constexpr short iCountDownAnnounce[28] = {-1, -1, -1, 12, -1, -1, -1, -1, -1, -1, 13, -1, -1, -1, -1, -1, -1, 14, -1, -1, -1, -1, -1, -1, 15, -1, -1, -1};

//TODO  - Review what is colliding with what and remove duplicates (i.e. shell vs. throwblock and throwblock vs. shell should only detect one way)
constexpr short g_iCollisionMap[MOVINGOBJECT_LAST][MOVINGOBJECT_LAST] = {
//   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //movingobject_none = 0
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //movingobject_powerup = 1
    {0,0,0,1,1,0,0,1,1,0,0,0,1,1,0,0,0,0,0,0,0,1,0,1,1,0,0,0,0,0,1,0,0,1}, //movingobject_fireball = 2
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //movingobject_goomba = 3
    {0,0,0,1,1,0,0,0,0,0,0,0,1,1,0,0,0,0,1,0,0,0,0,1,1,0,0,0,0,0,1,0,0,0}, //movingobject_bulletbill = 4
    {0,0,0,1,1,0,0,1,1,0,0,0,1,1,0,0,0,0,0,0,0,1,0,1,1,0,0,0,0,0,1,0,0,1}, //movingobject_hammer = 5
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //movingobject_poisonpowerup = 6
    {0,0,0,1,1,0,0,1,0,0,0,0,1,1,0,0,0,0,1,0,1,1,0,1,1,0,0,0,0,0,1,0,0,0}, //movingobject_shell = 7
    {0,0,0,1,1,0,0,1,1,0,0,0,1,1,0,0,0,0,1,0,1,1,0,1,1,0,0,0,0,0,1,0,0,0}, //movingobject_throwblock = 8
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0}, //movingobject_egg = 9
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //movingobject_star = 10
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0}, //movingobject_flag = 11
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //movingobject_cheepcheep = 12
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //movingobject_koopa = 13
    {0,0,0,1,1,0,0,1,1,0,0,0,1,1,0,0,0,0,0,0,0,1,0,1,1,0,0,0,0,0,1,0,0,1}, //movingobject_boomerang = 14
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //movingobject_carried = 15
    {0,0,0,1,0,0,0,1,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,1}, //movingobject_iceblast = 16
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //movingobject_bomb = 17
    {0,0,0,1,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0}, //movingobject_podobo = 18
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //movingobject_treasurechest = 19
    {0,0,0,1,1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,0,1,1,0,0,0,0,0,1,0,0,0}, //movingobject_attackzone = 20
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //movingobject_pirhanaplant = 21
    {0,0,0,1,1,0,0,1,1,0,0,0,1,1,0,0,0,0,0,0,0,1,0,1,1,0,0,0,0,0,1,0,0,1}, //movingobject_explosion = 22
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //movingobject_buzzybeetle = 23
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //movingobject_spiny = 24
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //movingobject_phantokey = 25
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //movingobject_flagbase = 26
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //movingobject_yoshi = 27
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //movingobject_coin = 28
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //movingobject_collectioncard = 29
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //movingobject_sledgebrother = 30
    {0,0,0,1,1,0,0,1,1,0,0,0,1,1,0,0,0,0,0,0,0,1,0,1,1,0,0,0,0,0,0,0,0,1}, //movingobject_sledgehammer = 31
    {0,0,0,1,1,0,0,1,1,0,0,0,1,1,0,0,0,0,0,0,0,1,0,1,1,0,0,0,0,0,0,0,0,1}, //movingobject_superfireball = 32
    {0,0,0,1,1,0,0,1,1,0,0,0,1,1,0,0,0,0,0,0,1,1,0,1,1,0,0,0,0,0,1,0,0,1}, //movingobject_throwbox = 33
};


#ifdef _DEBUG
short iSledgeBrotherSetting = 0;
short iSledgeBrotherAttribute = 0;

bool g_fAutoTest = false;
bool g_fRecordTest = false;
static int exitgametimer = 0;
#endif

bool coldec_player2player(CPlayer * o1, CPlayer * o2);
bool coldec_player2obj(CPlayer * o1, CObject * o2);
bool coldec_obj2obj(CObject * o1, CObject * o2);

//handles a collision between a powerup and an object
void collisionhandler_o2o(IO_MovingObject * o1, IO_MovingObject * o2)
{
    o2->collide(o1);
}


//Must only be called after organizeteams() is called
short LookupTeamID(short id, short * teamID, short * subTeamID)
{
    for (short i = 0; i < score_cnt; i++) {
        for (short j = 0; j < game_values.teamcounts[i]; j++) {
            if (game_values.teamids[i][j] == id) {
                if (teamID)
                    *teamID = i;

                if (subTeamID)
                    *subTeamID = j;

                return i;
            }
        }
    }

    if (teamID)
        *teamID = -1;

    if (subTeamID)
        *subTeamID = -1;

    return -1;
}

short LookupTeamID(short id)
{
    return LookupTeamID(id, NULL, NULL);
}

void SDLCALL musicfinished()
{
    if (!game_values.music)
        return;

    if (game_values.appstate == AppState::Game && !game_values.gamemode->gameover)
        return;

    if (fResumeMusic)
        rm->backgroundmusic[3].play(false, false);
}

short GetModeIconIndexFromMode(short iMode)
{
    if (iMode == game_mode_pipe_minigame)
        iMode = 25;
    else if (iMode == game_mode_boss_minigame)
        iMode = 26;
    else if (iMode == game_mode_boxes_minigame)
        iMode = 27;

    return iMode;
}

//
// INIT
//
void GameplayState::createPlayers()
{
    //Create players for this game
    for (short iPlayer = 0; iPlayer < 4; iPlayer++) {
        respawnCount[iPlayer] = 0;

        if (game_values.singleplayermode == -1 || game_values.singleplayermode == iPlayer) {
            if (game_values.playercontrol[iPlayer] > 0) {
                short teamid, subteamid;
                LookupTeamID(iPlayer, &teamid, &subteamid);

                CPlayerAI * ai = NULL;
                if (game_values.playercontrol[iPlayer] == 2)
                    ai = new CPlayerAI();

                players.emplace_back(new CPlayer(iPlayer, players.size(), teamid, subteamid, game_values.colorids[iPlayer], rm->spr_player[iPlayer], score[teamid], &(respawnCount[iPlayer]), ai));
            } else if (!game_values.keeppowerup) {
                //Reset off player's stored powerups if they are not playing
                game_values.storedpowerups[iPlayer] = -1;
            }
        }

        //If the gamemode allows stored powerups, then assign the game stored slot to the powerup this player has
        if (game_values.gamemode->HasStoredPowerups())
            game_values.gamepowerups[iPlayer] = game_values.storedpowerups[iPlayer];
        else {
            game_values.gamepowerups[iPlayer] = -1;
        }

        game_values.bulletbilltimer[iPlayer] = 0;
        game_values.bulletbillspawntimer[iPlayer] = 0;
    }
}

void GameplayState::initScoreDisplayPosition()
{
    short i;
    short totalspace = 0;
    for (i = 0; i < score_cnt; i++) {
        totalspace += 56 + game_values.teamcounts[i] * 34;
    }
    totalspace += 20 * (score_cnt - 1);

    for (i = 0; i < score_cnt; i++) {
        if (game_values.scoreboardstyle == ScoreboardStyle::Top || game_values.scoreboardstyle == ScoreboardStyle::Bottom) {
            score[i]->x = ((App::screenWidth - totalspace) >> 1);

            for (short k = 0; k < i; k++)
                score[i]->x += 76 + game_values.teamcounts[k] * 34;

            short iScoreOffsetY = 0;
            if (game_values.gamemode->gamemode == game_mode_health || game_values.gamemode->gamemode == game_mode_collection)
                iScoreOffsetY = 18;

            score[i]->y = 5 + (game_values.scoreboardstyle == ScoreboardStyle::Bottom ? 429 - iScoreOffsetY : 0);
        } else {
            short iScoreOffsetY = 0;
            if (game_values.gamemode->gamemode == game_mode_health || game_values.gamemode->gamemode == game_mode_collection)
                iScoreOffsetY = 18;

            score[i]->x = 5 + (574 - (34 * game_values.teamcounts[i])) * (i % 2);
            score[i]->y = 5 + (429 - iScoreOffsetY) * (i > 1 ? 1 : 0);
        }

        score[i]->fromx = score[i]->x;
        score[i]->fromy = score[i]->y;
        score[i]->place = i;
        score[i]->order = -1;
    }
}

void GameplayState::initEyeCandy()
{
    short i;
    for (short iEyeCandyLayer = 0; iEyeCandyLayer < 3; iEyeCandyLayer++) {
        //Clouds
        if (g_map->eyecandy[iEyeCandyLayer] & 1) {
            for (i = 0; i < 4; i++) {
                float velx;         //speed of cloud, small clouds are slower than big ones
                short srcy, w, h;

                if (RANDOM_BOOL()) {
                    velx = (short)(RANDOM_INT(51) - 25) / 10.0f;    //big clouds: -3 - +3 pixel/frame
                    srcy = 0;
                    w = 60;
                    h = 28;
                } else {
                    velx = (short)(RANDOM_INT(41) - 20) / 10.0f;    //small clouds: -2 - +2 pixel/frame
                    srcy = 28;
                    w = 28;
                    h = 12;
                }

                velx = velx < 0.5f && velx > -0.5f ? 1 : velx;  //no static clouds please

                //add cloud to eyecandy array
                eyecandy[iEyeCandyLayer].add(new EC_Cloud(&rm->spr_clouds, (float)(RANDOM_INT(App::screenWidth)), (float)(RANDOM_INT(100)), velx, 0, srcy, w, h));
            }
        }

        //Ghosts
        if (g_map->eyecandy[iEyeCandyLayer] & 2) {
            for (i = 0; i < 8; i++) {
                short iGhostSrcY = (short)(RANDOM_INT(3)) << 5; //ghost type
                float velx = (short)(RANDOM_INT(51) - 25) / 10.0f;  //big clouds: -3 - +3 pixel/frame

                velx = velx < 0.5f && velx > -0.5f ? (RANDOM_INT(1) ? 1.0f : -1.0f) : velx; //no static clouds please

                //add cloud to eyecandy array
                eyecandy[iEyeCandyLayer].add(new EC_Ghost(&rm->spr_ghosts, (float)(RANDOM_INT(App::screenWidth)), (float)RANDOM_INT(100), velx, 8, 2, velx < 0.0f ? 64 : 0, iGhostSrcY, 32, 32));
            }
        }

        //Leaves
        if (g_map->eyecandy[iEyeCandyLayer] & 4) {
            for (i = 0; i < 15; i++)
                eyecandy[iEyeCandyLayer].add(new EC_Leaf(&rm->spr_leaves, (float)(RANDOM_INT(App::screenWidth)), (float)RANDOM_INT(App::screenHeight)));
        }

        //Snow
        if (g_map->eyecandy[iEyeCandyLayer] & 8) {
            for (i = 0; i < 15; i++)
                eyecandy[iEyeCandyLayer].add(new EC_Snow(&rm->spr_snow, (float)(RANDOM_INT(App::screenWidth)), (float)RANDOM_INT(App::screenHeight), 0));
        }

        //Fish
        short iFishWeights[] = {20, 20, 15, 10, 10, 5, 10, 10};
        short iFishSettings[][4] = { {0, 0, 64, 44}, {0, 44, 64, 44}, {0, 44, 48, 44}, {32, 32, 16, 12}, {32, 44, 16, 12}, {32, 16, 16, 28}, {32, 0, 32, 28}, {32, 44, 32, 28}};
        if (g_map->eyecandy[iEyeCandyLayer] & 16) {
            for (i = 0; i < 8; i++) {
                float velx = (short)(RANDOM_INT(41) - 20) / 10.0f;
                velx = velx < 0.5f && velx > -0.5f ? 1.0f : velx; //Keep fish from moving too slowly

                short srcx = iFishSettings[0][0], srcy = iFishSettings[0][1], w = iFishSettings[0][2], h = iFishSettings[0][3];

                short iRandomFish = RANDOM_INT(100);

                short iFishWeightCount = 0;
                for (short iFish = 0; iFish < 8; iFish++) {
                    iFishWeightCount += iFishWeights[iFish];

                    if (iRandomFish < iFishWeightCount) {
                        srcx = iFishSettings[iFish][0];
                        srcy = iFishSettings[iFish][1];
                        w = iFishSettings[iFish][2];
                        h = iFishSettings[iFish][3];
                        break;
                    }
                }

                //add cloud to eyecandy array
                short iPossibleY = (App::screenHeight - h) / 10;
                float dDestY = (float)(RANDOM_INT(iPossibleY) + iPossibleY * i);
                eyecandy[iEyeCandyLayer].add(new EC_Cloud(&rm->spr_fish, (float)(RANDOM_INT(App::screenWidth)), dDestY, velx, srcx + (velx > 0.0f ? 64 : 0), srcy, w, h));
            }
        }

        //Rain
        if (g_map->eyecandy[iEyeCandyLayer] & 32) {
            for (i = 0; i < 20; i++)
                eyecandy[iEyeCandyLayer].add(new EC_Rain(&rm->spr_rain, (float)(RANDOM_INT(App::screenWidth)), RANDOM_INT(App::screenHeight)));
        }

        //Bubbles
        if (g_map->eyecandy[iEyeCandyLayer] & 64) {
            for (i = 0; i < 10; i++)
                eyecandy[iEyeCandyLayer].add(new EC_Bubble(&rm->spr_rain, (float)(RANDOM_INT(App::screenWidth)), RANDOM_INT(App::screenHeight)));
        }
    }
}

void GameplayState::initRunGame()
{
    y_shake = 0;
    x_shake = 0;

    //Reset the screen spin variables
    spinangle = 0.0f;
    spinspeed = 0.0f;
    spindirection = 1;
    spintimer = 0;

    game_values.resetSecretCounters();

    //Reset the keys each time we switch from menu to game and back
    game_values.playerInput.ResetKeys();


    createPlayers();
    game_values.resetGameplaySettings();
    initScoreDisplayPosition();
    initEyeCandy();


    iWindTimer = 0;
    dNextWind = (float)(RANDOM_INT(41) - 20) / 4.0f;
    game_values.flags.gamewindx = (float)((RANDOM_INT(41)) - 20) / 4.0f;

    //Initialize players after game init has finished
    for (CPlayer* player : players)
        player->Init();
}

//
// RUNNING
//

short CountAliveTeams(short * lastteam)
{
    short findlastteam = 0;

    bool teamalive[4] = {false, false, false, false};
    for (CPlayer* player : players) {
        if (!player->isdead())
            teamalive[player->teamID] = true;
    }

    short numteams = 0;
    for (short k = 0; k < 4; k++) {
        if (teamalive[k]) {
            findlastteam = k;
            numteams++;
        }
    }

    if (lastteam != NULL) {
        if (numteams == 1)
            *lastteam = findlastteam;
        else
            *lastteam = -1;
    }

    return numteams;
}

void GameplayState::CleanDeadPlayers()
{
    bool fCheckForGameOver = false;

    auto* gmChicken = dynamic_cast<CGM_Chicken*>(game_values.gamemode);
    auto* gmTag = dynamic_cast<CGM_Tag*>(game_values.gamemode);

    for (size_t i = 0; i < players.size(); i++) {
        if (players[i]->state == PlayerState::Dead) {
            fCheckForGameOver = true;

            if (respawnCount[players[i]->globalID] <= 0)
                players[i]->die(PlayerDeathStyle::Jump, true, false);

            //Set this to zero so we don't display a spawn egg when player is removed from game
            respawnCount[players[i]->globalID] = 0;

            if (gmTag && gmTag->tagged() == players[i])
                gmTag->setTagged(nullptr);

            if (gmChicken && gmChicken->chicken() == players[i])
                gmChicken->clearChicken();

            delete players[i];

            for (size_t j = i; j < players.size() - 1; j++) {
                players[j] = players[j + 1];
                players[j]->localID = j;
            }

            players.pop_back();
        }
    }

    if (fCheckForGameOver && game_values.gamemode->gamemode != game_mode_bonus && game_values.gamemode->gamemode != game_mode_boss_minigame) {
        short lastteam = -1;
        if (!game_values.gamemode->gameover && CountAliveTeams(&lastteam) <= 1) {
            game_values.gamemode->gameover = true;
            game_values.gamemode->winningteam = lastteam;
            SetupScoreBoard(true);  //pass true because the order the players died in matters
            ShowScoreBoard();
        }
    }
}

void checkWindEvent(short& iWindTimer, float& dNextWind)
{
    if (iWindTimer <= 0) {
        //Then trigger next wind event
        if (game_values.flags.gamewindx < dNextWind) {
            game_values.flags.gamewindx += 0.02f;

            if (game_values.flags.gamewindx >= dNextWind)
                iWindTimer = (RANDOM_INT(60)) + 30;
        } else if (game_values.flags.gamewindx >= dNextWind) {
            game_values.flags.gamewindx -= 0.02f;

            if (game_values.flags.gamewindx <= dNextWind)
                iWindTimer = (RANDOM_INT(60)) + 30;
        }
    } else {
        if (--iWindTimer <= 0) {
            dNextWind = (float)((RANDOM_INT(41)) - 20) / 4.0f;
        }
    }
}

void cleanDeadNonPlayerObjects()
{
    eyecandy[0].cleanDeadObjects();
    eyecandy[1].cleanDeadObjects();
    eyecandy[2].cleanDeadObjects();

    objectcontainer[2].cleanDeadObjects();
    objectcontainer[1].cleanDeadObjects();
    objectcontainer[0].cleanDeadObjects();
    noncolcontainer.cleanDeadObjects();
}

void animateDuringCountdown()
{
    //Move platforms
    g_map->updatePlatforms();

    cleanDeadNonPlayerObjects();

    //Keep updating map hazards
    noncolcontainer.update();
    objectcontainer[0].update();
    objectcontainer[1].update();
    objectcontainer[2].update();

    eyecandy[0].update();
    eyecandy[1].update();
    eyecandy[2].update();

    g_map->update();
}

void shakeScreen()
{
    if (game_values.flags.screenshaketimer <= 0) {
        //Make sure we zero out the shake value after it is done
        x_shake = 0;
        return;
    }

    game_values.flags.screenshaketimer--;

    static bool shakeleft = false;
    if (shakeleft) {
        x_shake -= 2;
        if (x_shake <= -2) {
            shakeleft = false;
        }
    } else {
        x_shake += 2;
        if (x_shake >= 2) {
            shakeleft = true;
        }
    }

    //Kill players touching the ground (or in air for MOd blocks)
    short iNumKillPlayers = 0;
    CPlayer * pKillPlayers[4];

    CPlayer * killer1 = GetPlayerFromGlobalID(game_values.flags.screenshakeplayerid);

    for (CPlayer* player : players) {
        //Don't kill the player that triggered the POW/MOd
        if (player->globalID == game_values.flags.screenshakeplayerid)
            continue;

        //Don't kill players on his team either (if friendly fire is off)
        if (game_values.teamcollision != TeamCollisionStyle::On && game_values.flags.screenshaketeamid == player->teamID)
            continue;

        //Kill other players
        if (!player->IsInvincibleOnBottom() && player->isready()) {
            if (game_values.flags.screenshakekillinair == player->inair) {
                pKillPlayers[iNumKillPlayers++] = player;

                if (killer1) {
                    game_values.flags.screenshakekillscount++;

                    if (killer1->inair)
                        killer1->killsinrowinair--;  //Don't want to give both shake and in air award
                }
            }
        }
    }

    //Randomize the order in which the players are killed (so that game modes where order matters is fair)
    if (iNumKillPlayers > 0) {
        short iRandPlayer = RANDOM_INT( iNumKillPlayers);
        for (short iPlayer = 0; iPlayer < iNumKillPlayers; iPlayer++) {
            PlayerKilledPlayer(game_values.flags.screenshakeplayerid, pKillPlayers[iRandPlayer], PlayerDeathStyle::Jump, KillStyle::Pow, false, false);

            if (++iRandPlayer >= iNumKillPlayers)
                iRandPlayer = 0;
        }
    }

    //Kill goombas and koopas
    for (const std::unique_ptr<CObject>& obj : objectcontainer[0].list()) {
        if (auto* movingobject = dynamic_cast<IO_MovingObject*>(obj.get())) {
            MovingObjectType type = movingobject->getMovingObjectType();

            if ((type == movingobject_goomba || type == movingobject_koopa || type == movingobject_buzzybeetle || type == movingobject_spiny)
                    && game_values.flags.screenshakekillinair == movingobject->inair) {
                CPlayer * killer = GetPlayerFromGlobalID(game_values.flags.screenshakeplayerid);

                if (killer) {
                    if (!game_values.gamemode->gameover)
                        killer->score->AdjustScore(1);

                    ifSoundOnPlay(rm->sfx_kicksound);
                    ((MO_WalkingEnemy*)movingobject)->DieAndDropShell(true, true);

                    game_values.flags.screenshakekillscount++;

                    if (killer->inair)
                        killer->killsinrowinair--;  //Don't want to give both shake and in air award
                }
            }
        }
    }

    //Destroy throw blocks and flip shells over
    for (const std::unique_ptr<CObject>& obj : objectcontainer[1].list()) {
        if (auto* movingobject = dynamic_cast<IO_MovingObject*>(obj.get())) {
            if (game_values.flags.screenshakekillinair == movingobject->inair) {
                if (movingobject->getMovingObjectType() == movingobject_shell) {
                    CO_Shell * shell = (CO_Shell*)movingobject;
                    if (shell->frozen || !shell->owner || shell->owner->inair == game_values.flags.screenshakekillinair)
                        shell->Flip();  //also breaks shells if frozen
                } else if (movingobject->getMovingObjectType() == movingobject_throwblock) {
                    CO_ThrowBlock * throwblock = (CO_ThrowBlock*)movingobject;
                    if (throwblock->frozen || !throwblock->owner || throwblock->owner->inair == game_values.flags.screenshakekillinair)
                        throwblock->Die();
                } else if (movingobject->getMovingObjectType() == movingobject_throwbox) {
                    CO_ThrowBox * throwbox = (CO_ThrowBox*)movingobject;
                    if (throwbox->frozen)
                        throwbox->Die();
                } else if (movingobject->getMovingObjectType() == movingobject_pirhanaplant) {
                    MO_PirhanaPlant * plant = (MO_PirhanaPlant*)movingobject;
                    plant->KillPlant();
                } else if (movingobject->getMovingObjectType() == movingobject_bulletbill) {
                    MO_BulletBill * bulletbill = (MO_BulletBill*)movingobject;
                    bulletbill->Die();
                }
            }
        }
    }

    //Add kills in row for kills from pow and mod
    if (game_values.flags.screenshakekillscount > 1 && game_values.awardstyle != AwardStyle::None) {
        game_values.flags.screenshakekillscount = 0;

        CPlayer * killer2 = GetPlayerFromGlobalID(game_values.flags.screenshakeplayerid);

        if (killer2)
            killer2->AddKillsInRowInAirAward();
    }
}

//Move the screen in a small circle
void GameplayState::spinScreen()
{
    if (!game_values.spinscreen)
        return;

    if (spindirection == 0 || spindirection == 2) {
        if (++spintimer >= 300) {
            spindirection++;
            spintimer = 0;
        }
    } else if (spindirection == 1) {
        spinspeed += 0.0008f;

        if (spinspeed >= 0.05f) {
            spinspeed = 0.05f;
            spindirection++;
        }
    } else {
        spinspeed -= 0.0008f;

        if (spinspeed <= -0.05f) {
            spinspeed = -0.05f;
            spindirection = 0;
        }
    }

    spinangle += spinspeed;

    if (spinangle >= TWO_PI) {
        spinangle -= TWO_PI;
    } else if (spinangle < 0.0f) {
        spinangle += TWO_PI;
    }

    float shakey = spinspeed * App::screenWidth * sin(spinangle);
    if (shakey < 0.0f)
        shakey -= 1.0f;

    x_shake = (short)(spinspeed * App::screenWidth * cos(spinangle));
    y_shake = (short)(shakey);
}

void handleP2PCollisions()
{
    //Player to player collisions
    size_t i, j;
    for (i = 0; i < players.size(); i++) {
        CPlayer* player1 = players[i];
        assert(player1);
        if (player1->state > PlayerState::Dead) {
            for (j = i + 1; j < players.size(); j++) {
                CPlayer* player2 = players[j];
                assert(player2);
                if (player2->state > PlayerState::Dead) {
                    if (coldec_player2player(player1, player2)) {
                        if (netplay.active)
                            netplay.client.local_gamehost.sendP2PCollisionEvent(*player1, *player2);

                        player1->collidesWith(player2);

                        //if player was killed by another player, continue with next player for collision detection
                        if (player1->state <= PlayerState::Dead)
                            break;
                    }
                }
            }
        }
    }
}

void handleP2ObjCollisions()
{
    //Collide player with objects
    for (CPlayer* player : players) {
        if (player->state != PlayerState::Ready)
            continue;

        //Collide with objects
        for (short iLayer = 0; iLayer < 3; iLayer++) {
            for (const std::unique_ptr<CObject>& obj : objectcontainer[iLayer].list()) {
                if (!obj->isDead()) {
                    if (coldec_player2obj(player, obj.get())) {
                        if (player->collidesWith(obj.get()))
                            break;
                    }
                }
            }

            //if the object killed the player, then continue with the other players
            if (player->state != PlayerState::Ready)
                break;

            //If player collided with a swap mushroom, the break from colliding with everything else
            if (game_values.flags.swapplayers)
                return;
        }
    }
}

void handleObj2ObjCollisions()
{
    for (size_t iLayer1 = 0; iLayer1 < 3; iLayer1++) {
        size_t iContainerEnd1 = objectcontainer[iLayer1].list().size();
        for (size_t iObject1 = 0; iObject1 < iContainerEnd1; iObject1++) {
            CObject * object1 = objectcontainer[iLayer1].list()[iObject1].get();

            auto* movingobject1 = dynamic_cast<IO_MovingObject*>(object1);
            if (!movingobject1)
                continue;

            for (size_t iLayer2 = iLayer1; iLayer2 < 3; iLayer2++) {
                size_t iContainerEnd2 = objectcontainer[iLayer2].list().size();
                for (size_t iObject2 = (iLayer1 == iLayer2 ? iObject1 + 1 : 0); iObject2 < iContainerEnd2; iObject2++) {
                    CObject * object2 = objectcontainer[iLayer2].list()[iObject2].get();

                    if (object2->isDead())
                        continue;

                    auto* movingobject2 = dynamic_cast<IO_MovingObject*>(object2);
                    if (!movingobject2)
                        continue;

                    //if (g_iCollisionMap[movingobject1->getMovingObjectType()][movingobject2->getMovingObjectType()])
                    //  continue;

                    //if (iLayer1 == iLayer2 && iObject1 == iObject2)
                    //  continue;

                    MovingObjectType iType1 = movingobject1->getMovingObjectType();
                    MovingObjectType iType2 = movingobject2->getMovingObjectType();
                    if (g_iCollisionMap[iType1][iType2]) {
                        if (coldec_obj2obj(movingobject1, movingobject2)) {
                            collisionhandler_o2o(movingobject1, movingobject2);
                        }
                    } else if (g_iCollisionMap[iType2][iType1]) {
                        if (coldec_obj2obj(movingobject2, movingobject1)) {
                            collisionhandler_o2o(movingobject2, movingobject1);
                        }
                    }

                    if (object1->isDead())
                        return; // CONTINUEOBJECT1
                }
            }
        }
    }
}

void GameplayState::drawScoreboard(short iScoreTextOffset[4])
{
    g_iWinningPlayer = -1;

    //Draw scoreboards for all games (except special cases where we have a single player walking the map)
    short i;
    if (game_values.singleplayermode == -1) {
        short highestScore = 0;

        bool fReverseScoring = game_values.gamemode->GetReverseScoring();
        if (fReverseScoring)
            highestScore = 32000;

        for (i = 0; i < score_cnt; i++) {
            int scoreValue = score[i]->score;
            if (game_values.gamemode->gamemode == game_mode_boxes_minigame)
                scoreValue = score[i]->subscore[0];

            if ((scoreValue > highestScore && !fReverseScoring) || (scoreValue < highestScore && fReverseScoring)) {
                highestScore = scoreValue;
                g_iWinningPlayer = i;
            } else if (scoreValue == highestScore) {
                g_iWinningPlayer = -1;
            }
        }

        //big end game scoreboard (sorted)
        if (game_values.flags.showscoreboard) {
            char gameovertext[128] = "";
            if (game_values.gamemode->winningteam > -1) {
                if (game_values.teamcounts[game_values.gamemode->winningteam] == 1)
                    sprintf(gameovertext, "Player %d Wins!", game_values.teamids[game_values.gamemode->winningteam][0] + 1);
                else
                    sprintf(gameovertext, "Team %d Wins!", game_values.gamemode->winningteam + 1);
            } else {
                sprintf(gameovertext, "Tie Game");
            }

            rm->game_font_large.drawCentered(App::screenWidth/2, 90, gameovertext);

        }

        //in game scoreboards
        for (i = 0; i < score_cnt; i++) {
            if (game_values.gamemode->gamemode == game_mode_health || game_values.gamemode->gamemode == game_mode_collection || game_values.gamemode->gamemode == game_mode_boxes_minigame)
                rm->spr_shade[game_values.teamcounts[i] - 1].draw(score[i]->x, score[i]->y);
            else
                rm->spr_shade[game_values.teamcounts[i] - 1].draw(score[i]->x, score[i]->y, 0, 0, 256, 41);

            for (short k = 0; k < game_values.teamcounts[i]; k++) {
                short globalID = game_values.teamids[i][k];

                //If player is respawning, draw an animated egg counter
                if (respawnCount[globalID] > 0 && !game_values.gamemode->gameover) {
                    if (++respawnanimationtimer[globalID] > 8) {
                        respawnanimationtimer[globalID] = 0;
                        respawnanimationframe[globalID] += 32;

                        if (respawnanimationframe[globalID] > 32)
                            respawnanimationframe[globalID] = 0;
                    }

                    short scorex = score[i]->x + scoreoffsets[k];
                    short scorey = score[i]->y + 2;
                    rm->spr_egg.draw(scorex, scorey, respawnanimationframe[globalID], game_values.colorids[globalID] << 5, 32, 32);
                    rm->spr_eggnumbers.draw(scorex, scorey, ((respawnCount[globalID] - 1) >> 1) << 5, game_values.colorids[globalID] << 5, 32, 32);
                } else { //otherwise draw the player's skin in the scoreboard
                    short iScoreboardSprite;
                    if (game_values.gamemode->gameover) {
                        if (g_iWinningPlayer != i)
                            iScoreboardSprite = PGFX_DEADFLYING;
                        else
                            iScoreboardSprite = PGFX_JUMPING_R;
                    } else {
                        iScoreboardSprite = PGFX_STANDING_R;
                    }

                    //Search for player state to display
                    CPlayer * player = GetPlayerFromGlobalID(globalID);

                    if (player && !game_values.gamemode->gameover) {
                        short iScoreOffsetX = score[i]->x + scoreoffsets[k];
                        short iScoreOffsetY = score[i]->y + 2;

                        if (player->ownerPlayerID > -1)
                            rm->spr_ownedtags.draw(iScoreOffsetX - 8, iScoreOffsetY - 8, player->ownerColorOffsetX, 0, 48, 48);

                        player->GetScoreboardSprite()[iScoreboardSprite]->draw(iScoreOffsetX, iScoreOffsetY, player->iSrcOffsetX, 0, 32, 32);

                        //Display jail if player is jailed
                        if (player->jail.isActive())
                            rm->spr_jail.draw(iScoreOffsetX - 6, iScoreOffsetY - 6, (player->jail.getColor() + 1) * 44, 0, 44, 44);

                        //Display current powerup if player is using one
                        if (player->powerup > 0)
                            rm->spr_storedpowerupsmall.draw(iScoreOffsetX, iScoreOffsetY + 16, g_iPowerupToIcon[player->powerup - 1], 0, 16, 16);

                        //Display tanooki powerup if player has it
                        if (player->tanookisuit.isOn())
                            rm->spr_storedpowerupsmall.draw(iScoreOffsetX + 16, iScoreOffsetY + 16, App::screenWidth/2, 0, 16, 16);
                    } else {
                        rm->spr_player[globalID][iScoreboardSprite]->draw(score[i]->x + scoreoffsets[k], score[i]->y + 2, 0, 0, 32, 32);
                    }

                    //give crown to player(s) with most kills
                    if (g_iWinningPlayer == i)
                        rm->spr_crown.draw(score[i]->x + scoreoffsets[k] + 12, score[i]->y - 4);
                }

                short storedpowerupid = game_values.gamepowerups[globalID];

                //Draw stored powerup
                if (storedpowerupid != -1) {
                    if (!game_values.flags.swapplayers) {
                        rm->spr_storedpowerupsmall.draw(score[i]->x + scorepowerupoffsets[game_values.teamcounts[i] - 1][k], score[i]->y + 25, storedpowerupid * 16, 0, 16, 16);
                    }
                }
            }

            //Draw hearts for health mode
            if (game_values.gamemode->gamemode == game_mode_health) {
                short iLife = score[i]->subscore[0];
                short iMax = score[i]->subscore[1];
                short iHeartX = score[i]->x + scorepowerupoffsets[game_values.teamcounts[i] - 1][0] - 32;

                for (short iHeart = 0; iHeart < iLife; iHeart++) {
                    if (iHeart == iMax - 1 && iHeart % 2 == 0)
                        rm->spr_scorehearts.draw(iHeartX + iHeart * 8, score[i]->y + 43, 32, 0, 8, 16);
                    else
                        rm->spr_scorehearts.draw(iHeartX + iHeart * 8, score[i]->y + 43, (iHeart % 2) ? 8 : 0, 0, 8, 16);
                }

                for (short iHeart = iLife; iHeart < iMax; iHeart++) {
                    if (iHeart == iMax - 1 && iHeart % 2 == 0)
                        rm->spr_scorehearts.draw(iHeartX + iHeart * 8, score[i]->y + 43, 40, 0, 8, 16);
                    else
                        rm->spr_scorehearts.draw(iHeartX + iHeart * 8, score[i]->y + 43, (iHeart % 2) ? 24 : 16, 0, 8, 16);
                }
            } else if (game_values.gamemode->gamemode == game_mode_collection) { //Draw cards for collection mode
                //Flash collected cards if 3 have been collected
                if (score[i]->subscore[0] < 3 || score[i]->subscore[2] % 20 < 10) {
                    short iNumCards = score[i]->subscore[0];
                    short iCardValues = score[i]->subscore[1];
                    short iCardX = score[i]->x + scorepowerupoffsets[game_values.teamcounts[i] - 1][0] - 20;

                    for (short iCard = 0; iCard < iNumCards; iCard++) {
                        rm->spr_scorecards.draw(iCardX + iCard * 20, score[i]->y + 43, (iCardValues & 3) << 4, 0, 16, 16);
                        iCardValues >>= 2;
                    }
                }
            } else if (game_values.gamemode->gamemode == game_mode_boxes_minigame) { //Draw coins for boxes minigame
                //Flash collected cards if 3 have been collected
                short iNumCoins = score[i]->subscore[0];
                short iCoinX = score[i]->x + scorepowerupoffsets[game_values.teamcounts[i] - 1][0] - 32;

                short iCoin = 0;
                for (; iCoin < iNumCoins; iCoin++) {
                    rm->spr_scorecoins.draw(iCoinX + iCoin * 16, score[i]->y + 43, 0, 0, 16, 16);
                }

                for (short iEmptyCoin = iCoin; iEmptyCoin < 5; iEmptyCoin++) {
                    rm->spr_scorecoins.draw(iCoinX + iEmptyCoin * 16, score[i]->y + 43, 16, 0, 16, 16);
                }
            }

            short iScoreX = score[i]->x + iScoreTextOffset[i];
            short iScoreY = score[i]->y + 4;

            rm->spr_scoretext.draw(iScoreX, iScoreY, score[i]->iDigitLeft, (score[i]->iDigitLeft == 0 ? 16 : 0), 16, 16);
            rm->spr_scoretext.draw(iScoreX + 18, iScoreY, score[i]->iDigitMiddle, (score[i]->iDigitLeft == 0 && score[i]->iDigitMiddle == 0 ? 16 : 0), 16, 16);
            rm->spr_scoretext.draw(iScoreX + 36, iScoreY, score[i]->iDigitRight, 0, 16, 16);
        }
    }
}

void GameplayState::drawScreenFade()
{
    if (game_values.screenfadespeed != 0) {
        g_map->update();
        game_values.screenfade += game_values.screenfadespeed;

        if (game_values.screenfade <= 0) {
            game_values.screenfadespeed = 0;
            game_values.screenfade = 0;

            //display the mode and goal at the start of the game
            //if (game_values.matchtype == MatchType::QuickGame)
            if (game_values.startmodedisplay && game_values.singleplayermode == -1) {
                char szMode[128];
                if (game_values.gamemode->goal < 0)
                    sprintf(szMode, "%s  %s: X", game_values.gamemode->GetModeName().c_str(), game_values.gamemode->GetGoalName().c_str());
                else
                    sprintf(szMode, "%s  %s: %d", game_values.gamemode->GetModeName().c_str(), game_values.gamemode->GetGoalName().c_str(), game_values.gamemode->goal);

                short iMode = GetModeIconIndexFromMode(game_values.gamemode->gamemode);

                eyecandy[2].add(new EC_Announcement(&rm->game_font_large, &rm->menu_mode_large, szMode, iMode, 130, 90));
            }
        } else if (game_values.screenfade >= 255) {
            game_values.screenfadespeed = 0;
            game_values.screenfade = 255;
        }
    }

    if (game_values.screenfade > 0) {
        rm->menu_shade.setalpha((Uint8)game_values.screenfade);
        rm->menu_shade.draw(0, 0);
    }
}

void GameplayState::drawScreenShakeBackground()
{
    //Draw black "behind" the game if we are shaking/moving the screen
    if (y_shake > 0) {
        SDL_Rect rect = {0, 0, App::screenWidth, y_shake};
        SDL_FillRect(screen, &rect, 0x0);       //fill empty area with black
    } else if (y_shake < 0) {
        SDL_Rect rect = {0, App::screenHeight + y_shake, App::screenWidth, App::screenHeight};
        SDL_FillRect(screen, &rect, 0x0);       //fill empty area with black
    }

    if (x_shake > 0) {
        SDL_Rect rect = {0, 0, x_shake, App::screenHeight};
        SDL_FillRect(screen, &rect, 0x0);       //fill empty area with black
    } else if (x_shake < 0) {
        SDL_Rect rect = {App::screenWidth + x_shake, 0, App::screenWidth, App::screenHeight};
        SDL_FillRect(screen, &rect, 0x0);       //fill empty area with black
    }
}

void GameplayState::drawPlayerSwap()
{
    if (game_values.flags.swapplayers) {
        for (CPlayer* player : players) {
            player->drawswap();

            short storedpowerupid = game_values.gamepowerups[player->getGlobalID()];

            if (storedpowerupid != -1) {
                short iPowerupX, iPowerupY;

                if (game_values.swapstyle == 1) {
                    iPowerupX = game_values.flags.swapplayersblink ? player->iOldPowerupX : player->iNewPowerupX;
                    iPowerupY = game_values.flags.swapplayersblink ? player->iOldPowerupY : player->iNewPowerupY;
                } else {
                    iPowerupX = (short)((float)(player->iNewPowerupX - player->iOldPowerupX) * game_values.flags.swapplayersposition) + player->iOldPowerupX;
                    iPowerupY = (short)((float)(player->iNewPowerupY - player->iOldPowerupY) * game_values.flags.swapplayersposition) + player->iOldPowerupY;
                }

                rm->spr_storedpowerupsmall.draw(iPowerupX, iPowerupY, storedpowerupid * 16, 0, 16, 16);
            }
        }

        if (game_values.swapstyle == 0) {
            if (!rm->sfx_skid.isPlaying())
                ifSoundOnPlay(rm->sfx_skid);
        }

        if (++game_values.flags.swapplayersblinkcount > 10) {
            game_values.flags.swapplayersblinkcount = 0;
            game_values.flags.swapplayersblink = !game_values.flags.swapplayersblink;
        }

        game_values.flags.swapplayersposition += 0.02f;
        if (game_values.flags.swapplayersposition >= 1.0f) {
            game_values.flags.swapplayersposition = 0.0f;
            game_values.flags.swapplayers = false;
            game_values.screenfade = 0;

            if (game_values.swapstyle == 0)
                ifsoundonstop(rm->sfx_skid);

            ifSoundOnPlay(rm->sfx_transform);

            if (game_values.swapstyle == 1) {
                for (CPlayer* player : players)
                    eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion, player->leftX() + (HALFPW) - 16, player->topY() + (HALFPH) - 16, 3, 8));
            }
        }
    }
}

void GameplayState::drawBackLayer()
{
    rm->spr_backmap[g_iCurrentDrawIndex].draw(0, 0);

    //draw back eyecandy behind players
    g_map->drawPlatforms(0);

    eyecandy[0].draw();
    noncolcontainer.draw();

    game_values.gamemode->draw_background();

    objectcontainer[0].draw();
}

void GameplayState::drawMiddleLayer()
{
    g_map->drawPlatforms(1);

    if (!game_values.flags.swapplayers) {
        for (CPlayer* player : players)
            player->draw();
    }

    eyecandy[1].draw();

    objectcontainer[1].draw();
}

void GameplayState::drawFrontLayer()
{
    g_map->drawPlatforms(2);

    if (game_values.toplayer)
        g_map->drawfrontlayer();

    g_map->drawWarpLocks();

    g_map->drawPlatforms(3);

    objectcontainer[2].draw();
    eyecandy[2].draw();
    game_values.gamemode->draw_foreground();

    g_map->drawPlatforms(4);
}

void GameplayState::drawWindMeter()
{
    if (game_values.windaffectsplayers) {
        short iDisplayWindMeterY = game_values.scoreboardstyle == ScoreboardStyle::Bottom ? 8 : 440;
        rm->spr_windmeter.draw(210, iDisplayWindMeterY, 0, 0, 220, 32);
        rm->spr_windmeter.draw((short)(game_values.flags.gamewindx * 20.0f) + App::screenWidth/2, iDisplayWindMeterY + 6, 220, 0, 12, 20);
    }
}

void GameplayState::drawCountdown()
{
    //Draw countdown start timer
    if (iCountDownState > 0 && game_values.screenfade == 0) {
        const SDL_Rect * rects = iCountDownNumbers[iCountDownRectGroup[28 - iCountDownState]][iCountDownRectSize[28 - iCountDownState]];
        rm->spr_countdown_numbers.draw(rects[1].x, rects[1].y, rects[0].x, rects[0].y, rects[0].w, rects[0].h);
    }
}

void GameplayState::drawSpotlights()
{
    if (game_values.spotlights)
        spotlightManager.DrawSpotlights();
}

void GameplayState::drawOutOfScreenIndicators()
{
    //draw arrows for being above the top of the screen
    for (CPlayer* player : players)
        player->drawOutOfScreenIndicators();
}

void GameplayState::drawEverything(short iCountDownState, short iScoreTextOffset[4])
{
    drawBackLayer();
    drawMiddleLayer();
    drawFrontLayer();

    drawSpotlights();
    drawScoreboard(iScoreTextOffset);
    drawWindMeter();
    drawOutOfScreenIndicators();
    drawCountdown();
    drawScreenFade();
    drawPlayerSwap();
    drawScreenShakeBackground();
}

void drawExitPauseDialog()
{
    if (game_values.flags.pausegame) {
        rm->spr_dialog.draw(224, 176);
        rm->menu_font_large.drawCentered(App::screenWidth/2, 194, "Pause");

        //menu_font_large.drawCentered(App::screenWidth/2, App::screenHeight/2, game_values.gamemode->GetModeName());
        short iMode = GetModeIconIndexFromMode(game_values.gamemode->gamemode);

        rm->menu_mode_large.draw(304, 224, iMode << 5, 0, 32, 32);

        std::string szGoal = game_values.gamemode->GetGoalName() + ": ";

        if (game_values.gamemode->goal == -1)
            szGoal += "Unlimited";
        else
            szGoal += std::to_string(game_values.gamemode->goal);

        rm->menu_font_large.drawCentered(App::screenWidth/2, 264, szGoal.c_str());
    }

    if (game_values.flags.exitinggame) {
        rm->spr_dialog.draw(App::screenWidth * 0.35f, App::screenHeight*0.37f);
        rm->menu_font_large.drawCentered(App::screenWidth * 0.5f, App::screenHeight*0.46f - (rm->menu_font_large.getHeight() >> 1), "Exit Game");

        rm->spr_dialogbutton.draw(App::screenWidth * 0.37f, App::screenHeight*0.52f, 0, (game_values.flags.exityes ? 34 : 0), 80, 34);
        rm->spr_dialogbutton.draw(App::screenWidth * 0.51f, App::screenHeight*0.52f, 0, (game_values.flags.exityes ? 0 : 34), 80, 34);

        rm->menu_font_large.draw(App::screenWidth * 0.43f - (rm->menu_font_large.getWidth("Yes") >> 1),  App::screenHeight*0.56f - (rm->menu_font_large.getHeight() >> 1), "Yes");
        rm->menu_font_large.draw(App::screenWidth * 0.57f - (rm->menu_font_large.getWidth("No") >> 1),  App::screenHeight*0.56f - (rm->menu_font_large.getHeight() >> 1), "No");
    }
}

bool IsPauseAllowed()
{
    return !game_values.flags.noexit;
}

bool IsExitAllowed()
{
    if (!game_values.flags.noexit || players.empty())
        return true;

    for (CPlayer* player : players) {
        if (game_values.playercontrol[player->getGlobalID()] == 1)
            return false;
    }

    return true;
}

void UpdateScoreBoard()
{
    if (game_values.matchtype == MatchType::World) {
        //If no one won, then nothing on the world map has changed
        if (game_values.gamemode->winningteam < 0)
            return;

        //If this was the last stage, signal that the world is over
        if (game_values.tourstops[game_values.tourstopcurrent]->fEndStage) {
            game_values.tournamentwinner = 1;
            rm->backgroundmusic[4].play(true, true);
        }

        //Add up all the winnings so far and determine overall place in the standings
        for (short iScore = 0; iScore < score_cnt; iScore++)
            game_values.tournament_scores[iScore].wins = 0;

        //Assign the order that players will show up on the scoreboard (ordered by score)
        for (short iMyScore = 0; iMyScore < score_cnt; iMyScore++) {
            for (short iTheirScore = 0; iTheirScore < score_cnt; iTheirScore++) {
                if (game_values.tournament_scores[iMyScore].total > game_values.tournament_scores[iTheirScore].total) {
                    game_values.tournament_scores[iTheirScore].wins++;
                }
            }
        }

        //Give players the item bonuses that were won
        TourStop * tourStop = game_values.tourstops[game_values.tourstopcurrent];

        for (short iScore = 0; iScore < score_cnt; iScore++) {
            for (short iBonus = 0; iBonus < tourStop->iNumBonuses; iBonus++) {
                if (tourStop->wsbBonuses[iBonus].iWinnerPlace == score[iScore]->place) {
                    if (game_values.worldpowerupcount[iScore] < 32)
                        game_values.worldpowerups[iScore][game_values.worldpowerupcount[iScore]++] = tourStop->wsbBonuses[iBonus].iBonus;
                    else
                        game_values.worldpowerups[iScore][31] = tourStop->wsbBonuses[iBonus].iBonus;
                }
            }
        }

        //Add powerups to player's world item inventory that they held at the end of the game
        for (short iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++) {
            if (game_values.gamepowerups[iPlayer] != -1) {
                short iTeamId = LookupTeamID(iPlayer);
                if (game_values.worldpowerupcount[iTeamId] < 32)
                    game_values.worldpowerups[iTeamId][game_values.worldpowerupcount[iTeamId]++] = game_values.gamepowerups[iPlayer];
            }
        }
    } else if (game_values.matchtype == MatchType::Tour) {
        //If no one won (tied game), then there is no need to update the scores because nothing has changed
        if (game_values.gamemode->winningteam < 0)
            return;

        //For this game, set the player's place as the type of win
        for (short iScore = 0; iScore < score_cnt; iScore++) {
            game_values.tournament_scores[iScore].type[game_values.tourstopcurrent] = score[iScore]->place;
        }

        //Add up all the winnings so far and determine overall place in tour
        for (short iScore = 0; iScore < score_cnt; iScore++) {
            game_values.tournament_scores[iScore].total = 0;

            for (short iTourStop = 0; iTourStop <= game_values.tourstopcurrent; iTourStop++) {
                game_values.tournament_scores[iScore].total += (3 - game_values.tournament_scores[iScore].type[iTourStop]) * game_values.tourstops[iTourStop]->iPoints;
            }

            game_values.tournament_scores[iScore].wins = 0;
        }

        for (short iMyScore = 0; iMyScore < score_cnt; iMyScore++) {
            for (short iTheirScore = 0; iTheirScore < score_cnt; iTheirScore++) {
                if (game_values.tournament_scores[iMyScore].total > game_values.tournament_scores[iTheirScore].total) {
                    game_values.tournament_scores[iTheirScore].wins++;
                }
            }
        }

        if (++game_values.tourstopcurrent >= game_values.tourstops.size()) {
            //Calculate Tour Winner by counting up 1st, 2nd, 3rd, and 4th place wins

            short iWinningTeam = -2;  //Set winning team to -2 to signify a tie between teams
            short iWinningScore = 0;

            for (short iScore = 0; iScore < score_cnt; iScore++) {
                if (game_values.tournament_scores[iScore].total > iWinningScore) { //New winner found
                    iWinningTeam = iScore;
                    iWinningScore = game_values.tournament_scores[iScore].total;
                } else if (game_values.tournament_scores[iScore].total == iWinningScore) { //Winning position tied
                    iWinningTeam = -2;
                }
            }

            game_values.tournamentwinner = iWinningTeam;
            rm->backgroundmusic[4].play(true, true);
        }
    } else if (game_values.matchtype == MatchType::Tournament) {
        short maxScore = -1;  //Max score for game
        short maxTeam = -1;  //Team ID with the max score -> reset to -1 if two teams tied for win

        for (short i = 0; i < score_cnt; i++) {
            if (score[i]->score > maxScore) {
                maxScore = score[i]->score;
                maxTeam = i;
            } else if (score[i]->score == maxScore) {
                maxTeam = -1;
            }
        }

        if (maxTeam > -1) {
            game_values.tournament_scores[maxTeam].type[game_values.tournament_scores[maxTeam].wins] = currentgamemode;

            if (++game_values.tournament_scores[maxTeam].wins >= game_values.tournamentgames) {
                game_values.tournamentwinner = maxTeam;

                if (game_values.music)
                    rm->backgroundmusic[4].play(true, true);
            }
        }
    }
}

/// NOTE: The elements are ordered!
enum MapBlockType {
    MapBlock_BrickYellow,
    MapBlock_Powerup,
    MapBlock_Donut,
    MapBlock_Flip,
    MapBlock_Bounce,
    MapBlock_NoteGray,
    MapBlock_BrickBlue,
    MapBlock_SwitchToggleRed,
    MapBlock_SwitchToggleGreen,
    MapBlock_SwitchToggleYellow,
    MapBlock_SwitchToggleBlue,
    MapBlock_SwitchBlockRed,
    MapBlock_SwitchBlockGreen,
    MapBlock_SwitchBlockYellow,
    MapBlock_SwitchBlockBlue,
    MapBlock_View,
    MapBlock_BrickRed,
    MapBlock_NoteRed,
    MapBlock_NoteBlue,
    MapBlock_BrickGray,
    MapBlock_WpnBreakFireball,
    MapBlock_WpnBreakFeather,
    MapBlock_WpnBreakShell,
    MapBlock_WpnBreakBomb,
    MapBlock_WpnBreakBoomerang,
    MapBlock_WpnBreakHammer,
    MapBlock_WpnBreakKuriboShoe,
    MapBlock_WpnBreakPWings,
    MapBlock_WpnBreakStar,
    MapBlock_WpnBreakLeaf,
};

IO_Block* spawnMapBlock(short typeId, Vec2s drawPos, const MapBlock& objdata, const std::array<short, 4>& switchStates)
{
    switch (typeId) {
        case MapBlock_BrickYellow: return new B_BreakableBlock(&rm->spr_breakableblock, drawPos, 4, 10);
        case MapBlock_Powerup: return new B_PowerupBlock(&rm->spr_powerupblock, drawPos, 4, 10, objdata.fHidden, objdata.iSettings);
        case MapBlock_Donut: return new B_DonutBlock(&rm->spr_donutblock, drawPos);
        case MapBlock_Flip: return new B_FlipBlock(&rm->spr_flipblock, drawPos, objdata.fHidden);
        case MapBlock_Bounce: return new B_BounceBlock(&rm->spr_bounceblock, drawPos, objdata.fHidden);
        case MapBlock_NoteGray: return new B_NoteBlock(&rm->spr_noteblock, drawPos, 4, 10, NoteBlockType::Gray, objdata.fHidden);
        case MapBlock_BrickBlue: return new B_ThrowBlock(&rm->spr_throwblock, drawPos, 4, 10, ThrowBlockType::Blue);
        case MapBlock_SwitchToggleRed: return new B_OnOffSwitchBlock(&rm->spr_switchblocks, drawPos, SwitchColor::Red, switchStates[0]);
        case MapBlock_SwitchToggleGreen: return new B_OnOffSwitchBlock(&rm->spr_switchblocks, drawPos, SwitchColor::Green, switchStates[1]);
        case MapBlock_SwitchToggleYellow: return new B_OnOffSwitchBlock(&rm->spr_switchblocks, drawPos, SwitchColor::Yellow, switchStates[2]);
        case MapBlock_SwitchToggleBlue: return new B_OnOffSwitchBlock(&rm->spr_switchblocks, drawPos, SwitchColor::Blue, switchStates[3]);
        case MapBlock_SwitchBlockRed: return new B_SwitchBlock(&rm->spr_switchblocks, drawPos, SwitchColor::Red, objdata.iSettings[0]);
        case MapBlock_SwitchBlockGreen: return new B_SwitchBlock(&rm->spr_switchblocks, drawPos, SwitchColor::Green, objdata.iSettings[0]);
        case MapBlock_SwitchBlockYellow: return new B_SwitchBlock(&rm->spr_switchblocks, drawPos, SwitchColor::Yellow, objdata.iSettings[0]);
        case MapBlock_SwitchBlockBlue: return new B_SwitchBlock(&rm->spr_switchblocks, drawPos, SwitchColor::Blue, objdata.iSettings[0]);
        case MapBlock_View: return new B_ViewBlock(&rm->spr_viewblock, drawPos, objdata.fHidden, objdata.iSettings);
        case MapBlock_BrickRed: return new B_ThrowBlock(&rm->spr_throwblock, drawPos, 4, 10, ThrowBlockType::Red);
        case MapBlock_NoteRed: return new B_NoteBlock(&rm->spr_noteblock, drawPos, 4, 10, NoteBlockType::Red, objdata.fHidden);
        case MapBlock_NoteBlue: return new B_NoteBlock(&rm->spr_noteblock, drawPos, 4, 10, NoteBlockType::Blue, objdata.fHidden);
        case MapBlock_BrickGray: return new B_ThrowBlock(&rm->spr_throwblock, drawPos, 4, 10, ThrowBlockType::Gray);
        case MapBlock_WpnBreakFireball: return new B_WeaponBreakableBlock(&rm->spr_weaponbreakableblock, drawPos, WeaponDamageType::Fireball);
        case MapBlock_WpnBreakFeather: return new B_WeaponBreakableBlock(&rm->spr_weaponbreakableblock, drawPos, WeaponDamageType::Feather);
        case MapBlock_WpnBreakShell: return new B_WeaponBreakableBlock(&rm->spr_weaponbreakableblock, drawPos, WeaponDamageType::Shell);
        case MapBlock_WpnBreakBomb: return new B_WeaponBreakableBlock(&rm->spr_weaponbreakableblock, drawPos, WeaponDamageType::Bomb);
        case MapBlock_WpnBreakBoomerang: return new B_WeaponBreakableBlock(&rm->spr_weaponbreakableblock, drawPos, WeaponDamageType::Boomerang);
        case MapBlock_WpnBreakHammer: return new B_WeaponBreakableBlock(&rm->spr_weaponbreakableblock, drawPos, WeaponDamageType::Hammer);
        case MapBlock_WpnBreakKuriboShoe: return new B_WeaponBreakableBlock(&rm->spr_weaponbreakableblock, drawPos, WeaponDamageType::KuriboShoe);
        case MapBlock_WpnBreakPWings: return new B_WeaponBreakableBlock(&rm->spr_weaponbreakableblock, drawPos, WeaponDamageType::PWings);
        case MapBlock_WpnBreakStar: return new B_WeaponBreakableBlock(&rm->spr_weaponbreakableblock, drawPos, WeaponDamageType::Star);
        case MapBlock_WpnBreakLeaf: return new B_WeaponBreakableBlock(&rm->spr_weaponbreakableblock, drawPos, WeaponDamageType::Leaf);
        default: return nullptr;
    }
}

void LoadMapObjects(bool fPreview)
{
    LoadMapHazards(fPreview);

    //Clear all the previous switch settings
    for (short iSwitch = 0; iSwitch < 8; iSwitch++)
        g_map->switchBlocks[iSwitch].clear();

    //Add blocks (breakable, note, switch, throwable, etc)
    for (short x = 0; x < MAPWIDTH; x++) {
        for (short y = 0; y < MAPHEIGHT; y++) {
            const short typeId = g_map->objectdata[x][y].iType;
            IO_Block* block = spawnMapBlock(typeId, {x * 32, y * 32}, g_map->objectdata[x][y], g_map->iSwitches);
            g_map->blockdata[x][y] = block;

            if (block)
                noncolcontainer.add(block);

            switch (typeId) {
                case MapBlock_SwitchToggleRed:
                case MapBlock_SwitchToggleGreen:
                case MapBlock_SwitchToggleYellow:
                case MapBlock_SwitchToggleBlue:
                case MapBlock_SwitchBlockRed:
                case MapBlock_SwitchBlockGreen:
                case MapBlock_SwitchBlockYellow:
                case MapBlock_SwitchBlockBlue:
                    g_map->switchBlocks[typeId - MapBlock_SwitchToggleRed].push_back(block);
                    break;
            }
        }
    }

    //Scan for throw box objects and add items to them if approprate
    //Add special coins to them for the boxes minigame
    short iCountWeight = 0;
    for (short iPowerup = 0; iPowerup < NUM_POWERUPS; iPowerup++)
        iCountWeight += game_values.powerupweights[iPowerup];

    short iThrowBoxCount = 0;
    bool * fBoxHasCoin = NULL;

    if (game_values.gamemode->gamemode == game_mode_boxes_minigame) {
        for (const MapItem& item : g_map->mapitems) {
            if (item.itype == 5)
                iThrowBoxCount++;
        }

        fBoxHasCoin = new bool[iThrowBoxCount];
        for (short iItem = 0; iItem < iThrowBoxCount; iItem++)
            fBoxHasCoin[iItem] = false;

        //Randomly choose boxes to put 5 coins in
        for (short iItem = 0; iItem < 5 && iItem < iThrowBoxCount; iItem++) {
            short iBoxIndex = RANDOM_INT(iThrowBoxCount);

            while (fBoxHasCoin[iBoxIndex]) {
                if (++iBoxIndex >= iThrowBoxCount)
                    iBoxIndex = 0;
            }

            fBoxHasCoin[iBoxIndex] = true;
        }

        //If map has less than 5 boxes, then insert coins into map in random locations
        short iExtraCoinsNeeded = 5 - iThrowBoxCount;
        for (short iExtraCoin = 0; iExtraCoin < iExtraCoinsNeeded; iExtraCoin++) {
            objectcontainer[1].add(new MO_Coin(&rm->spr_coin, Vec2f::zero(), Vec2s::zero(), 2, -1, 2, 0, true));
        }
    }

    //Add map objects like springs, shoes and spikes
    short iAddThrowBoxIndex = 0;
    for (const MapItem& item : g_map->mapitems) {
        const short iType = item.itype;
        const Vec2s pos(item.ix * 32, item.iy * 32);

        if (iType == 0)
            objectcontainer[1].add(new CO_Spring(&rm->spr_spring, pos, false));
        else if (iType == 1)
            objectcontainer[1].add(new CO_Spike(&rm->spr_spike, pos));
        else if (iType == 2)
            objectcontainer[1].add(new CO_KuriboShoe(&rm->spr_kuriboshoe, pos, false));
        else if (iType == 3)
            objectcontainer[1].add(new CO_Spring(&rm->spr_spring, pos, true));
        else if (iType == 4)
            objectcontainer[1].add(new CO_KuriboShoe(&rm->spr_kuriboshoe, pos, true));
        else if (iType == 5) {
            short iItem = NO_POWERUP;
            if (!fPreview) {
                if (game_values.gamemode->gamemode == game_mode_boxes_minigame) {
                    if (fBoxHasCoin[iAddThrowBoxIndex])
                        iItem = MINIGAME_COIN;
                } else if (game_values.gamemode->gamemode == game_mode_health && RANDOM_INT(100) < game_values.gamemodesettings.health.percentextralife) {
                    iItem = HEALTH_POWERUP;
                } else if ((game_values.gamemode->gamemode == game_mode_timelimit && RANDOM_INT(100) < game_values.gamemodesettings.time.percentextratime) ||
                          (game_values.gamemode->gamemode == game_mode_star && RANDOM_INT(100) < game_values.gamemodesettings.star.percentextratime)) {
                    iItem = TIME_POWERUP;
                } else if ((game_values.gamemode->gamemode == game_mode_coins && RANDOM_INT(100) < game_values.gamemodesettings.coins.percentextracoin) ||
                          (game_values.gamemode->gamemode == game_mode_greed && RANDOM_INT(100) < game_values.gamemodesettings.greed.percentextracoin)) {
                    iItem = COIN_POWERUP;
                } else if (game_values.gamemode->gamemode == game_mode_jail && (RANDOM_INT(100)) < game_values.gamemodesettings.jail.percentkey) {
                    iItem = JAIL_KEY_POWERUP;
                } else if (iCountWeight > 0 && (RANDOM_INT(100)) < 40) {
                    int iRandPowerup = RANDOM_INT( iCountWeight )+ 1;
                    iItem = 0;

                    int iPowerupWeightCount = game_values.powerupweights[iItem];

                    while (iPowerupWeightCount < iRandPowerup)
                        iPowerupWeightCount += game_values.powerupweights[++iItem];
                }
            }

            objectcontainer[1].add(new CO_ThrowBox(&rm->spr_throwbox, pos, iItem));
            iAddThrowBoxIndex++;
        }
    }

    if (fBoxHasCoin) {
        delete [] fBoxHasCoin;
        fBoxHasCoin = NULL;
    }

    //Set all the 1x1 gaps up so players can run across them
    g_map->UpdateAllTileGaps();
}

void CleanUp()
{
    short i;
    //delete object list
    for (CPlayer* player : players) {
        delete player;
    }
    players.clear();

    eyecandy[0].clean();
    eyecandy[1].clean();
    eyecandy[2].clean();
    spotlightManager.ClearSpotlights();

    noncolcontainer.clean();

    objectcontainer[0].clean();
    objectcontainer[1].clean();
    objectcontainer[2].clean();

    LoadMapObjects(true);
    g_map->clearWarpLocks();
    g_map->resetPlatforms();

    //Stop all game sounds
    sfx_stopallsounds();
    rm->sfx_invinciblemusic.resetPause();
    rm->sfx_slowdownmusic.resetPause();

    x_shake = 0;
    y_shake = 0;
}

bool updateExitPauseDialog(short iCountDownState) // true on exit
{
    if (game_values.screenfade == 0 && iCountDownState <= COUNTDOWN_START_INDEX) {
        //If the cancel button is pressed
        if (game_values.flags.forceexittimer > 0) {
            if (--game_values.flags.forceexittimer <= 0) {
                game_values.appstate = AppState::EndGame;
                game_values.screenfade = 8;
                game_values.screenfadespeed = 8;
            }
        }

        for (int iPlayer = 0; iPlayer < 4; iPlayer++) {
            COutputControl * playerKeys = &game_values.playerInput.outputControls[iPlayer];

            //If the start key is pressed (pause key)
            if (playerKeys->game_start.fPressed && IsPauseAllowed()) {
                if (!game_values.flags.showscoreboard && !game_values.flags.exitinggame) {
                    game_values.flags.pausegame = !game_values.flags.pausegame;

                    if (game_values.flags.pausegame) {
                        rm->menu_shade.setalpha(App::menuTransparency   );
                        rm->menu_shade.draw(0, 0);

                        //Stop the pwings sound if it is on
                        if (rm->sfx_flyingsound.isPlaying())
                            ifsoundonstop(rm->sfx_flyingsound);
                    }

                    //ifsoundonpause(rm->sfx_invinciblemusic);
                    //ifsoundonpause(rm->sfx_slowdownmusic);
                    ifSoundOnPlay(rm->sfx_pause);
                }
            }

            //Only player 1 is allowed to exit a game
            //if (iPlayer != 0)
            //  continue;

            if ((playerKeys->game_cancel.fPressed || (playerKeys->game_start.fPressed && game_values.gamemode->gameover)) && IsExitAllowed()) {
                if (game_values.gamemode->gameover) {
                    if (game_values.matchtype == MatchType::Tour || game_values.matchtype == MatchType::Tournament)
                        UpdateScoreBoard();

                    CleanUp();
                    game_values.appstate = AppState::Menu;

                    return true;
                } else {
                    if (!game_values.flags.pausegame && !game_values.flags.exitinggame) {
                        rm->menu_shade.setalpha(App::menuTransparency   );
                        rm->menu_shade.draw(0, 0);
                        game_values.flags.exitinggame = true;
                        //ifsoundonpause(rm->sfx_invinciblemusic);
                        //ifsoundonpause(rm->sfx_slowdownmusic);

                        //Reset the keys each time we switch from menu to game and back
                        game_values.playerInput.ResetKeys();
                    }
                }
            }

            //Deal with input to game exit dialog box
            if (game_values.flags.exitinggame) {
                if (playerKeys->menu_left.fPressed)
                    game_values.flags.exityes = true;
                else if (playerKeys->menu_right.fPressed)
                    game_values.flags.exityes = false;

                if (playerKeys->menu_select.fPressed) {
                    if (game_values.flags.exityes) {
                        CleanUp();
                        game_values.flags.exitinggame = false;
                        game_values.flags.exityes = false;
                        game_values.appstate = AppState::Menu;
                        return true;
                    } else {
                        game_values.flags.exitinggame = false;
                        //ifsoundonpause(rm->sfx_invinciblemusic);
                        //ifsoundonpause(rm->sfx_slowdownmusic);

                        //Reset the keys each time we switch from menu to game and back
                        game_values.playerInput.ResetKeys();
                    }
                }
            }
        }
    }

    return false;
}

void updateScreenShake()
{
    if (y_shake > 0 && !game_values.spinscreen) {
        y_shake -= CRUNCHVELOCITY;

        if (y_shake < 0)
            y_shake = 0;
    }
}

// updates the bullet bills spawned by a powerup
void updateBulletBillPowerup()
{
    for (short iPlayer = 0; iPlayer < 4; iPlayer++) {
        if (game_values.bulletbilltimer[iPlayer] > 0) {
            game_values.bulletbilltimer[iPlayer]--;

            if (--game_values.bulletbillspawntimer[iPlayer] <= 0) {
                game_values.bulletbillspawntimer[iPlayer] = (short)(RANDOM_INT(20) + 25);
                float speed = ((float)(RANDOM_INT(21) + 20)) / 10.0f;
                objectcontainer[2].add(new MO_BulletBill(&rm->spr_bulletbill, &rm->spr_bulletbilldead, {0, (short)(RANDOM_INT(448))}, (RANDOM_INT(2) ? speed : -speed), iPlayer, false));
                ifSoundOnPlay(rm->sfx_bulletbillsound);
            }
        }
    }
}

void updateScoreboardAnimation() // scrolling to center at the end of game
{
    if (game_values.flags.showscoreboard) {
        if (game_values.flags.scorepercentmove < 1.0f) {
            game_values.flags.scorepercentmove += 0.01f;

            if (game_values.flags.scorepercentmove >= 1.0f)
                game_values.flags.scorepercentmove = 1.0f;
        } else {
           game_values.flags.scorepercentmove = 1.0f;
        }

        for (short i = 0; i < score_cnt; i++) {
            score[i]->x = (short)((float)(score[i]->destx - score[i]->fromx) * game_values.flags.scorepercentmove) + score[i]->fromx;
            score[i]->y = (short)((float)(score[i]->desty - score[i]->fromy) * game_values.flags.scorepercentmove) + score[i]->fromy;
        }
    }
}

void SetGameModeSettingsFromMenu()
{
    //If this is a tour stop and the tour has settings in it, use those.  Otherwise use the menu settings.
    if (game_values.tourstops[game_values.tourstopcurrent]->fUseSettings &&
            (game_values.matchtype == MatchType::Tour || game_values.matchtype == MatchType::World))
        memcpy(&game_values.gamemodesettings, &game_values.tourstops[game_values.tourstopcurrent]->gmsSettings, sizeof(GameModeSettings));
    else
        memcpy(&game_values.gamemodesettings, &game_values.gamemodemenusettings, sizeof(GameModeSettings));
}

void playSFX()
{
    //Play sound for skidding players
    if (game_values.flags.playskidsound) {
        if (!rm->sfx_skid.isPlaying())
            ifSoundOnPlay(rm->sfx_skid);
    } else {
        if (rm->sfx_skid.isPlaying())
            ifsoundonstop(rm->sfx_skid);
    }

    //Play sound for players using PWings
    if (game_values.flags.playflyingsound) {
        if (!rm->sfx_flyingsound.isPlaying())
            ifSoundOnPlay(rm->sfx_flyingsound);
    } else {
        if (rm->sfx_flyingsound.isPlaying())
            ifsoundonstop(rm->sfx_flyingsound);
    }
}

void playMusic()
{
    //Make sure music and sound effects keep playing
    if (game_values.flags.slowdownon != -1) {
        if (!rm->sfx_slowdownmusic.isPlaying())
            ifSoundOnPlay(rm->sfx_slowdownmusic);
    } else {
        if (rm->sfx_slowdownmusic.isPlaying())
            ifsoundonstop(rm->sfx_slowdownmusic);
    }

    if (game_values.flags.playinvinciblesound && game_values.musicvolume > 0) {
        if (!rm->sfx_invinciblemusic.isPlaying() && !rm->sfx_timewarning.isPlaying() && !rm->backgroundmusic[0].isPlaying())
            ifSoundOnPlay(rm->sfx_invinciblemusic);
    } else {
        if (rm->sfx_invinciblemusic.isPlaying())
            ifsoundonstop(rm->sfx_invinciblemusic);
    }

    //If no background music is playing, then play some
    if (!rm->backgroundmusic[0].isPlaying() && !rm->sfx_invinciblemusic.isPlaying() && !rm->sfx_timewarning.isPlaying() && !game_values.gamemode->gameover) {
        if (game_values.playnextmusic) {
            musiclist->setNextMusic(g_map->musicCategoryID, maplist->currentShortmapname(), g_map->szBackgroundFile);
            rm->backgroundmusic[0].load(musiclist->currentMusic());
        }

        rm->backgroundmusic[0].play(game_values.playnextmusic, false);
    }
}

void PlayNextMusicTrack()
{
    if (game_values.gamemode->gameover || game_values.flags.playinvinciblesound || rm->sfx_timewarning.isPlaying())
        return;

    rm->backgroundmusic[0].stop();
    musiclist->setNextMusic(g_map->musicCategoryID, maplist->currentShortmapname(), g_map->szBackgroundFile);
    rm->backgroundmusic[0].load(musiclist->currentMusic());
    rm->backgroundmusic[0].play(game_values.playnextmusic, false);
}

/*
void secretBoss()
{
    //This code will help stop a game midway and load a new map and mode
    //It was used for the secret boss mode in 1.7 AFE
    game_values.gamemode = bossgamemode;  //boss type has already been set at this point
    bossgamemode->SetBossType(0);

    if (bossgamemode->GetBossType() == 0)
        g_map->loadMap(convertPath("maps/special/dungeon.map"), read_type_full);
    else if (bossgamemode->GetBossType() == 1)
        g_map->loadMap(convertPath("maps/special/hills.map"), read_type_full);
    else if (bossgamemode->GetBossType() == 2)
        g_map->loadMap(convertPath("maps/special/volcano.map"), read_type_full);

    LoadCurrentMapBackground();

    g_map->predrawbackground(rm->spr_background, rm->spr_backmap[0]);
    g_map->predrawforeground(rm->spr_frontmap[0]);

    g_map->predrawbackground(rm->spr_background, rm->spr_backmap[1]);
    g_map->predrawforeground(rm->spr_frontmap[1]);

    g_map->SetupAnimatedTiles();
    LoadMapObjects(false);
    ///////////////////
}
*/

#ifdef _DEBUG
void debugAutoKillEveryone()
{
    if (game_values.autokill) {
        for (CPlayer* player : players) {
            player->DeathAwards();

            if (game_values.gamemode->playerkilledself(*player, KillStyle::Environment) != PlayerKillType::None)
                player->die(PlayerDeathStyle::Jump, false, false);
        }
    }
}

void debug_gameplay()
{
    static short endgametimer = (short)(RANDOM_INT(200));
    if (g_fAutoTest && !game_values.flags.swapplayers) {
        for (CPlayer* player : players) {
            if (player->isready()) {
                //Detect player is in center of tile only
                short x = (player->centerX()) / TILESIZE;

                if (player->centerX() >= App::screenWidth)
                    x = (player->centerX() - App::screenWidth) / TILESIZE;

                short y = (player->centerY()) / TILESIZE;

                int tile = tile_flag_nonsolid;
                IO_Block * block = NULL;
                short blocktype = -1;

                if (player->centerY() >= 0 && player->centerY() < App::screenHeight) {
                    tile = g_map->map(x, y);
                    block = g_map->block(x, y);
                    blocktype = g_map->blockat(x, y)->iType;
                }

                if ((tile & tile_flag_solid) ||
                        (block && blocktype != 3 && blocktype < 11)) {
                    game_values.flags.pausegame = true;
                    game_values.frameadvance = true;
                    g_fAutoTest = false;
                    break;
                }

                //Detect if any corner of player is in a tile
                short actualvalues[2][2];
                actualvalues[0][0] = player->leftX();

                if (actualvalues[0][0] < 0)
                    actualvalues[0][0] += App::screenWidth;

                actualvalues[0][1] = player->rightX();

                if (actualvalues[0][1] >= App::screenWidth)
                    actualvalues[0][1] -= App::screenWidth;

                actualvalues[1][0] = player->topY();
                actualvalues[1][1] = player->bottomY();

                short corners[2][2];
                corners[0][0] = player->leftX() / TILESIZE;

                if (player->leftX() < 0)
                    corners[0][0] = (player->leftX() + App::screenWidth) / TILESIZE;

                corners[0][1] = (player->rightX()) / TILESIZE;

                if (player->rightX() >= App::screenWidth)
                    corners[0][1] = (player->rightX() - App::screenWidth) / TILESIZE;

                corners[1][0] = player->topY() / TILESIZE;
                corners[1][1] = (player->bottomY()) / TILESIZE;

                for (short i = 0; i < 2; i++) {
                    for (short j = 0; j < 2; j++) {
                        int tile = tile_flag_nonsolid;
                        IO_Block * block = NULL;
                        short blocktype = -1;

                        if (actualvalues[0][j] >= 0 && actualvalues[0][j] < App::screenWidth && actualvalues[1][i] > 0 && actualvalues[1][i] < App::screenHeight) {
                            tile = g_map->map(corners[0][j], corners[1][i]);
                            block = g_map->block(corners[0][j], corners[1][i]);
                            blocktype = g_map->blockat(corners[0][j], corners[1][i])->iType;
                        }

                        if ( (tile & tile_flag_solid) ||
                                (block && blocktype != 3 && blocktype < 11)) {
                            game_values.flags.pausegame = true;
                            game_values.frameadvance = true;
                            g_fAutoTest = false;
                            break;
                        }
                    }

                    if (game_values.flags.pausegame)
                        break;
                }
            }

            if (game_values.flags.pausegame)
                break;
        }

#if 0
        //Kill off players to test spawning
        static short chooseplayer = 0;
        static short killtimer = 0;
        if (++killtimer > 20)
        {
            killtimer = 0;

            if (++chooseplayer >= list_players_cnt)
                chooseplayer = 0;

            list_players[chooseplayer]->DeathAwards();

            if ( !game_values.gamemode->playerkilledself(*(list_players[chooseplayer])) )
            {
                list_players[chooseplayer]->die(0, false);
            }
        }
#endif

        //Automatically run menus
        if (game_values.flags.showscoreboard) {
            if (--endgametimer < 0) {
                endgametimer = (short)(RANDOM_INT(200));

                if (game_values.matchtype != MatchType::SingleGame && game_values.matchtype != MatchType::QuickGame && game_values.matchtype != MatchType::MiniGame)
                    UpdateScoreBoard();

                CleanUp();
                game_values.appstate = AppState::Menu;

                return;
            }
        } else {
            if (++exitgametimer >= 8000) {
                CleanUp();
                game_values.flags.exitinggame = false;
                game_values.flags.exityes = false;
                game_values.appstate = AppState::Menu;

                return;
            }
        }
    }
}
#endif

void GameplayState::onEnterState()
{
    iCountDownState = 0;
    iCountDownTimer = 0;

    if (game_values.startgamecountdown && game_values.singleplayermode == -1) {
        iCountDownState = 28;
        iCountDownTimer = iCountDownTimes[0];
    }

    current_playerKeys = &game_values.playerInput.outputControls[0];
    previous_playerKeys = *current_playerKeys;

    initRunGame();

#ifdef _DEBUG
    exitgametimer = 0;
#endif
}

void GameplayState::handleInput()
{
    game_values.playerInput.ClearPressedKeys(game_values.flags.exitinggame ? 1 : 0);
    // netplay.netPlayerInput.ClearGameActionKeys();

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT: {
            CleanUp();
            game_values.appstate = AppState::Quit;
            return;
        }
        break;

        case SDL_KEYDOWN: {
            if (event.key.keysym.mod & (KMOD_LALT | KMOD_RALT)) {
                if (event.key.keysym.sym == SDLK_F4) {
                    CleanUp();
                    game_values.appstate = AppState::Quit;
                    return;
                } else if (event.key.keysym.sym == SDLK_RETURN) {
                    game_values.fullscreen = !game_values.fullscreen;
                    gfx_changefullscreen(game_values.fullscreen);
                    blitdest = screen;

                    //Continue with input -> don't feed this event to the input
                    //otherwise it will pause the game when switching to full/windowed screen
                    continue;
                }
            }
            if (event.key.keysym.sym == SDLK_F1) {
                game_values.showfps = !game_values.showfps;
            } else if (event.key.keysym.sym == SDLK_ESCAPE) {
                game_values.playerInput.outputControls[0].game_cancel.fPressed = true;
            } else if (event.key.keysym.sym == SDLK_TAB) {
                PlayNextMusicTrack();
            } else if (event.key.keysym.sym == SDLK_INSERT) {
                gfx_take_screenshot();
            }
#ifdef _DEBUG
            else if (event.key.keysym.sym == SDLK_LEFTBRACKET) {
                game_values.framelimiter++;
            } else if (event.key.keysym.sym == SDLK_RIGHTBRACKET) {
                if (game_values.framelimiter > 0)
                    game_values.framelimiter--;
            } else if (event.key.keysym.sym == SDLK_F2) {
                game_values.frameadvance = !game_values.frameadvance;
            } else if (event.key.keysym.sym == SDLK_F5) {
                game_values.autokill = !game_values.autokill;
            } else if (event.key.keysym.sym == SDLK_z) {
                for (CPlayer* player : players) {
                    player->DeathAwards();

                    if (game_values.gamemode->playerkilledself(*player, KillStyle::Environment) == PlayerKillType::Normal)
                        player->die(PlayerDeathStyle::Jump, false, false);
                }
            } else if (event.key.keysym.sym == SDLK_x) {
                short iplayer = RANDOM_INT(players.size());
                players[iplayer]->makefrozen(300);
            } else if (event.key.keysym.sym == SDLK_c) {
                short iplayer = RANDOM_INT(players.size());
                players[iplayer]->shield.type = (PlayerShieldType)(RANDOM_INT(3) + 1);
                players[iplayer]->shield.timer = 620;
            } else if (event.key.keysym.sym == SDLK_1) {
                if (event.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL))
                    objectcontainer[0].add(new PU_IceWandPowerup(&rm->spr_icewandpowerup, {players[0]->leftX() + 32, players[0]->topY()}, 1, 0, 30, 30, 1, 1));
                else if (event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT))
                    objectcontainer[0].add(new PU_BobombPowerup(&rm->spr_bobombpowerup, {players[0]->leftX() + 32, players[0]->topY() - 1}, 1, true, 0, 30, 30, 1, 1));
                else
                    objectcontainer[0].add(new PU_StarPowerup(&rm->spr_starpowerup, {players[0]->leftX() + 32, players[0]->topY()}, 4, true, 2, 30, 30, 1, 1));
            } else if (event.key.keysym.sym == SDLK_2) {
                if (event.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL))
                    objectcontainer[0].add(new PU_BombPowerup(&rm->spr_bombpowerup, {players[0]->leftX() + 32, players[0]->topY()}, 1, 0, 30, 30, 1, 1));
                else if (event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT))
                    objectcontainer[0].add(new PU_PowPowerup(&rm->spr_powpowerup, {players[0]->leftX() + 32, players[0]->topY() - 1}, 8, true, 8, 30, 30, 1, 1));
                else
                    objectcontainer[0].add(new PU_ExtraGuyPowerup(&rm->spr_1uppowerup, {players[0]->leftX() + 32, players[0]->topY()}, 1, true, 0, 30, 30, 1, 1, 1));
            } else if (event.key.keysym.sym == SDLK_3) {
                if (event.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL))
                    objectcontainer[0].add(new PU_PodoboPowerup(&rm->spr_podobopowerup, {players[0]->leftX() + 32, players[0]->topY()}, 1, 0, 30, 30, 1, 1));
                else if (event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT))
                    objectcontainer[0].add(new PU_BulletBillPowerup(&rm->spr_bulletbillpowerup, {players[0]->leftX() + 32, players[0]->topY() - 1}, 1, true, 0, 30, 30, 1, 1));
                else
                    objectcontainer[0].add(new PU_ExtraGuyPowerup(&rm->spr_2uppowerup, {players[0]->leftX() + 32, players[0]->topY()}, 1, true, 0, 30, 30, 1, 1, 2));
            } else if (event.key.keysym.sym == SDLK_4) {
                if (event.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL))
                    objectcontainer[0].add(new PU_Tanooki({players[0]->leftX() + 32, players[0]->topY()}));
                else if (event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT))
                    objectcontainer[1].add(new CO_Shell(ShellType::Green, {players[0]->leftX() + 32, players[0]->topY()}, true, true, true, false));
                else
                    objectcontainer[0].add(new PU_ExtraGuyPowerup(&rm->spr_3uppowerup, {players[0]->leftX() + 32, players[0]->topY()}, 1, true, 0, 30, 30, 1, 1, 3));
            } else if (event.key.keysym.sym == SDLK_5) {
                if (event.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL))
                    objectcontainer[0].add(new PU_PWingsPowerup(&rm->spr_pwingspowerup, {players[0]->leftX() + 32, players[0]->topY()}));
                else if (event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT))
                    objectcontainer[1].add(new CO_Shell(ShellType::Red, {players[0]->leftX() + 32, players[0]->topY()}, false, true, true, false));
                else
                    objectcontainer[0].add(new PU_ExtraGuyPowerup(&rm->spr_5uppowerup, {players[0]->leftX() + 32, players[0]->topY()}, 1, true, 0, 30, 30, 1, 1, 5));
            } else if (event.key.keysym.sym == SDLK_6) {
                if (event.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL))
                    objectcontainer[1].add(new CO_Spring(&rm->spr_spring, {players[0]->leftX() + 32, players[0]->topY()}, true));
                else if (event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT))
                    objectcontainer[1].add(new CO_Shell(ShellType::Spiny, {players[0]->leftX() + 32, players[0]->topY()}, false, false, true, true));
                else
                    objectcontainer[0].add(new PU_FirePowerup(&rm->spr_firepowerup, {players[0]->leftX() + 32, players[0]->topY()}, 1, true, 0, 30, 30, 1, 1));
            } else if (event.key.keysym.sym == SDLK_7) {
                if (event.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL))
                    objectcontainer[1].add(new CO_ThrowBox(&rm->spr_throwbox, {players[0]->leftX() + 32, players[0]->topY()}, (RANDOM_INT(NUM_POWERUPS) + 3) - 3));
                else if (event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT))
                    objectcontainer[1].add(new CO_Shell(ShellType::Buzzy, {players[0]->leftX() + 32, players[0]->topY()}, false, true, false, false));
                else
                    objectcontainer[0].add(new PU_HammerPowerup(&rm->spr_hammerpowerup, {players[0]->leftX() + 32, players[0]->topY()}, 1, true, 0, 30, 30, 1, 1));
            } else if (event.key.keysym.sym == SDLK_8) {
                if (event.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL))
                    objectcontainer[1].add(new CO_Spike(&rm->spr_spike, {players[0]->leftX() + 32, players[0]->topY()}));
                else if (event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT))
                    objectcontainer[0].add(new PU_ModPowerup(&rm->spr_modpowerup, {players[0]->leftX() + 32, players[0]->topY()}, 8, true, 8, 30, 30, 1, 1));
                else
                    objectcontainer[0].add(new PU_PoisonPowerup(&rm->spr_poisonpowerup, {players[0]->leftX() + 32, players[0]->topY()}, 1, true, 0, 30, 30, 1, 1));
            } else if (event.key.keysym.sym == SDLK_9) {
                if (event.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL))
                    objectcontainer[1].add(new CO_KuriboShoe(&rm->spr_kuriboshoe, {players[0]->leftX() + 32, players[0]->topY()}, true));
                else if (event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT))
                    objectcontainer[0].add(new PU_FeatherPowerup(&rm->spr_featherpowerup, {players[0]->leftX() + 32, players[0]->topY() - 1}, 1, 0, 30, 30, 1, 1));
                else
                    objectcontainer[0].add(new PU_ClockPowerup(&rm->spr_clockpowerup, {players[0]->leftX() + 32, players[0]->topY() - 1}, 1, true, 0, 30, 30, 1, 1));
            } else if (event.key.keysym.sym == SDLK_0) {
                if (event.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL))
                    objectcontainer[0].add(new PU_LeafPowerup(&rm->spr_leafpowerup, {players[0]->leftX() + 32, players[0]->topY() - 1}, 1, 0, 30, 30, 1, 1));
                else if (event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT))
                    objectcontainer[0].add(new PU_BoomerangPowerup(&rm->spr_boomerangpowerup, {players[0]->leftX() + 32, players[0]->topY() - 1}, 1, true, 0, 30, 30, 1, 1));
                else
                    objectcontainer[0].add(new PU_MysteryMushroomPowerup(&rm->spr_mysterymushroompowerup, {players[0]->leftX() + 32, players[0]->topY() - 1}, 1, true, 0, 30, 30, 1, 1));
            } else if (event.key.keysym.sym == SDLK_F8) {
                g_fAutoTest = !g_fAutoTest;
            }
#endif
            break;
        }

        default:
            break;
        }

        //Feed the player control structures with input data
        //Use menu controls when exit game dialog is up
        game_values.playerInput.Update(event, (game_values.flags.exitinggame ? 1 : 0));
        // net player inputs are not updated here,
        // it is handled in update()
    }
}

bool shouldUpdate()
{
    if (netplay.active)
        return true;

    if (game_values.flags.pausegame || game_values.flags.exitinggame)
        return false;

    return true;
}

void GameplayState::read_network()
{
    if (!netplay.active)
        return;

    // React to network events
    // The host receives remote input
    netplay.gamestate_changed = false;
    netplay.client.update();

    float percent_new = std::min(1.0f, (float)netplay.frames_since_last_gamestate / NET_GAMESTATE_FRAMES_TO_SEND);
    float percent_old = 1.0f - percent_new;
    assert(0 <= percent_new && percent_new <= 1.0f);
    assert(0 <= percent_old && percent_old <= 1.0f);
    assert(0.99 < percent_old + percent_new && percent_old + percent_new <= 1.01);

    if (!netplay.theHostIsMe) {
        for (size_t p = 0; p < players.size(); p++) {
            CPlayer* player = players[p];

            // if local player
            if (p == netplay.remotePlayerNumber) {
                // save the player data that was cause by the input of the previous frame
                Net_IndexedPlayerData pdata(netplay.current_input_counter);
                pdata.x = player->fx;
                pdata.y = player->fy;
                pdata.xvel = player->velx;
                pdata.yvel = player->vely;
                netplay.local_playerdata_buffer.push_back(pdata);
                netplay.local_playerdata_store_time[netplay.current_input_counter] = std::chrono::system_clock::now();
                netplay.current_input_counter++;

                if (netplay.gamestate_changed) {
                    // remove old, saved player data
                    uint8_t confirmed_index = netplay.last_confirmed_input + 1;
                    nettimepoint confirmed_until = netplay.local_playerdata_store_time[confirmed_index];
                    uint8_t iid;
                    Net_IndexedPlayerData last_popped_local(0xFF);
                    while (!netplay.local_playerdata_buffer.empty()) {
                        iid = netplay.local_playerdata_buffer.front().input_id;
                        if (confirmed_until <= netplay.local_playerdata_store_time[iid])
                            break;

                        last_popped_local = netplay.local_playerdata_buffer.front();
                        netplay.local_playerdata_buffer.pop_front();
                    }

                    // check if we can use the local player data,
                    // or the difference is so big we have to fall back
                    // to the remote, confirmed positions
                    bool use_remote_pdata = false;
                    if (netplay.local_playerdata_buffer.empty()) {
                        use_remote_pdata = true;
                    }
                    else {
                        Net_PlayerData pd_remote = netplay.latest_playerdata.player[p];

                        if (std::abs(last_popped_local.x - pd_remote.x) > 0.45f ||
                            std::abs(last_popped_local.y - pd_remote.y) > 0.45f)
                        {
                            use_remote_pdata = true;
                        }
                    }

                    if (use_remote_pdata) {
                        // the buffered data is now invalid
                        netplay.local_playerdata_buffer.clear();

                        // set confirmed data
                        player->fx = netplay.latest_playerdata.player[p].x;
                        player->fy = netplay.latest_playerdata.player[p].y;
                        player->velx = netplay.latest_playerdata.player[p].xvel;
                        player->vely = netplay.latest_playerdata.player[p].yvel;
                    }
                }
            }
            // for remote players, interpolate
            else {
                player->fx   = percent_old * netplay.previous_playerdata.player[p].x    + percent_new * netplay.latest_playerdata.player[p].x;
                player->fy   = percent_old * netplay.previous_playerdata.player[p].y    + percent_new * netplay.latest_playerdata.player[p].y;
                player->velx = percent_old * netplay.previous_playerdata.player[p].xvel + percent_new * netplay.latest_playerdata.player[p].xvel;
                player->vely = percent_old * netplay.previous_playerdata.player[p].yvel + percent_new * netplay.latest_playerdata.player[p].yvel;
            }
        }

        netplay.frames_since_last_gamestate++;
    }


    if (previous_playerKeys != *current_playerKeys) {
        previous_playerKeys = *current_playerKeys;
    }

    // Consume the next input from the remote input buffer
    netplay.netPlayerInput.ClearGameActionKeys();
    if (netplay.theHostIsMe)
        netplay.client.local_gamehost.confirmCurrentInputs();
    for (size_t p = 0; p < players.size(); p++) {
        if (netplay.remote_input_buffer[p].size() > 0) {
            netplay.netPlayerInput.outputControls[p] = netplay.remote_input_buffer[p].front().second;
            netplay.remote_input_buffer[p].pop_front();
        }
    }

    //printf("[%d;%d]\n", current_playerKeys->keys[0].fDown, current_playerKeys->keys[0].fPressed);
    //printf("%d -> %f\n", list_players[0]->leftX(), list_players[0]->fx);
}

void network_send_local_input()
{
    if (!netplay.active)
        return;

    netplay.client.storeLocalInput();
    netplay.client.sendLocalInput();
}

void network_broadcast_game_state()
{
    // The host sends the game state to clients
    if (netplay.active && netplay.theHostIsMe) {
        netplay.client.local_gamehost.sendCurrentGameStateIfNeeded();
        netplay.client.local_gamehost.update();
    }
}

void start_gameplay()
{
    CleanUp();
    SetGameModeSettingsFromMenu();
    game_values.appstate = AppState::Game;

    //secretBoss();

    if (game_values.music) {
        musiclist->setRandomMusic(g_map->musicCategoryID, "", "");
        rm->backgroundmusic[0].load(musiclist->currentMusic());
        rm->backgroundmusic[0].play(game_values.playnextmusic, false);
    }
}

void end_gameplay()
{
    CleanUp();
    game_values.appstate = AppState::Menu;
    game_values.screenfadespeed = -8;
    GameStateManager::instance().changeStateTo(&MenuState::instance());
}

void GameplayState::update_countdown_timer()
{
    //Count down start timer before each game
    if (iCountDownState > 0 && --iCountDownTimer <= 0) {
        //There is one extra count here so we hit all the numbers.  When we
        //reach 0 that means we are done counting (index 28 would be out of
        //bounds on this array)
        if (--iCountDownState != 0) {
            //28 is the magic number as there are 28 frames of animation
            //spread over a few seconds for the countdown numbers
            iCountDownTimer = iCountDownTimes[28 - iCountDownState];

            short countDownAnnounce = iCountDownAnnounce[28 - iCountDownState];
            if (countDownAnnounce >= 0)
                ifsoundonandreadyplay(rm->sfx_announcer[countDownAnnounce]);
        }
    }
}

void update_playerswap()
{
    if (game_values.flags.swapplayers) {
        for (CPlayer* player : players) {
            player->updateswap();
        }
    }
}

void GameplayState::update_world()
{
    shakeScreen();
    spinScreen();
    updateBulletBillPowerup();

    if (game_values.matchtype == MatchType::World &&
        game_values.gamemode->gameover &&
        game_values.flags.forceexittimer <= 0) {
        if (--game_values.flags.noexittimer <= 0)
            game_values.flags.noexit = false;
    }

    //------------- update objects -----------------------

#ifdef _DEBUG
    debugAutoKillEveryone();
#endif

    //Advance the cpu's turn (AI only calculates player's actions 1 out of 4 frames)
    if (++game_values.cputurn > 3)
        game_values.cputurn = 0;

    if (!netplay.active || (netplay.active && netplay.theHostIsMe))
        handleP2PCollisions();

    //Move platforms
    g_map->updatePlatforms();

    game_values.flags.playskidsound = false;
    game_values.flags.playinvinciblesound = false;
    game_values.flags.playflyingsound = false;

    for (CPlayer* player : players)
        player->move();    //move all objects before doing object-object collision detection in
    //->think(), so we test against the new position after object-map collision detection

    playSFX();

    noncolcontainer.update();
    objectcontainer[0].update();
    objectcontainer[1].update();
    objectcontainer[2].update();

    handleP2ObjCollisions();
    if (game_values.flags.swapplayers) {
        update_playerswap();
        return;
    }

    handleObj2ObjCollisions();

    //Commit all player actions at this point (after we have collided with any objects
    //that the player might have picked up)
    for (CPlayer* player : players)
        player->CommitAction();

    cleanDeadNonPlayerObjects();
    CleanDeadPlayers();

    eyecandy[0].update();
    eyecandy[1].update();
    eyecandy[2].update();

    game_values.gamemode->think();

    if (game_values.flags.slowdownon != -1 && ++game_values.flags.slowdowncounter > 580) {
        game_values.flags.slowdownon = -1;
        game_values.flags.slowdowncounter = 0;
    }

    g_map->update();

    updateScreenShake();

    updateScoreboardAnimation();
}

void GameplayState::update()
{
    read_network();

    if (!netplay.active) {
        checkWindEvent(iWindTimer, dNextWind);
    }

    for (short iTeam = 0; iTeam < score_cnt; iTeam++) {
        iScoreTextOffset[iTeam] = 34 * game_values.teamcounts[iTeam] + 1;
    }

#ifdef _DEBUG
    debug_gameplay();
#endif

    handleInput();
    network_send_local_input();

    if (updateExitPauseDialog(iCountDownState)) {
        if (netplay.active)
            netplay.client.sendLeaveGameMessage();
        GameStateManager::instance().changeStateTo(&MenuState::instance());
        return;
    }

    if (shouldUpdate()) {
        if (!game_values.flags.swapplayers && game_values.screenfade == 0) {
            update_countdown_timer();

            //Make updates to background stuff (animate map while countdown is counting)
            if (iCountDownState > COUNTDOWN_START_INDEX) {
                animateDuringCountdown();
            } else {
                update_world();
            }
        }
        update_playerswap();
        network_broadcast_game_state();

        if (game_values.screenfade == 255) {
            if (game_values.appstate == AppState::StartGame) {
                start_gameplay();
                return;
            } else if (game_values.appstate == AppState::EndGame) {
                end_gameplay();
                return;
            }
        }

        //--------------- draw everything ----------------------
        drawEverything(iCountDownState, iScoreTextOffset);
    }

    if (game_values.flags.pausegame || game_values.flags.exitinggame) {
        drawExitPauseDialog();
    }

    playMusic();


#ifdef _DEBUG
    if (g_fAutoTest)
        rm->menu_font_small.drawRightJustified(635, 5, "Auto");
#endif

}

bool coldec_player2player(CPlayer * o1, CPlayer * o2)
{
    //Special cases to deal with players overlapping the right and left sides of the screen
    if (o1->rightX() < o2->ix) {
        return o1->ix + App::screenWidth < o2->rightX() && o1->rightX() + App::screenWidth >= o2->ix && o1->iy <= o2->bottomY() && o1->bottomY() >= o2->iy;
    } else if (o2->rightX() < o1->ix) {
        return o1->ix < o2->rightX() + App::screenWidth && o1->rightX() >= o2->ix + App::screenWidth && o1->iy <= o2->bottomY() && o1->bottomY() >= o2->iy;
    } else { //Normal case where no overlap
        return o1->ix < o2->rightX() && o1->rightX() >= o2->ix && o1->iy <= o2->bottomY() && o1->bottomY() >= o2->iy;
    }
}

bool coldec_player2obj(CPlayer * o1, CObject * o2)
{
    //Special cases to deal with players overlapping the right and left sides of the screen
    if (o1->rightX() < o2->x()) {
        return o1->ix + App::screenWidth < o2->x() + o2->collisionRectW() && o1->rightX() + App::screenWidth >= o2->x() && o1->iy < o2->y() + o2->collisionRectH() && o1->bottomY() >= o2->y();
    } else if (o2->x() + o2->collisionRectW() < o1->ix) {
        return o1->ix < o2->x() + o2->collisionRectW() + App::screenWidth && o1->rightX() >= o2->x() + App::screenWidth && o1->iy < o2->y() + o2->collisionRectH() && o1->bottomY() >= o2->y();
    } else { //Normal case where no overlap
        return o1->ix < o2->x() + o2->collisionRectW() && o1->rightX() >= o2->x() && o1->iy < o2->y() + o2->collisionRectH() && o2->y() <= o1->bottomY();
    }
}

bool coldec_obj2obj(CObject * o1, CObject * o2)
{
    //Special cases to deal with players overlapping the right and left sides of the screen
    short o1r = o1->x() + o1->collisionRectW();
    short o1b = o1->y() + o1->collisionRectH();
    short o2r = o2->x() + o2->collisionRectW();
    short o2b = o2->y() + o2->collisionRectH();

    if (o1r < o2->x()) {
        return o1->x() + App::screenWidth < o2r && o1r + App::screenWidth >= o2->x() && o1->y() < o2b && o1b >= o2->y();
    } else if (o2r < o1->x()) {
        return o1->x() < o2r + App::screenWidth && o1r >= o2->x() + App::screenWidth && o1->y() < o2b && o1b >= o2->y();
    } else {
        return o1->x() < o2r && o1r >= o2->x() && o1->y() < o2b && o1b >= o2->y();
    }
}

bool SwapPlayers(short iUsingPlayerID)
{
    //Count available players to switch with
    short iNumAvailablePlayers = 0;
    for (CPlayer* player : players) {
        if (player->isready()) {
            iNumAvailablePlayers++;
            player->fOldSwapX = player->leftX();
            player->fOldSwapY = player->topY();

            player->iNewPowerupX = player->score->x + scorepowerupoffsets[game_values.teamcounts[player->teamID] - 1][player->subTeamID];
            player->iNewPowerupY = player->score->y + 25;
        }
    }

    if (iNumAvailablePlayers <= 1)
        return false;

    if (game_values.swapstyle != 2) {
        game_values.flags.swapplayers = true;
        game_values.flags.swapplayersposition = 0.0f;

        if (game_values.swapstyle == 1) {
            game_values.flags.swapplayersblink = false;
            game_values.flags.swapplayersblinkcount = 0;
        } else {
            game_values.screenfade = App::menuTransparency;
        }
    }

    short iIncrement = RANDOM_INT (iNumAvailablePlayers - 1);

    MysteryMushroomTempPlayer spots[4];
    for (size_t iPlayer = 0; iPlayer < players.size(); iPlayer++) {
        CPlayer* player  = players[iPlayer];

        if (!player->isready())
            continue;

        short iNewSpot = iPlayer + iIncrement;

        do {
            if (++iNewSpot >= players.size())
                iNewSpot = 0;
        } while (spots[iNewSpot].fUsed || !players[iNewSpot]->isready());

        spots[iNewSpot].fUsed = true;
        spots[iPlayer].SetPlayer(players[iNewSpot], game_values.gamepowerups[players[iNewSpot]->getGlobalID()]);

        //Give credit for deaths to the player that used the mystery mushroom
        if (iUsingPlayerID == iNewSpot) {
            player->iSuicideCreditPlayerID = players[iNewSpot]->globalID;
            player->iSuicideCreditTimer = 62;
        }
    }

    for (short iPlayer = 0; iPlayer < players.size(); iPlayer++) {
        CPlayer* player  = players[iPlayer];

        if (!player->isready())
            continue;

        spots[iPlayer].GetPlayer(player, &game_values.gamepowerups[player->getGlobalID()]);

        if (game_values.swapstyle != 1)
            eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion, (short)player->fNewSwapX + (HALFPW) - 16, (short)player->fNewSwapY + (HALFPH) - 16, 3, 8));

        if (game_values.swapstyle == 2) {
            player->setXf(player->fNewSwapX);
            player->setYf(player->fNewSwapY);

            if (player->carriedItem)
                player->carriedItem->MoveToOwner();
        }
    }

    return true;
}

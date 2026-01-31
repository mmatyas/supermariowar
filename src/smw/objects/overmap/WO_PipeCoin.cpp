#include "WO_PipeCoin.h"

#include "eyecandy.h"
#include "GameValues.h"
#include "player.h"
#include "ResourceManager.h"
#include "gamemodes/MiniPipe.h"

extern CGM_Pipe_MiniGame* pipegamemode;
extern CEyecandyContainer eyecandy[3];
extern CGameValues game_values;
extern CResourceManager* rm;

//------------------------------------------------------------------------------
// class pipe coin (for coin pipe minigame)
//------------------------------------------------------------------------------
OMO_PipeCoin::OMO_PipeCoin(gfxSprite* nspr, Vec2f vel, Vec2s pos, short teamid, short colorid, short uncollectabletime)
    : IO_OverMapObject(nspr, pos, 4, 8, 30, 30, 1, 1, 0, colorid << 5, 32, 32)
    , iTeamID(teamid)
    , iColorID(colorid)
{
    state = 1;
    objectType = object_pipe_coin;

    velx = vel.x;

    if (pipegamemode->IsSlowdown())
        vely = vel.y / 2.0f;
    else
        vely = vel.y;

    iUncollectableTime = uncollectabletime;
}

bool OMO_PipeCoin::collide(CPlayer* player)
{
    if (iUncollectableTime > 0)
        return false;

    if (!game_values.gamemode->gameover) {
        if (iTeamID != -1) {
            if (player->teamID == iTeamID) {
                player->Score().AdjustScore(1);
                ifSoundOnPlay(rm->sfx_coin);
            }
        } else {
            if (iColorID == 2) {
                player->Score().AdjustScore(1);
                ifSoundOnPlay(rm->sfx_coin);
            } else if (iColorID == 0) {
                player->Score().AdjustScore(-1);
                ifSoundOnPlay(rm->sfx_stun);
            } else if (iColorID == 1) {
                player->Score().AdjustScore(5);
                ifSoundOnPlay(rm->sfx_extraguysound);
            }
        }

        game_values.gamemode->CheckWinner(player);
    }

    eyecandy[2].emplace<EC_SingleAnimation>(&rm->spr_coinsparkle, ix, iy, 7, 4);

    dead = true;
    return false;
}

void OMO_PipeCoin::update()
{
    setXf(fx + velx);
    setYf(fy + vely);

    if (iTeamID == -1)
        animate();

    if (iy >= App::screenHeight)
        dead = true;

    if (pipegamemode->IsSlowdown())
        vely += GRAVITATION / 1.5f;
    else
        vely += GRAVITATION;

    if (++sparkleanimationtimer >= 4) {
        sparkleanimationtimer = 0;
        sparkledrawframe += 32;
        if (sparkledrawframe >= App::screenHeight)
            sparkledrawframe = 0;
    }

    if (iUncollectableTime > 0)
        --iUncollectableTime;
}

void OMO_PipeCoin::draw()
{
    if (iUncollectableTime > 0) {
        spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, animationOffsetY, iw, ih, 2, 256);

        // Draw sparkles
        if (iTeamID == -1)
            rm->spr_shinesparkle.draw(ix - collisionOffsetX, iy - collisionOffsetY, sparkledrawframe, 0, 32, 32, 2, 256);

    } else {
        spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, animationOffsetY, iw, ih);

        // Draw sparkles
        if (iTeamID == -1)
            rm->spr_shinesparkle.draw(ix - collisionOffsetX, iy - collisionOffsetY, sparkledrawframe, 0, 32, 32);
    }
}

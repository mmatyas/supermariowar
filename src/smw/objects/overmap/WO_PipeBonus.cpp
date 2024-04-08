#include "WO_PipeBonus.h"

#include "eyecandy.h"
#include "player.h"
#include "GameValues.h"
#include "ResourceManager.h"
#include "gamemodes/MiniPipe.h"

extern CGM_Pipe_MiniGame * pipegamemode;
extern CEyecandyContainer eyecandy[3];

extern CGameValues game_values;
extern CResourceManager* rm;

//------------------------------------------------------------------------------
// class pipe powerup (for coin pipe minigame)
//------------------------------------------------------------------------------
OMO_PipeBonus::OMO_PipeBonus(gfxSprite *nspr, float dvelx, float dvely, short iX, short iY, short type, short duration, short uncollectabletime) :
    IO_OverMapObject(nspr, iX, iY, 4, 8, 30, 30, 1, 1, 0, type << 5, 32, 32)
    , iType(type)
    , iDuration(duration)
{
    state = 1;
    objectType = object_pipe_bonus;

    velx = dvelx;

    if (pipegamemode->IsSlowdown())
        vely = dvely / 1.5f;
    else
        vely = dvely;

    iUncollectableTime = uncollectabletime;
}

bool OMO_PipeBonus::collide(CPlayer * player)
{
    if (iUncollectableTime > 0)
        return false;

    //fireball
    if (iType == 5) {
        if (!player->isShielded()) {
            dead = true;
            eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion, ix - 1, iy - 1, 3, 8));

            if (!player->isInvincible()) {
                return player->KillPlayerMapHazard(false, KillStyle::Environment, false) != PlayerKillType::NonKill;
            }
        }

        return false;
    } else {
        if (!game_values.gamemode->gameover)
            pipegamemode->SetBonus(iType + 1, iDuration, player->getTeamID());
    }

    dead = true;
    return false;
}

void OMO_PipeBonus::update()
{
    IO_OverMapObject::update();

    if (iy >= App::screenHeight)
        dead = true;

    if (pipegamemode->IsSlowdown())
        vely += GRAVITATION / 2.0f;
    else
        vely += GRAVITATION;

    if (iUncollectableTime > 0)
        --iUncollectableTime;
}

void OMO_PipeBonus::draw()
{
    if (iUncollectableTime > 0)
        spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, animationOffsetY, iw, ih, 2, 256);
    else
        spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, animationOffsetY, iw, ih);
}


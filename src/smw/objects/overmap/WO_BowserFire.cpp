#include "WO_BowserFire.h"

#include "GameValues.h"
#include "player.h"

extern CGameValues game_values;

extern CPlayer* GetPlayerFromGlobalID(short iGlobalID);

//------------------------------------------------------------------------------
// class Bowser Fire (for survival mode)
//------------------------------------------------------------------------------
OMO_BowserFire::OMO_BowserFire(gfxSprite* nspr, short x, short y, float dVelX, float dVelY, short id, short teamid, short colorid)
    : IO_OverMapObject(nspr, x, y, 3, 6)
    , iPlayerID(id)
    , iTeamID(teamid)
    , iColorOffsetY((colorid + 1) * 64)
{
    objectType = object_bowserfire;
    velx = dVelX;
    vely = dVelY;

    ih = 32;
    collisionHeight = ih;
}

void OMO_BowserFire::update()
{
    IO_OverMapObject::update();

    if ((velx < 0 && ix < -iw) || (velx > 0 && ix > App::screenWidth)) {
        CPlayer* player = GetPlayerFromGlobalID(iPlayerID);

        if (player != NULL)
            player->decreaseProjectilesCount();

        dead = true;
    }
}

void OMO_BowserFire::draw()
{
    spr->draw(ix, iy, drawframe, (velx > 0.0f ? 32 : 0) + iColorOffsetY, iw, ih);
}

bool OMO_BowserFire::collide(CPlayer* player)
{
    // if the fire is off the screen, don't wrap it to collide
    if ((ix < 0 && velx < 0.0f && player->ix > ix + iw && player->ix + PW < App::screenWidth) || (ix + iw >= App::screenWidth && velx > 0.0f && player->ix + PW < ix && player->ix >= 0)) {
        return false;
    }

    if (player->globalID != iPlayerID && (game_values.teamcollision == TeamCollisionStyle::On || iTeamID != player->teamID) && !player->isInvincible() && !player->isShielded()) {
        // Find the player that made this explosion so we can attribute a kill
        PlayerKilledPlayer(iPlayerID, player, death_style_jump, KillStyle::Fireball, false, false);
        return true;
    }

    return false;
}

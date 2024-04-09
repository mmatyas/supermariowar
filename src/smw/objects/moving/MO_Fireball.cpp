#include "MO_Fireball.h"

#include "eyecandy.h"
#include "GameValues.h"
#include "player.h"

extern SpotlightManager spotlightManager;
extern CGameValues game_values;

//------------------------------------------------------------------------------
// class fireball
//------------------------------------------------------------------------------
MO_Fireball::MO_Fireball(gfxSprite *nspr, short x, short y, short iNumSpr, bool moveToRight, short aniSpeed, short iGlobalID, short teamID, short iColorID) :
    IO_MovingObject(nspr, x, y, iNumSpr, aniSpeed, (short)nspr->getWidth() >> 2, (short)nspr->getHeight() >> 3, 0, 0)
{
    if (moveToRight)
        velx = 5.0f;
    else
        velx = -5.0f;

    //fireball sprites have both right and left sprites in them
    ih = ih >> 3;

    bounce = -FIREBALLBOUNCE;

    iPlayerID = iGlobalID;
    iTeamID = teamID;

    colorOffset = iColorID * 36;
    movingObjectType = movingobject_fireball;

    state = 1;

    ttl = game_values.fireballttl;

    sSpotlight = NULL;
}

void MO_Fireball::update()
{
    IO_MovingObject::update();

    if (--ttl <= 0) {
        removeifprojectile(this, true, true);
    } else if (game_values.spotlights) {
        if (!sSpotlight) {
            sSpotlight = spotlightManager.AddSpotlight(ix - collisionOffsetX + (iw >> 1), iy - collisionOffsetY + (ih >> 1), 3);
        }

        if (sSpotlight) {
            sSpotlight->UpdatePosition(ix - collisionOffsetX + (iw >> 1), iy - collisionOffsetY + (ih >> 1));
        }
    }
}

bool MO_Fireball::collide(CPlayer * player)
{
    if (iPlayerID != player->getGlobalID() && (game_values.teamcollision == TeamCollisionStyle::On || iTeamID != player->getTeamID())) {
        if (!player->isShielded()) {
            removeifprojectile(this, false, false);

            if (!player->isInvincible() && !player->shyguy) {
                //Find the player that shot this fireball so we can attribute a kill
                PlayerKilledPlayer(iPlayerID, player, death_style_jump, KillStyle::Fireball, false, false);
                return true;
            }
        }
    }

    return false;
}

void MO_Fireball::draw()
{
    spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, (velx > 0 ? 0 : 18) + colorOffset, iw, ih);
}



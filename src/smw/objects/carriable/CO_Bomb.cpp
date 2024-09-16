#include "CO_Bomb.h"

#include "eyecandy.h"
#include "GameValues.h"
#include "ObjectContainer.h"
#include "player.h"
#include "ResourceManager.h"
#include "objects/moving/MO_Explosion.h"

extern CPlayer* GetPlayerFromGlobalID(short iGlobalID);
extern SpotlightManager spotlightManager;
extern CObjectContainer objectcontainer[3];
extern CGameValues game_values;
extern CResourceManager* rm;

//------------------------------------------------------------------------------
// class bomb
//------------------------------------------------------------------------------
CO_Bomb::CO_Bomb(gfxSprite* nspr, Vec2s pos, Vec2f vel, short aniSpeed, short iGlobalID, short teamID, short iColorID, short timetolive)
    : MO_CarriedObject(nspr, pos, 5, aniSpeed, 24, 24, 4, 13)
{
    iw = 28;
    ih = 38;

    iPlayerID = iGlobalID;
    iTeamID = teamID;
    iColorOffsetY = (iColorID + 1) * 38;

    movingObjectType = movingobject_bomb;
    state = 1;

    ttl = timetolive;

    velx = vel.x;
    vely = vel.y;

    iOwnerRightOffset = 14;
    iOwnerLeftOffset = -16;
    iOwnerUpOffset = 40;

    sSpotlight = NULL;
}

bool CO_Bomb::collide(CPlayer* player)
{
    if (state == 1 && owner == NULL) {
        if (player->AcceptItem(this)) {
            owner = player;

            velx = 0.0f;
            vely = 0.0f;

            if (iPlayerID == -1) {
                iPlayerID = owner->globalID;
                iTeamID = owner->teamID;
                iColorOffsetY = (owner->colorID + 1) * 38;
            }
        }
    }

    return false;
}

void CO_Bomb::update()
{
    if (--ttl <= 0)
        Die();

    if (dead)
        return;

    if (owner) {
        MoveToOwner();
    } else {
        applyfriction();

        // Collision detect map
        fOldX = fx;
        fOldY = fy;

        collision_detection_map();
    }

    animate();

    if (game_values.spotlights) {
        if (!sSpotlight) {
            sSpotlight = spotlightManager.AddSpotlight(ix - collisionOffsetX + (iw >> 1), iy - collisionOffsetY + (ih >> 1), 3);
        }

        if (sSpotlight) {
            sSpotlight->UpdatePosition(ix - collisionOffsetX + (iw >> 1), iy - collisionOffsetY + (ih >> 1));
        }
    }
}

void CO_Bomb::draw()
{
    if (owner && owner->iswarping())
        spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, iColorOffsetY, iw, ih, owner->GetWarpState(), owner->GetWarpPlane());
    else
        spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, iColorOffsetY, iw, ih);
}

void CO_Bomb::Die()
{
    if (owner) {
        owner->carriedItem = NULL;
        owner = NULL;
    }

    CPlayer* player = GetPlayerFromGlobalID(iPlayerID);

    if (player != NULL)
        player->decreaseProjectilesCount();

    dead = true;
    objectcontainer[2].add(new MO_Explosion(&rm->spr_explosion, ix + (iw >> 2) - 96, iy + (ih >> 2) - 64, 2, 4, iPlayerID, iTeamID, KillStyle::Bomb));
    ifSoundOnPlay(rm->sfx_bobombsound);
}

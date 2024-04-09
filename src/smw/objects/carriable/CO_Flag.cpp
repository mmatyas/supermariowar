#include "CO_Flag.h"

#include "player.h"
#include "GameValues.h"
#include "map.h"
#include "ResourceManager.h"
#include "objects/moving/MO_FlagBase.h"

extern CMap* g_map;
extern CGameValues game_values;
extern CResourceManager* rm;

//------------------------------------------------------------------------------
// class flag (for Capture the Flag mode)
//------------------------------------------------------------------------------
CO_Flag::CO_Flag(gfxSprite *nspr, MO_FlagBase * base, short iTeamID, short iColorID) :
    MO_CarriedObject(nspr, 0, 0, 4, 8, 30, 30, 1, 1, 0, iColorID << 6, 32, 32)
{
    state = 1;
    movingObjectType = movingobject_flag;
    flagbase = base;
    teamID = iTeamID;
    fLastFlagDirection = false;
    owner_throw = NULL;
    owner_throw_timer = 0;

    centerflag = teamID == -1;

    iOwnerRightOffset = HALFPW - 31;
    iOwnerLeftOffset = HALFPW + 1;
    iOwnerUpOffset = 38;

    fCarriedByKuriboShoe = true;

    placeFlag();
}

bool CO_Flag::collide(CPlayer * player)
{
    if (owner == NULL && player->isready() && (!fInBase || teamID != player->teamID)) {
        if (game_values.gamemodesettings.flag.touchreturn && teamID == player->teamID) {
            placeFlag();
            ifSoundOnPlay(rm->sfx_areatag);
        } else if (player->AcceptItem(this)) {
            owner = player;
            owner_throw = player;

            if (flagbase)
                flagbase->setFlag(NULL);
        }
    }

    return false;
}

void CO_Flag::update()
{
    if (owner) {
        MoveToOwner();
        timer = 0;
        fInBase = false;
        owner_throw = owner;
    } else if (fInBase) {
        if (flagbase) {
            setXf(flagbase->fx);
            setYf(flagbase->fy);
        }

        owner_throw = NULL;
    } else if (game_values.gamemodesettings.flag.autoreturn > 0 && ++timer > game_values.gamemodesettings.flag.autoreturn) {
        timer = 0;
        placeFlag();
        owner_throw = NULL;
    } else {
        if (owner_throw && --owner_throw_timer < 0) {
            owner_throw_timer = 0;
            owner_throw = NULL;
        }

        applyfriction();

        //Collision detect map
        fOldX = fx;
        fOldY = fy;

        collision_detection_map();
    }

    animate();

    if (centerflag) {
        if (animationtimer % 2 == 0) {
            animationOffsetY += 64;
            if (animationOffsetY > 192)
                animationOffsetY = 0;
        }
    }
}

void CO_Flag::draw()
{
    if (owner) {
        if (centerflag)
            animationOffsetY = owner->colorID << 6;

        if (owner->iswarping())
            spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, animationOffsetY + (fLastFlagDirection ? 32 : 0), iw, ih, owner->GetWarpState(), owner->GetWarpPlane());
        else
            spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, animationOffsetY + (fLastFlagDirection ? 32 : 0), iw, ih);
    } else {
        spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, animationOffsetY + (fLastFlagDirection ? 32 : 0), iw, ih);
    }
}

void CO_Flag::MoveToOwner()
{
    MO_CarriedObject::MoveToOwner();

    if (owner)
        fLastFlagDirection = owner->isFacingRight();
}

void CO_Flag::placeFlag()
{
    if (centerflag) {
        Drop();
        fInBase = false;

        short iAttempts = 10;
        while (!g_map->findspawnpoint(5, &ix, &iy, collisionWidth, collisionHeight, false) && iAttempts-- > 0);
        fx = (float)ix;
        fy = (float)iy;

        velx = 0.0f;
        vely = 0.0f;
        fLastFlagDirection = false;
    } else if (flagbase) {
        Drop();
        fInBase = true;
        setXi(flagbase->ix);
        setYi(flagbase->iy);
        fLastFlagDirection = false;
        flagbase->setFlag(this);
    }

    owner_throw = NULL;
    owner_throw_timer = 0;

    timer = 0;
}

void CO_Flag::Drop()
{
    MO_CarriedObject::Drop();
    owner_throw_timer = 62;
}


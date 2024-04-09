#include "CO_Egg.h"

#include "GameValues.h"
#include "map.h"
#include "ObjectContainer.h"
#include "objecthazard.h"
#include "ResourceManager.h"

extern CObjectContainer objectcontainer[3];
extern CMap* g_map;
extern CGameValues game_values;
extern CResourceManager* rm;

//------------------------------------------------------------------------------
// class egg (for egg mode)
//------------------------------------------------------------------------------
CO_Egg::CO_Egg(gfxSprite* nspr, short iColor)
    : MO_CarriedObject(nspr, 0, 0, 2, 16, 28, 30, 2, 1, 0, iColor << 5, 32, 32)
{
    state = 1;
    movingObjectType = movingobject_egg;

    owner_throw = NULL;
    owner_throw_timer = 0;

    sparkleanimationtimer = 0;
    sparkledrawframe = 0;

    color = iColor;

    egganimationrates[0] = 2;
    egganimationrates[1] = 4;
    egganimationrates[2] = 6;
    egganimationrates[3] = 8;
    egganimationrates[4] = 12;
    egganimationrates[5] = 16;

    iOwnerRightOffset = HALFPW;
    iOwnerLeftOffset = HALFPW - 28;
    iOwnerUpOffset = 32;

    fCarriedByKuriboShoe = true;

    placeEgg();
}

bool CO_Egg::collide(CPlayer* player)
{
    if (owner == NULL && player->isready()) {
        if (player->AcceptItem(this)) {
            owner = player;
            owner_throw = player;
        }
    }

    return false;
}

void CO_Egg::update()
{
    if (owner) {
        MoveToOwner();
        relocatetimer = 0;
        owner_throw = owner;
    } else if (++relocatetimer > 1500) {
        placeEgg();
        owner_throw = NULL;
    } else {
        if (owner_throw && --owner_throw_timer <= 0) {
            owner_throw_timer = 0;
            owner_throw = NULL;
        }

        applyfriction();

        // Collision detect map
        fOldX = fx;
        fOldY = fy;

        collision_detection_map();
    }

    if (++sparkleanimationtimer >= 4) {
        sparkleanimationtimer = 0;
        sparkledrawframe += 32;
        if (sparkledrawframe >= App::screenHeight)
            sparkledrawframe = 0;
    }

    // Explode
    if (game_values.gamemodesettings.egg.explode > 0) {
        if (--explosiondrawtimer <= 0) {
            explosiondrawtimer = 62;
            if (--explosiondrawframe < 0) {
                objectcontainer[2].add(new MO_Explosion(&rm->spr_explosion, ix + (iw >> 1) - 96, iy + (ih >> 1) - 64, 2, 4, -1, -1, KillStyle::Bomb));
                placeEgg();

                ifSoundOnPlay(rm->sfx_bobombsound);
            } else {
                if (explosiondrawframe < 5)
                    animationspeed = egganimationrates[explosiondrawframe];
                else
                    animationspeed = egganimationrates[5];
            }
        }
    }

    animate();
}

void CO_Egg::draw()
{
    MO_CarriedObject::draw();

    // Display explosion timer
    if (game_values.gamemodesettings.egg.explode > 0 && explosiondrawframe < 5) {
        if (owner && owner->iswarping())
            rm->spr_eggnumbers.draw(ix - collisionOffsetX, iy - collisionOffsetY, explosiondrawframe << 5, color << 5, 32, 32, owner->GetWarpState(), owner->GetWarpPlane());
        else
            rm->spr_eggnumbers.draw(ix - collisionOffsetX, iy - collisionOffsetY, explosiondrawframe << 5, color << 5, 32, 32);
    }
}

void CO_Egg::placeEgg()
{
    relocatetimer = 0;
    if (game_values.gamemodesettings.egg.explode > 0) {
        explosiondrawframe = game_values.gamemodesettings.egg.explode - 1;
        explosiondrawtimer = 62;

        if (explosiondrawframe < 5)
            animationspeed = egganimationrates[explosiondrawframe];
        else
            animationspeed = egganimationrates[5];
    } else {
        animationspeed = egganimationrates[5];
    }

    short x = 0, y = 0;
    short iAttempts = 32;
    while ((!g_map->findspawnpoint(5, &x, &y, collisionWidth, collisionHeight, false) || objectcontainer[1].getClosestMovingObject(x, y, movingobject_yoshi) < 250.0f)
        && iAttempts-- > 0)
        ;

    setXi(x);
    setYi(y);

    vely = GRAVITATION;
    velx = 0.0f;

    owner_throw = NULL;
    owner_throw_timer = 0;

    Drop();
}

void CO_Egg::Drop()
{
    MO_CarriedObject::Drop();
    owner_throw_timer = 62;
}

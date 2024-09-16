#include "PU_SecretPowerup.h"

#include "eyecandy.h"
#include "Game.h"
#include "GameValues.h"
#include "map.h"
#include "RandomNumberGenerator.h"
#include "ResourceManager.h"

extern CMap* g_map;
extern CEyecandyContainer eyecandy[3];

extern CGameValues game_values;
extern CResourceManager* rm;

//------------------------------------------------------------------------------
// secret powerup
//------------------------------------------------------------------------------
PU_SecretPowerup::PU_SecretPowerup(gfxSprite* nspr, Vec2s pos, short type)
    : MO_Powerup(nspr, pos, 4, 8, 30, 30, 1, 1)
    , itemtype(type)
{
    place();
}

void PU_SecretPowerup::update()
{
    MO_Powerup::update();

    if (++sparkleanimationtimer >= 4) {
        sparkleanimationtimer = 0;
        sparkledrawframe += 32;
        if (sparkledrawframe >= App::screenHeight)
            sparkledrawframe = 0;
    }
}

void PU_SecretPowerup::draw()
{
    MO_Powerup::draw();

    // Draw sparkles
    rm->spr_shinesparkle.draw(ix - collisionOffsetX, iy - collisionOffsetY, sparkledrawframe, 0, 32, 32);
}

bool PU_SecretPowerup::collide(CPlayer* player)
{
    if (itemtype == 0) {
        game_values.windaffectsplayers = true;

        for (short i = 0; i < 15; i++)
            eyecandy[2].add(new EC_Snow(&rm->spr_snow, (float)(RANDOM_INT(App::screenWidth)), (float)RANDOM_INT(App::screenHeight), RANDOM_INT(4) + 1));
    } else if (itemtype == 1) {
        game_values.spinscreen = true;
    } else if (itemtype == 2) {
        game_values.reversewalk = true;
    } else if (itemtype == 3) {
        game_values.spotlights = true;
    }

    ifSoundOnPlay(rm->sfx_pickup);

    dead = true;
    return false;
}

void PU_SecretPowerup::place()
{
    short iAttempts = 10;
    while (!g_map->findspawnpoint(5, &ix, &iy, collisionWidth, collisionHeight, false) && iAttempts-- > 0)
        ;

    fx = (float)ix;
    fy = (float)iy;
}

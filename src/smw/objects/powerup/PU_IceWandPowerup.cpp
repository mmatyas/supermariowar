#include "PU_IceWandPowerup.h"

#include "Game.h"
#include "player.h"
#include "ResourceManager.h"

extern CResourceManager* rm;

//------------------------------------------------------------------------------
// class ice wand powerup
//------------------------------------------------------------------------------
PU_IceWandPowerup::PU_IceWandPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY) :
    MO_Powerup(nspr, x, y, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY)
{
    velx = 0.0f;

    sparkleanimationtimer = 0;
    sparkledrawframe = 0;
}

void PU_IceWandPowerup::update()
{
    MO_Powerup::update();

    if (++sparkleanimationtimer >= 4) {
        sparkleanimationtimer = 0;
        sparkledrawframe += 32;
        if (sparkledrawframe >= App::screenHeight)
            sparkledrawframe = 0;
    }
}

void PU_IceWandPowerup::draw()
{
    MO_Powerup::draw();

    //Draw sparkles
    if (state == 1)
        rm->spr_shinesparkle.draw(ix - collisionOffsetX, iy - collisionOffsetY, sparkledrawframe, 0, 32, 32);
}

bool PU_IceWandPowerup::collide(CPlayer * player)
{
    if (state > 0) {
        player->SetPowerup(5);
        dead = true;
    }

    return false;
}

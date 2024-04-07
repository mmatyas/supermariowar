#include "PU_TreasureChestBonus.h"

#include "eyecandy.h"
#include "Game.h"
#include "GameValues.h"
#include "map.h"
#include "ResourceManager.h"

extern CMap* g_map;
extern CEyecandyContainer eyecandy[3];

extern CGameValues game_values;
extern CResourceManager* rm;

//------------------------------------------------------------------------------
// class treasure chest powerup
//------------------------------------------------------------------------------
PU_TreasureChestBonus::PU_TreasureChestBonus(gfxSprite *nspr, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY, short iBonusItem) :
    MO_Powerup(nspr, 0, 0, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY)
{
    velx = 0.0f;

    sparkleanimationtimer = 0;
    sparkledrawframe = 0;
    bounce = -VELPOWERUPBOUNCE * 2;
    numbounces = 5;
    state = 2;
    bonusitem = iBonusItem;

    short iAttempts = 10;
    while (!g_map->findspawnpoint(5, &ix, &iy, collisionWidth, collisionHeight, false) && iAttempts-- > 0);
    fx = (float)ix;
    fy = (float)iy;

    drawbonusitemx = 0;
    drawbonusitemy = 0;
    drawbonusitemtimer = 0;

    fObjectDiesOnSuperDeathTiles = false;
}

void PU_TreasureChestBonus::update()
{
    MO_Powerup::update();

    if (++sparkleanimationtimer >= 4) {
        sparkleanimationtimer = 0;
        sparkledrawframe += 32;
        if (sparkledrawframe >= App::screenHeight)
            sparkledrawframe = 0;
    }

    //Draw rising powerup from chest
    if (state == 3) {
        drawbonusitemy -= 2;

        if (--drawbonusitemtimer <= 0)
            state = 4;
    } else if (state == 4) {
        eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion, ix, drawbonusitemy, 3, 8));
        dead = true;
    }
}

void PU_TreasureChestBonus::draw()
{
    if (state < 3) {
        MO_Powerup::draw();

        //Draw sparkles
        rm->spr_shinesparkle.draw(ix - collisionOffsetX, iy - collisionOffsetY, sparkledrawframe, 0, 32, 32);
    } else {
        if (bonusitem >= NUM_POWERUPS)
            rm->spr_worlditems.draw(drawbonusitemx, drawbonusitemy, (bonusitem - NUM_POWERUPS) << 5, 0, 32, 32);
        else
            rm->spr_storedpoweruplarge.draw(drawbonusitemx, drawbonusitemy, bonusitem << 5, 0, 32, 32);
    }
}

bool PU_TreasureChestBonus::collide(CPlayer * player)
{
    if (state == 1) {
        ifSoundOnPlay(rm->sfx_treasurechest);
        //if (game_values.worldpowerupcount[player->teamID] < 32)
        //    game_values.worldpowerups[player->teamID][game_values.worldpowerupcount[player->teamID]++] = bonusitem;
        //else
        //	game_values.worldpowerups[player->teamID][31] = bonusitem;

        state = 3;

        drawbonusitemx = ix;
        drawbonusitemy = iy;
        drawbonusitemtimer = 60;

        eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion, ix, iy, 3, 8));

        game_values.flags.noexit = false;
    }

    return false;
}

float PU_TreasureChestBonus::BottomBounce()
{
    if (state == 2) {
        if (--numbounces <= 0) {
            numbounces = 0;
            state = 1;
            bounce = GRAVITATION;
        } else {
            if (vely > 0.0f)
                bounce = -vely / 2.0f;
            else
                bounce /= 2.0f;
        }
    }

    return bounce;
}

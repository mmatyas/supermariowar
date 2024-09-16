#include "MO_BonusHouseChest.h"

#include "eyecandy.h"
#include "GameMode.h"
#include "GameValues.h"
#include "player.h"
#include "ResourceManager.h"

extern CEyecandyContainer eyecandy[3];
extern CGameValues game_values;
extern CResourceManager* rm;

//------------------------------------------------------------------------------
// class treasure chest powerup
//------------------------------------------------------------------------------
MO_BonusHouseChest::MO_BonusHouseChest(gfxSprite* nspr, Vec2s pos, short iBonusItem)
    : IO_MovingObject(nspr, pos, 1, 0, 64, 64, 0, 0)
{
    iw = 64;
    ih = 64;

    state = 1;
    bonusitem = iBonusItem;

    drawbonusitemy = 0;
    drawbonusitemtimer = 0;

    movingObjectType = movingobject_treasurechest;

    fObjectDiesOnSuperDeathTiles = false;
    fObjectCollidesWithMap = false;
}

void MO_BonusHouseChest::update()
{
    // Draw rising powerup from chest
    if (state == 2) {
        drawbonusitemy -= 2;

        if (--drawbonusitemtimer <= 0) {
            eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion, ix + 16, drawbonusitemy, 3, 8));
            state = 3;
        }
    }
}

void MO_BonusHouseChest::draw()
{
    if (state < 2)
        spr->draw(ix, iy, 0, 0, iw, ih);

    if (state >= 2)
        spr->draw(ix, iy, 128, 0, iw, ih);

    if (state == 2) {
        if (bonusitem >= NUM_POWERUPS + NUM_WORLD_POWERUPS) {  // Score bonuses
            short iBonus = bonusitem - NUM_POWERUPS - NUM_WORLD_POWERUPS;
            short iBonusX = (iBonus % 10) << 5;
            short iBonusY = ((iBonus / 10) << 5) + 32;
            rm->spr_worlditems.draw(ix + 16, drawbonusitemy, iBonusX, iBonusY, 32, 32);
        } else if (bonusitem >= NUM_POWERUPS)  // World Item
            rm->spr_worlditems.draw(ix + 16, drawbonusitemy, (bonusitem - NUM_POWERUPS) << 5, 0, 32, 32);
        else  // Normal Powerup
            rm->spr_storedpoweruplarge.draw(ix + 16, drawbonusitemy, bonusitem << 5, 0, 32, 32);
    }

    if (state >= 2)
        spr->draw(ix, iy, 64, 0, iw, ih);
}

bool MO_BonusHouseChest::collide(CPlayer* player)
{
    if (state == 1 && !game_values.gamemode->gameover && player->playerKeys->game_turbo.fPressed) {
        if (bonusitem < NUM_POWERUPS + NUM_WORLD_POWERUPS) {
            if (game_values.worldpowerupcount[player->teamID] < 32)
                game_values.worldpowerups[player->teamID][game_values.worldpowerupcount[player->teamID]++] = bonusitem;
            else
                game_values.worldpowerups[player->teamID][31] = bonusitem;
        } else {
            short iBonus = bonusitem - NUM_POWERUPS - NUM_WORLD_POWERUPS;
            if (iBonus < 10)
                iBonus = iBonus + 1;
            else
                iBonus = 9 - iBonus;

            game_values.tournament_scores[player->teamID].total += iBonus;

            if (game_values.tournament_scores[player->teamID].total < 0)
                game_values.tournament_scores[player->teamID].total = 0;
        }

        ifSoundOnPlay(rm->sfx_treasurechest);
        state = 2;

        drawbonusitemy = iy + 32;
        drawbonusitemtimer = 75;

        game_values.flags.forceexittimer = 180;
        game_values.gamemode->gameover = true;
        game_values.gamemode->winningteam = player->teamID;
    }

    return false;
}

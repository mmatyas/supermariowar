#include "BonusHouse.h"

#include "Game.h"
#include "GameValues.h"
#include "ObjectContainer.h"
#include "player.h"
#include "RandomNumberGenerator.h"
#include "ResourceManager.h"
#include "WorldTourStop.h"
#include "objects/moving/MO_BonusHouseChest.h"

extern CObjectContainer objectcontainer[3];
extern std::vector<CPlayer*> players;
extern CResourceManager* rm;
extern CGameValues game_values;


//Bonus Mode (not really a game mode, but involves using the map so we need a mode to play)
CGM_Bonus::CGM_Bonus() : CGameMode()
{
    gamemode = game_mode_bonus;
    SetupModeStrings("Bonus", "", 0);
}

void CGM_Bonus::init()
{
    CGameMode::init();

    //Unlock the chests
    game_values.flags.noexit = true;

    //Will cause the flow to skip the scoreboard screen and go straight back to the world map
    game_values.worldskipscoreboard = true;

    //Add number of treasure chests to the bonus house
    tsTourStop = game_values.tourstops[game_values.tourstopcurrent];
    short iNumBonuses = tsTourStop->iNumBonuses;

    bool fChestUsed[MAX_BONUS_CHESTS];
    short iChestOrder[MAX_BONUS_CHESTS];
    short iNumChests = 0;

    if (tsTourStop->iBonusType == 0) {
        for (short iChest = 0; iChest < iNumBonuses; iChest++)
            iChestOrder[iNumChests++] = iChest;
    } else {
        for (short iChest = 0; iChest < iNumBonuses; iChest++)
            fChestUsed[iChest] = false;

        for (short iChest = 0; iChest < iNumBonuses; iChest++) {
            short iRandChest = RANDOM_INT(iNumBonuses);

            while (fChestUsed[iRandChest]) {
                if (++iRandChest >= iNumBonuses)
                    iRandChest = 0;
            }

            fChestUsed[iRandChest] = true;
            iChestOrder[iNumChests++] = iRandChest;
        }
    }

    float dSpacing = (384.0f - (float)(iNumBonuses * 64)) / (float)(iNumBonuses + 1);

    float dx = 128.0f + dSpacing;

    //float dx = 288.0f - (dSpacing * (float)(iNumBonuses - 1) / 2.0f);
    for (short iChest = 0; iChest < iNumBonuses; iChest++) {
        objectcontainer[0].add(new MO_BonusHouseChest(&rm->spr_worldbonushouse, (short)dx, 384, tsTourStop->wsbBonuses[iChestOrder[iChest]].iBonus));
        dx += dSpacing + 64.0f;
    }
}


void CGM_Bonus::draw_background()
{
    //Draw Toad
    rm->spr_worldbonushouse.draw(544, 256, players[0]->leftX() > 544 ? 224 : 192, 0, 32, 64);

    //Draw Bonus House Title
    rm->menu_plain_field.draw(0, 0, 0, 0, App::screenWidth/2, 32);
    rm->menu_plain_field.draw(App::screenWidth/2, 0, 192, 0, App::screenWidth/2, 32);
    rm->game_font_large.drawCentered(App::screenWidth/2, 5, tsTourStop->szName);

    //Draw Bonus House Text
    if (tsTourStop->iBonusTextLines > 0) {
        rm->spr_worldbonushouse.draw(128, 128, 0, 64, 384, 128);

        for (short iTextLine = 0; iTextLine < tsTourStop->iBonusTextLines; iTextLine++)
            rm->game_font_large.drawChopCentered(App::screenWidth/2, 132 + 24 * iTextLine, 372, tsTourStop->szBonusText[iTextLine]);
    }
}

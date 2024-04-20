#include "Frenzy.h"

#include "GameValues.h"
#include "ObjectContainer.h"
#include "player.h"
#include "RandomNumberGenerator.h"
#include "ResourceManager.h"
#include "objects/moving/MO_FrenzyCard.h"

extern short list_players_cnt;
extern CObjectContainer objectcontainer[3];
extern CResourceManager* rm;
extern CGameValues game_values;


//Fireball:
//Frag limit death match, but powerup cards appear randomly
CGM_Frenzy::CGM_Frenzy() : CGM_Frag()
{
    gamemode = game_mode_frenzy;
    szModeName = "Frenzy";
}

void CGM_Frenzy::init()
{
    CGameMode::init();
    timer = 0;

    iItemWeightCount = 0;
    for (short iPowerup = 0; iPowerup < NUMFRENZYCARDS; iPowerup++)
        iItemWeightCount += game_values.gamemodesettings.frenzy.powerupweight[iPowerup];

    setFrenzyOwner(nullptr);
}

void CGM_Frenzy::setFrenzyOwner(CPlayer* player)
{
    m_frenzyowner = player;
}

void CGM_Frenzy::think()
{
    if (gameover) {
        displayplayertext();
    } else {
        short iPowerupQuantity = game_values.gamemodesettings.frenzy.quantity;

        if ((iPowerupQuantity != 0 && ++timer >= game_values.gamemodesettings.frenzy.rate) || (iPowerupQuantity == 0 && !m_frenzyowner)) {
            timer = 0;

            if (0 == iPowerupQuantity)
                iPowerupQuantity = 1;
            if (5 < iPowerupQuantity)
                iPowerupQuantity = list_players_cnt + iPowerupQuantity - 7;

            if (objectcontainer[1].countTypes(object_frenzycard) < iPowerupQuantity) {
                if (iItemWeightCount == 0) {
                    //If all weights are zero, then choose the random powerup
                    iSelectedPowerup = NUMFRENZYCARDS - 1;
                } else {
                    //Randomly choose a powerup from the weighted list
                    int iRandPowerup = RANDOM_INT(iItemWeightCount) + 1;
                    iSelectedPowerup = 0;
                    int iWeightCount = game_values.gamemodesettings.frenzy.powerupweight[iSelectedPowerup];

                    while (iWeightCount < iRandPowerup)
                        iWeightCount += game_values.gamemodesettings.frenzy.powerupweight[++iSelectedPowerup];
                }

                objectcontainer[1].add(new MO_FrenzyCard(&rm->spr_frenzycards, iSelectedPowerup));
            }
        }
    }

    if (m_frenzyowner) {
        if (0 == iSelectedPowerup) {
            if (!m_frenzyowner->IsBobomb())
                m_frenzyowner = NULL;
        } else if (5 > iSelectedPowerup) {
            if (m_frenzyowner->powerup != iSelectedPowerup)
                m_frenzyowner = NULL;
        } else if (5 == iSelectedPowerup) {
            if (game_values.gamepowerups[m_frenzyowner->getGlobalID()] != 9)
                m_frenzyowner = NULL;
        } else if (6 == iSelectedPowerup) {
            if (game_values.gamepowerups[m_frenzyowner->getGlobalID()] != 16)
                m_frenzyowner = NULL;
        } else if (7 == iSelectedPowerup) {
            if (game_values.gamepowerups[m_frenzyowner->getGlobalID()] != 10)
                m_frenzyowner = NULL;
        }
    }
}

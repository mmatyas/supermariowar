#include "Survival.h"

#include "Game.h"
#include "GameValues.h"
#include "ObjectContainer.h"
#include "RandomNumberGenerator.h"
#include "ResourceManager.h"
#include "objects/moving/MO_Podobo.h"
#include "objects/overmap/WO_BowserFire.h"
#include "objects/overmap/WO_Thwomp.h"

extern CObjectContainer objectcontainer[3];
extern CResourceManager* rm;
extern CGameValues game_values;


//Survival Mode! - just like mario war classic, but you have
// to dodge thwomps from the sky.  Idea from ziotok.
CGM_Survival::CGM_Survival()
    : CGM_Classic()
{
    goal = 20;
    gamemode = game_mode_survival;
    szModeName = "Survival";
}

void CGM_Survival::init()
{
    CGM_Classic::init();

    rate = 3 * game_values.gamemodesettings.survival.density;
    timer = (short)(RANDOM_INT(21) - 10 + rate);
    ratetimer = 0;

    iEnemyWeightCount = 0;
    for (short iEnemy = 0; iEnemy < NUMSURVIVALENEMIES; iEnemy++)
        iEnemyWeightCount += game_values.gamemodesettings.survival.enemyweight[iEnemy];

    if (iEnemyWeightCount == 0)
        iEnemyWeightCount = 1;
}

void CGM_Survival::think()
{
    if (gameover) {
        displayplayertext();
    } else {
        if (--timer <= 0) {
            if (++ratetimer == 10) {
                ratetimer = 0;

                if (--rate < game_values.gamemodesettings.survival.density)
                    rate = game_values.gamemodesettings.survival.density;
            }

            //Randomly choose an enemy from the weighted list
            int iRandEnemy = RANDOM_INT(iEnemyWeightCount) + 1;
            iSelectedEnemy = 0;
            int iWeightCount = game_values.gamemodesettings.survival.enemyweight[iSelectedEnemy];

            while (iWeightCount < iRandEnemy)
                iWeightCount += game_values.gamemodesettings.survival.enemyweight[++iSelectedEnemy];

#pragma warning("Replace all these magic constants with proportional values")
            if (0 == iSelectedEnemy) {
                objectcontainer[2].add(new OMO_Thwomp(&rm->spr_thwomp, (short)RANDOM_INT(App::screenWidth * 0.92f), (float)game_values.gamemodesettings.survival.speed / 2.0f + (float)(RANDOM_INT(20))/10.0f));
                timer = (short)(RANDOM_INT(21) - 10 + rate);
            } else if (1 == iSelectedEnemy) {
                objectcontainer[2].add(new MO_Podobo(&rm->spr_podobo, (short)RANDOM_INT(App::screenWidth * 0.95f ), App::screenHeight, -(float(RANDOM_INT(9)) / 2.0f) - 8.0f, -1, -1, -1, false));
                timer = (short)(RANDOM_INT(21) - 10 + rate - 20);
            } else {
                float dSpeed = ((float)(RANDOM_INT(21) + 20)) / 10.0f;
                float dVel = RANDOM_BOOL() ? dSpeed : -dSpeed;

                short x = -54;
                if (dVel < 0)
                    x = 694;

                objectcontainer[2].add(new OMO_BowserFire(&rm->spr_bowserfire, {x, (short)RANDOM_INT( App::screenHeight * 0.93f )}, {dVel, 0.0f}, -1, -1, -1));
                timer = (short)(RANDOM_INT(21) - 10 + rate);
            }
        }
    }
}

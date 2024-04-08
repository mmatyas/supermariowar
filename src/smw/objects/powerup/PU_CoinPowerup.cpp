#include "PU_CoinPowerup.h"

#include "Game.h"
#include "GameMode.h"
#include "GameValues.h"
#include "player.h"
#include "ResourceManager.h"

extern CGameValues game_values;
extern CResourceManager* rm;

//------------------------------------------------------------------------------
// class special extra coin powerup for coin or greed mode
//------------------------------------------------------------------------------
PU_CoinPowerup::PU_CoinPowerup(gfxSprite* nspr, short x, short y, short color, short value)
    : MO_Powerup(nspr, x, y, 4, 8, 30, 30, 1, 1)
{
    velx = 0.0f;
    iColorOffsetY = color << 5;
    iValue = value;

    sparkleanimationtimer = 0;
    sparkledrawframe = 0;
}

void PU_CoinPowerup::update()
{
    MO_Powerup::update();

    if (++sparkleanimationtimer >= 4) {
        sparkleanimationtimer = 0;
        sparkledrawframe += 32;
        if (sparkledrawframe >= App::screenHeight)
            sparkledrawframe = 0;
    }
}

void PU_CoinPowerup::draw()
{
    if (state == 0) {
        short iHeight = (short)(32 - fy + desty);
        spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, iColorOffsetY, 32, iHeight);
        rm->spr_shinesparkle.draw(ix - collisionOffsetX, iy - collisionOffsetY, sparkledrawframe, 0, 32, iHeight);
    } else {
        spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, iColorOffsetY, 32, 32);
        rm->spr_shinesparkle.draw(ix - collisionOffsetX, iy - collisionOffsetY, sparkledrawframe, 0, 32, 32);
    }
}

bool PU_CoinPowerup::collide(CPlayer* player)
{
    if (state > 0) {
        if (game_values.gamemode->gamemode == game_mode_coins || game_values.gamemode->gamemode == game_mode_greed) {
            if (!game_values.gamemode->gameover) {
                player->Score().AdjustScore(iValue);
                game_values.gamemode->CheckWinner(player);
            }
        }

        ifSoundOnPlay(rm->sfx_coin);
        dead = true;
    }

    return false;
}

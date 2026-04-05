#include "WO_Phanto.h"

#include "Game.h"
#include "GameValues.h"
#include "player.h"
#include "RandomNumberGenerator.h"
#include "ResourceManager.h"
#include "gamemodes/Chase.h"

extern CGameValues game_values;
extern CResourceManager* rm;

//------------------------------------------------------------------------------
// class Phanto (for phanto mode)
//------------------------------------------------------------------------------
OMO_Phanto::OMO_Phanto(gfxSprite* nspr, Vec2s pos, float dVelX, float dVelY, short type)
    : IO_OverMapObject(nspr, pos, 1, 0, 30, 32, 1, 0, type << 5, 0, 32, 32)
    , iType(type)
{
    objectType = object_phanto;
    velx = dVelX;
    vely = dVelY;

    dMaxSpeedY = (float)game_values.gamemodesettings.chase.phantospeed / 2.0f;
    dMaxSpeedX = dMaxSpeedY + 1.0f;
    dSpeedRatio = (float)game_values.gamemodesettings.chase.phantospeed / 6.0f;

    dReactionSpeed = 0.2f;
}

void OMO_Phanto::update()
{
    setXf(fx + velx);
    setYf(fy + vely);

    if (fx < 0.0f)
        setXf(fx + App::screenWidth);
    else if (fx + iw >= App::screenWidth)
        setXf(fx - App::screenWidth);

    if (++iSpeedTimer > 62) {
        iSpeedTimer = 0;
        dReactionSpeed = (0.05f + (float)(RANDOM_INT(20)) / 100.0f) * dSpeedRatio;
    }

    // Chase player or move off screen if there is no player holding the key
    if (game_values.gamemode->gamemode == game_mode_chase) {
        CGM_Chase* chasemode = (CGM_Chase*)game_values.gamemode;

        CPlayer* player = chasemode->GetKeyHolder();

        if (!game_values.gamemode->gameover && player) {  // Chase the player
            bool fWrap = false;
            if (abs(player->ix - ix) > 320)
                fWrap = true;

            // Chase wrapped around edge of screen
            if ((player->ix < ix && !fWrap) || (player->ix > ix && fWrap)) {
                velx -= dReactionSpeed;

                if (velx < -dMaxSpeedX)
                    velx = -dMaxSpeedX;
            } else {
                velx += dReactionSpeed;

                if (velx > dMaxSpeedX)
                    velx = dMaxSpeedX;
            }

            if (player->iy < iy) {
                vely -= dReactionSpeed;

                if (vely < -dMaxSpeedY)
                    vely = -dMaxSpeedY;
            } else {
                vely += dReactionSpeed;

                if (vely > dMaxSpeedY)
                    vely = dMaxSpeedY;
            }
        } else {  // Wander off screen
            if (iy > 240) {
                vely += dReactionSpeed;

                if (vely > dMaxSpeedY)
                    vely = dMaxSpeedY;
            } else {
                vely -= dReactionSpeed;

                if (vely < -dMaxSpeedY)
                    vely = -dMaxSpeedY;
            }

            if (iy >= App::screenHeight || iy + ih < -CRUNCHMAX) {
                vely = 0.0f;
                velx = 0.0f;

                // Randomly position phanto off screen
                setXi(RANDOM_INT(App::screenWidth));
                setYi(RANDOM_BOOL() ? -ih - CRUNCHMAX : App::screenHeight);
            }
        }
    }
}

bool OMO_Phanto::collide(CPlayer* player)
{
    if (iy <= -ih || iy >= App::screenHeight)
        return false;

    // If the player is holding the key, kill him
    if (!player->isInvincible() && !player->isShielded()) {
        if (iType == 2) {
            player->KillPlayerMapHazard(false, KillStyle::Phanto, false);
            return true;
        } else {
            if (game_values.gamemode->gamemode == game_mode_chase) {
                CGM_Chase* chasemode = (CGM_Chase*)game_values.gamemode;
                CPlayer* keyholder = chasemode->GetKeyHolder();

                if (keyholder == player) {
                    player->KillPlayerMapHazard(false, KillStyle::Phanto, false);

                    if (!game_values.gamemode->gameover && iType == 1) {
                        player->Score().AdjustScore(-10);
                        ifSoundOnPlay(rm->sfx_stun);
                    }

                    return true;
                }
            }
        }
    }

    return false;
}

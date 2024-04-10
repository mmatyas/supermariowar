#include "PlayerWarpStatus.h"

#include "IO_Block.h"
#include "GameValues.h"
#include "map.h"
#include "player.h"
#include "ResourceManager.h"

#include <cassert>

extern CMap* g_map;
extern CGameValues game_values;
extern CResourceManager* rm;

PlayerWarpStatus::PlayerWarpStatus()
    : warpcounter(0)
    , warpconnection(0)
    , warpid(0)
    , warpplane(0)
{}

void PlayerWarpStatus::increasewarpcounter(CPlayer& player, short iGoal)
{
    if (++warpcounter > iGoal) {
        warpcounter = iGoal;
        chooseWarpExit(player);
    }
}

void PlayerWarpStatus::decreasewarpcounter(CPlayer& player)
{
    if (--warpcounter < 0) {
        warpcounter = 0;
        player.state = PlayerState::Ready;
    }
}

short PlayerWarpStatus::getWarpPlane() const {
    return warpplane;
}

void PlayerWarpStatus::update(CPlayer& player) {
    assert(player.iswarping());

    player.fOldY = player.fy;
    player.fOldX = player.fx;

    assert(warpcounter >= 0);

    switch (player.state) {
        case PlayerState::EnteringWarpLeft:
            player.setXi(player.leftX() - 1);
            increasewarpcounter(player, PW + PWOFFSET);
            break;
        case PlayerState::EnteringWarpRight:
            player.setXi(player.leftX() + 1);
            increasewarpcounter(player, PW + PWOFFSET);
            break;
        case PlayerState::EnteringWarpUp:
            player.setYi(player.topY() - 1);
            increasewarpcounter(player, TILESIZE - PHOFFSET);
            break;
        case PlayerState::EnteringWarpDown:
            player.setYi(player.topY() + 1);
            increasewarpcounter(player, PH + PHOFFSET);
            break;
        case PlayerState::LeavingWarpLeft:
            player.setXi(player.leftX() - 1);
            decreasewarpcounter(player);
            break;
        case PlayerState::LeavingWarpRight:
            player.setXi(player.leftX() + 1);
            decreasewarpcounter(player);
            break;
        case PlayerState::LeavingWarpUp:
            player.setYi(player.topY() - 1);
            decreasewarpcounter(player);
            break;
        case PlayerState::LeavingWarpDown:
            player.setYi(player.topY() + 1);
            decreasewarpcounter(player);
            break;
        default:
            assert(false);
            break;
    }

    assert(warpcounter >= 0);
}

void PlayerWarpStatus::enterWarp(CPlayer& player, Warp* warp)
{
    assert(warp->direction != WARP_UNDEFINED);

    switch (warp->direction) {
        case WARP_DOWN:
            player.state = PlayerState::EnteringWarpDown;
            player.vely = 0.0f;
            player.velx = 0.0f;
            warpplane = player.bottomY() + 1;
            break;
        case WARP_LEFT:
            player.state = PlayerState::EnteringWarpLeft;
            player.vely = 0.0f;
            player.velx = -1.0f;
            warpplane = player.leftX();
            break;
        case WARP_UP:
            player.state = PlayerState::EnteringWarpUp;
            player.vely = 0.0f;
            player.velx = 0.0f;
            warpplane = player.topY();
            break;
        case WARP_RIGHT:
            player.state = PlayerState::EnteringWarpRight;
            player.vely = 0.0f;
            player.velx = 1.0f;
            warpplane = player.rightX() + 1;
            break;
        default:
            assert(false);
            break;
    }
    player.oldvelx = player.velx;

    warpconnection = warp->connection;
    warpid = warp->id;

    if (game_values.warplocktime > 0) {
        switch (game_values.warplockstyle) {
            case WarpLockStyle::EntranceOnly:
            case WarpLockStyle::EntranceAndExit:
                g_map->warpexits[warp->id].locktimer = game_values.warplocktime;
                break;
            case WarpLockStyle::EntireConnection:
                g_map->lockconnection(warpconnection);
                break;
            case WarpLockStyle::AllWarps:
                g_map->lockconnection(-1);
                break;
            default:
                break;
        }
    }

    ifSoundOnPlay(rm->sfx_pipe);
}

void PlayerWarpStatus::chooseWarpExit(CPlayer& player)
{
    WarpExit * exit = g_map->getRandomWarpExit(warpconnection, warpid);
    assert(exit);
    player.setXi(exit->x);
    player.setYi(exit->y);
    player.fOldX = player.fx;
    player.fOldY = player.fy;
    player.oldvelx = player.velx;

    player.lockjump = false;
    player.ClearPowerupStates();

    //Trigger block that we warp into
    short iCol = player.leftX() / TILESIZE;
    short iRow = player.topY() / TILESIZE;
    IO_Block* block = NULL;

    assert(exit->direction != WARP_EXIT_UNDEFINED);

    switch(exit->direction) {
        case WARP_EXIT_UP:
            player.state = PlayerState::LeavingWarpUp;
            player.velx = 0.0f;
            player.vely = -4.0f;
            warpcounter = PH + PHOFFSET;
            warpplane = exit->warpy << 5;

            if (iRow - 1 >= 0) {
                block = g_map->block(iCol, iRow - 1);

                if (block && !block->isTransparent() && !block->isHidden())
                    block->triggerBehavior();
            }
            break;
        case WARP_EXIT_RIGHT:
            player.state = PlayerState::LeavingWarpRight;
            player.velx = 1.0f;
            player.vely = 1.0f;
            warpcounter = PW + PWOFFSET;
            warpplane = (exit->warpx << 5) + TILESIZE;

            if (iCol + 1 >= 20)
                iCol -= 20;

            block = g_map->block(iCol + 1, iRow);

            if (block && !block->isTransparent() && !block->isHidden())
                block->triggerBehavior();
            break;
        case WARP_EXIT_DOWN:
            player.state = PlayerState::LeavingWarpDown;
            player.velx = 0.0f;
            player.vely = 1.1f;
            player.inair = true;
            warpcounter = TILESIZE - PHOFFSET;
            warpplane = (exit->warpy << 5) + TILESIZE;

            if (iRow + 1 < 15) {
                block = g_map->block(iCol, iRow + 1);

                if (block && !block->isTransparent() && !block->isHidden())
                    block->triggerBehavior();
            }
            break;
        case WARP_EXIT_LEFT:
            player.state = PlayerState::LeavingWarpLeft;
            player.velx = -1.0f;
            player.vely = 1.0f;
            warpcounter = PW + PWOFFSET;
            warpplane = (exit->warpx << 5);

            if (iCol - 1 < 0)
                iCol += 20;

            block = g_map->block(iCol - 1, iRow);

            if (block && !block->isTransparent() && !block->isHidden())
                block->triggerBehavior();
            break;
        default:
            assert(false);
            break;
    }

    //Make player shielded when exiting the warp (if that option is turned on)
    if (game_values.shieldstyle != ShieldStyle::NoShield) {
        if (!player.isShielded() || player.shield.time_left() < game_values.shieldtime) {
            player.shield.reset();
        }
    }

    //Lock the warp (if that option is turned on)
    if (game_values.warplocktime > 0) {
        if (game_values.warplockstyle == WarpLockStyle::ExitOnly || game_values.warplockstyle == WarpLockStyle::EntranceAndExit) //Lock the warp exit
            exit->locktimer = game_values.warplocktime;
    }
}

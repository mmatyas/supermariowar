#include "PlayerCollisions.h"

#include "IO_Block.h"
#include "gamemodes.h"
#include "GameValues.h"
#include "map.h"
#include "player.h"
#include "ResourceManager.h"

extern CMap* g_map;

extern CGameValues game_values;
extern CResourceManager* rm;

bool PlayerCollisions::checktop(CPlayer& player)
{
    if (player.topY() < 0.0f)
        return false;

    short tile_y = player.topY() / TILESIZE;

    if (tile_y < 0 || tile_y >= MAPHEIGHT)
        return false;

    short tile_x_left = player.leftX() / TILESIZE;

    if (tile_x_left < 0 || tile_x_left >= MAPWIDTH)
        return false;

    short tile_x_right = -1;
    if (player.rightX() >= 640)
        tile_x_right = (player.rightX() - 640) / TILESIZE;
    else
        tile_x_right = player.rightX() / TILESIZE;

    if (tile_x_right < 0 || tile_x_right >= MAPWIDTH)
        return false;

    int leftTile = g_map->map(tile_x_left, tile_y);
    int rightTile = g_map->map(tile_x_right, tile_y);
    IO_Block * leftBlock = g_map->block(tile_x_left, tile_y);
    IO_Block * rightBlock = g_map->block(tile_x_right, tile_y);

    if ((leftTile & tile_flag_solid) || (rightTile & tile_flag_solid) ||
            (leftBlock && !leftBlock->isTransparent() && !leftBlock->isHidden()) ||
            (rightBlock && !rightBlock->isTransparent() && !rightBlock->isHidden())) {
        player.setYf((float)((tile_y << 5) + TILESIZE) + 0.2f);
        return true;
    }

    return false;
}

bool PlayerCollisions::checkleft(CPlayer& player)
{
    if (player.fy < 0.0f)
        return false;

    short tile_y = (short)player.fy / TILESIZE;

    if (tile_y < 0 || tile_y >= MAPHEIGHT)
        return false;

    short tile_y2 = ((short)player.fy + PH) / TILESIZE;

    if (tile_y2 < 0 || tile_y2 >= MAPHEIGHT)
        return false;

    short tile_x = player.ix / TILESIZE;

    if (tile_x < 0 || tile_x >= MAPWIDTH)
        return false;

    int topTile = g_map->map(tile_x, tile_y);
    int bottomTile = g_map->map(tile_x, tile_y2);
    IO_Block * topBlock = g_map->block(tile_x, tile_y);
    IO_Block * bottomBlock = g_map->block(tile_x, tile_y2);

    if ((topTile & tile_flag_solid) || (bottomTile & tile_flag_solid) ||
            (topBlock && !topBlock->isTransparent() && !topBlock->isHidden()) ||
            (bottomBlock && !bottomBlock->isTransparent() && !bottomBlock->isHidden())) {
        player.setXf((float)((tile_x << 5) + TILESIZE) + 0.2f);
        player.flipsidesifneeded();
        return true;
    }

    return false;
}

bool PlayerCollisions::checkright(CPlayer& player)
{
    if (player.fy < 0.0f)
        return false;

    short tile_y = (short)player.fy / TILESIZE;

    if (tile_y < 0 || tile_y >= MAPHEIGHT)
        return false;

    short tile_y2 = ((short)player.fy + PH) / TILESIZE;

    if (tile_y2 < 0 || tile_y2 >= MAPHEIGHT)
        return false;

    short tile_x = -1;

    if (player.rightX() >= 640)
        tile_x = (player.rightX() - 640) / TILESIZE;
    else
        tile_x = player.rightX() / TILESIZE;

    if (tile_x < 0 || tile_x >= MAPWIDTH)
        return false;

    int topTile = g_map->map(tile_x, tile_y);
    int bottomTile = g_map->map(tile_x, tile_y2);
    IO_Block * topBlock = g_map->block(tile_x, tile_y);
    IO_Block * bottomBlock = g_map->block(tile_x, tile_y2);

    if ((topTile & tile_flag_solid) || (bottomTile & tile_flag_solid) ||
            (topBlock && !topBlock->isTransparent() && !topBlock->isHidden()) ||
            (bottomBlock && !bottomBlock->isTransparent() && !bottomBlock->isHidden())) {
        player.setXf((float)((tile_x << 5) - PW) - 0.2f);
        player.flipsidesifneeded();
        return true;
    }

    return false;
}

void PlayerCollisions::checksides(CPlayer& player)
{
    //First figure out where the corners of this object are touching
    Uint8 iCase = 0;

    short tile_x_left = player.leftX() >> 5;

    short tile_x_right = -1;
    if (player.rightX() >= 640)
        tile_x_right = (player.rightX() - 640) >> 5;
    else
        tile_x_right = player.rightX() >> 5;

    short tile_y = player.topY() >> 5;
    short tile_y2 = player.bottomY() >> 5;

    if (player.topY() >= 0) {
        if (tile_y < MAPHEIGHT) {
            if (tile_x_left >= 0 && tile_x_left < MAPWIDTH) {
                IO_Block * block = g_map->block(tile_x_left, tile_y);

                if ((block && !block->isTransparent() && !block->isHidden()) || (g_map->map(tile_x_left, tile_y) & tile_flag_solid)) {
                    iCase |= 0x01;
                }
            }

            if (tile_x_right >= 0 && tile_x_right < MAPWIDTH) {
                IO_Block * block = g_map->block(tile_x_right, tile_y);

                if ((block && !block->isTransparent() && !block->isHidden()) || (g_map->map(tile_x_right, tile_y) & tile_flag_solid)) {
                    iCase |= 0x02;
                }
            }
        }

    }

    // TODO: is this correct?
    if (player.topY() + PW >= 0.0f) {
        if (tile_y2 < MAPHEIGHT) {
            if (tile_x_left >= 0 && tile_x_left < MAPWIDTH) {
                IO_Block * block = g_map->block(tile_x_left, tile_y2);

                if ((block && !block->isTransparent() && !block->isHidden()) || (g_map->map(tile_x_left, tile_y2) & tile_flag_solid)) {
                    iCase |= 0x04;
                }
            }

            if (tile_x_right >= 0 && tile_x_right < MAPWIDTH) {
                IO_Block * block = g_map->block(tile_x_right, tile_y2);

                if ((block && !block->isTransparent() && !block->isHidden()) || (g_map->map(tile_x_right, tile_y2) & tile_flag_solid)) {
                    iCase |= 0x08;
                }
            }
        }
    }

    //Then determine which way is the best way to move this object out of the solid map areas
    switch (iCase) {
        //Do nothing
        //[ ][ ]
        //[ ][ ]
    case 0:
        break;

        //[X][ ]
        //[ ][ ]
    case 1: {
        if (player.leftX() + (PW >> 1) > (tile_x_left << 5) + TILESIZE) {
            player.setXf((float)((tile_x_left << 5) + TILESIZE) + 0.2f);
            player.flipsidesifneeded();
        } else {
            player.setYf((float)((tile_y << 5) + TILESIZE) + 0.2f);
        }

        break;
    }

    //[ ][X]
    //[ ][ ]
    case 2: {
        if (player.leftX() + (PW >> 1) < (tile_x_right << 5)) {
            player.setXf((float)((tile_x_right << 5) - PW) - 0.2f);
            player.flipsidesifneeded();
        } else {
            player.setYf((float)((tile_y << 5) + TILESIZE) + 0.2f);
        }

        break;
    }

    //[X][X]
    //[ ][ ]
    case 3: {
        player.setYf((float)((tile_y << 5) + TILESIZE) + 0.2f);
        break;
    }

    //[ ][ ]
    //[X][ ]
    case 4: {
        if (player.leftX() + (PW >> 1) > (tile_x_left << 5) + TILESIZE) {
            player.setXf((float)((tile_x_left << 5) + TILESIZE) + 0.2f);
            player.flipsidesifneeded();
        } else {
            player.setYf((float)((tile_y2 << 5) - PH) - 0.2f);
        }

        break;
    }

    //[X][ ]
    //[X][ ]
    case 5: {
        player.setXf((float)((tile_x_left << 5) + TILESIZE) + 0.2f);
        player.flipsidesifneeded();
        break;
    }

    //[ ][X]
    //[X][ ]
    case 6: {
        if (player.leftX() + (PW >> 1) > (tile_x_left << 5) + TILESIZE) {
            player.setYf((float)((tile_y << 5) + TILESIZE) + 0.2f);
            player.setXf((float)((tile_x_left << 5) + TILESIZE) + 0.2f);
            player.flipsidesifneeded();
        } else {
            player.setYf((float)((tile_y2 << 5) - PH) - 0.2f);
            player.setXf((float)((tile_x_right << 5) - PW) - 0.2f);
            player.flipsidesifneeded();
        }

        break;
    }

    //[X][X]
    //[X][ ]
    case 7: {
        player.setYf((float)((tile_y << 5) + TILESIZE) + 0.2f);
        player.setXf((float)((tile_x_left << 5) + TILESIZE) + 0.2f);
        player.flipsidesifneeded();
        break;
    }

    //[ ][ ]
    //[ ][X]
    case 8: {
        if (player.leftX() + (PW >> 1) < (tile_x_right << 5)) {
            player.setXf((float)((tile_x_right << 5) - PW) - 0.2f);
            player.flipsidesifneeded();
        } else {
            player.setYf((float)((tile_y2 << 5) - PH) - 0.2f);
        }

        break;
    }

    //[X][ ]
    //[ ][X]
    case 9: {
        if (player.leftX() + (PW >> 1) > (tile_x_left << 5) + TILESIZE) {
            player.setYf((float)((tile_y2 << 5) - PH) - 0.2f);
            player.setXf((float)((tile_x_left << 5) + TILESIZE) + 0.2f);
            player.flipsidesifneeded();
        } else {
            player.setYf((float)((tile_y << 5) + TILESIZE) + 0.2f);
            player.setXf((float)((tile_x_right << 5) - PW) - 0.2f);
            player.flipsidesifneeded();
        }

        break;
    }

    //[ ][X]
    //[ ][X]
    case 10: {
        player.setXf((float)((tile_x_right << 5) - PW) - 0.2f);
        player.flipsidesifneeded();
        break;
    }

    //[X][X]
    //[ ][X]
    case 11: {
        player.setYf((float)((tile_y << 5) + TILESIZE) + 0.2f);
        player.setXf((float)((tile_x_right << 5) - PW) - 0.2f);
        player.flipsidesifneeded();
        break;
    }

    //[ ][ ]
    //[X][X]
    case 12: {
        player.setYf((float)((tile_y2 << 5) - PH) - 0.2f);
        break;
    }

    //[X][ ]
    //[X][X]
    case 13: {
        player.setYf((float)((tile_y2 << 5) - PH) - 0.2f);
        player.setXf((float)((tile_x_left << 5) + TILESIZE) + 0.2f);
        player.flipsidesifneeded();
        break;
    }

    //[ ][X]
    //[X][X]
    case 14: {
        player.setYf((float)((tile_y2 << 5) - PH) - 0.2f);
        player.setXf((float)((tile_x_right << 5) - PW) - 0.2f);
        player.flipsidesifneeded();
        break;
    }

    //If object is completely inside a block, default to moving it down
    //[X][X]
    //[X][X]
    case 15: {
        player.setYf((float)((tile_y2 << 5) + TILESIZE) + 0.2f);
        break;
    }

    default:
        break;
    }
}

bool PlayerCollisions::is_stomping(CPlayer& player, CPlayer& other)
{
    if (player.fOldY + PH <= other.fOldY && player.bottomY() >= other.topY()) {
        //don't reposition if player is warping when he kills the other player
        if (player.isready()) {
            player.setYi(other.iy - PH);      //set new position to top of other player
            player.collisions.checktop(player);
            player.platform = NULL;
        }

        bool fKillPotential = false;
        if (player.vely > 1.0f && !other.isShielded())
            fKillPotential = true;

        player.bouncejump();

        if (fKillPotential) {
            killstyle style = kill_style_stomp;
            if (player.flying)
                style = kill_style_pwings;
            else if (player.kuriboshoe.is_on())
                style = kill_style_kuriboshoe;
            else if (player.tail.isInUse())
                style = kill_style_leaf;
            else if (player.extrajumps > 0)
                style = kill_style_feather;

            player.KilledPlayer(&other, death_style_squish, style, false, false);
        } else {
            if (game_values.gamemode->gamemode == game_mode_tag)
                player.TransferTag(&other);

            if (game_values.gamemode->gamemode == game_mode_shyguytag)
                player.TransferShyGuy(&other);

            player.iSuicideCreditPlayerID = other.globalID;
            player.iSuicideCreditTimer = 20;
        }

        return true;
    }

    return false;
}

//handles a collision between two players (is being called if o1, o2 collide)
void PlayerCollisions::handle_p2p(CPlayer* o1, CPlayer* o2)
{
    //If teams tag each other
    if (o1->teamID == o2->teamID) {
        //Free teammates that are jailed
        if (game_values.gamemode->gamemode == game_mode_jail && game_values.gamemodesettings.jail.tagfree) {
            o1->jail.free_by_teammate(*o1);
            o2->jail.free_by_teammate(*o2);
        }

        //Transfer tag if assist is on
        if ((game_values.gamemode->gamemode == game_mode_tag && game_values.teamcollision == 1) || game_values.gamemodesettings.tag.tagontouch)
            o1->TransferTag(o2);

        //Don't collision detect players on same team if friendly fire is turned off
        if (game_values.teamcollision == 0)
            return;

        //Team assist is enabled so allow powerup trading and super jumping
        if (game_values.teamcollision == 1) {
            o1->BounceAssistPlayer(o2);
            o2->BounceAssistPlayer(o1);

            //Allow players on team to swap stored items
            if (o1->playerKeys->game_powerup.fPressed || o2->playerKeys->game_powerup.fPressed) {
                short iTempPowerup = game_values.gamepowerups[o1->globalID];
                game_values.gamepowerups[o1->globalID] = game_values.gamepowerups[o2->globalID];
                game_values.gamepowerups[o2->globalID] = iTempPowerup;

                ifSoundOnPlay(rm->sfx_storepowerup);

                o1->playerKeys->game_powerup.fPressed = false;
                o1->playerKeys->game_powerup.fDown = false;
                o2->playerKeys->game_powerup.fPressed = false;
                o2->playerKeys->game_powerup.fDown = false;
            }

            return;
        }
    }

    //Quit checking collision if either player is soft shielded
    if (o1->shield.getType() == SOFT || o2->shield.getType() == SOFT) {
        //Do tag transfer if there is one to do
        if (game_values.gamemode->gamemode == game_mode_tag && game_values.gamemodesettings.tag.tagontouch)
            o1->TransferTag(o2);

        if (game_values.gamemode->gamemode == game_mode_shyguytag && game_values.gamemodesettings.shyguytag.tagtransfer != 1)
            o1->TransferShyGuy(o2);

        return;
    }

    //--- 1. kill frozen players ---
    bool fFrozenDeath = false;
    if (o1->frozen && !o1->isShielded() && !o1->isInvincible()) {
        o2->KilledPlayer(o1, death_style_shatter, kill_style_iceblast, true, false);
        fFrozenDeath = true;
    }

    if (o2->frozen && !o2->isShielded() && !o2->isInvincible()) {
        o1->KilledPlayer(o2, death_style_shatter, kill_style_iceblast, true, false);
        fFrozenDeath = true;
    }

    if (fFrozenDeath)
        return;

    //--- 2. is player invincible? ---
    if (o1->isInvincible() && !o2->isShielded() && !o2->isInvincible()) {
        o1->KilledPlayer(o2, death_style_jump, kill_style_star, false, false);
        return;
    }

    if (o2->isInvincible() && !o1->isShielded() && !o1->isInvincible()) {
        o2->KilledPlayer(o1, death_style_jump, kill_style_star, false, false);
        return;
    }

    //If both players are warping, ignore collision
    if (o1->iswarping() && o2->iswarping())
        return;

    //--- 3. stomping other player? ---
    if ((o2->shield.getType() == OFF || o2->shield.getType() == HARD) && !o2->isInvincible() && is_stomping(*o1, *o2))
        return;
    if ((o1->shield.getType() == OFF || o1->shield.getType() == HARD) && !o1->isInvincible() && is_stomping(*o2, *o1))
        return;


    //Quit checking collision if either player is hard shielded
    if (o1->shield.getType() == SOFT_WITH_STOMP || o2->shield.getType() == SOFT_WITH_STOMP) {
        //Do tag transfer if there is one to do
        if (game_values.gamemode->gamemode == game_mode_tag && game_values.gamemodesettings.tag.tagontouch)
            o1->TransferTag(o2);

        if (game_values.gamemode->gamemode == game_mode_shyguytag && game_values.gamemodesettings.shyguytag.tagtransfer != 1)
            o1->TransferShyGuy(o2);

        return;
    }

    //--- 4. push back (horizontal) ---
    if (o1->ix < o2->ix)                //o1 is left -> o1 pushback left, o2 pushback right
        handle_p2p_pushback(o1, o2);
    else
        handle_p2p_pushback(o2, o1);
}

//handles a collision between a player and an object
bool PlayerCollisions::handle_p2o(CPlayer * o1, CObject * o2)
{
    return o2->collide(o1);
}

//calculates the new positions for both players when they are pushing each other
void PlayerCollisions::handle_p2p_pushback(CPlayer * o1, CPlayer * o2)
{
    //o1 is left to o2
    //  |o1||o2|
    //-----------

    //Transfer tag on touching other players
    if (game_values.gamemode->gamemode == game_mode_tag && game_values.gamemodesettings.tag.tagontouch)
        o1->TransferTag(o2);

    if (game_values.gamemode->gamemode == game_mode_shyguytag && game_values.gamemodesettings.shyguytag.tagtransfer != 1)
        o1->TransferShyGuy(o2);

    bool overlapcollision = false;
    if (o1->ix + PW < 320 && o2->ix > 320)
        overlapcollision = true;

    if (/*(o1->velx == 0 && o2->iswarping() && o2->velx != 0) ||*/ o1->iswarping()) {
        if (overlapcollision) {
            //o2 reposition to the right side of o1, o1 stays
            o2->setXi(o1->ix - PW + smw->ScreenWidth - 1);
            o2->collisions.checkleft(*o2);
        } else {
            o2->setXi(o1->ix + PW + 1);
            o2->collisions.checkright(*o2);
        }

        return;
    } else if (/*(o2->velx == 0 && o1->iswarping() && o1->velx != 0) ||*/ o2->iswarping()) {
        if (overlapcollision) {
            //o1 reposition to the left side of o2, o2 stays
            o1->setXi(o2->ix + PW - smw->ScreenWidth - 1);
            o1->collisions.checkright(*o1);
        } else {
            o1->setXi(o2->ix - PW - 1);
            o1->collisions.checkleft(*o1);
        }

        return;
    } else if (!o1->iswarping() && !o2->iswarping()) {
        //both objects moving - calculate middle and set both objects

        if (overlapcollision) {
            short middle = o2->ix - smw->ScreenWidth + ((o1->ix + PW) - o2->ix - smw->ScreenWidth) / 2;     //no ABS needed (o1->x < o2->x -> o1->x+w > o2->x !)
            o1->setXi(middle + 1);  //o1 is left
            o2->setXi(middle - PW + smw->ScreenWidth - 1);      //o2 is right

            o1->collisions.checkright(*o1);
            o2->collisions.checkleft(*o2);
        } else {
            short middle = o2->ix + ((o1->ix + PW) - o2->ix) / 2;       //no ABS needed (o1->x < o2->x -> o1->x+w > o2->x !)
            //printf("hlf:%f, o1x:%f, o2x:%f\n", hlf, o1->x, o2->x);
            o1->setXi(middle - PW - 1); //o1 is left
            o2->setXi(middle + 1);      //o2 is right

            o1->collisions.checkleft(*o1);
            o2->collisions.checkright(*o2);
        }
    }

    float absv1 = 0.0f;
    float absv2 = 0.0f;

    float dPlayer1Pushback = 1.5f;
    float dPlayer2Pushback = 1.5f;

    if (o1->kuriboshoe.is_on() && !o2->kuriboshoe.is_on()) {
        dPlayer1Pushback = 0.5f;
        dPlayer2Pushback = 2.5f;
    } else if (!o1->kuriboshoe.is_on() && o2->kuriboshoe.is_on()) {
        dPlayer1Pushback = 2.5f;
        dPlayer2Pushback = 0.5f;
    }

    if (overlapcollision) {
        absv1 = ( o1->velx < 0 ? o1->velx : -1.0f ) * dPlayer2Pushback; //o1 is on the left side (only positive velx counts)
        absv2 = ( o2->velx > 0 ? o2->velx : 1.0f ) * dPlayer1Pushback;  //o2 right (only negative velx counts)
    } else {
        absv1 = ( o1->velx > 0 ? o1->velx : 1.0f ) * dPlayer2Pushback;  //o1 is on the left side (only positive velx counts)
        absv2 = ( o2->velx < 0 ? o2->velx : -1.0f ) * dPlayer1Pushback; //o2 right (only negative velx counts)
    }

    if (o1->state == player_ready) {
        o1->velx = CapSideVelocity(absv2);
        o1->iSuicideCreditPlayerID = o2->globalID;
        o1->iSuicideCreditTimer = 62;
    }

    if (o2->state == player_ready) {
        o2->velx = CapSideVelocity(absv1);
        o2->iSuicideCreditPlayerID = o1->globalID;
        o2->iSuicideCreditTimer = 62;
    }
}

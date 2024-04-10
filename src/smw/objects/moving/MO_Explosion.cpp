#include "MO_Explosion.h"

#include "Game.h"
#include "GameValues.h"
#include "map.h"
#include "player.h"
#include "objects/blocks/WeaponBreakableBlock.h"

extern CMap* g_map;
extern CGameValues game_values;


//------------------------------------------------------------------------------
// class explosion (for bob-omb mode)
//------------------------------------------------------------------------------
MO_Explosion::MO_Explosion(gfxSprite* nspr, short x, short y, short iNumSpr, short aniSpeed, short playerid, short teamid, KillStyle style)
    : IO_MovingObject(nspr, x, y, iNumSpr, aniSpeed)
{
    state = 1;

    iPlayerID = playerid;
    iTeamID = teamid;
    timer = 0;
    movingObjectType = movingobject_explosion;
    iStyle = style;

    fObjectCollidesWithMap = false;
}

bool MO_Explosion::collide(CPlayer* player)
{
    if (player->globalID != iPlayerID && (game_values.teamcollision == TeamCollisionStyle::On || iTeamID != player->teamID) && !player->isInvincible() && !player->isShielded() && !player->shyguy) {
        // Find the player that made this explosion so we can attribute a kill
        PlayerKilledPlayer(iPlayerID, player, PlayerDeathStyle::Jump, iStyle, false, false);
        return true;
    }

    return false;
}

void MO_Explosion::update()
{
    animate();

    // If this is the first frame, look for blocks to kill
    if (timer == 0) {
        short iTestY = iy;

        for (short iRow = 0; iRow < 5; iRow++) {
            short iTestX = ix;

            if (iTestX < 0)
                iTestX += App::screenWidth;

            if (iTestY >= 0 && iTestY < App::screenHeight) {
                short iTestRow = iTestY / TILESIZE;
                for (short iCol = 0; iCol < 7; iCol++) {
                    IO_Block* block = g_map->block(iTestX / TILESIZE, iTestRow);
                    if (block && block->getBlockType() == BlockType::WeaponBreakable) {
                        B_WeaponBreakableBlock* weaponbreakableblock = (B_WeaponBreakableBlock*)block;
                        if (weaponbreakableblock->iType == 3) {
                            weaponbreakableblock->triggerBehavior(iPlayerID, iTeamID);
                        }
                    }

                    iTestX += TILESIZE;

                    if (iTestX >= App::screenWidth)
                        iTestX -= App::screenWidth;
                }
            }


            iTestY += TILESIZE;

            if (iTestY >= App::screenHeight)
                break;
        }
    }

    // RFC: why 48?
    if (++timer >= 48)
        dead = true;
}

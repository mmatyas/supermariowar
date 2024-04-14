#include "MO_Hammer.h"

#include "eyecandy.h"
#include "GameValues.h"
#include "player.h"
#include "objects/blocks/WeaponBreakableBlock.h"

extern SpotlightManager spotlightManager;
extern CGameValues game_values;

//------------------------------------------------------------------------------
// class hammer
//------------------------------------------------------------------------------
MO_Hammer::MO_Hammer(gfxSprite* nspr, short x, short y, short iNumSpr, float fVelyX, float fVelyY, short aniSpeed, short iGlobalID, short teamID, short iColorID, bool superHammer)
    : IO_MovingObject(nspr, x, y, iNumSpr, aniSpeed, (short)nspr->getWidth() / iNumSpr, (short)nspr->getHeight() >> 2, 0, 0)
{
    ih = ih >> 2;

    iPlayerID = iGlobalID;
    iTeamID = teamID;
    // RFC
    colorOffset = iColorID * 28;
    movingObjectType = movingobject_hammer;

    state = 1;

    velx = fVelyX;
    vely = fVelyY;
    ttl = game_values.hammerttl;

    fSuper = superHammer;

    if (velx > 0.0f)
        drawframe = 0;
    else
        drawframe = animationWidth - iw;

    fObjectCollidesWithMap = false;

    sSpotlight = NULL;
}

void MO_Hammer::update()
{
    if (++animationtimer == animationspeed) {
        animationtimer = 0;

        if (velx > 0) {
            drawframe += iw;
            if (drawframe >= animationWidth)
                drawframe = 0;
        } else {
            drawframe -= iw;
            if (drawframe < 0)
                drawframe = animationWidth - iw;
        }
    }

    setXf(fx + velx);
    setYf(fy + vely);

    if (!fSuper)
        vely += GRAVITATION;

    if (ix < 0)
        setXi(ix + App::screenWidth);
    else if (ix > App::screenWidth - 1)
        setXi(ix - App::screenWidth);

    if (iy > App::screenHeight || --ttl <= 0 || (fSuper && iy < -ih)) {
        removeifprojectile(this, false, true);
    } else if (game_values.spotlights) {
        if (!sSpotlight) {
            sSpotlight = spotlightManager.AddSpotlight(ix - collisionOffsetX + (iw >> 1), iy - collisionOffsetY + (ih >> 1), 3);
        }

        if (sSpotlight) {
            sSpotlight->UpdatePosition(ix - collisionOffsetX + (iw >> 1), iy - collisionOffsetY + (ih >> 1));
        }
    }

    // Detection collision with hammer breakable blocks
    const std::array<IO_Block*, 4> blocks = GetCollisionBlocks();
    for (short iBlock = 0; iBlock < 4; iBlock++) {
        if (blocks[iBlock] && blocks[iBlock]->getBlockType() == BlockType::WeaponBreakable) {
            B_WeaponBreakableBlock* weaponbreakableblock = (B_WeaponBreakableBlock*)blocks[iBlock];
            if (weaponbreakableblock->iType == 5) {
                weaponbreakableblock->triggerBehavior(iPlayerID, iTeamID);
                removeifprojectile(this, false, false);
                return;
            }
        }
    }
}

bool MO_Hammer::collide(CPlayer* player)
{
    if (iPlayerID != player->globalID && (game_values.teamcollision == TeamCollisionStyle::On || iTeamID != player->teamID)) {
        if (!player->isShielded()) {
            removeifprojectile(this, false, false);

            if (!player->isInvincible() && !player->shyguy) {
                // Find the player that shot this hammer so we can attribute a kill
                PlayerKilledPlayer(iPlayerID, player, PlayerDeathStyle::Jump, KillStyle::Hammer, false, false);
                return true;
            }
        }
    }

    return false;
}

void MO_Hammer::draw()
{
    spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, colorOffset, iw, ih);
}

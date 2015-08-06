#include "PlayerCape.h"

#include "../player.h"

#include "GameValues.h"
#include "GlobalConstants.h"
#include "ObjectContainer.h"
#include "objectgame.h"
#include "ResourceManager.h"

extern CGameValues game_values;
extern CResourceManager* rm;

extern CObjectContainer objectcontainer[3];

void PlayerCape::reset()
{
    iCapeTimer = 0;
    iCapeFrameX = 0;
    iCapeFrameY = 0;
    fCapeUp = false;
    iCapeYOffset = 0;
}

void PlayerCape::restart_animation()
{
    iCapeTimer = 4;
}

void PlayerCape::spin(CPlayer &player)
{
    ifSoundOnPlay(rm->sfx_tailspin);

    restart_animation(); //Add one extra frame to sync with spinning player sprite

    player.spin.spin(player);

    objectcontainer[1].add(new MO_SpinAttack(player.getGlobalID(), player.getTeamID(),  kill_style_feather, player.isFacingRight(), 24));
}

void PlayerCape::draw(CPlayer &player)
{
    /* See eyecandy/cape.png for reference */

    if (++iCapeTimer > 3) {
        if (player.spin.isSpinInProgress()) {
            iCapeFrameX = player.spin.toCapeFrameX();
            iCapeFrameY = 32;
            iCapeYOffset = -8;
        } else if ((!player.inair && player.velx != 0.0f) || (player.inair && player.vely < 1.0f)) {
            iCapeFrameX += 32;
            if (iCapeFrameX > 96)
                iCapeFrameX = 0;

            iCapeFrameY = 0;
            fCapeUp = true;
            iCapeYOffset = 0;
        } else if (!player.inair) {
            if (fCapeUp) {
                fCapeUp = false;
                iCapeFrameX = 0;
            } else {
                iCapeFrameX = 32;
            }

            iCapeFrameY = 96;
            iCapeYOffset = 0;
        } else if (player.inair) {
            iCapeFrameX += 32;
            if (iCapeFrameX > 64)
                iCapeFrameX = 0;

            iCapeFrameY = 64;
            fCapeUp = true;
            iCapeYOffset = -18;
        }

        iCapeTimer = 0;
    }

    bool fPlayerFacingRight = !game_values.reversewalk;
    if (player.spin.isSpinInProgress()) {
        if (player.spin.isReverseWalking())
            fPlayerFacingRight = game_values.reversewalk;
    } else {
        fPlayerFacingRight = player.isFacingRight();
    }

    if (player.iswarping())
        rm->spr_cape.draw(
            player.leftX() - PWOFFSET + (fPlayerFacingRight ? - 18 : 18), player.topY() - PHOFFSET + 4 + iCapeYOffset,
            (fPlayerFacingRight ? 128 : 0) + iCapeFrameX, iCapeFrameY,
            32, 32,
            (short)player.state %4, player.GetWarpPlane());
    else
        rm->spr_cape.draw(
            player.leftX() - PWOFFSET + (fPlayerFacingRight ? - 18 : 18), player.topY() - PHOFFSET + 4 + iCapeYOffset,
            (fPlayerFacingRight ? 128 : 0) + iCapeFrameX, iCapeFrameY,
            32, 32);
}

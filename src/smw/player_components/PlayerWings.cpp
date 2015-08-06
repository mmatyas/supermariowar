#include "PlayerWings.h"

#include "../player.h"

#include "ResourceManager.h"
extern CResourceManager* rm;

void PlayerWings::reset()
{
    iWingsTimer = 0;
    iWingsFrame = 0;
}

void PlayerWings::draw(CPlayer &player)
{
    if (player.flying) {
        if (++iWingsTimer >= 8) {
            iWingsTimer = 0;
            iWingsFrame += 26;

            if (iWingsFrame > 26)
                iWingsFrame = 0;
        }
    } else {
        iWingsFrame = 26;
    }

    bool fPlayerFacingRight = player.isFacingRight();
    if (player.iswarping())
        rm->spr_wings.draw(
            player.ix + (fPlayerFacingRight ? - 19 : 15), player.iy - 10,
            iWingsFrame, fPlayerFacingRight ? 0 : 32,
            26, 32,
            (short)player.state % 4, player.GetWarpPlane());
    else
        rm->spr_wings.draw(
            player.ix + (fPlayerFacingRight ? - 19 : 15), player.iy - 10,
            iWingsFrame, fPlayerFacingRight ? 0 : 32,
            26, 32);
}
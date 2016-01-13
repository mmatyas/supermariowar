#include "PlayerTail.h"

#include "../player.h"

#include "GameValues.h"
#include "ObjectContainer.h"
#include "objectgame.h"
#include "ResourceManager.h"

extern CGameValues game_values;
extern CResourceManager* rm;

extern CObjectContainer objectcontainer[3];

void PlayerTail::reset() {
    iTailTimer = 0;
    iTailState = TAIL_NOT_IN_USE;
    iTailFrame = 0;
}

bool PlayerTail::isInUse() const {
    return iTailState > TAIL_NOT_IN_USE;
}

void PlayerTail::shake(CPlayer& player) {
    ifsoundonstop(rm->sfx_tailspin);
    ifSoundOnPlay(rm->sfx_tailspin);
    player.lockjump = true;

    iTailState = TAIL_SHAKE; //cause tail to shake
    iTailTimer = 0;
    iTailFrame = 110;
}

void PlayerTail::spin(CPlayer& player)
{
    ifSoundOnPlay(rm->sfx_tailspin);

    iTailState = TAIL_SPIN_AND_SHAKE; //cause tail to shake
    iTailTimer = -1; //Add one extra frame to sync with spinning player sprite
    iTailFrame = 22;

    player.spin.spin(player);

    objectcontainer[1].add(new MO_SpinAttack(player.getGlobalID(), player.getTeamID(), kill_style_leaf, player.isFacingRight(), 13));
}

//If player is shaking tail, slow decent
void PlayerTail::slowDescent(CPlayer& player) {
    if (iTailState == TAIL_SHAKE) {
        if (player.vely > 1.5f)
            player.vely = 1.5f;
    }
}

void PlayerTail::draw(CPlayer& player) {
    short iOffsetY = 0;
    if (iTailState == TAIL_SHAKE) {
        if (++iTailTimer >= 4) {
            iTailTimer = 0;
            iTailFrame -= 22;

            if (iTailFrame < 66) {
                iTailState = TAIL_NOT_IN_USE;

                if (player.powerup == 7 && game_values.leaflimit > 0)
                    player.DecreaseProjectileLimit();
            }
        }
    } else if (iTailState == TAIL_SPIN_AND_SHAKE) {
        iOffsetY = 52;
        if (++iTailTimer >= 4) {
            iTailTimer = 0;
            iTailFrame += 22;
            if (iTailFrame > 110) {
                iTailState = TAIL_NOT_IN_USE;
                iTailFrame = 66;

                if (player.powerup == 7 && game_values.leaflimit > 0)
                    player.DecreaseProjectileLimit();
            }
        }
    }

    //Draw tail will be called by the chicken if he is allowed to glide
    //but we don't want to draw the actual tail for the chicken
    if (player.powerup == 7) {
        if (iTailState == TAIL_NOT_IN_USE) {
            if (++iTailTimer >= 4) {
                iTailTimer = 0;

                if (!player.inair && player.velx != 0.0f) {
                    iTailFrame += 22;
                    if (iTailFrame > 66)
                        iTailFrame = 22;
                } else if (!player.inair) {
                    iTailFrame = 22;
                } else if (player.inair) {
                    if (player.vely <= 0.0f)
                        iTailFrame = 66;
                    else
                        iTailFrame = 110;
                }
            }
        }

        bool fPlayerFacingRight = !game_values.reversewalk;
        if (iTailState == TAIL_SPIN_AND_SHAKE) {
            if (player.spin.isReverseWalking())
                fPlayerFacingRight = game_values.reversewalk;
        } else {
            fPlayerFacingRight = player.isFacingRight();
        }

        if (player.iswarping())
            rm->spr_tail.draw(player.leftX() + (fPlayerFacingRight ? - 18 : 18),
                player.topY() + 6,
                iTailFrame,
                (fPlayerFacingRight ? 0 : 26) + iOffsetY,
                22,
                26,
                (short)player.state %4, player.GetWarpPlane());
        else
            rm->spr_tail.draw(player.leftX() + (fPlayerFacingRight ? - 18 : 18),
                player.topY() + 6,
                iTailFrame,
                (fPlayerFacingRight ? 0 : 26) + iOffsetY,
                22, 26);

    }
}

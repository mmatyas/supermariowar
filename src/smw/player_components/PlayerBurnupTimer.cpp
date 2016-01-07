#include "PlayerBurnupTimer.h"

#include "eyecandy.h"
#include "GameValues.h"
#include "GlobalConstants.h"
#include "player.h"
#include "PlayerKillStyles.h"
#include "ResourceManager.h"

extern CGameValues game_values;
extern CResourceManager* rm;
extern CEyecandyContainer eyecandy[3];

PlayerBurnupTimer::PlayerBurnupTimer()
    : timer(0)
    , starttimer(0)
{}

void PlayerBurnupTimer::update(CPlayer& player)
{
    if (player.vely >= MAXVELY) {
        if (!player.isInvincible() && !player.isShielded()) {
            if (++starttimer >= 20) {
                if (starttimer == 20)
                    ifSoundOnPlay(rm->sfx_burnup);

                if (++timer > 80)
                    player.KillPlayerMapHazard(true, kill_style_environment, false);
                else {
                    eyecandy[0].add(new EC_SingleAnimation(&rm->spr_burnup,
                        player.centerX() - 16,
                        player.centerY() - 16,
                        5, 4));
                }
            }
        }
    } else {
        timer = 0;
        starttimer = 0;
    }
}

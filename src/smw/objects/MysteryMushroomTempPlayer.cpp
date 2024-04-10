#include "MysteryMushroomTempPlayer.h"

#include "GameValues.h"
#include "objectgame.h"
#include "player.h"


extern CGameValues game_values;
extern short scorepowerupoffsets[3][3];


void MysteryMushroomTempPlayer::SetPlayer(CPlayer* player, short iPowerup)
{
    fx = player->fx;
    fy = player->fy;

    fOldX = player->fOldX;
    fOldY = player->fOldY;

    velx = player->velx;
    vely = player->vely;

    // bobomb = player->bobomb;
    // powerup = player->powerup;

    burnupstarttimer = player->burnup.starttimer;
    burnuptimer = player->burnup.timer;

    inair = player->inair;
    onice = player->onice;
    // invincible = player->invincible;
    // invincibletimer = player->invincibletimer;

    platform = player->platform;
    // iCapeFrameX = player->iCapeFrameX;
    // iCapeFrameY = player->iCapeFrameY;
    // iCapeTimer = player->iCapeTimer;
    // iCapeYOffset = player->iCapeYOffset;

    gamepowerup = iPowerup;

    iOldPowerupX = player->Score().x + scorepowerupoffsets[game_values.teamcounts[player->teamID] - 1][player->subTeamID];
    iOldPowerupY = player->Score().y + 25;
}

void MysteryMushroomTempPlayer::GetPlayer(CPlayer* player, short* iPowerup)
{
    player->fNewSwapX = fx;
    player->fNewSwapY = fy;

    player->iOldPowerupX = iOldPowerupX;
    player->iOldPowerupY = iOldPowerupY;

    player->fOldX = fOldX;
    player->fOldY = fOldY;

    player->velx = velx;
    player->vely = vely;

    // player->bobomb = bobomb;
    // player->powerup = powerup;

    player->burnup.starttimer = burnupstarttimer;
    player->burnup.timer = burnuptimer;

    player->inair = inair;
    player->onice = onice;
    // player->invincible = invincible;
    // player->invincibletimer = invincibletimer;

    player->platform = platform;
    // player->iCapeFrameX = iCapeFrameX;
    // player->iCapeFrameY	= iCapeFrameY;
    // player->iCapeTimer = iCapeTimer;
    // player->iCapeYOffset = iCapeYOffset;

    *iPowerup = gamepowerup;

    if (player->carriedItem)
        player->carriedItem->MoveToOwner();
}

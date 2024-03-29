#include "Health.h"

#include "GameValues.h"
#include "player.h"
#include "RandomNumberGenerator.h"
#include "ResourceManager.h"
#include "Score.h"

extern CScore *score[4];
extern short score_cnt;
extern CGameValues game_values;
extern CResourceManager* rm;


//mariowar (x lives - counting down)
CGM_Health::CGM_Health() : CGM_Classic()
{
    goal = 5;
    gamemode = game_mode_health;

    SetupModeStrings("Health", "Lives", 1);
};

void CGM_Health::init()
{
    CGM_Classic::init();

    for (short iScore = 0; iScore < score_cnt; iScore++) {
        score[iScore]->subscore[0] = game_values.gamemodesettings.health.startlife;
        score[iScore]->subscore[1] = score[iScore]->subscore[0];
    }
}

PlayerKillType CGM_Health::playerkilledplayer(CPlayer &inflictor, CPlayer &other, KillStyle style)
{
    if (gameover)
        return PlayerKillType::Normal;

    if (style == KillStyle::Bobomb || style == KillStyle::Bomb || style == KillStyle::IceBlast)
        other.Score().subscore[0] -= 2;
    else
        other.Score().subscore[0]--;

    if (other.Score().subscore[0] <= 0) {
        other.Score().subscore[0] = 0;
        PlayerKillType iRet = CGM_Classic::playerkilledplayer(inflictor, other, style);

        if (iRet == PlayerKillType::Normal)
            other.Score().subscore[0] = other.Score().subscore[1];

        return iRet;
    } else {
        ifSoundOnPlay(rm->sfx_powerdown);
        other.Shield().reset();
    }

    return PlayerKillType::NonKill;
}

PlayerKillType CGM_Health::playerkilledself(CPlayer &player, KillStyle style)
{
    if (gameover)
        return PlayerKillType::Normal;

    if (style == KillStyle::Bobomb || style == KillStyle::Bomb || style == KillStyle::Environment)
        player.Score().subscore[0] -= 2;
    else
        player.Score().subscore[0]--;

    if (player.Score().subscore[0] <= 0) {
        player.Score().subscore[0] = 0;
        PlayerKillType iRet = CGM_Classic::playerkilledself(player, style);

        if (iRet == PlayerKillType::Normal)
            player.Score().subscore[0] = player.Score().subscore[1];

        return iRet;
    } else {
        ifSoundOnPlay(rm->sfx_powerdown);
        player.Shield().reset();
    }

    return PlayerKillType::NonKill;
}

void CGM_Health::playerextraguy(CPlayer &player, short iType)
{
    if (!gameover) {
        player.Score().subscore[0] += iType;

        if (player.Score().subscore[0] > player.Score().subscore[1])
            player.Score().subscore[0] = player.Score().subscore[1];
    }
}

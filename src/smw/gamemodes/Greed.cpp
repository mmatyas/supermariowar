#include "Greed.h"

#include "GameValues.h"
#include "ObjectContainer.h"
#include "player.h"
#include "RandomNumberGenerator.h"
#include "ResourceManager.h"
#include "Score.h"
#include "objects/moving/Coin.h"

#include <cmath>

extern CScore *score[4];
extern short score_cnt;
extern CObjectContainer objectcontainer[3];
extern CGameValues game_values;
extern CResourceManager* rm;


namespace {
constexpr short killStyleDamage(KillStyle style) {
    switch (style) {
        case KillStyle::Stomp: return 5;
        case KillStyle::Star: return 5;
        case KillStyle::Fireball: return 3;
        case KillStyle::Bobomb: return 8;
        case KillStyle::Bounce: return 3;
        case KillStyle::Pow: return 5;
        case KillStyle::Goomba: return 2;
        case KillStyle::BulletBill: return 3;
        case KillStyle::Hammer: return 3;
        case KillStyle::Shell: return 5;
        case KillStyle::ThrowBlock: return 5;
        case KillStyle::CheepCheep: return 2;
        case KillStyle::Koopa: return 2;
        case KillStyle::Boomerang: return 3;
        case KillStyle::Feather: return 5;
        case KillStyle::IceBlast: return 8;
        case KillStyle::Podobo: return 3;
        case KillStyle::Bomb: return 8;
        case KillStyle::Leaf: return 5;
        case KillStyle::PWings: return 5;
        case KillStyle::KuriboShoe: return 8;
        case KillStyle::PoisonMushroom: return 5;
        case KillStyle::Environment: return 3;
        case KillStyle::Push: return 3;
        case KillStyle::BuzzyBeetle: return 2;
        case KillStyle::Spiny: return 2;
        case KillStyle::Phanto: return 2;
    }
}
} // namespace


//Greed - steal other players coins - if you have 0 coins, you're removed from the game!
CGM_Greed::CGM_Greed() : CGM_Classic()
{
    goal = 40;
    gamemode = game_mode_greed;

    SetupModeStrings("Greed", "Coins", 10);
};

void CGM_Greed::init()
{
    CGameMode::init();

    short iGoal = goal == -1 ? 0 : goal;

    for (short iScore = 0; iScore < score_cnt; iScore++) {
        score[iScore]->SetScore(iGoal);
    }
}

PlayerKillType CGM_Greed::playerkilledplayer(CPlayer &inflictor, CPlayer &other, KillStyle style)
{
    if (gameover)
        return PlayerKillType::Normal;

    //create coins around player
    return ReleaseCoins(other, style);
}

PlayerKillType CGM_Greed::playerkilledself(CPlayer &player, KillStyle style)
{
    if (gameover)
        return PlayerKillType::Normal;

    //create coins around player
    return ReleaseCoins(player, style);
}

void CGM_Greed::playerextraguy(CPlayer &player, short iType)
{
    if (!gameover)
        player.Score().AdjustScore(iType * 5);
}

PlayerKillType CGM_Greed::ReleaseCoins(CPlayer &player, KillStyle style)
{
    ifSoundOnPlay(rm->sfx_cannon);

    player.Shield().turn_on();

    short iDamage = killStyleDamage(style) * game_values.gamemodesettings.greed.multiplier / 2;

    if (goal != -1) {
        if (player.Score().score < iDamage)
            iDamage = player.Score().score;

        player.Score().AdjustScore(-iDamage);
    }

    short ix = player.centerX() - 16;
    short iy = player.centerY() - 16;

    for (short k = 0; k < iDamage; k++) {
        float vel = 7.0f + ((float)RANDOM_INT(9)) / 2.0f;
        float angle = -((float)RANDOM_INT(314)) / 100.0f;
        float velx = vel * cos(angle);
        float vely = vel * sin(angle);

        objectcontainer[1].add(new MO_Coin(&rm->spr_coin, velx, vely, ix, iy, player.getColorID(), player.getTeamID(), 1, 30, false));
    }

    //Play warning sound if game is almost over
    if (goal != -1 && !playedwarningsound) {
        bool playwarning = false;
        for (short j = 0; j < score_cnt; j++) {
            short countscore = 0;
            for (short k = 0; k < score_cnt; k++) {
                if (j == k)
                    continue;

                countscore += score[k]->score;
            }

            if (countscore <= 10) {
                playwarning = true;
                break;
            }
        }

        if (playwarning)
            playwarningsound();
    }

    if (goal != -1 && player.Score().score <= 0) {
        RemoveTeam(player.getTeamID());
        return PlayerKillType::Removed;
    }

    return PlayerKillType::NonKill;
}

#include "TimeLimit.h"

#include "GameValues.h"
#include "player.h"
#include "ResourceManager.h"

extern CGameValues game_values;
extern CResourceManager* rm;

extern short CountAliveTeams(short * lastteam);


//timelimit
CGM_TimeLimit::CGM_TimeLimit() : CGameMode()
{
    goal = 60;
    gamemode = game_mode_timelimit;

    SetupModeStrings("Time Limit", "Time", 30);
};

void CGM_TimeLimit::init()
{
    CGameMode::init();

    if (goal == -1)
        gameClock.Init(0, false);
    else
        gameClock.Init(goal, true);
}


void CGM_TimeLimit::think()
{
    CGameMode::think();
    short iTime = gameClock.RunClock();

    if (goal > 0) {
        if (iTime == 20 && !playedwarningsound) {
            playwarningsound();
        }

        if (iTime == 0) {
            //the game ends
            SetupScoreBoard(false);
            ShowScoreBoard();

            RemovePlayersButHighestScoring();
            gameover = true;

            CountAliveTeams(&winningteam);
        }
    }
}


PlayerKillType CGM_TimeLimit::playerkilledplayer(CPlayer &inflictor, CPlayer &other, KillStyle style)
{
    if (!gameover) {
        if (game_values.gamemode->gamemode != game_mode_timelimit || game_values.gamemodesettings.time.scoring == 0 || style == KillStyle::Push) {
            //Penalize killing your team mates
            if (inflictor.getTeamID() == other.getTeamID())
                inflictor.Score().AdjustScore(-1);
            else
                inflictor.Score().AdjustScore(1);
        }

        if (game_values.gamemode->gamemode == game_mode_timelimit && game_values.gamemodesettings.time.style == 1) {
            ifSoundOnPlay(rm->sfx_powerdown);
            other.Shield().reset();
            return PlayerKillType::NonKill;
        }
    }

    return PlayerKillType::Normal;
}

PlayerKillType CGM_TimeLimit::playerkilledself(CPlayer &player, KillStyle style)
{
    CGameMode::playerkilledself(player, style);

    if (player.Score().score > 0 && !gameover)
        player.Score().AdjustScore(-1);

    return PlayerKillType::Normal;
}

void CGM_TimeLimit::draw_foreground()
{
    if (!gameover) {
        gameClock.Draw();
    }
}

void CGM_TimeLimit::addtime(short iTime)
{
    if (!gameover) {
        gameClock.AddTime(iTime);
    }
}

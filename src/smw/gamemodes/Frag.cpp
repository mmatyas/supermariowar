#include "Frag.h"

#include "GameValues.h"
#include "player.h"
#include "ResourceManager.h"

extern CGameValues game_values;
extern CResourceManager* rm;


//fraglimit:

CGM_Frag::CGM_Frag() : CGameMode()
{
    goal = 20;
    gamemode = game_mode_frag;

    SetupModeStrings("Frag Limit", "Kills", 5);
};

PlayerKillType CGM_Frag::playerkilledplayer(CPlayer &inflictor, CPlayer &other, KillStyle style)
{
    if (gameover)
        return PlayerKillType::Normal;

    //Don't score if "sumo" style scoring is turned on
    if (game_values.gamemode->gamemode != game_mode_frag || game_values.gamemodesettings.frag.scoring == ScoringStyle::AllKills || style == KillStyle::Push) {
        //Penalize killing your team mates
        if (inflictor.getTeamID() == other.getTeamID())
            inflictor.Score().AdjustScore(-1);
        else
            inflictor.Score().AdjustScore(1);
    }

    PlayerKillType iRet = CheckWinner(&inflictor);

    if (game_values.gamemode->gamemode == game_mode_frag && game_values.gamemodesettings.frag.style == DeathStyle::Shield) {
        ifSoundOnPlay(rm->sfx_powerdown);
        other.Shield().reset();
        return PlayerKillType::NonKill;
    }

    return iRet;
}

PlayerKillType CGM_Frag::playerkilledself(CPlayer &player, KillStyle style)
{
    CGameMode::playerkilledself(player, style);

    if (!gameover) {
        player.Score().AdjustScore(-1);

        if (game_values.gamemode->gamemode == game_mode_frag && game_values.gamemodesettings.frag.style == DeathStyle::Shield) {
            ifSoundOnPlay(rm->sfx_powerdown);
            player.Shield().reset();
            return PlayerKillType::NonKill;
        }
    }

    return PlayerKillType::Normal;
}

void CGM_Frag::playerextraguy(CPlayer &player, short iType)
{
    if (!gameover) {
        player.Score().AdjustScore(iType);
        CheckWinner(&player);
    }
}

PlayerKillType CGM_Frag::CheckWinner(CPlayer * player)
{
    if (goal > -1) {
        if (player->Score().score >= goal) {
            player->Score().SetScore(goal);
            winningteam = player->getTeamID();
            gameover = true;

            RemovePlayersButTeam(winningteam);
            SetupScoreBoard(false);
            ShowScoreBoard();

            return PlayerKillType::Removed;
        } else if (player->Score().score >= goal - 2 && !playedwarningsound) {
            playwarningsound();
        }
    }

    return PlayerKillType::Normal;
}

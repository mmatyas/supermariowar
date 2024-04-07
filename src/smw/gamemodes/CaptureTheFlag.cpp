#include "CaptureTheFlag.h"

#include "GameValues.h"
#include "ObjectContainer.h"
#include "player.h"
#include "objects/carriable/CO_Flag.h"
#include "objects/moving/MO_FlagBase.h"
#include "ResourceManager.h"

extern CObjectContainer objectcontainer[3];
extern CPlayer* list_players[4];
extern short list_players_cnt;
extern CResourceManager* rm;
extern CGameValues game_values;


//Capture The Flag mode - each team has a base and a flag
//Protect your colored flag from being taken and score a point
//for stealing another teams flag and returning it to your base
CGM_CaptureTheFlag::CGM_CaptureTheFlag() : CGameMode()
{
    goal = 20;
    gamemode = game_mode_ctf;

    SetupModeStrings("Capture The Flag", "Flags", 5);
};

void CGM_CaptureTheFlag::init()
{
    CGameMode::init();

    bool fTeamUsed[4] = {false, false, false, false};

    for (short iPlayer = 0; iPlayer < list_players_cnt; iPlayer++) {
        short iTeamID = list_players[iPlayer]->getTeamID();
        if (!fTeamUsed[iTeamID]) {
            fTeamUsed[iTeamID] = true;

            short iColorID = list_players[iPlayer]->getColorID();
            MO_FlagBase * base = new MO_FlagBase(&rm->spr_flagbases, iTeamID, iColorID);
            objectcontainer[0].add(base);

            if (!game_values.gamemodesettings.flag.centerflag) {
                CO_Flag * flag = new CO_Flag(&rm->spr_flags, base, iTeamID, iColorID);
                objectcontainer[1].add(flag);
            }
        }
    }

    if (game_values.gamemodesettings.flag.centerflag) {
        CO_Flag * centerflag = new CO_Flag(&rm->spr_flags, NULL, -1, -1);
        objectcontainer[1].add(centerflag);
    }
}

PlayerKillType CGM_CaptureTheFlag::playerkilledplayer(CPlayer &, CPlayer &, KillStyle)
{
    return PlayerKillType::Normal;
}

PlayerKillType CGM_CaptureTheFlag::playerkilledself(CPlayer &player, KillStyle style)
{
    CGameMode::playerkilledself(player, style);

    if (player.carriedItem && player.carriedItem->getMovingObjectType() == movingobject_flag) {
        ((CO_Flag*)player.carriedItem)->placeFlag();
        ifSoundOnPlay(rm->sfx_transform);
    }

    return PlayerKillType::Normal;
}

void CGM_CaptureTheFlag::playerextraguy(CPlayer &player, short iType)
{
    if (gameover)
        return;

    player.Score().AdjustScore(iType);
    CheckWinner(&player);
}

PlayerKillType CGM_CaptureTheFlag::CheckWinner(CPlayer * player)
{
    if (goal > -1) {
        if (player->Score().score >= goal) {
            player->Score().SetScore(goal);
            winningteam = player->getTeamID();
            gameover = true;

            RemovePlayersButTeam(winningteam);
            SetupScoreBoard(false);
            ShowScoreBoard();
        } else if (player->Score().score >= goal - 2 && !playedwarningsound) {
            playwarningsound();
        }
    }

    return PlayerKillType::Normal;
}

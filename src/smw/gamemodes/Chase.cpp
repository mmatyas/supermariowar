#include "Chase.h"

#include "GameValues.h"
#include "ObjectContainer.h"
#include "player.h"
#include "RandomNumberGenerator.h"
#include "ResourceManager.h"
#include "objects/carriable/CO_PhantoKey.h"
#include "objects/overmap/WO_Phanto.h"

extern CObjectContainer objectcontainer[3];
extern CGameValues game_values;
extern CResourceManager* rm;


//Chase (hold a key for points while phantos chase you)
CGM_Chase::CGM_Chase() : CGameMode()
{
    goal = 200;
    gamemode = game_mode_chase;

    SetupModeStrings("Phanto", "Points", 50);
}

void CGM_Chase::init()
{
    CGameMode::init();

    //Add phantos based on settings
    for (short iPhanto = 0; iPhanto < 3; iPhanto++) {
        for (short iNumPhantos = 0; iNumPhantos < game_values.gamemodesettings.chase.phantoquantity[iPhanto]; iNumPhantos++)
            objectcontainer[1].add(new OMO_Phanto(&rm->spr_phanto, {RANDOM_INT(App::screenWidth), RANDOM_BOOL() ? -32 - CRUNCHMAX : App::screenHeight}, 0.0f, 0.0f, iPhanto));
    }

    //Add a key
    key = new CO_PhantoKey(&rm->spr_phantokey);
    objectcontainer[1].add(key);
}

void CGM_Chase::think()
{
    if (gameover) {
        displayplayertext();
        return;
    }

    CPlayer * keyholder = key->owner;
    if (keyholder) {
        static short counter = 0;

        if (keyholder->isready() && !keyholder->IsTanookiStatue()) {
            if (++counter >= game_values.pointspeed) {
                counter = 0;
                keyholder->Score().AdjustScore(1);
                CheckWinner(keyholder);
            }
        }
    }
}

PlayerKillType CGM_Chase::playerkilledplayer(CPlayer &player, CPlayer &other, KillStyle style)
{
    return PlayerKillType::Normal;
}

PlayerKillType CGM_Chase::playerkilledself(CPlayer &player, KillStyle style)
{
    CGameMode::playerkilledself(player, style);

    return PlayerKillType::Normal;
}

void CGM_Chase::playerextraguy(CPlayer &player, short iType)
{
    if (!gameover) {
        player.Score().AdjustScore(5 * iType);
        CheckWinner(&player);
    }
}

PlayerKillType CGM_Chase::CheckWinner(CPlayer * player)
{
    if (goal == -1)
        return PlayerKillType::Normal;

    if (player->Score().score >= goal) {
        player->Score().SetScore(goal);
        winningteam = player->getTeamID();
        gameover = true;

        SetupScoreBoard(false);
        ShowScoreBoard();
        RemovePlayersButTeam(winningteam);

        return PlayerKillType::Removed;
    } else if (player->Score().score >= goal * 0.8 && !playedwarningsound) {
        playwarningsound();
    }

    return PlayerKillType::Normal;
}

CPlayer * CGM_Chase::GetKeyHolder()
{
    return key->owner;
}

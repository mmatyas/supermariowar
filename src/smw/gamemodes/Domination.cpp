#include "Domination.h"

#include "GameValues.h"
#include "ObjectContainer.h"
#include "player.h"
#include "ResourceManager.h"
#include "Score.h"
#include "objects/overmap/WO_Area.h"

extern CScore *score[4];
extern CObjectContainer objectcontainer[3];
extern std::vector<CPlayer*> players;
extern CGameValues game_values;
extern CResourceManager* rm;


//Domination (capture the area blocks)
//Touch all the dotted blocks to turn them your color
//The more blocks you have, the faster you rack up points

CGM_Domination::CGM_Domination() : CGameMode()
{
    goal = 200;
    gamemode = game_mode_domination;

    SetupModeStrings("Domination", "Points", 50);
}

void CGM_Domination::init()
{
    CGameMode::init();

    short iNumAreas = game_values.gamemodesettings.domination.quantity;

    if (iNumAreas < 1)
        game_values.gamemodesettings.domination.quantity = iNumAreas = 1;

    if (iNumAreas > 18)
        iNumAreas = 2 * players.size() + iNumAreas - 22;
    else if (iNumAreas > 10)
        iNumAreas = players.size() + iNumAreas - 12;

    for (short k = 0; k < iNumAreas; k++)
        objectcontainer[0].add(new OMO_Area(&rm->spr_areas, iNumAreas));
}

PlayerKillType CGM_Domination::playerkilledplayer(CPlayer &player, CPlayer &other, KillStyle style)
{
    //Update areas the dead player owned
    objectcontainer[0].adjustPlayerAreas(&player, &other);

    return PlayerKillType::Normal;
}

PlayerKillType CGM_Domination::playerkilledself(CPlayer &player, KillStyle style)
{
    CGameMode::playerkilledself(player, style);

    //Update areas the dead player owned
    objectcontainer[0].adjustPlayerAreas(NULL, &player);

    return PlayerKillType::Normal;
}

void CGM_Domination::playerextraguy(CPlayer &player, short iType)
{
    if (!gameover) {
        player.Score().AdjustScore(10 * iType);
        CheckWinner(&player);
    }
}

PlayerKillType CGM_Domination::CheckWinner(CPlayer * player)
{
    if (goal > -1) {
        if (player->Score().score >= goal) {
            player->Score().SetScore(goal);
            winningteam = player->getTeamID();
            gameover = true;

            SetupScoreBoard(false);
            ShowScoreBoard();
            RemovePlayersButTeam(winningteam);
        } else if (player->Score().score >= goal * 0.8 && !playedwarningsound) {
            playwarningsound();
        }
    }

    return PlayerKillType::Normal;
}

#include "KingOfTheHill.h"

#include "ObjectContainer.h"
#include "ResourceManager.h"
#include "objects/overmap/WO_KingOfTheHillZone.h"

extern CObjectContainer objectcontainer[3];
extern CResourceManager* rm;


//King of the Hill (Control an area for a certain amount of time)
CGM_KingOfTheHill::CGM_KingOfTheHill() : CGM_Domination()
{
    goal = 200;
    gamemode = game_mode_koth;

    SetupModeStrings("King Of The Hill", "Points", 50);
}

void CGM_KingOfTheHill::init()
{
    CGameMode::init();
    objectcontainer[2].add(new OMO_KingOfTheHillZone(&rm->spr_kingofthehillarea));
}

PlayerKillType CGM_KingOfTheHill::playerkilledplayer(CPlayer &inflictor, CPlayer &other, KillStyle style)
{
    return PlayerKillType::Normal;
}

PlayerKillType CGM_KingOfTheHill::playerkilledself(CPlayer &player, KillStyle style)
{
    return CGameMode::playerkilledself(player, style);
}

void CGM_KingOfTheHill::playerextraguy(CPlayer &player, short iType)
{
    if (!gameover) {
        player.Score().AdjustScore(5 * iType);
        CheckWinner(&player);
    }
}

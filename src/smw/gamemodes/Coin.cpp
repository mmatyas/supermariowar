#include "Coins.h"

#include "GameValues.h"
#include "ObjectContainer.h"
#include "player.h"
#include "ResourceManager.h"
#include "objects/moving/MO_Coin.h"

extern CObjectContainer objectcontainer[3];
extern CResourceManager* rm;
extern CGameValues game_values;


//Coin mode:
//Collect randomly appearing coins on map
//First one to set amount wins
CGM_Coins::CGM_Coins() : CGameMode()
{
    goal = 20;
    gamemode = game_mode_coins;

    SetupModeStrings("Coin Collection", "Coins", 5);
};

void CGM_Coins::init()
{
    CGameMode::init();

    if (game_values.gamemodesettings.coins.quantity < 1)
        game_values.gamemodesettings.coins.quantity = 1;

    for (short iCoin = 0; iCoin < game_values.gamemodesettings.coins.quantity; iCoin++)
        objectcontainer[1].add(new MO_Coin(&rm->spr_coin, 0.0f, 0.0f, 0, 0, 2, 0, 0, 0, true));
}


PlayerKillType CGM_Coins::playerkilledplayer(CPlayer &player, CPlayer &other, KillStyle style)
{
    if (gameover)
        return PlayerKillType::Normal;

    if (game_values.gamemodesettings.coins.penalty)
        other.Score().AdjustScore(-1);

    return PlayerKillType::Normal;
}

PlayerKillType CGM_Coins::playerkilledself(CPlayer &player, KillStyle style)
{
    if (gameover)
        return PlayerKillType::Normal;

    if (game_values.gamemodesettings.coins.penalty)
        player.Score().AdjustScore(-1);

    return PlayerKillType::Normal;
}

void CGM_Coins::playerextraguy(CPlayer &player, short iType)
{
    if (!gameover) {
        player.Score().AdjustScore(iType << 1);
        CheckWinner(&player);
    }
}

PlayerKillType CGM_Coins::CheckWinner(CPlayer * player)
{
    if (!gameover && goal > -1) {
        if (player->Score().score >= goal) {
            player->Score().SetScore(goal);
            winningteam = player->getTeamID();
            gameover = true;

            RemovePlayersButTeam(winningteam);
            SetupScoreBoard(false);
            ShowScoreBoard();
        } else if (player->Score().score >= goal - 5 && !playedwarningsound) {
            playwarningsound();
        }
    }

    return PlayerKillType::Normal;
}

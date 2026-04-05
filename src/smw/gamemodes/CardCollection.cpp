#include "CardCollection.h"

#include "GameValues.h"
#include "ObjectContainer.h"
#include "player.h"
#include "RandomNumberGenerator.h"
#include "ResourceManager.h"
#include "Score.h"
#include "objects/moving/MO_CollectionCard.h"

#include <cmath>

extern CScore *score[4];
extern short score_cnt;
extern CObjectContainer objectcontainer[3];
extern std::vector<CPlayer*> players;
extern CGameValues game_values;
extern CResourceManager* rm;


//Collection (collect cards for points)
CGM_Collection::CGM_Collection() : CGameMode()
{
    goal = 30;
    gamemode = game_mode_collection;

    SetupModeStrings("Card Collection", "Points", 10);
}

void CGM_Collection::init()
{
    CGameMode::init();
    timer = 0;

    //Zero out the number of cards teams have
    for (short iScore = 0; iScore < score_cnt; iScore++) {
        score[iScore]->subscore[0] = 0;
        score[iScore]->subscore[1] = 0;
    }
}

void CGM_Collection::think()
{
    if (++timer >= game_values.gamemodesettings.collection.rate) {
        timer = 0;

        short iPowerupQuantity = game_values.gamemodemenusettings.collection.quantity;

        if (5 < iPowerupQuantity)
            iPowerupQuantity = players.size() + iPowerupQuantity - 7;

        if (objectcontainer[1].countMovingTypes(movingobject_collectioncard) < iPowerupQuantity) {
            short iRandom = RANDOM_INT(5);
            short iRandomCard = 0;
            if (iRandom == 4)
                iRandomCard = 2;
            else if (iRandom >= 2)
                iRandomCard = 1;

            objectcontainer[1].add(new MO_CollectionCard(&rm->spr_collectcards, 0, iRandomCard, 0, Vec2f::zero(), Vec2s::zero()));
        }
    }

    if (gameover) {
        displayplayertext();
        return;
    }

    //Check if this team has collected 3 cards
    for (short iScore = 0; iScore < score_cnt; iScore++) {
        if (score[iScore]->subscore[0] >= 3) {
            if (++score[iScore]->subscore[2] >= game_values.gamemodemenusettings.collection.banktime) {
                short iPoints = 1;

                if ((score[iScore]->subscore[1] & 63) == 0) //All Mushrooms
                    iPoints = 2;
                else if ((score[iScore]->subscore[1] & 63) == 21) //All Flowers
                    iPoints = 3;
                else if ((score[iScore]->subscore[1] & 63) == 42) //All Stars
                    iPoints = 5;

                score[iScore]->AdjustScore(iPoints);

                score[iScore]->subscore[0] = 0;
                score[iScore]->subscore[1] = 0;
                score[iScore]->subscore[2] = 0;
            }
        }
    }

    for (CPlayer* player : players) {
        CheckWinner(player);
    }
}

PlayerKillType CGM_Collection::playerkilledplayer(CPlayer &player, CPlayer &other, KillStyle style)
{
    //Causes a card to come out of the player
    ReleaseCard(other);

    return PlayerKillType::Normal;
}

PlayerKillType CGM_Collection::playerkilledself(CPlayer &player, KillStyle style)
{
    CGameMode::playerkilledself(player, style);

    //Causes a card to come out of the player
    ReleaseCard(player);

    return PlayerKillType::Normal;
}

void CGM_Collection::ReleaseCard(CPlayer &player)
{
    if (player.Score().subscore[0] > 0) {
        Vec2s pos(player.centerX() - 16, player.centerY() - 16);

        float speed = 7.0f + (float)RANDOM_INT(9) / 2.0f;
        float angle = -(float)RANDOM_INT(314) / 100.0f;
        Vec2f vel(speed * cos(angle), speed * sin(angle));

        player.Score().subscore[0]--;

        short iCardMask = 3 << (player.Score().subscore[0] << 1);
        short iCardMaskInverted = ~iCardMask;

        short iValue = (player.Score().subscore[1] & iCardMask) >> (player.Score().subscore[0] << 1);

        player.Score().subscore[1] &= iCardMaskInverted;

        objectcontainer[1].add(new MO_CollectionCard(&rm->spr_collectcards, 1, iValue, 30, vel, pos));
    }
}

void CGM_Collection::playerextraguy(CPlayer &player, short iType)
{
    if (!gameover) {
        player.Score().AdjustScore(iType);
        CheckWinner(&player);
    }
}

PlayerKillType CGM_Collection::CheckWinner(CPlayer * player)
{
    if (goal > -1) {
        if (player->Score().score >= goal) {
            player->Score().SetScore(goal);
            winningteam = player->getTeamID();
            gameover = true;

            SetupScoreBoard(false);
            ShowScoreBoard();
            RemovePlayersButTeam(winningteam);

            for (short iScore = 0; iScore < score_cnt; iScore++) {
                score[iScore]->subscore[0] = 0;
                score[iScore]->subscore[1] = 0;
                score[iScore]->subscore[2] = 0;
            }
        } else if (player->Score().score >= goal * 0.8 && !playedwarningsound) {
            playwarningsound();
        }
    }

    return PlayerKillType::Normal;
}

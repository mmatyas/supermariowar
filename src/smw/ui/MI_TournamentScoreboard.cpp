#include "MI_TournamentScoreboard.h"

#include "eyecandy.h"
#include "Game.h"
#include "GameValues.h"
#include "RandomNumberGenerator.h"
#include "ResourceManager.h"
#include "Score.h"
#include "uimenu.h"
#include "WorldTourStop.h"
#include "ui/MI_Image.h"
#include "ui/MI_ScoreText.h"

#include <algorithm>
#include <cmath>

extern CGameValues game_values;
extern CResourceManager* rm;
extern CScore* score[MAX_PLAYERS];

namespace {
const short iScoreboardPlayerOffsetsX[3][3] = {{40, 0, 0}, {19, 59, 0}, {6, 40, 74}};
constexpr short NORMAL_ROW_HEIGHT = 64;
constexpr short NORMAL_ROW_PITCH = 69;
constexpr short COMPACT_FIRST_ROW_Y = 74;
} // namespace


short MI_TournamentScoreboard::GetRowHeight() const
{
    if (!IsCompact())
        return NORMAL_ROW_HEIGHT;

    return game_values.matchtype == MatchType::Tour ? 36 : 40;
}

short MI_TournamentScoreboard::GetYFromPlace(short iPlace) const
{
    if (!IsCompact())
        return m_pos.y + iPlace * NORMAL_ROW_PITCH + (4 - iNumTeams) * 35;

    return COMPACT_FIRST_ROW_Y + iPlace * (GetRowHeight() + 2);
}

short MI_TournamentScoreboard::GetPlayerOffsetX(short iTeamSize, short iSlot)
{
    if (iTeamSize <= 3)
        return iScoreboardPlayerOffsetsX[iTeamSize - 1][iSlot];

    // Larger teams share the same space
    return 6 + iSlot * (100 - 32) / (iTeamSize - 1);
}

short MI_TournamentScoreboard::GetPlaceSprite(short iPlace)
{
    static const short iPlaceSprite[4] = {4, 0, 8, 9};
    return iPlaceSprite[std::min<short>(iPlace, 3)];
}


//Call with x = 70 and y == 80
MI_TournamentScoreboard::MI_TournamentScoreboard(gfxSprite* spr_background, short x, short y)
    : UI_Control(x, y)
    , sprBackground(spr_background)
{

    for (short iTeam = 0; iTeam < MAX_PLAYERS; iTeam++) {
        tourScores[iTeam] = nullptr;

        for (short iBonus = 0; iBonus < MAX_WORLD_BONUSES_AWARDED; iBonus++)
            worldBonus[iTeam][iBonus] = nullptr;

        worldPlace[iTeam] = nullptr;
        worldPointsBackground[iTeam] = nullptr;
    }

    for (short iGame = 0; iGame < 10; iGame++) {
        tourPoints[iGame] = nullptr;
        tourBonus[iGame] = nullptr;
    }

    miTourPointBar = nullptr;
}

MI_TournamentScoreboard::~MI_TournamentScoreboard()
{
    FreeScoreboard();
}

void MI_TournamentScoreboard::FreeScoreboard()
{
    if (!fCreated)
        return;

    for (short iTeam = 0; iTeam < iNumTeams; iTeam++) {
        for (int iGame = 0; iGame < iNumGames; iGame++) {
            delete miIconImages[iTeam][iGame];
        }

        for (int iPlayer = 0; iPlayer < iTeamCounts[iTeam]; iPlayer++) {
            delete miPlayerImages[iTeam][iPlayer];
        }

        delete [] miPlayerImages[iTeam];
        delete [] miIconImages[iTeam];
        delete miTeamImages[iTeam];

        if (tourScores[iTeam]) {
            delete tourScores[iTeam];
            tourScores[iTeam] = nullptr;
        }

        if (worldPlace[iTeam]) {
            delete worldPlace[iTeam];
            worldPlace[iTeam] = nullptr;
        }

        for (short iBonus = 0; iBonus < MAX_WORLD_BONUSES_AWARDED; iBonus++) {
            if (worldBonus[iTeam][iBonus]) {
                delete worldBonus[iTeam][iBonus];
                worldBonus[iTeam][iBonus] = nullptr;
            }
        }

        if (worldPointsBackground[iTeam]) {
            delete worldPointsBackground[iTeam];
            worldPointsBackground[iTeam] = nullptr;
        }
    }

    for (short iGame = 0; iGame < iNumGames; iGame++) {
        if (tourPoints[iGame]) {
            delete tourPoints[iGame];
            tourPoints[iGame] = nullptr;
        }

        if (tourBonus[iGame]) {
            delete tourBonus[iGame];
            tourBonus[iGame] = nullptr;
        }
    }

    if (miTourPointBar) {
        delete miTourPointBar;
        miTourPointBar = nullptr;
    }

    if (worldScore) {
        delete worldScore;
        worldScore = nullptr;
    }

    if (worldScoreModifier) {
        delete worldScoreModifier;
        worldScoreModifier = nullptr;
    }

    delete [] miPlayerImages;
    delete [] miIconImages;
}

void MI_TournamentScoreboard::Update()
{
    for (short iTeam = 0; iTeam < iNumTeams; iTeam++) {
        miTeamImages[iTeam]->Update();

        for (short iGame = 0; iGame < iNumGames; iGame++) {
            miIconImages[iTeam][iGame]->Update();
        }

        for (short iPlayer = 0; iPlayer < iTeamCounts[iTeam]; iPlayer++) {
            miPlayerImages[iTeam][iPlayer]->Update();
        }

        if (worldPointsBackground[iTeam])
            worldPointsBackground[iTeam]->Update();
    }

    if (game_values.matchtype == MatchType::Tour) {
        for (short iGame = 0; iGame < iNumGames; iGame++) {
            if (tourBonus[iGame])
                tourBonus[iGame]->Update();
        }
    }

    if (m_parentMenu) {
        if (iTournamentWinner != -1) { //Single tournament winning team
            if (--iFireworksCounter < 0 && iTournamentWinner >= 0) {
                iFireworksCounter = (short)(RANDOM_INT(30) + 10);

                if (--iExplosionCounter < 0) {
                    iExplosionCounter = (short)(RANDOM_INT(6) + 5);

                    ifSoundOnPlay(rm->sfx_bobombsound);

                    float dAngle = 0.0f;
                    short iRandX = (short)(RANDOM_INT(440) + 100);
                    short iRandY = (short)(RANDOM_INT(280) + 100);

                    for (short iBlock = 0; iBlock < 28; iBlock++) {
                        float dVel = 7.0f + ((iBlock % 2) * 5.0f);
                        float dVelX = dVel * cos(dAngle);
                        float dVelY = dVel * sin(dAngle);

                        short iRandomColor = (short)RANDOM_INT(iTeamCounts[iTournamentWinner]);
                        m_parentMenu->AddEyeCandy<EC_FallingObject>(&rm->spr_bonus, iRandX, iRandY, dVelX, dVelY, 4, 2, 0, game_values.colorids[iTeamIDs[iTournamentWinner][iRandomColor]] << 4, 16, 16);
                        dAngle -= (float)PI / 14;
                    }
                } else {
                    ifSoundOnPlay(rm->sfx_cannon);

                    short iRandX = (short)(RANDOM_INT(576));
                    short iRandY = (short)(RANDOM_INT(416));
                    short iRandomColor = (short)RANDOM_INT(iTeamCounts[iTournamentWinner]);

                    m_parentMenu->AddEyeCandy<EC_SingleAnimation>(&rm->spr_fireworks, iRandX, iRandY, 8, 4, 0, game_values.colorids[iTeamIDs[iTournamentWinner][iRandomColor]] << 6, 64, 64);
                }
            }

            if (--iWinnerTextCounter < 0) {
                iWinnerTextCounter = (short)(RANDOM_INT(35) + 15);

                char szWinnerText[64];
                if (iTournamentWinner == -2)
                    sprintf(szWinnerText, "Tied Game!");
                else if (iTeamCounts[iTournamentWinner] == 1)
                    sprintf(szWinnerText, "Player %d Wins!", iTeamIDs[iTournamentWinner][0] + 1);
                else if (iTeamCounts[iTournamentWinner] > 1)
                    sprintf(szWinnerText, "Team %d Wins!", iTournamentWinner + 1);

                short iStringWidth = (short)rm->menu_font_large.getWidth(szWinnerText);
                short iRandX = (short)(RANDOM_INT(App::screenWidth - iStringWidth) + (iStringWidth >> 1));
                short iRandY = (short)(RANDOM_INT(App::screenHeight - 100) + 100);

                m_parentMenu->AddEyeCandy<EC_GravText>(&rm->menu_font_large, iRandX, iRandY, szWinnerText, -VELJUMP);
            }
        }
    }
}

void MI_TournamentScoreboard::Draw()
{
    if (!m_visible)
        return;

    for (int iTeam = 0; iTeam < iNumTeams; iTeam++) {
        miTeamImages[iTeam]->Draw();

        for (int iGame = 0; iGame < iNumGames; iGame++) {
            if (iSwirlIconTeam != iTeam || iSwirlIconGame != iGame)
                miIconImages[iTeam][iGame]->Draw();
        }

        for (int iPlayer = 0; iPlayer < iTeamCounts[iTeam]; iPlayer++) {
            miPlayerImages[iTeam][iPlayer]->Draw();
        }
    }

    if (iSwirlIconTeam > -1) {
        miIconImages[iSwirlIconTeam][iSwirlIconGame]->Draw();
    }

           //Draw tour totals
    if (game_values.matchtype == MatchType::Tour) {
        for (short iTeam = 0; iTeam < iNumTeams; iTeam++)
            tourScores[iTeam]->Draw();

        miTourPointBar->Draw();

        for (short iGame = 0; iGame < iNumGames; iGame++) {
            if (tourBonus[iGame])
                tourBonus[iGame]->Draw();

            tourPoints[iGame]->Draw();
        }
    }

    if (game_values.matchtype == MatchType::World) {
        for (short iTeam = 0; iTeam < iNumTeams; iTeam++) {
            worldPointsBackground[iTeam]->Draw();
        }

        for (short iTeam = 0; iTeam < iNumTeams; iTeam++) {
            tourScores[iTeam]->Draw();
            worldPlace[iTeam]->Draw();

            for (short iBonus = 0; iBonus < MAX_WORLD_BONUSES_AWARDED; iBonus++) {
                worldBonus[iTeam][iBonus]->Draw();
            }
        }

        worldScoreModifier->Draw();
        worldScore->Draw();
    }
}

void MI_TournamentScoreboard::CreateScoreboard(short numTeams, short numGames, gfxSprite * spr_icons_ref)
{
    FreeScoreboard();

    sprIcons = spr_icons_ref;

    for (short iTeam = 0; iTeam < MAX_PLAYERS; iTeam++) {
        iTeamCounts[iTeam] = game_values.teamcounts[iTeam];

        for (short iTeamSpot = 0; iTeamSpot < MAX_TEAM_SIZE; iTeamSpot++) {
            iTeamIDs[iTeam][iTeamSpot] = game_values.teamids[iTeam][iTeamSpot];
        }
    }

    iTournamentWinner = -1;
    iGameWinner = -1;

    iSwirlIconTeam = -1;
    iSwirlIconGame = -1;

    iExplosionCounter = 0;
    iFireworksCounter = 0;
    iWinnerTextCounter = 0;

    iNumTeams = numTeams;
    iNumGames = numGames;

    miTeamImages = new MI_Image * [iNumTeams];
    miIconImages = new MI_Image ** [iNumTeams];
    miPlayerImages = new MI_Image ** [iNumTeams];

    bool fTour = game_values.matchtype == MatchType::Tour;
    bool fNotTournament = game_values.matchtype != MatchType::Tournament;

    for (short iTeam = 0; iTeam < iNumTeams; iTeam++) {
        short iTeamY = GetYFromPlace(iTeam);

        if (fTour)
            iTeamY += 28; //shift down 28 pxls for extra tour points bar

        miTeamImages[iTeam] = new MI_Image(sprBackground, m_pos.x - (fNotTournament ? 40 : 0), iTeamY, 0, game_values.matchtype == MatchType::World ? 160 : 0, fNotTournament ? 580 : 500, 64, 1, 2, 0);
        if (IsCompact())
            miTeamImages[iTeam]->SetHiddenRows(GetRowHeight() / 2, NORMAL_ROW_HEIGHT - GetRowHeight());
        miIconImages[iTeam] = new MI_Image * [iNumGames];
        miPlayerImages[iTeam] = new MI_Image * [iTeamCounts[iTeam]];

        if (game_values.matchtype != MatchType::World) {
            for (short iGame = 0; iGame < iNumGames; iGame++) {
                float dSpacing = GetIconSpacing();

                miIconImages[iTeam][iGame] = new MI_Image(sprIcons, m_pos.x + 128 + (short)dSpacing + (short)((float)iGame * (32.0f + dSpacing)) - (fTour ? 40 : 0), iTeamY + GetContentOffsetY(), 0, 0, 32, 32, fTour ? 4 : 1, 1, fTour ? 8 : 0);
                miIconImages[iTeam][iGame]->setVisible(false);
            }
        }

        for (short iPlayer = 0; iPlayer < iTeamCounts[iTeam]; iPlayer++) {
            miPlayerImages[iTeam][iPlayer] = new MI_Image(&rm->spr_player[iTeamIDs[iTeam][iPlayer]][PGFX_STANDING_R], m_pos.x + GetPlayerOffsetX(iTeamCounts[iTeam], iPlayer) - (fTour ? 40 : 0), iTeamY + GetContentOffsetY(), 0, 0, 32, 32, 2, 1, 0);
        }

        if (fNotTournament)
            tourScores[iTeam] = new MI_ScoreText(0, 0);

        if (game_values.matchtype == MatchType::World) {
            worldPointsBackground[iTeam] = new MI_Image(sprBackground, m_pos.x + 476, iTeamY, 516, 160, 64, 64, 1, 2, 0);
            if (IsCompact())
                worldPointsBackground[iTeam]->SetHiddenRows(GetRowHeight() / 2, NORMAL_ROW_HEIGHT - GetRowHeight());
            worldPlace[iTeam] = new MI_Image(sprIcons, m_pos.x + 102, iTeamY + GetContentOffsetY() - 2, 0, 0, 32, 32, 4, 1, 8);

            for (short iBonus = 0; iBonus < MAX_WORLD_BONUSES_AWARDED; iBonus++) {
                worldBonus[iTeam][iBonus] = new MI_Image(&rm->spr_worlditems, m_pos.x + 180 + 38 * iBonus, iTeamY + GetContentOffsetY() - 2, 0, 0, 32, 32, 1, 1, 0);
                worldBonus[iTeam][iBonus]->setVisible(false);
            }
        }
    }

    if (game_values.matchtype == MatchType::World) {
        worldScoreModifier = new MI_Image(&rm->spr_worlditems, 0, 0, 0, 0, 32, 32, 1, 1, 0);
        worldScore = new MI_ScoreText(0, 0);
    }

    if (fTour) {
        for (short iGame = 0; iGame < iNumGames; iGame++) {
            float dSpacing = GetIconSpacing();
            short iTourPointX = m_pos.x + 105 + (short)dSpacing + (short)((float)iGame * (32.0f + dSpacing));
            tourPoints[iGame] = new MI_ScoreText(iTourPointX, GetYFromPlace(0));
            tourPoints[iGame]->SetScore(game_values.tourstops[iGame]->iPoints);

            if (game_values.tourstops[iGame]->iBonusType)
                tourBonus[iGame] = new MI_Image(sprBackground, iTourPointX - 19, GetYFromPlace(0) - 12, 372, 128, 38, 31, 2, 1, 8);
            //else
            //	tourBonus[iGame] = new MI_Image(sprBackground, iTourPointX - 11, GetYFromPlace(0) - 3, 448, 128, 22, 22, 1, 1, 0);
        }

        miTourPointBar = new MI_Image(sprBackground, m_pos.x + 88, GetYFromPlace(0) - 8, 0, 128, 372, 32, 1, 1, 0);
    }

    fCreated = true;
}

void MI_TournamentScoreboard::RefreshWorldScores(short gameWinner)
{
    iGameWinner = gameWinner;
    DetermineScoreboardWinners();

    TourStop * tourStop = game_values.tourstops[game_values.tourstopcurrent];

    for (short iTeam = 0; iTeam < iNumTeams; iTeam++) {
        short iTeamY = GetYFromPlace(game_values.tournament_scores[iTeam].wins);

        miTeamImages[iTeam]->SetPosition(m_pos.x - 40, iTeamY);
        worldPointsBackground[iTeam]->SetPosition(m_pos.x + 476, iTeamY);

        for (short iPlayer = 0; iPlayer < iTeamCounts[iTeam]; iPlayer++) {
            miPlayerImages[iTeam][iPlayer]->SetPosition(m_pos.x + GetPlayerOffsetX(iTeamCounts[iTeam], iPlayer) - 40, iTeamY + GetContentOffsetY());
            const size_t player_idx = iTeamIDs[iTeam][iPlayer];
            const size_t sprite_idx = GetPlaceSprite(game_values.tournament_scores[iTeam].wins);
            miPlayerImages[iTeam][iPlayer]->SetImageSource(&rm->spr_player[player_idx][sprite_idx]);
        }

        tourScores[iTeam]->SetPosition(m_pos.x + 508, iTeamY + GetContentOffsetY() + 8);
        tourScores[iTeam]->SetScore(game_values.tournament_scores[iTeam].total);

        if (iGameWinner == -1) {
            worldScore->setVisible(false);
            worldScoreModifier->setVisible(false);
        }

        if (iGameWinner == iTeam) {
            worldScore->SetPosition(m_pos.x + 350, iTeamY + GetContentOffsetY() + 8);
            worldScore->SetScore(tourStop->iPoints);
            worldScore->setVisible(true);

            miTeamImages[iTeam]->SetImage(0, 160, 496, 64);

            if (game_values.worldpointsbonus >= 0) {
                worldScoreModifier->SetImage((game_values.worldpointsbonus + 9) << 5, 0, 32, 32);
                worldScoreModifier->SetPosition(m_pos.x + 410, iTeamY + GetContentOffsetY() - 2);
                worldScoreModifier->setVisible(true);
            } else {
                worldScoreModifier->setVisible(false);
            }
        } else {
            miTeamImages[iTeam]->SetImage(0, 160, 344, 64);
        }

        worldPlace[iTeam]->SetPosition(m_pos.x + 102, iTeamY + GetContentOffsetY() - 2);
        // NOTE: The place markers only go up to 4th place
        worldPlace[iTeam]->SetImage(0, std::min<short>(score[iTeam]->place, 3) << 5, 32, 32);
        worldPlace[iTeam]->setVisible(gameWinner >= 0);
    }

    short iBonusCounts[MAX_PLAYERS] = {};

    if (gameWinner >= 0) {
        for (short iBonus = 0; iBonus < tourStop->iNumBonuses; iBonus++) {
            WorldStageBonus * bonus = &tourStop->wsbBonuses[iBonus];

            if (bonus->iWinnerPlace < iNumTeams) {
                for (short iTeam = 0; iTeam < iNumTeams; iTeam++) {
                    if (score[iTeam]->place == bonus->iWinnerPlace) {
                        short iDisplayPosition = game_values.tournament_scores[iTeam].wins;

                        if (iBonusCounts[iDisplayPosition] < MAX_WORLD_BONUSES_AWARDED) {
                            worldBonus[iDisplayPosition][iBonusCounts[iDisplayPosition]]->setVisible(true);
                            worldBonus[iDisplayPosition][iBonusCounts[iDisplayPosition]]->SetImageSource(bonus->iBonus < NUM_POWERUPS ? &rm->spr_storedpoweruplarge : &rm->spr_worlditems);
                            worldBonus[iDisplayPosition][iBonusCounts[iDisplayPosition]]->SetImage((bonus->iBonus < NUM_POWERUPS ? bonus->iBonus : bonus->iBonus - NUM_POWERUPS) << 5, 0, 32, 32);

                            iBonusCounts[iDisplayPosition]++;
                        }
                    }
                }
            }
        }
    }

    for (short iTeam = 0; iTeam < iNumTeams;  iTeam++) {
        for (short iBonus = iBonusCounts[iTeam]; iBonus < MAX_WORLD_BONUSES_AWARDED; iBonus++) {
            worldBonus[iTeam][iBonus]->setVisible(false);
        }
    }

    if (gameWinner >= 0)
        game_values.worldpointsbonus = -1;
}

//Called by Tour -- Arranges players in terms of standings
void MI_TournamentScoreboard::RefreshTourScores()
{
    DetermineScoreboardWinners();

    for (short iTeam = 0; iTeam < iNumTeams; iTeam++) {
        short iTeamY = GetYFromPlace(game_values.tournament_scores[iTeam].wins);
        iTeamY += 28; //shift down 28 pxls for extra tour points bar

        float dSpacing = GetIconSpacing();

        miTeamImages[iTeam]->SetPosition(m_pos.x - 40, iTeamY);

        for (short iPlayer = 0; iPlayer < iTeamCounts[iTeam]; iPlayer++) {
            miPlayerImages[iTeam][iPlayer]->SetPosition(m_pos.x + GetPlayerOffsetX(iTeamCounts[iTeam], iPlayer) - 40, iTeamY + GetContentOffsetY());
            miPlayerImages[iTeam][iPlayer]->SetImageSource(&rm->spr_player[iTeamIDs[iTeam][iPlayer]][GetPlaceSprite(game_values.tournament_scores[iTeam].wins)]);
        }

        for (size_t iGame = 0; iGame < game_values.tourstopcurrent; iGame++) {
            // NOTE: The place markers only go up to 4th place
            miIconImages[iTeam][iGame]->SetImage(0, std::min<short>(game_values.tournament_scores[iTeam].type[iGame], 3) * 32, 32, 32);

            miIconImages[iTeam][iGame]->SetPosition(m_pos.x + 128 + (short)dSpacing + (short)((float)iGame * (32.0f + dSpacing)) - 40, iTeamY + GetContentOffsetY());

            miIconImages[iTeam][iGame]->SetSwirl(false, 0.0f, 0.0f, 0.0f, 0.0f);
            miIconImages[iTeam][iGame]->SetPulse(false);
            miIconImages[iTeam][iGame]->setVisible(true);
        }

        for (size_t iGame = game_values.tourstopcurrent; iGame < game_values.tourstops.size(); iGame++) {
            miIconImages[iTeam][iGame]->setVisible(false);
        }

        tourScores[iTeam]->SetPosition(m_pos.x + 508, iTeamY + GetContentOffsetY() + 8);
        tourScores[iTeam]->SetScore(game_values.tournament_scores[iTeam].total);
    }
}

void MI_TournamentScoreboard::DetermineScoreboardWinners()
{
    //Detect a Tie
    short iNumWinningTeams = 0;
    short iWinningTeams[MAX_PLAYERS];
    std::fill(std::begin(iWinningTeams), std::end(iWinningTeams), -1);

    for (short iTeam = 0; iTeam < iNumTeams; iTeam++) {
        if (game_values.tournament_scores[iTeam].wins == 0) {
            iWinningTeams[iNumWinningTeams++] = iTeam;
        }
    }

           //Adjust tied scores so display is right
    for (short iMyTeam = 0; iMyTeam < iNumTeams; iMyTeam++) {
        for (short iTheirTeam = 0; iTheirTeam < iNumTeams; iTheirTeam++) {
            if (iMyTeam == iTheirTeam)
                continue;

            if (game_values.tournament_scores[iMyTeam].wins == game_values.tournament_scores[iTheirTeam].wins) {
                game_values.tournament_scores[iTheirTeam].wins++;
            }
        }
    }

           //There was a single team winner
    if (game_values.tournamentwinner != -1) {
        if (iNumWinningTeams == 1)
            iTournamentWinner = iWinningTeams[0];
        else
            iTournamentWinner = -2;

        game_values.tournamentwinner = iTournamentWinner;

               //Flash the background of the winning teams
        for (short iTeam = 0; iTeam < iNumWinningTeams; iTeam++) {
            miTeamImages[iWinningTeams[iTeam]]->SetAnimationSpeed(20);

            if (worldPointsBackground[iWinningTeams[iTeam]])
                worldPointsBackground[iWinningTeams[iTeam]]->SetAnimationSpeed(20);
        }
    }
}

//Called by Tournament -- Keeps players where they are and displays number of wins and mode type of win
void MI_TournamentScoreboard::RefreshTournamentScores(short gameWinner)
{
    iGameWinner = gameWinner;
    iSwirlIconTeam = -1;
    iSwirlIconGame = -1;

    if (game_values.tournament_scores[iGameWinner].wins == iNumGames) {
        iTournamentWinner = iGameWinner;
        miTeamImages[iTournamentWinner]->SetAnimationSpeed(20);

        if (worldPointsBackground[iTournamentWinner])
            worldPointsBackground[iTournamentWinner]->SetAnimationSpeed(20);
    }

    for (short iTeam = 0; iTeam < iNumTeams; iTeam++) {
        for (short iGame = 0; iGame < game_values.tournament_scores[iTeam].wins; iGame++) {
            miIconImages[iTeam][iGame]->SetImage(game_values.tournament_scores[iTeam].type[iGame] * 32, 0, 32, 32);

            if (iTournamentWinner < 0 && iGameWinner == iTeam && iGame == game_values.tournament_scores[iTeam].wins - 1) {
                iSwirlIconTeam = iTeam;
                iSwirlIconGame = iGame;

                miIconImages[iTeam][iGame]->SetSwirl(true, 250.0f, (float)(RANDOM_INT(1000)) * TWO_PI / 1000.0f, 3.0f, 0.1f);
            }

            miIconImages[iTeam][iGame]->SetPulse(false);
            miIconImages[iTeam][iGame]->setVisible(true);
        }

        for (short iGame = game_values.tournament_scores[iTeam].wins; iGame < iNumGames; iGame++) {
            miIconImages[iTeam][iGame]->setVisible(false);
        }
    }
}

void MI_TournamentScoreboard::StopSwirl()
{
    if (iSwirlIconTeam >= 0) {
        miIconImages[iSwirlIconTeam][iSwirlIconGame]->StopSwirl();
        iSwirlIconTeam = -1;
        iSwirlIconGame = -1;
    }
}

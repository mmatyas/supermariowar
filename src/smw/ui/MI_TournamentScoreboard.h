#pragma once

#include "GlobalConstants.h"
#include "uicontrol.h"

class gfxSprite;
class MI_Image;
class MI_ScoreText;


class MI_TournamentScoreboard : public UI_Control {
public:
    MI_TournamentScoreboard(gfxSprite* spr_background, short x, short y);
    virtual ~MI_TournamentScoreboard();

    void Update() override;
    void Draw() override;

    void CreateScoreboard(short numTeams, short numGames, gfxSprite* spr_icons);
    void RefreshWorldScores(short gameWinner);
    void RefreshTourScores();
    void RefreshTournamentScores(short gameWinner);
    void DetermineScoreboardWinners();
    void StopSwirl();

private:
    void FreeScoreboard();
    short GetYFromPlace(short iPlace) const {
        return iy + iPlace * 69 + (4 - iNumTeams) * 35;
    }
    float GetIconSpacing() const {
        return (float)(372 - iNumGames * 32) / (float)(iNumGames + 1);
    }

    bool fCreated = false;

    MI_Image** miTeamImages;
    MI_Image*** miIconImages;
    MI_Image*** miPlayerImages;

    short iNumTeams = 0;
    short iNumGames = 0;

    short iTournamentWinner = 0;
    short iGameWinner = 0;

    short iSwirlIconTeam = 0;
    short iSwirlIconGame = 0;

    short iFireworksCounter = 0;
    short iWinnerTextCounter = 0;
    short iExplosionCounter = 0;

    gfxSprite* sprBackground = nullptr;
    gfxSprite* sprIcons = nullptr;

    short iTeamIDs[4][3];
    short iTeamCounts[4];

    MI_ScoreText* tourScores[4];
    MI_ScoreText* tourPoints[10];
    MI_Image* miTourPointBar = nullptr;
    MI_Image* tourBonus[10];

    MI_Image* worldBonus[4][MAX_WORLD_BONUSES_AWARDED];
    MI_Image* worldScoreModifier = nullptr;
    MI_Image* worldPlace[4];
    MI_ScoreText* worldScore = nullptr;
    MI_Image* worldPointsBackground[4];
};

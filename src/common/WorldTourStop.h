#pragma once

#include "GameModeSettings.h"

#include <string>

struct TourStop;
struct Version;


struct WorldStageBonus {
    short iWinnerPlace;
    short iBonus;
    char szBonusString[8];
};

struct TourStop {
    std::string pszMapFile;
    short iMode;
    short iGoal;
    short iPoints;
    short iBonusType;
    char szName[128];

    bool fEndStage;
    short iNumBonuses;
    WorldStageBonus wsbBonuses[10];
    short iStageType;

    bool fUseSettings;
    short iNumUsedSettings;
    GameModeSettings gmsSettings;

    short iBonusTextLines;
    char szBonusText[5][128];
};


TourStop ParseTourStopLine(char* buffer, const Version& version, bool fIsWorld);
std::string WriteTourStopLine(const TourStop& ts, bool fIsWorld);
void ResetTourStops();

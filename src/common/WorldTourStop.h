#pragma once

#include "GameModeSettings.h"

#include <array>
#include <string>

struct TourStop;
struct Version;


struct WorldStageBonus {
    short iWinnerPlace;
    short iBonus;
    std::string szBonusString;
};

struct TourStop {
    std::string pszMapFile;
    short iMode;
    short iGoal;
    short iPoints;
    short iBonusType;
    std::string szName;

    bool fEndStage;
    short iNumBonuses;
    WorldStageBonus wsbBonuses[10];
    short iStageType;

    bool fUseSettings;
    short iNumUsedSettings;
    GameModeSettings gmsSettings;

    short iBonusTextLines;
    std::array<std::string, 5> szBonusText;
};


TourStop ParseTourStopLine(char* buffer, const Version& version, bool fIsWorld);
std::string WriteTourStopLine(const TourStop& ts, bool fIsWorld);
void ResetTourStops();
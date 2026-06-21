#include "WorldTourStop.h"

#include "GameMode.h"
#include "GameModeSettingsSerialization.h"
#include "GameValues.h"
#include "MapList.h"
#include "RandomNumberGenerator.h"
#include "Version.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>

extern CGameMode * gamemodes[GAMEMODE_LAST];
extern CGameValues game_values;
extern MapList *maplist;


namespace {
constexpr const char* const DELIM_EMPTY = "";
constexpr const char* const DELIM_COMMA = ",";
constexpr const char* const DELIM_PIPE = "|";
constexpr const char* const UNKNOWN_MAP_NAME = "-";
} // namespace


TourStop ParseTourStopLine(char* buffer, const Version& version, bool fIsWorld)
{
    TourStop ts;
    ts.fUseSettings = false;
    ts.iNumUsedSettings = 0;

    char* pszTemp = strtok(buffer, ",\n");

    ts.iStageType = 0;
    if (fIsWorld) {
        ts.iStageType = atoi(pszTemp);
        if (ts.iStageType < 0 || ts.iStageType > 1)
            ts.iStageType = 0;

        pszTemp = strtok(NULL, ",\n");
    }

    ts.szBonusText[0].clear();
    ts.szBonusText[1].clear();
    ts.szBonusText[2].clear();
    ts.szBonusText[3].clear();
    ts.szBonusText[4].clear();

    if (ts.iStageType == 0) {
        char* szMap = new char[strlen(pszTemp) + 1];
        strcpy(szMap, pszTemp);
        pszTemp = strtok(NULL, ",\n");

        if (pszTemp)
            ts.iMode = atoi(pszTemp);
        else
            ts.iMode = -1;

        const bool isMinigame = ts.iMode == game_mode_pipe_minigame || ts.iMode == game_mode_boss_minigame || ts.iMode == game_mode_boxes_minigame;

        //If this is 1.8.0.2 or earlier and we are playing a minigame, use the default map
        if (version <= Version {1, 8, 0, 2} && isMinigame) {
            //Get a bogus map name so the mode will know to load the default map
            ts.pszMapFile = UNKNOWN_MAP_NAME;
        } else {
            //Using the maplist to cheat and find a map for us
            maplist->SaveCurrent();

            //If that map is not found
            bool fMapFound = maplist->findexact(szMap, true);
            if (!fMapFound) {
                if (isMinigame) {
                    //Get a bogus map name so the mode will know to load the default map
                    ts.pszMapFile = UNKNOWN_MAP_NAME;
                } else {
                    maplist->random(false);
                    ts.pszMapFile = maplist->currentShortmapname();
                }
                printf("WARNING: No map file found with the name '%s'\n", szMap);
            } else {
                ts.pszMapFile = maplist->currentShortmapname();
            }

            maplist->ResumeCurrent();
        }

        delete [] szMap;

        //The pipe minigame was using the value 24 from version 1.8.0.0 to 1.8.0.2
        //It was later switched to 1000 to accomodate new modes easily
        if (version <= Version {1, 8, 0, 2}) {
            if (ts.iMode == 24)
                ts.iMode = game_mode_pipe_minigame;
        }

        //If a valid mode was not detected, then just choose a random mode
        if (ts.iMode < 0 || (ts.iMode >= GAMEMODE_LAST && !isMinigame))
            ts.iMode = RANDOM_INT(GAMEMODE_LAST);

        pszTemp = strtok(NULL, ",\n");

        //This gets the closest game mode to what the tour has
        ts.iGoal = -1;
        if (pszTemp) {
            //If it is commented out, this will allow things like 33 coins, 17 kill goals, etc.
            //ts.iGoal = gamemodes[ts.iMode]->GetClosestGoal(atoi(pszTemp));
            ts.iGoal = atoi(pszTemp);
        }

        //Default to a random goal if an invalid goal was used
        if (ts.iGoal <= 0) {
            if (ts.iMode < GAMEMODE_LAST)
                ts.iGoal = gamemodes[ts.iMode]->GetOptions()[RANDOM_INT(GAMEMODE_NUM_OPTIONS - 1)].iValue;
            else
                ts.iGoal = 50;
        }

        if (version >= Version {1, 7, 0, 2}) {
            pszTemp = strtok(NULL, ",\n");

            //Read in point value for tour stop
            if (pszTemp)
                ts.iPoints = atoi(pszTemp);
            else
                ts.iPoints = 1;

            pszTemp = strtok(NULL, ",\n");

            if (fIsWorld) {
                ts.iBonusType = 0;
                ts.iNumBonuses = 0;

                char * pszStart = pszTemp;

                while (pszStart != NULL) {
                    char * pszEnd = strstr(pszStart, "|");
                    if (pszEnd)
                        *pszEnd = 0;

                    //if it is "0", then no bonuses
                    short iWinnerPlace = pszStart[0] - 48;
                    if (iWinnerPlace == 0)
                        break;
                    else if (iWinnerPlace < 1 || iWinnerPlace > 4)
                        iWinnerPlace = 1;

                    ts.wsbBonuses[ts.iNumBonuses].szBonusString = pszStart;

                    ts.wsbBonuses[ts.iNumBonuses].iWinnerPlace = iWinnerPlace - 1;

                    short iPowerupOffset = 0;
                    if (pszStart[1] == 'w' || pszStart[1] == 'W')
                        iPowerupOffset += NUM_POWERUPS;

                    pszStart += 2;

                    short iBonus = atoi(pszStart) + iPowerupOffset;
                    if (iBonus < 0 || iBonus >= NUM_POWERUPS + NUM_WORLD_POWERUPS)
                        iBonus = 0;

                    ts.wsbBonuses[ts.iNumBonuses].iBonus = iBonus;

                    if (++ts.iNumBonuses >= 10)
                        break;

                    if (pszEnd)
                        pszStart = pszEnd + 1;
                    else
                        pszStart = NULL;
                }
            } else {
                if (pszTemp)
                    ts.iBonusType = atoi(pszTemp);
                else
                    ts.iBonusType = 0;
            }

            pszTemp = strtok(NULL, ",\n");

            if (pszTemp) {
                ts.szName = pszTemp;
            } else {
                ts.szName = "Tour Stop " + std::to_string(game_values.tourstops.size() + 1);
            }
        } else {
            ts.iPoints = 1;
            ts.iBonusType = 0;
            ts.szName = "Tour Stop " + std::to_string(game_values.tourstops.size() + 1);
        }

        if (version >= Version {1, 8, 0, 0}) {
            if (fIsWorld) {
                //is this a world ending stage?
                pszTemp = strtok(NULL, ",\n");

                if (pszTemp)
                    ts.fEndStage = pszTemp[0] == '1';
                else
                    ts.fEndStage = false;
            }

            //Copy in default values first
            memcpy(&ts.gmsSettings, &game_values.gamemodemenusettings, sizeof(GameModeSettings));

            const std::vector<bool> usedSettings = deserializeGMS(ts.iMode, ts.gmsSettings, game_values.gamemodesettings);
            ts.iNumUsedSettings = std::count(usedSettings.cbegin(), usedSettings.cend(), true);
            ts.fUseSettings = !usedSettings.empty();
        }
    } else if (ts.iStageType == 1) { //Bonus House
        if (pszTemp) {
            ts.szName = pszTemp;
        } else {
            ts.szName = "Bonus House " + std::to_string(game_values.tourstops.size() + 1);
        }

        pszTemp = strtok(NULL, ",\n");

        short iBonusOrdering = atoi(pszTemp);
        if (iBonusOrdering < 0 || iBonusOrdering > 1)
            iBonusOrdering = 0;

        ts.iBonusType = iBonusOrdering;

        pszTemp = strtok(NULL, ",\n");

        char * pszStart = pszTemp;

        ts.iBonusTextLines = 0;
        while (pszStart != NULL && pszStart[0] != '-') {
            char * pszEnd = strstr(pszStart, "|");

            if (pszEnd)
                *pszEnd = 0;

            ts.szBonusText[ts.iBonusTextLines] = pszStart;

            if (++ts.iBonusTextLines >= 5 || !pszEnd)
                break;

            pszStart = pszEnd + 1;
        }

        ts.iNumBonuses = 0;
        pszTemp = strtok(NULL, ",\n");
        while (pszTemp) {
            ts.wsbBonuses[ts.iNumBonuses].szBonusString = pszTemp;

            short iPowerupOffset = 0;
            if (pszTemp[0] == 'w' || pszTemp[0] == 'W')
                iPowerupOffset += NUM_POWERUPS;
            else if (pszTemp[0] == 's' || pszTemp[0] == 'S')
                iPowerupOffset += NUM_POWERUPS + NUM_WORLD_POWERUPS - 1;

            pszTemp++;

            short iBonus = atoi(pszTemp) + iPowerupOffset;
            if (iBonus < 0 || iBonus >= NUM_POWERUPS + NUM_WORLD_POWERUPS + NUM_WORLD_SCORE_BONUSES)
                iBonus = 0;

            ts.wsbBonuses[ts.iNumBonuses].iBonus = iBonus;
            ts.wsbBonuses[ts.iNumBonuses].iWinnerPlace = -1;

            if (++ts.iNumBonuses >= MAX_BONUS_CHESTS)
                break;

            pszTemp = strtok(NULL, ",\n");
        }
    }

    return ts;
}

std::string WriteTourStopLine(const TourStop& ts, bool fIsWorld)
{
    std::vector<std::string> fields;

    if (fIsWorld) {
        //Write stage type (battle stage vs. bonus house, etc.)
        fields.emplace_back(std::to_string(ts.iStageType));
    }

    //Battle stage
    if (ts.iStageType == 0) {
        fields.emplace_back(ts.pszMapFile);
        fields.emplace_back(std::to_string(ts.iMode));
        fields.emplace_back(std::to_string(ts.iGoal));
        fields.emplace_back(std::to_string(ts.iPoints));

        if (fIsWorld) {
            if (ts.iNumBonuses <= 0) {
                fields.emplace_back("0");
            } else {
                std::string field;
                const char* delim = DELIM_EMPTY;
                for (short iBonus = 0; iBonus < ts.iNumBonuses; iBonus++) {
                    field += delim;
                    field += ts.wsbBonuses[iBonus].szBonusString;
                    delim = DELIM_PIPE;
                }
                fields.emplace_back(std::move(field));
            }
        } else {
            fields.emplace_back(std::to_string(ts.iBonusType));
        }

        fields.emplace_back(ts.szName);

        if (fIsWorld) {
            fields.emplace_back(std::to_string(ts.fEndStage));
        }

        if (ts.fUseSettings) {
            std::vector<short> values = serializeGMS(ts.iMode, ts.gmsSettings);
            values.resize(ts.iNumUsedSettings);
            for (short value : values) {
                fields.emplace_back(std::to_string(value));
            }
        }
    } else if (ts.iStageType == 1) { //Bonus House
        fields.emplace_back(ts.szName);
        fields.emplace_back(std::to_string(ts.iBonusType));

        std::string bonus_text_field;
        const char* delim = DELIM_EMPTY;
        for (short iText = 0; iText < ts.iBonusTextLines; iText++) {
            bonus_text_field += delim;
            bonus_text_field += ts.szBonusText[iText];
            delim = DELIM_PIPE;
        }
        fields.emplace_back(std::move(bonus_text_field));

        if (ts.iNumBonuses == 0) {
            fields.emplace_back("p0");
        } else {
            for (short iBonus = 0; iBonus < ts.iNumBonuses; iBonus++) {
                fields.emplace_back(ts.wsbBonuses[iBonus].szBonusString);
            }
        }
    }

    size_t out_size = 0;
    for (const std::string& field : fields) {
        out_size += field.size() + 1;  // Either comma or line break
    }

    std::string out;
    out.reserve(out_size);

    const char* delim = DELIM_EMPTY;
    for (std::string& field : fields) {
        out += delim;
        out += std::move(field);
        delim = DELIM_COMMA;
    }

    out += "\n";
    return out;
}

void ResetTourStops()
{
    game_values.tourstopcurrent = 0;

    // added to prevent 'vector iterators incompatible' exception
    if (!game_values.tourstops.empty())
        game_values.tourstops.clear();
}

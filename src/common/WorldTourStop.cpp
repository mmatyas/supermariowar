#include "WorldTourStop.h"

#include "GameMode.h"
#include "GameValues.h"
#include "MapList.h"
#include "RandomNumberGenerator.h"
#include "Version.h"

#include <cstdlib>
#include <cstring>

extern CGameMode * gamemodes[GAMEMODE_LAST];
extern CGameValues game_values;
extern MapList *maplist;

/**********************************
* Tour Stop functions
**********************************/

template<typename T>
short ReadTourStopSetting(T& output, T defaultVal)
{
    char* tok = strtok(NULL, ",\n");
    if (tok) {
        output = static_cast<T>(atoi(tok));
        return 1;
    } else {
        output = defaultVal;
        return 0;
    }
}

TourStop * ParseTourStopLine(char * buffer, const Version& version, bool fIsWorld)
{
    TourStop * ts = new TourStop();
    ts->fUseSettings = false;
    ts->iNumUsedSettings = 0;

    char * pszTemp = strtok(buffer, ",\n");

    short iStageType = 0;
    if (fIsWorld) {
        iStageType = atoi(pszTemp);
        if (iStageType < 0 || iStageType > 1)
            iStageType = 0;

        pszTemp = strtok(NULL, ",\n");
    }

    ts->iStageType = iStageType;

    ts->szBonusText[0][0] = 0;
    ts->szBonusText[1][0] = 0;
    ts->szBonusText[2][0] = 0;
    ts->szBonusText[3][0] = 0;
    ts->szBonusText[4][0] = 0;

    if (iStageType == 0) {
        char * szMap = new char[strlen(pszTemp) + 1];
        strcpy(szMap, pszTemp);

        pszTemp = strtok(NULL, ",\n");

        if (pszTemp)
            ts->iMode = atoi(pszTemp);
        else
            ts->iMode = -1;

        //If this is 1.8.0.2 or earlier and we are playing a minigame, use the default map
        if (version <= Version {1, 8, 0, 2} &&
                (ts->iMode == game_mode_pipe_minigame || ts->iMode == game_mode_boss_minigame || ts->iMode == game_mode_boxes_minigame)) {
            //Get a bogus map name so the mode will know to load the default map
            ts->pszMapFile = maplist->GetUnknownMapName();
        } else {
            //Using the maplist to cheat and find a map for us
            maplist->SaveCurrent();

            //If that map is not found
            bool fMapFound = maplist->findexact(szMap, true);

            if (!fMapFound) {
                if (ts->iMode == game_mode_pipe_minigame || ts->iMode == game_mode_boss_minigame || ts->iMode == game_mode_boxes_minigame) {
                    //Get a bogus map name so the mode will know to load the default map
                    ts->pszMapFile = maplist->GetUnknownMapName();
                } else {
                    maplist->random(false);
                    ts->pszMapFile = maplist->currentShortmapname();
                }
            } else {
                ts->pszMapFile = maplist->currentShortmapname();
            }

            maplist->ResumeCurrent();
        }

        delete [] szMap;

        //The pipe minigame was using the value 24 from version 1.8.0.0 to 1.8.0.2
        //It was later switched to 1000 to accomodate new modes easily
        if (version <= Version {1, 8, 0, 2}) {
            if (ts->iMode == 24)
                ts->iMode = game_mode_pipe_minigame;
        }

        //If a valid mode was not detected, then just choose a random mode
        if (ts->iMode < 0 || (ts->iMode >= GAMEMODE_LAST && ts->iMode != game_mode_pipe_minigame && ts->iMode != game_mode_boss_minigame && ts->iMode != game_mode_boxes_minigame))
            ts->iMode = RANDOM_INT(GAMEMODE_LAST);

        pszTemp = strtok(NULL, ",\n");

        //This gets the closest game mode to what the tour has
        ts->iGoal = -1;
        if (pszTemp) {
            //If it is commented out, this will allow things like 33 coins, 17 kill goals, etc.
            //ts->iGoal = gamemodes[ts->iMode]->GetClosestGoal(atoi(pszTemp));
            ts->iGoal = atoi(pszTemp);
        }

        //Default to a random goal if an invalid goal was used
        if (ts->iGoal <= 0) {
            if (ts->iMode < GAMEMODE_LAST)
                ts->iGoal = gamemodes[ts->iMode]->GetOptions()[RANDOM_INT(GAMEMODE_NUM_OPTIONS - 1)].iValue;
            else
                ts->iGoal = 50;
        }

        if (version >= Version {1, 7, 0, 2}) {
            pszTemp = strtok(NULL, ",\n");

            //Read in point value for tour stop
            if (pszTemp)
                ts->iPoints = atoi(pszTemp);
            else
                ts->iPoints = 1;

            pszTemp = strtok(NULL, ",\n");

            if (fIsWorld) {
                ts->iBonusType = 0;
                ts->iNumBonuses = 0;

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

                    strcpy(ts->wsbBonuses[ts->iNumBonuses].szBonusString, pszStart);

                    ts->wsbBonuses[ts->iNumBonuses].iWinnerPlace = iWinnerPlace - 1;

                    short iPowerupOffset = 0;
                    if (pszStart[1] == 'w' || pszStart[1] == 'W')
                        iPowerupOffset += NUM_POWERUPS;

                    pszStart += 2;

                    short iBonus = atoi(pszStart) + iPowerupOffset;
                    if (iBonus < 0 || iBonus >= NUM_POWERUPS + NUM_WORLD_POWERUPS)
                        iBonus = 0;

                    ts->wsbBonuses[ts->iNumBonuses].iBonus = iBonus;

                    if (++ts->iNumBonuses >= 10)
                        break;

                    if (pszEnd)
                        pszStart = pszEnd + 1;
                    else
                        pszStart = NULL;
                }
            } else {
                if (pszTemp)
                    ts->iBonusType = atoi(pszTemp);
                else
                    ts->iBonusType = 0;
            }

            pszTemp = strtok(NULL, ",\n");

            if (pszTemp) {
                strncpy(ts->szName, pszTemp, 127);
                ts->szName[127] = 0;
            } else {
                sprintf(ts->szName, "Tour Stop %d", game_values.tourstoptotal + 1);
            }
        } else {
            ts->iPoints = 1;
            ts->iBonusType = 0;
            sprintf(ts->szName, "Tour Stop %d", game_values.tourstoptotal + 1);
        }

        if (version >= Version {1, 8, 0, 0}) {
            if (fIsWorld) {
                //is this a world ending stage?
                pszTemp = strtok(NULL, ",\n");

                if (pszTemp)
                    ts->fEndStage = pszTemp[0] == '1';
                else
                    ts->fEndStage = false;
            }

            //Copy in default values first
            memcpy(&ts->gmsSettings, &game_values.gamemodemenusettings, sizeof(GameModeSettings));

            if (ts->iMode == game_mode_classic) {
                ts->fUseSettings = true;
                ts->iNumUsedSettings += ReadTourStopSetting(ts->gmsSettings.classic.style, game_values.gamemodemenusettings.classic.style);
                ts->iNumUsedSettings += ReadTourStopSetting(ts->gmsSettings.classic.scoring, game_values.gamemodemenusettings.classic.scoring);
            } else if (ts->iMode == game_mode_frag) {
                ts->fUseSettings = true;
                ts->iNumUsedSettings += ReadTourStopSetting(ts->gmsSettings.frag.style, game_values.gamemodemenusettings.frag.style);
                ts->iNumUsedSettings += ReadTourStopSetting(ts->gmsSettings.frag.scoring, game_values.gamemodemenusettings.frag.scoring);
            } else if (ts->iMode == game_mode_timelimit) {
                ts->fUseSettings = true;
                ts->iNumUsedSettings += ReadTourStopSetting(ts->gmsSettings.time.style, game_values.gamemodemenusettings.time.style);
                ts->iNumUsedSettings += ReadTourStopSetting(ts->gmsSettings.time.scoring, game_values.gamemodemenusettings.time.scoring);
                ts->iNumUsedSettings += ReadTourStopSetting(ts->gmsSettings.time.percentextratime, game_values.gamemodemenusettings.time.percentextratime);
            } else if (ts->iMode == game_mode_jail) {
                ts->fUseSettings = true;

                ts->iNumUsedSettings += ReadTourStopSetting(ts->gmsSettings.jail.style, game_values.gamemodemenusettings.jail.style);
                ts->iNumUsedSettings += ReadTourStopSetting(ts->gmsSettings.jail.timetofree, game_values.gamemodemenusettings.jail.timetofree);
                ts->iNumUsedSettings += ReadTourStopSetting(ts->gmsSettings.jail.tagfree, game_values.gamemodemenusettings.jail.tagfree);
                ts->iNumUsedSettings += ReadTourStopSetting(ts->gmsSettings.jail.percentkey, game_values.gamemodemenusettings.jail.percentkey);
            } else if (ts->iMode == game_mode_coins) {
                ts->fUseSettings = true;

                ts->iNumUsedSettings += ReadTourStopSetting(ts->gmsSettings.coins.penalty, game_values.gamemodemenusettings.coins.penalty);
                ts->iNumUsedSettings += ReadTourStopSetting(ts->gmsSettings.coins.quantity, game_values.gamemodemenusettings.coins.quantity);
                ts->iNumUsedSettings += ReadTourStopSetting(ts->gmsSettings.coins.percentextracoin, game_values.gamemodemenusettings.coins.percentextracoin);
            } else if (ts->iMode == game_mode_stomp) {
                ts->fUseSettings = true;

                ts->iNumUsedSettings += ReadTourStopSetting(ts->gmsSettings.stomp.rate, game_values.gamemodemenusettings.stomp.rate);

                for (int iEnemy = 0; iEnemy < NUMSTOMPENEMIES; iEnemy++)
                    ts->iNumUsedSettings += ReadTourStopSetting((ts->gmsSettings.stomp.enemyweight[iEnemy]), game_values.gamemodemenusettings.stomp.enemyweight[iEnemy]);
            } else if (ts->iMode == game_mode_eggs) {
                ts->fUseSettings = true;

                for (int iEgg = 0; iEgg < 4; iEgg++)
                    ts->iNumUsedSettings += ReadTourStopSetting((ts->gmsSettings.egg.eggs[iEgg]), game_values.gamemodemenusettings.egg.eggs[iEgg]);

                for (int iYoshi = 0; iYoshi < 4; iYoshi++)
                    ts->iNumUsedSettings += ReadTourStopSetting((ts->gmsSettings.egg.yoshis[iYoshi]), game_values.gamemodemenusettings.egg.yoshis[iYoshi]);

                ts->iNumUsedSettings += ReadTourStopSetting((ts->gmsSettings.egg.explode), game_values.gamemodemenusettings.egg.explode);
            } else if (ts->iMode == game_mode_ctf) {
                ts->fUseSettings = true;

                ts->iNumUsedSettings += ReadTourStopSetting(ts->gmsSettings.flag.speed, game_values.gamemodemenusettings.flag.speed);
                ts->iNumUsedSettings += ReadTourStopSetting(ts->gmsSettings.flag.touchreturn, game_values.gamemodemenusettings.flag.touchreturn);
                ts->iNumUsedSettings += ReadTourStopSetting(ts->gmsSettings.flag.pointmove, game_values.gamemodemenusettings.flag.pointmove);
                ts->iNumUsedSettings += ReadTourStopSetting(ts->gmsSettings.flag.autoreturn, game_values.gamemodemenusettings.flag.autoreturn);
                ts->iNumUsedSettings += ReadTourStopSetting(ts->gmsSettings.flag.homescore, game_values.gamemodemenusettings.flag.homescore);
                ts->iNumUsedSettings += ReadTourStopSetting(ts->gmsSettings.flag.centerflag, game_values.gamemodemenusettings.flag.centerflag);
            } else if (ts->iMode == game_mode_chicken) {
                ts->fUseSettings = true;

                ts->iNumUsedSettings += ReadTourStopSetting(ts->gmsSettings.chicken.usetarget, game_values.gamemodemenusettings.chicken.usetarget);
                ts->iNumUsedSettings += ReadTourStopSetting(ts->gmsSettings.chicken.glide, game_values.gamemodemenusettings.chicken.glide);
            } else if (ts->iMode == game_mode_tag) {
                ts->fUseSettings = true;

                ts->iNumUsedSettings += ReadTourStopSetting(ts->gmsSettings.tag.tagontouch, game_values.gamemodemenusettings.tag.tagontouch);
            } else if (ts->iMode == game_mode_star) {
                ts->fUseSettings = true;

                ts->iNumUsedSettings += ReadTourStopSetting(ts->gmsSettings.star.time, game_values.gamemodemenusettings.star.time);
                ts->iNumUsedSettings += ReadTourStopSetting(ts->gmsSettings.star.shine, game_values.gamemodemenusettings.star.shine);
                ts->iNumUsedSettings += ReadTourStopSetting(ts->gmsSettings.star.percentextratime, game_values.gamemodemenusettings.star.percentextratime);
            } else if (ts->iMode == game_mode_domination) {
                ts->fUseSettings = true;

                ts->iNumUsedSettings += ReadTourStopSetting(ts->gmsSettings.domination.quantity, game_values.gamemodemenusettings.domination.quantity);
                ts->iNumUsedSettings += ReadTourStopSetting(ts->gmsSettings.domination.relocationfrequency, game_values.gamemodemenusettings.domination.relocationfrequency);
                ts->iNumUsedSettings += ReadTourStopSetting(ts->gmsSettings.domination.loseondeath, game_values.gamemodemenusettings.domination.loseondeath);
                ts->iNumUsedSettings += ReadTourStopSetting(ts->gmsSettings.domination.relocateondeath, game_values.gamemodemenusettings.domination.relocateondeath);
                ts->iNumUsedSettings += ReadTourStopSetting(ts->gmsSettings.domination.stealondeath, game_values.gamemodemenusettings.domination.stealondeath);
            } else if (ts->iMode == game_mode_koth) {
                ts->fUseSettings = true;

                ts->iNumUsedSettings += ReadTourStopSetting(ts->gmsSettings.kingofthehill.areasize, game_values.gamemodemenusettings.kingofthehill.areasize);
                ts->iNumUsedSettings += ReadTourStopSetting(ts->gmsSettings.kingofthehill.relocationfrequency, game_values.gamemodemenusettings.kingofthehill.relocationfrequency);
                ts->iNumUsedSettings += ReadTourStopSetting(ts->gmsSettings.kingofthehill.maxmultiplier, game_values.gamemodemenusettings.kingofthehill.maxmultiplier);
            } else if (ts->iMode == game_mode_race) {
                ts->fUseSettings = true;

                ts->iNumUsedSettings += ReadTourStopSetting(ts->gmsSettings.race.quantity, game_values.gamemodemenusettings.race.quantity);
                ts->iNumUsedSettings += ReadTourStopSetting(ts->gmsSettings.race.speed, game_values.gamemodemenusettings.race.speed);
                ts->iNumUsedSettings += ReadTourStopSetting(ts->gmsSettings.race.penalty, game_values.gamemodemenusettings.race.penalty);
            } else if (ts->iMode == game_mode_frenzy) {
                ts->fUseSettings = true;

                ts->iNumUsedSettings += ReadTourStopSetting(ts->gmsSettings.frenzy.quantity, game_values.gamemodemenusettings.frenzy.quantity);
                ts->iNumUsedSettings += ReadTourStopSetting(ts->gmsSettings.frenzy.rate, game_values.gamemodemenusettings.frenzy.rate);
                ts->iNumUsedSettings += ReadTourStopSetting(ts->gmsSettings.frenzy.storedshells, game_values.gamemodemenusettings.frenzy.storedshells);

                for (short iPowerup = 0; iPowerup < NUMFRENZYCARDS; iPowerup++)
                    ts->iNumUsedSettings += ReadTourStopSetting((ts->gmsSettings.frenzy.powerupweight[iPowerup]), game_values.gamemodemenusettings.frenzy.powerupweight[iPowerup]);
            } else if (ts->iMode == game_mode_survival) {
                ts->fUseSettings = true;

                for (short iEnemy = 0; iEnemy < NUMSURVIVALENEMIES; iEnemy++)
                    ts->iNumUsedSettings += ReadTourStopSetting((ts->gmsSettings.survival.enemyweight[iEnemy]), game_values.gamemodemenusettings.survival.enemyweight[iEnemy]);

                ts->iNumUsedSettings += ReadTourStopSetting(ts->gmsSettings.survival.density, game_values.gamemodemenusettings.survival.density);
                ts->iNumUsedSettings += ReadTourStopSetting(ts->gmsSettings.survival.speed, game_values.gamemodemenusettings.survival.speed);
                ts->iNumUsedSettings += ReadTourStopSetting(ts->gmsSettings.survival.shield, game_values.gamemodemenusettings.survival.shield);
            } else if (ts->iMode == game_mode_greed) {
                ts->fUseSettings = true;

                ts->iNumUsedSettings += ReadTourStopSetting(ts->gmsSettings.greed.coinlife, game_values.gamemodemenusettings.greed.coinlife);
                ts->iNumUsedSettings += ReadTourStopSetting(ts->gmsSettings.greed.owncoins, game_values.gamemodemenusettings.greed.owncoins);
                ts->iNumUsedSettings += ReadTourStopSetting(ts->gmsSettings.greed.multiplier, game_values.gamemodemenusettings.greed.multiplier);
                ts->iNumUsedSettings += ReadTourStopSetting(ts->gmsSettings.greed.percentextracoin, game_values.gamemodemenusettings.greed.percentextracoin);
            } else if (ts->iMode == game_mode_health) {
                ts->fUseSettings = true;

                ts->iNumUsedSettings += ReadTourStopSetting(ts->gmsSettings.health.startlife, game_values.gamemodemenusettings.health.startlife);
                ts->iNumUsedSettings += ReadTourStopSetting(ts->gmsSettings.health.maxlife, game_values.gamemodemenusettings.health.maxlife);
                ts->iNumUsedSettings += ReadTourStopSetting(ts->gmsSettings.health.percentextralife, game_values.gamemodemenusettings.health.percentextralife);
            } else if (ts->iMode == game_mode_collection) {
                ts->fUseSettings = true;

                ts->iNumUsedSettings += ReadTourStopSetting(ts->gmsSettings.collection.quantity, game_values.gamemodemenusettings.collection.quantity);
                ts->iNumUsedSettings += ReadTourStopSetting(ts->gmsSettings.collection.rate, game_values.gamemodemenusettings.collection.rate);
                ts->iNumUsedSettings += ReadTourStopSetting(ts->gmsSettings.collection.banktime, game_values.gamemodemenusettings.collection.banktime);
                ts->iNumUsedSettings += ReadTourStopSetting(ts->gmsSettings.collection.cardlife, game_values.gamemodemenusettings.collection.cardlife);
            } else if (ts->iMode == game_mode_chase) {
                ts->fUseSettings = true;

                ts->iNumUsedSettings += ReadTourStopSetting(ts->gmsSettings.chase.phantospeed, game_values.gamemodemenusettings.chase.phantospeed);

                for (short iPhanto = 0; iPhanto < 3; iPhanto++)
                    ts->iNumUsedSettings += ReadTourStopSetting(ts->gmsSettings.chase.phantoquantity[iPhanto], game_values.gamemodemenusettings.chase.phantoquantity[iPhanto]);
            } else if (ts->iMode == game_mode_shyguytag) {
                ts->fUseSettings = true;

                ts->iNumUsedSettings += ReadTourStopSetting(ts->gmsSettings.shyguytag.tagonsuicide, game_values.gamemodemenusettings.shyguytag.tagonsuicide);
                ts->iNumUsedSettings += ReadTourStopSetting(ts->gmsSettings.shyguytag.tagtransfer, game_values.gamemodemenusettings.shyguytag.tagtransfer);
                ts->iNumUsedSettings += ReadTourStopSetting(ts->gmsSettings.shyguytag.freetime, game_values.gamemodemenusettings.shyguytag.freetime);
            } else if (ts->iMode == game_mode_boss_minigame) {
                ts->fUseSettings = true;

                ts->iNumUsedSettings += ReadTourStopSetting(ts->gmsSettings.boss.bosstype, game_values.gamemodemenusettings.boss.bosstype);
                ts->iNumUsedSettings += ReadTourStopSetting(ts->gmsSettings.boss.difficulty, game_values.gamemodemenusettings.boss.difficulty);
                ts->iNumUsedSettings += ReadTourStopSetting(ts->gmsSettings.boss.hitpoints, game_values.gamemodemenusettings.boss.hitpoints);
            }
        }
    } else if (iStageType == 1) { //Bonus House
        if (pszTemp) {
            strncpy(ts->szName, pszTemp, 127);
            ts->szName[127] = 0;
        } else {
            sprintf(ts->szName, "Bonus House %d", game_values.tourstoptotal + 1);
        }

        pszTemp = strtok(NULL, ",\n");

        short iBonusOrdering = atoi(pszTemp);
        if (iBonusOrdering < 0 || iBonusOrdering > 1)
            iBonusOrdering = 0;

        ts->iBonusType = iBonusOrdering;

        pszTemp = strtok(NULL, ",\n");

        char * pszStart = pszTemp;

        ts->iBonusTextLines = 0;
        while (pszStart != NULL && pszStart[0] != '-') {
            char * pszEnd = strstr(pszStart, "|");

            if (pszEnd)
                *pszEnd = 0;

            strcpy(ts->szBonusText[ts->iBonusTextLines], pszStart);

            if (++ts->iBonusTextLines >= 5 || !pszEnd)
                break;

            pszStart = pszEnd + 1;
        }

        ts->iNumBonuses = 0;
        pszTemp = strtok(NULL, ",\n");
        while (pszTemp) {
            strcpy(ts->wsbBonuses[ts->iNumBonuses].szBonusString, pszTemp);

            short iPowerupOffset = 0;
            if (pszTemp[0] == 'w' || pszTemp[0] == 'W')
                iPowerupOffset += NUM_POWERUPS;
            else if (pszTemp[0] == 's' || pszTemp[0] == 'S')
                iPowerupOffset += NUM_POWERUPS + NUM_WORLD_POWERUPS - 1;

            pszTemp++;

            short iBonus = atoi(pszTemp) + iPowerupOffset;
            if (iBonus < 0 || iBonus >= NUM_POWERUPS + NUM_WORLD_POWERUPS + NUM_WORLD_SCORE_BONUSES)
                iBonus = 0;

            ts->wsbBonuses[ts->iNumBonuses].iBonus = iBonus;
            ts->wsbBonuses[ts->iNumBonuses].iWinnerPlace = -1;

            if (++ts->iNumBonuses >= MAX_BONUS_CHESTS)
                break;

            pszTemp = strtok(NULL, ",\n");
        }
    }

    return ts;
}

void WriteTourStopLine(TourStop * ts, char * buffer, bool fIsWorld)
{
    buffer[0] = 0;
    char szTemp[32];

    if (fIsWorld) {
        //Write stage type (battle stage vs. bonus house, etc.)
        sprintf(szTemp, "%d,", ts->iStageType);
        strcat(buffer, szTemp);
    }

    //Battle stage
    if (ts->iStageType == 0) {
        strcat(buffer, ts->pszMapFile);
        strcat(buffer, ",");

        sprintf(szTemp, "%d,", ts->iMode);
        strcat(buffer, szTemp);

        sprintf(szTemp, "%d,", ts->iGoal);
        strcat(buffer, szTemp);

        sprintf(szTemp, "%d,", ts->iPoints);
        strcat(buffer, szTemp);

        if (fIsWorld) {
            if (ts->iNumBonuses <= 0) {
                strcat(buffer, "0");
            } else {
                for (short iBonus = 0; iBonus < ts->iNumBonuses; iBonus++) {
                    if (iBonus > 0)
                        strcat(buffer, "|");

                    strcat(buffer, ts->wsbBonuses[iBonus].szBonusString);
                }
            }

            strcat(buffer, ",");
        } else {
            sprintf(szTemp, "%d,", ts->iBonusType);
            strcat(buffer, szTemp);
        }

        strcat(buffer, ts->szName);
        strcat(buffer, ",");

        if (fIsWorld) {
            sprintf(szTemp, "%d", ts->fEndStage);
            strcat(buffer, szTemp);
        }

        if (ts->fUseSettings) {
            if (ts->iMode == game_mode_classic) { //classic
                if (ts->iNumUsedSettings > 0) {
                    sprintf(szTemp, ",%d", static_cast<short>(ts->gmsSettings.classic.style));
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 1) {
                    sprintf(szTemp, ",%d", static_cast<short>(ts->gmsSettings.classic.scoring));
                    strcat(buffer, szTemp);
                }
            } else if (ts->iMode == game_mode_frag) { //frag
                if (ts->iNumUsedSettings > 0) {
                    sprintf(szTemp, ",%d", static_cast<short>(ts->gmsSettings.frag.style));
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 1) {
                    sprintf(szTemp, ",%d", static_cast<short>(ts->gmsSettings.frag.scoring));
                    strcat(buffer, szTemp);
                }
            } else if (ts->iMode == game_mode_timelimit) { //time
                if (ts->iNumUsedSettings > 0) {
                    sprintf(szTemp, ",%d", static_cast<short>(ts->gmsSettings.time.style));
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 1) {
                    sprintf(szTemp, ",%d", static_cast<short>(ts->gmsSettings.time.scoring));
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 2) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.time.percentextratime);
                    strcat(buffer, szTemp);
                }
            } else if (ts->iMode == game_mode_jail) {
                if (ts->iNumUsedSettings > 0) {
                    sprintf(szTemp, ",%d", static_cast<short>(ts->gmsSettings.jail.style));
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 1) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.jail.timetofree);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 2) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.jail.tagfree);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 3) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.jail.percentkey);
                    strcat(buffer, szTemp);
                }
            } else if (ts->iMode == game_mode_coins) {
                if (ts->iNumUsedSettings > 0) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.coins.penalty);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 1) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.coins.quantity);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 2) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.coins.percentextracoin);
                    strcat(buffer, szTemp);
                }
            } else if (ts->iMode == game_mode_stomp) {
                if (ts->iNumUsedSettings > 0) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.stomp.rate);
                    strcat(buffer, szTemp);
                }

                for (int iEnemy = 0; iEnemy < NUMSTOMPENEMIES; iEnemy++) {
                    if (ts->iNumUsedSettings > iEnemy + 1) {
                        sprintf(szTemp, ",%d", ts->gmsSettings.stomp.enemyweight[iEnemy]);
                        strcat(buffer, szTemp);
                    }
                }
            } else if (ts->iMode == game_mode_eggs) {
                for (int iEgg = 0; iEgg < 4; iEgg++) {
                    if (ts->iNumUsedSettings > iEgg) {
                        sprintf(szTemp, ",%d", ts->gmsSettings.egg.eggs[iEgg]);
                        strcat(buffer, szTemp);
                    }
                }

                for (int iYoshi = 0; iYoshi < 4; iYoshi++) {
                    if (ts->iNumUsedSettings > iYoshi + 4) {
                        sprintf(szTemp, ",%d", ts->gmsSettings.egg.yoshis[iYoshi]);
                        strcat(buffer, szTemp);
                    }
                }

                if (ts->iNumUsedSettings > 8) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.egg.explode);
                    strcat(buffer, szTemp);
                }
            } else if (ts->iMode == game_mode_ctf) {
                if (ts->iNumUsedSettings > 0) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.flag.speed);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 1) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.flag.touchreturn);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 2) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.flag.pointmove);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 3) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.flag.autoreturn);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 4) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.flag.homescore);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 5) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.flag.centerflag);
                    strcat(buffer, szTemp);
                }
            } else if (ts->iMode == game_mode_chicken) {
                if (ts->iNumUsedSettings > 0) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.chicken.usetarget);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 1) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.chicken.glide);
                    strcat(buffer, szTemp);
                }
            } else if (ts->iMode == game_mode_tag) {
                if (ts->iNumUsedSettings > 0) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.tag.tagontouch);
                    strcat(buffer, szTemp);
                }
            } else if (ts->iMode == game_mode_star) {
                if (ts->iNumUsedSettings > 0) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.star.time);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 1) {
                    sprintf(szTemp, ",%d", static_cast<short>(ts->gmsSettings.star.shine));
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 2) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.star.percentextratime);
                    strcat(buffer, szTemp);
                }
            } else if (ts->iMode == game_mode_domination) {
                if (ts->iNumUsedSettings > 0) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.domination.quantity);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 1) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.domination.relocationfrequency);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 2) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.domination.loseondeath);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 3) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.domination.relocateondeath);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 4) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.domination.stealondeath);
                    strcat(buffer, szTemp);
                }
            } else if (ts->iMode == game_mode_koth) {
                if (ts->iNumUsedSettings > 0) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.kingofthehill.areasize);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 1) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.kingofthehill.relocationfrequency);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 2) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.kingofthehill.maxmultiplier);
                    strcat(buffer, szTemp);
                }
            } else if (ts->iMode == game_mode_race) {
                if (ts->iNumUsedSettings > 0) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.race.quantity);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 1) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.race.speed);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 2) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.race.penalty);
                    strcat(buffer, szTemp);
                }
            // NOTE: Owned missing
            } else if (ts->iMode == game_mode_frenzy) {
                if (ts->iNumUsedSettings > 0) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.frenzy.quantity);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 1) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.frenzy.rate);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 2) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.frenzy.storedshells);
                    strcat(buffer, szTemp);
                }

                for (short iPowerup = 0; iPowerup < NUMFRENZYCARDS; iPowerup++) {
                    if (ts->iNumUsedSettings > iPowerup + 3) {
                        sprintf(szTemp, ",%d", ts->gmsSettings.frenzy.powerupweight[iPowerup]);
                        strcat(buffer, szTemp);
                    }
                }
            } else if (ts->iMode == game_mode_survival) {
                for (short iEnemy = 0; iEnemy < NUMSURVIVALENEMIES; iEnemy++) {
                    if (ts->iNumUsedSettings > iEnemy) {
                        sprintf(szTemp, ",%d", ts->gmsSettings.survival.enemyweight[iEnemy]);
                        strcat(buffer, szTemp);
                    }
                }

                if (ts->iNumUsedSettings > 3) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.survival.density);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 4) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.survival.speed);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 5) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.survival.shield);
                    strcat(buffer, szTemp);
                }
            } else if (ts->iMode == game_mode_greed) {
                if (ts->iNumUsedSettings > 0) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.greed.coinlife);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 1) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.greed.owncoins);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 2) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.greed.multiplier);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 3) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.greed.percentextracoin);
                    strcat(buffer, szTemp);
                }
            } else if (ts->iMode == game_mode_health) {
                if (ts->iNumUsedSettings > 0) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.health.startlife);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 1) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.health.maxlife);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 2) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.health.percentextralife);
                    strcat(buffer, szTemp);
                }
            } else if (ts->iMode == game_mode_collection) {
                if (ts->iNumUsedSettings > 0) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.collection.quantity);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 1) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.collection.rate);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 2) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.collection.banktime);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 3) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.collection.cardlife);
                    strcat(buffer, szTemp);
                }
            } else if (ts->iMode == game_mode_chase) {
                if (ts->iNumUsedSettings > 0) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.chase.phantospeed);
                    strcat(buffer, szTemp);
                }

                for (short iPhanto = 0; iPhanto < 3; iPhanto++) {
                    if (ts->iNumUsedSettings > iPhanto + 1) {
                        sprintf(szTemp, ",%d", ts->gmsSettings.chase.phantoquantity[iPhanto]);
                        strcat(buffer, szTemp);
                    }
                }
            } else if (ts->iMode == game_mode_shyguytag) {
                if (ts->iNumUsedSettings > 0) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.shyguytag.tagonsuicide);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 1) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.shyguytag.tagtransfer);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 2) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.shyguytag.freetime);
                    strcat(buffer, szTemp);
                }
            } else if (ts->iMode == game_mode_boss_minigame) {
                if (ts->iNumUsedSettings > 0) {
                    sprintf(szTemp, ",%d", static_cast<short>(ts->gmsSettings.boss.bosstype));
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 1) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.boss.difficulty);
                    strcat(buffer, szTemp);
                }

                if (ts->iNumUsedSettings > 2) {
                    sprintf(szTemp, ",%d", ts->gmsSettings.boss.hitpoints);
                    strcat(buffer, szTemp);
                }
            }
        }
    } else if (ts->iStageType == 1) { //Bonus House
        strcat(buffer, ts->szName);
        strcat(buffer, ",");

        sprintf(szTemp, "%d,", ts->iBonusType);
        strcat(buffer, szTemp);

        for (short iText = 0; iText < ts->iBonusTextLines; iText++) {
            if (iText != 0)
                strcat(buffer, "|");

            strcat(buffer, ts->szBonusText[iText]);
        }

        if (ts->iNumBonuses == 0) {
            strcat(buffer, ",p0");
        } else {
            for (short iBonus = 0; iBonus < ts->iNumBonuses; iBonus++) {
                strcat(buffer, ",");
                strcat(buffer, ts->wsbBonuses[iBonus].szBonusString);
            }
        }
    }

    strcat(buffer, "\n");
}

void ResetTourStops()
{
    game_values.tourstopcurrent = 0;
    game_values.tourstoptotal = 0;

    // added to prevent 'vector iterators incompatible' exception
    if (!game_values.tourstops.empty())
        game_values.tourstops.clear();
}

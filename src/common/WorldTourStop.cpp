#include "WorldTourStop.h"

#include "GameMode.h"
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

/// Reads the next integer token, converts it to type T, and returns `true`.
/// If there are no more tokens, uses the default value, and returns `false`.
template<typename T>
bool ReadTourStopSetting(T& output, T defaultVal)
{
    char* tok = strtok(NULL, ",\n");
    if (tok) {
        output = static_cast<T>(atoi(tok));
        return true;
    } else {
        output = defaultVal;
        return false;
    }
}

// Serialization functions for each game mode setting

template<typename T> std::vector<short> serialize(const T& gms) = delete;

template<> std::vector<short> serialize(const ClassicGameModeSettings& gms) {
    return {
        static_cast<short>(gms.style),
        static_cast<short>(gms.scoring),
    };
}
template<> std::vector<short> serialize(const FragGameModeSettings& gms) {
    return {
        static_cast<short>(gms.style),
        static_cast<short>(gms.scoring),
    };
}
template<> std::vector<short> serialize(const TimeGameModeSettings& gms) {
    return {
        static_cast<short>(gms.style),
        static_cast<short>(gms.scoring),
        gms.percentextratime,
    };
}
template<> std::vector<short> serialize(const JailGameModeSettings& gms) {
    return {
        static_cast<short>(gms.style),
        gms.timetofree,
        gms.tagfree,
        gms.percentkey,
    };
}
template<> std::vector<short> serialize(const CoinGameModeSettings& gms) {
    return {
        gms.penalty,
        gms.quantity,
        gms.percentextracoin,
    };
}
template<> std::vector<short> serialize(const StompGameModeSettings& gms) {
    std::vector<short> values;
    values.reserve(1 + gms.enemyweight.size());
    values.emplace_back(gms.rate);
    values.insert(values.end(), gms.enemyweight.cbegin(), gms.enemyweight.cend());
    return values;
}
template<> std::vector<short> serialize(const EggGameModeSettings& gms) {
    std::vector<short> values;
    values.reserve(gms.eggs.size() + gms.yoshis.size() + 1);
    values.insert(values.end(), gms.eggs.cbegin(), gms.eggs.cend());
    values.insert(values.end(), gms.yoshis.cbegin(), gms.yoshis.cend());
    values.emplace_back(gms.explode);
    return values;
}
template<> std::vector<short> serialize(const FlagGameModeSettings& gms) {
    return {
        gms.speed,
        gms.touchreturn,
        gms.pointmove,
        gms.autoreturn,
        gms.homescore,
        gms.centerflag,
    };
}
template<> std::vector<short> serialize(const ChickenGameModeSettings& gms) {
    return {
        gms.usetarget,
        gms.glide,
    };
}
template<> std::vector<short> serialize(const TagGameModeSettings& gms) {
    return {
        gms.tagontouch,
    };
}
template<> std::vector<short> serialize(const StarGameModeSettings& gms) {
    return {
        gms.time,
        static_cast<short>(gms.shine),
        gms.percentextratime,
    };
}
template<> std::vector<short> serialize(const DominationGameModeSettings& gms) {
    return {
        gms.quantity,
        gms.relocationfrequency,
        gms.loseondeath,
        gms.relocateondeath,
        gms.stealondeath,
    };
}
template<> std::vector<short> serialize(const KingOfTheHillModeSettings& gms) {
    return {
        gms.areasize,
        gms.relocationfrequency,
        gms.maxmultiplier,
    };
}
template<> std::vector<short> serialize(const RaceGameModeSettings& gms) {
    return {
        gms.quantity,
        gms.speed,
        gms.penalty,
    };
}
template<> std::vector<short> serialize(const FrenzyGameModeSettings& gms) {
    std::vector<short> values;
    values.reserve(3 + gms.powerupweight.size());
    values.emplace_back(gms.quantity);
    values.emplace_back(gms.rate);
    values.emplace_back(gms.storedshells);
    values.insert(values.end(), gms.powerupweight.cbegin(), gms.powerupweight.cend());
    return values;
}
template<> std::vector<short> serialize(const SurvivalGameModeSettings& gms) {
    std::vector<short> values;
    values.reserve(gms.enemyweight.size() + 3);
    values.insert(values.end(), gms.enemyweight.cbegin(), gms.enemyweight.cend());
    values.emplace_back(gms.density);
    values.emplace_back(gms.speed);
    values.emplace_back(gms.shield);
    return values;
}
template<> std::vector<short> serialize(const GreedGameModeSettings& gms) {
    return {
        gms.coinlife,
        gms.owncoins,
        gms.multiplier,
        gms.percentextracoin,
    };
}
template<> std::vector<short> serialize(const HealthGameModeSettings& gms) {
    return {
        gms.startlife,
        gms.maxlife,
        gms.percentextralife,
    };
}
template<> std::vector<short> serialize(const CollectionGameModeSettings& gms) {
    return {
        gms.quantity,
        gms.rate,
        gms.banktime,
        gms.cardlife,
    };
}
template<> std::vector<short> serialize(const ChaseGameModeSettings& gms) {
    std::vector<short> values;
    values.reserve(1 + gms.phantoquantity.size());
    values.emplace_back(gms.phantospeed);
    values.insert(values.end(), gms.phantoquantity.cbegin(), gms.phantoquantity.cend());
    return values;
}
template<> std::vector<short> serialize(const ShyGuyTagGameModeSettings& gms) {
    return {
        gms.tagonsuicide,
        gms.tagtransfer,
        gms.freetime,
    };
}
template<> std::vector<short> serialize(const BossGameModeSettings& gms) {
    return {
        static_cast<short>(gms.bosstype),
        gms.difficulty,
        gms.hitpoints,
    };
}

/// Turns the selected game mode settings into a list of integers
std::vector<short> serializeGMS(short gamemodeId, const GameModeSettings& gmsSettings)
{
    switch (gamemodeId) {
        case game_mode_classic: return serialize(gmsSettings.classic);
        case game_mode_frag: return serialize(gmsSettings.frag);
        case game_mode_timelimit: return serialize(gmsSettings.time);
        case game_mode_jail: return serialize(gmsSettings.jail);
        case game_mode_coins: return serialize(gmsSettings.coins);
        case game_mode_stomp: return serialize(gmsSettings.stomp);
        case game_mode_eggs: return serialize(gmsSettings.egg);
        case game_mode_ctf: return serialize(gmsSettings.flag);
        case game_mode_chicken: return serialize(gmsSettings.chicken);
        case game_mode_tag: return serialize(gmsSettings.tag);
        case game_mode_star: return serialize(gmsSettings.star);
        case game_mode_domination: return serialize(gmsSettings.domination);
        case game_mode_koth: return serialize(gmsSettings.kingofthehill);
        case game_mode_race: return serialize(gmsSettings.race);
        case game_mode_frenzy: return serialize(gmsSettings.frenzy);
        case game_mode_survival: return serialize(gmsSettings.survival);
        case game_mode_greed: return serialize(gmsSettings.greed);
        case game_mode_health: return serialize(gmsSettings.health);
        case game_mode_collection: return serialize(gmsSettings.collection);
        case game_mode_chase: return serialize(gmsSettings.chase);
        case game_mode_shyguytag: return serialize(gmsSettings.shyguytag);
        case game_mode_boss_minigame: return serialize(gmsSettings.boss);
    }
    return {};
}

// Deserialization functions for each game mode

template<typename T> std::vector<bool> deserialize(T& gms) = delete;

template<> std::vector<bool> deserialize(ClassicGameModeSettings& gms) {
    return {
        ReadTourStopSetting(gms.style, game_values.gamemodemenusettings.classic.style),
        ReadTourStopSetting(gms.scoring, game_values.gamemodemenusettings.classic.scoring),
    };
}
template<> std::vector<bool> deserialize(FragGameModeSettings& gms) {
    return {
        ReadTourStopSetting(gms.style, game_values.gamemodemenusettings.frag.style),
        ReadTourStopSetting(gms.scoring, game_values.gamemodemenusettings.frag.scoring),
    };
}
template<> std::vector<bool> deserialize(TimeGameModeSettings& gms) {
    return {
        ReadTourStopSetting(gms.style, game_values.gamemodemenusettings.time.style),
        ReadTourStopSetting(gms.scoring, game_values.gamemodemenusettings.time.scoring),
        ReadTourStopSetting(gms.percentextratime, game_values.gamemodemenusettings.time.percentextratime),
    };
}
template<> std::vector<bool> deserialize(JailGameModeSettings& gms) {
    return {
        ReadTourStopSetting(gms.style, game_values.gamemodemenusettings.jail.style),
        ReadTourStopSetting(gms.timetofree, game_values.gamemodemenusettings.jail.timetofree),
        ReadTourStopSetting(gms.tagfree, game_values.gamemodemenusettings.jail.tagfree),
        ReadTourStopSetting(gms.percentkey, game_values.gamemodemenusettings.jail.percentkey),
    };
}
template<> std::vector<bool> deserialize(CoinGameModeSettings& gms) {
    return {
        ReadTourStopSetting(gms.penalty, game_values.gamemodemenusettings.coins.penalty),
        ReadTourStopSetting(gms.quantity, game_values.gamemodemenusettings.coins.quantity),
        ReadTourStopSetting(gms.percentextracoin, game_values.gamemodemenusettings.coins.percentextracoin),
    };
}
template<> std::vector<bool> deserialize(StompGameModeSettings& gms) {
    std::vector<bool> values;
    values.reserve(1 + gms.enemyweight.size());

    values.emplace_back(ReadTourStopSetting(gms.rate, game_values.gamemodemenusettings.stomp.rate));
    for (size_t i = 0; i < gms.enemyweight.size(); i++)
        values.emplace_back(ReadTourStopSetting(gms.enemyweight[i], game_values.gamemodemenusettings.stomp.enemyweight[i]));

    return values;
}
template<> std::vector<bool> deserialize(EggGameModeSettings& gms) {
    std::vector<bool> values;
    values.reserve(gms.eggs.size() + gms.yoshis.size() + 1);

    for (size_t i = 0; i < gms.eggs.size(); i++) {
        values.emplace_back(ReadTourStopSetting(gms.eggs[i], game_values.gamemodemenusettings.egg.eggs[i]));
    }
    for (size_t i = 0; i < gms.yoshis.size(); i++) {
        values.emplace_back(ReadTourStopSetting(gms.yoshis[i], game_values.gamemodemenusettings.egg.yoshis[i]));
    }
    values.emplace_back(ReadTourStopSetting(gms.explode, game_values.gamemodemenusettings.egg.explode));

    return values;
}
template<> std::vector<bool> deserialize(FlagGameModeSettings& gms) {
    return {
        ReadTourStopSetting(gms.speed, game_values.gamemodemenusettings.flag.speed),
        ReadTourStopSetting(gms.touchreturn, game_values.gamemodemenusettings.flag.touchreturn),
        ReadTourStopSetting(gms.pointmove, game_values.gamemodemenusettings.flag.pointmove),
        ReadTourStopSetting(gms.autoreturn, game_values.gamemodemenusettings.flag.autoreturn),
        ReadTourStopSetting(gms.homescore, game_values.gamemodemenusettings.flag.homescore),
        ReadTourStopSetting(gms.centerflag, game_values.gamemodemenusettings.flag.centerflag),
    };
}
template<> std::vector<bool> deserialize(ChickenGameModeSettings& gms) {
    return {
        ReadTourStopSetting(gms.usetarget, game_values.gamemodemenusettings.chicken.usetarget),
        ReadTourStopSetting(gms.glide, game_values.gamemodemenusettings.chicken.glide),
    };
}
template<> std::vector<bool> deserialize(TagGameModeSettings& gms) {
    return {
        ReadTourStopSetting(gms.tagontouch, game_values.gamemodemenusettings.tag.tagontouch),
    };
}
template<> std::vector<bool> deserialize(StarGameModeSettings& gms) {
    return {
        ReadTourStopSetting(gms.time, game_values.gamemodemenusettings.star.time),
        ReadTourStopSetting(gms.shine, game_values.gamemodemenusettings.star.shine),
        ReadTourStopSetting(gms.percentextratime, game_values.gamemodemenusettings.star.percentextratime),
    };
}
template<> std::vector<bool> deserialize(DominationGameModeSettings& gms) {
    return {
        ReadTourStopSetting(gms.quantity, game_values.gamemodemenusettings.domination.quantity),
        ReadTourStopSetting(gms.relocationfrequency, game_values.gamemodemenusettings.domination.relocationfrequency),
        ReadTourStopSetting(gms.loseondeath, game_values.gamemodemenusettings.domination.loseondeath),
        ReadTourStopSetting(gms.relocateondeath, game_values.gamemodemenusettings.domination.relocateondeath),
        ReadTourStopSetting(gms.stealondeath, game_values.gamemodemenusettings.domination.stealondeath),
    };
}
template<> std::vector<bool> deserialize(KingOfTheHillModeSettings& gms) {
    return {
        ReadTourStopSetting(gms.areasize, game_values.gamemodemenusettings.kingofthehill.areasize),
        ReadTourStopSetting(gms.relocationfrequency, game_values.gamemodemenusettings.kingofthehill.relocationfrequency),
        ReadTourStopSetting(gms.maxmultiplier, game_values.gamemodemenusettings.kingofthehill.maxmultiplier),
    };
}
template<> std::vector<bool> deserialize(RaceGameModeSettings& gms) {
    return {
        ReadTourStopSetting(gms.quantity, game_values.gamemodemenusettings.race.quantity),
        ReadTourStopSetting(gms.speed, game_values.gamemodemenusettings.race.speed),
        ReadTourStopSetting(gms.penalty, game_values.gamemodemenusettings.race.penalty),
    };
}
template<> std::vector<bool> deserialize(FrenzyGameModeSettings& gms) {
    std::vector<bool> values;
    values.reserve(3 + gms.powerupweight.size());

    values.emplace_back(ReadTourStopSetting(gms.quantity, game_values.gamemodemenusettings.frenzy.quantity));
    values.emplace_back(ReadTourStopSetting(gms.rate, game_values.gamemodemenusettings.frenzy.rate));
    values.emplace_back(ReadTourStopSetting(gms.storedshells, game_values.gamemodemenusettings.frenzy.storedshells));
    for (size_t i = 0; i < gms.powerupweight.size(); i++) {
        values.emplace_back(ReadTourStopSetting(gms.powerupweight[i], game_values.gamemodemenusettings.frenzy.powerupweight[i]));
    }

    return values;
}
template<> std::vector<bool> deserialize(SurvivalGameModeSettings& gms) {
    std::vector<bool> values;
    values.reserve(gms.enemyweight.size() + 3);

    for (size_t i = 0; i < gms.enemyweight.size(); i++) {
        values.emplace_back(ReadTourStopSetting(gms.enemyweight[i], game_values.gamemodemenusettings.survival.enemyweight[i]));
    }
    values.emplace_back(ReadTourStopSetting(gms.density, game_values.gamemodemenusettings.survival.density));
    values.emplace_back(ReadTourStopSetting(gms.speed, game_values.gamemodemenusettings.survival.speed));
    values.emplace_back(ReadTourStopSetting(gms.shield, game_values.gamemodemenusettings.survival.shield));

    return values;
}
template<> std::vector<bool> deserialize(GreedGameModeSettings& gms) {
    return {
        ReadTourStopSetting(gms.coinlife, game_values.gamemodemenusettings.greed.coinlife),
        ReadTourStopSetting(gms.owncoins, game_values.gamemodemenusettings.greed.owncoins),
        ReadTourStopSetting(gms.multiplier, game_values.gamemodemenusettings.greed.multiplier),
        ReadTourStopSetting(gms.percentextracoin, game_values.gamemodemenusettings.greed.percentextracoin),
    };
}
template<> std::vector<bool> deserialize(HealthGameModeSettings& gms) {
    return {
        ReadTourStopSetting(gms.startlife, game_values.gamemodemenusettings.health.startlife),
        ReadTourStopSetting(gms.maxlife, game_values.gamemodemenusettings.health.maxlife),
        ReadTourStopSetting(gms.percentextralife, game_values.gamemodemenusettings.health.percentextralife),
    };
}
template<> std::vector<bool> deserialize(CollectionGameModeSettings& gms) {
    return {
        ReadTourStopSetting(gms.quantity, game_values.gamemodemenusettings.collection.quantity),
        ReadTourStopSetting(gms.rate, game_values.gamemodemenusettings.collection.rate),
        ReadTourStopSetting(gms.banktime, game_values.gamemodemenusettings.collection.banktime),
        ReadTourStopSetting(gms.cardlife, game_values.gamemodemenusettings.collection.cardlife),
    };
}
template<> std::vector<bool> deserialize(ChaseGameModeSettings& gms) {
    std::vector<bool> values;
    values.reserve(1 + gms.phantoquantity.size());

    values.emplace_back(ReadTourStopSetting(gms.phantospeed, game_values.gamemodemenusettings.chase.phantospeed));
    for (size_t i = 0; i < gms.phantoquantity.size(); i++)
        values.emplace_back(ReadTourStopSetting(gms.phantoquantity[i], game_values.gamemodemenusettings.chase.phantoquantity[i]));

    return values;
}
template<> std::vector<bool> deserialize(ShyGuyTagGameModeSettings& gms) {
    return {
        ReadTourStopSetting(gms.tagonsuicide, game_values.gamemodemenusettings.shyguytag.tagonsuicide),
        ReadTourStopSetting(gms.tagtransfer, game_values.gamemodemenusettings.shyguytag.tagtransfer),
        ReadTourStopSetting(gms.freetime, game_values.gamemodemenusettings.shyguytag.freetime),
    };
}
template<> std::vector<bool> deserialize(BossGameModeSettings& gms) {
    return {
        ReadTourStopSetting(gms.bosstype, game_values.gamemodemenusettings.boss.bosstype),
        ReadTourStopSetting(gms.difficulty, game_values.gamemodemenusettings.boss.difficulty),
        ReadTourStopSetting(gms.hitpoints, game_values.gamemodemenusettings.boss.hitpoints),
    };
}

/// Parses the active tokenizer of `strtok` (a list of integers) into the selected game mode settings
std::vector<bool> deserializeGMS(short gamemodeId, GameModeSettings& gmsSettings)
{
    switch (gamemodeId) {
        case game_mode_classic: return deserialize(gmsSettings.classic);
        case game_mode_frag: return deserialize(gmsSettings.frag);
        case game_mode_timelimit: return deserialize(gmsSettings.time);
        case game_mode_jail: return deserialize(gmsSettings.jail);
        case game_mode_coins: return deserialize(gmsSettings.coins);
        case game_mode_stomp: return deserialize(gmsSettings.stomp);
        case game_mode_eggs: return deserialize(gmsSettings.egg);
        case game_mode_ctf: return deserialize(gmsSettings.flag);
        case game_mode_chicken: return deserialize(gmsSettings.chicken);
        case game_mode_tag: return deserialize(gmsSettings.tag);
        case game_mode_star: return deserialize(gmsSettings.star);
        case game_mode_domination: return deserialize(gmsSettings.domination);
        case game_mode_koth: return deserialize(gmsSettings.kingofthehill);
        case game_mode_race: return deserialize(gmsSettings.race);
        case game_mode_frenzy: return deserialize(gmsSettings.frenzy);
        case game_mode_survival: return deserialize(gmsSettings.survival);
        case game_mode_greed: return deserialize(gmsSettings.greed);
        case game_mode_health: return deserialize(gmsSettings.health);
        case game_mode_collection: return deserialize(gmsSettings.collection);
        case game_mode_chase: return deserialize(gmsSettings.chase);
        case game_mode_shyguytag: return deserialize(gmsSettings.shyguytag);
        case game_mode_boss_minigame: return deserialize(gmsSettings.boss);
    }
    return {};
}
} // namespace


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

            const std::vector<bool> usedSettings = deserializeGMS(ts->iMode, ts->gmsSettings);
            ts->iNumUsedSettings = std::count(usedSettings.cbegin(), usedSettings.cend(), true);
            ts->fUseSettings = !usedSettings.empty();
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

        std::string field;
        const char* delim = DELIM_EMPTY;
        for (short iText = 0; iText < ts.iBonusTextLines; iText++) {
            field += delim;
            field += ts.szBonusText[iText];
            delim = DELIM_PIPE;
        }

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
    game_values.tourstoptotal = 0;

    // added to prevent 'vector iterators incompatible' exception
    if (!game_values.tourstops.empty())
        game_values.tourstops.clear();
}

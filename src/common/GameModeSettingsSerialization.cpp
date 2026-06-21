#include "GameModeSettingsSerialization.h"

#include "GameModeType.h"
#include "GameModeSettings.h"

#include <cstdlib>
#include <cstring>


namespace {
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

// Deserialization functions for each game mode

template<typename T> std::vector<bool> deserialize(T& out, const T& current) = delete;

template<> std::vector<bool> deserialize(ClassicGameModeSettings& out, const ClassicGameModeSettings& current) {
    return {
        ReadTourStopSetting(out.style, current.style),
        ReadTourStopSetting(out.scoring, current.scoring),
    };
}
template<> std::vector<bool> deserialize(FragGameModeSettings& out, const FragGameModeSettings& current) {
    return {
        ReadTourStopSetting(out.style, current.style),
        ReadTourStopSetting(out.scoring, current.scoring),
    };
}
template<> std::vector<bool> deserialize(TimeGameModeSettings& out, const TimeGameModeSettings& current) {
    return {
        ReadTourStopSetting(out.style, current.style),
        ReadTourStopSetting(out.scoring, current.scoring),
        ReadTourStopSetting(out.percentextratime, current.percentextratime),
    };
}
template<> std::vector<bool> deserialize(JailGameModeSettings& out, const JailGameModeSettings& current) {
    return {
        ReadTourStopSetting(out.style, current.style),
        ReadTourStopSetting(out.timetofree, current.timetofree),
        ReadTourStopSetting(out.tagfree, current.tagfree),
        ReadTourStopSetting(out.percentkey, current.percentkey),
    };
}
template<> std::vector<bool> deserialize(CoinGameModeSettings& out, const CoinGameModeSettings& current) {
    return {
        ReadTourStopSetting(out.penalty, current.penalty),
        ReadTourStopSetting(out.quantity, current.quantity),
        ReadTourStopSetting(out.percentextracoin, current.percentextracoin),
    };
}
template<> std::vector<bool> deserialize(StompGameModeSettings& out, const StompGameModeSettings& current) {
    std::vector<bool> values;
    values.reserve(1 + out.enemyweight.size());

    values.emplace_back(ReadTourStopSetting(out.rate, current.rate));
    for (size_t i = 0; i < out.enemyweight.size(); i++)
        values.emplace_back(ReadTourStopSetting(out.enemyweight[i], current.enemyweight[i]));

    return values;
}
template<> std::vector<bool> deserialize(EggGameModeSettings& out, const EggGameModeSettings& current) {
    std::vector<bool> values;
    values.reserve(out.eggs.size() + out.yoshis.size() + 1);

    for (size_t i = 0; i < out.eggs.size(); i++) {
        values.emplace_back(ReadTourStopSetting(out.eggs[i], current.eggs[i]));
    }
    for (size_t i = 0; i < out.yoshis.size(); i++) {
        values.emplace_back(ReadTourStopSetting(out.yoshis[i], current.yoshis[i]));
    }
    values.emplace_back(ReadTourStopSetting(out.explode, current.explode));

    return values;
}
template<> std::vector<bool> deserialize(FlagGameModeSettings& out, const FlagGameModeSettings& current) {
    return {
        ReadTourStopSetting(out.speed, current.speed),
        ReadTourStopSetting(out.touchreturn, current.touchreturn),
        ReadTourStopSetting(out.pointmove, current.pointmove),
        ReadTourStopSetting(out.autoreturn, current.autoreturn),
        ReadTourStopSetting(out.homescore, current.homescore),
        ReadTourStopSetting(out.centerflag, current.centerflag),
    };
}
template<> std::vector<bool> deserialize(ChickenGameModeSettings& out, const ChickenGameModeSettings& current) {
    return {
        ReadTourStopSetting(out.usetarget, current.usetarget),
        ReadTourStopSetting(out.glide, current.glide),
    };
}
template<> std::vector<bool> deserialize(TagGameModeSettings& out, const TagGameModeSettings& current) {
    return {
        ReadTourStopSetting(out.tagontouch, current.tagontouch),
    };
}
template<> std::vector<bool> deserialize(StarGameModeSettings& out, const StarGameModeSettings& current) {
    return {
        ReadTourStopSetting(out.time, current.time),
        ReadTourStopSetting(out.shine, current.shine),
        ReadTourStopSetting(out.percentextratime, current.percentextratime),
    };
}
template<> std::vector<bool> deserialize(DominationGameModeSettings& out, const DominationGameModeSettings& current) {
    return {
        ReadTourStopSetting(out.quantity, current.quantity),
        ReadTourStopSetting(out.relocationfrequency, current.relocationfrequency),
        ReadTourStopSetting(out.loseondeath, current.loseondeath),
        ReadTourStopSetting(out.relocateondeath, current.relocateondeath),
        ReadTourStopSetting(out.stealondeath, current.stealondeath),
    };
}
template<> std::vector<bool> deserialize(KingOfTheHillModeSettings& out, const KingOfTheHillModeSettings& current) {
    return {
        ReadTourStopSetting(out.areasize, current.areasize),
        ReadTourStopSetting(out.relocationfrequency, current.relocationfrequency),
        ReadTourStopSetting(out.maxmultiplier, current.maxmultiplier),
    };
}
template<> std::vector<bool> deserialize(RaceGameModeSettings& out, const RaceGameModeSettings& current) {
    return {
        ReadTourStopSetting(out.quantity, current.quantity),
        ReadTourStopSetting(out.speed, current.speed),
        ReadTourStopSetting(out.penalty, current.penalty),
    };
}
template<> std::vector<bool> deserialize(FrenzyGameModeSettings& out, const FrenzyGameModeSettings& current) {
    std::vector<bool> values;
    values.reserve(3 + out.powerupweight.size());

    values.emplace_back(ReadTourStopSetting(out.quantity, current.quantity));
    values.emplace_back(ReadTourStopSetting(out.rate, current.rate));
    values.emplace_back(ReadTourStopSetting(out.storedshells, current.storedshells));
    for (size_t i = 0; i < out.powerupweight.size(); i++) {
        values.emplace_back(ReadTourStopSetting(out.powerupweight[i], current.powerupweight[i]));
    }

    return values;
}
template<> std::vector<bool> deserialize(SurvivalGameModeSettings& out, const SurvivalGameModeSettings& current) {
    std::vector<bool> values;
    values.reserve(out.enemyweight.size() + 3);

    for (size_t i = 0; i < out.enemyweight.size(); i++) {
        values.emplace_back(ReadTourStopSetting(out.enemyweight[i], current.enemyweight[i]));
    }
    values.emplace_back(ReadTourStopSetting(out.density, current.density));
    values.emplace_back(ReadTourStopSetting(out.speed, current.speed));
    values.emplace_back(ReadTourStopSetting(out.shield, current.shield));

    return values;
}
template<> std::vector<bool> deserialize(GreedGameModeSettings& out, const GreedGameModeSettings& current) {
    return {
        ReadTourStopSetting(out.coinlife, current.coinlife),
        ReadTourStopSetting(out.owncoins, current.owncoins),
        ReadTourStopSetting(out.multiplier, current.multiplier),
        ReadTourStopSetting(out.percentextracoin, current.percentextracoin),
    };
}
template<> std::vector<bool> deserialize(HealthGameModeSettings& out, const HealthGameModeSettings& current) {
    return {
        ReadTourStopSetting(out.startlife, current.startlife),
        ReadTourStopSetting(out.maxlife, current.maxlife),
        ReadTourStopSetting(out.percentextralife, current.percentextralife),
    };
}
template<> std::vector<bool> deserialize(CollectionGameModeSettings& out, const CollectionGameModeSettings& current) {
    return {
        ReadTourStopSetting(out.quantity, current.quantity),
        ReadTourStopSetting(out.rate, current.rate),
        ReadTourStopSetting(out.banktime, current.banktime),
        ReadTourStopSetting(out.cardlife, current.cardlife),
    };
}
template<> std::vector<bool> deserialize(ChaseGameModeSettings& out, const ChaseGameModeSettings& current) {
    std::vector<bool> values;
    values.reserve(1 + out.phantoquantity.size());

    values.emplace_back(ReadTourStopSetting(out.phantospeed, current.phantospeed));
    for (size_t i = 0; i < out.phantoquantity.size(); i++)
        values.emplace_back(ReadTourStopSetting(out.phantoquantity[i], current.phantoquantity[i]));

    return values;
}
template<> std::vector<bool> deserialize(ShyGuyTagGameModeSettings& out, const ShyGuyTagGameModeSettings& current) {
    return {
        ReadTourStopSetting(out.tagonsuicide, current.tagonsuicide),
        ReadTourStopSetting(out.tagtransfer, current.tagtransfer),
        ReadTourStopSetting(out.freetime, current.freetime),
    };
}
template<> std::vector<bool> deserialize(BossGameModeSettings& out, const BossGameModeSettings& current) {
    return {
        ReadTourStopSetting(out.bosstype, current.bosstype),
        ReadTourStopSetting(out.difficulty, current.difficulty),
        ReadTourStopSetting(out.hitpoints, current.hitpoints),
    };
}
} // namespace

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

std::vector<bool> deserializeGMS(short gamemodeId, GameModeSettings& out, const GameModeSettings& current)
{
    switch (gamemodeId) {
        case game_mode_classic: return deserialize(out.classic, current.classic);
        case game_mode_frag: return deserialize(out.frag, current.frag);
        case game_mode_timelimit: return deserialize(out.time, current.time);
        case game_mode_jail: return deserialize(out.jail, current.jail);
        case game_mode_coins: return deserialize(out.coins, current.coins);
        case game_mode_stomp: return deserialize(out.stomp, current.stomp);
        case game_mode_eggs: return deserialize(out.egg, current.egg);
        case game_mode_ctf: return deserialize(out.flag, current.flag);
        case game_mode_chicken: return deserialize(out.chicken, current.chicken);
        case game_mode_tag: return deserialize(out.tag, current.tag);
        case game_mode_star: return deserialize(out.star, current.star);
        case game_mode_domination: return deserialize(out.domination, current.domination);
        case game_mode_koth: return deserialize(out.kingofthehill, current.kingofthehill);
        case game_mode_race: return deserialize(out.race, current.race);
        case game_mode_frenzy: return deserialize(out.frenzy, current.frenzy);
        case game_mode_survival: return deserialize(out.survival, current.survival);
        case game_mode_greed: return deserialize(out.greed, current.greed);
        case game_mode_health: return deserialize(out.health, current.health);
        case game_mode_collection: return deserialize(out.collection, current.collection);
        case game_mode_chase: return deserialize(out.chase, current.chase);
        case game_mode_shyguytag: return deserialize(out.shyguytag, current.shyguytag);
        case game_mode_boss_minigame: return deserialize(out.boss, current.boss);
    }
    return {};
}

#include "doctest.h"

#include "GameModeSettings.h"
#include "GameModeSettingsSerialization.h"
#include "GameModeType.h"

#include <cassert>
#include <cstring>
#include <string>
#include <vector>


std::string join(const std::vector<short>& values)
{
    std::string out;
    const char* delim = "";
    for (short value : values) {
        out += delim;
        out += std::to_string(value);
        delim = ",";
    }
    return out;
}

template<typename T> GameModeType keyByType() = delete;
template<> GameModeType keyByType<ClassicGameModeSettings>() { return game_mode_classic; }
template<> GameModeType keyByType<FragGameModeSettings>() { return game_mode_frag; }
template<> GameModeType keyByType<TimeGameModeSettings>() { return game_mode_timelimit; }
template<> GameModeType keyByType<JailGameModeSettings>() { return game_mode_jail; }
template<> GameModeType keyByType<CoinGameModeSettings>() { return game_mode_coins; }
template<> GameModeType keyByType<StompGameModeSettings>() { return game_mode_stomp; }
template<> GameModeType keyByType<EggGameModeSettings>() { return game_mode_eggs; }
template<> GameModeType keyByType<FlagGameModeSettings>() { return game_mode_ctf; }
template<> GameModeType keyByType<ChickenGameModeSettings>() { return game_mode_chicken; }
template<> GameModeType keyByType<TagGameModeSettings>() { return game_mode_tag; }
template<> GameModeType keyByType<StarGameModeSettings>() { return game_mode_star; }
template<> GameModeType keyByType<DominationGameModeSettings>() { return game_mode_domination; }
template<> GameModeType keyByType<KingOfTheHillModeSettings>() { return game_mode_koth; }
template<> GameModeType keyByType<RaceGameModeSettings>() { return game_mode_race; }
template<> GameModeType keyByType<FrenzyGameModeSettings>() { return game_mode_frenzy; }
template<> GameModeType keyByType<SurvivalGameModeSettings>() { return game_mode_survival; }
template<> GameModeType keyByType<GreedGameModeSettings>() { return game_mode_greed; }
template<> GameModeType keyByType<HealthGameModeSettings>() { return game_mode_health; }
template<> GameModeType keyByType<CollectionGameModeSettings>() { return game_mode_collection; }
template<> GameModeType keyByType<ChaseGameModeSettings>() { return game_mode_chase; }
template<> GameModeType keyByType<ShyGuyTagGameModeSettings>() { return game_mode_shyguytag; }
template<> GameModeType keyByType<BossGameModeSettings>() { return game_mode_boss_minigame; }

template<typename T> auto settingByType(const GameModeSettings& gms) = delete;
template<> auto settingByType<ClassicGameModeSettings>(const GameModeSettings& gms) { return gms.classic; }
template<> auto settingByType<FragGameModeSettings>(const GameModeSettings& gms) { return gms.frag; }
template<> auto settingByType<TimeGameModeSettings>(const GameModeSettings& gms) { return gms.time; }
template<> auto settingByType<JailGameModeSettings>(const GameModeSettings& gms) { return gms.jail; }
template<> auto settingByType<CoinGameModeSettings>(const GameModeSettings& gms) { return gms.coins; }
template<> auto settingByType<StompGameModeSettings>(const GameModeSettings& gms) { return gms.stomp; }
template<> auto settingByType<EggGameModeSettings>(const GameModeSettings& gms) { return gms.egg; }
template<> auto settingByType<FlagGameModeSettings>(const GameModeSettings& gms) { return gms.flag; }
template<> auto settingByType<ChickenGameModeSettings>(const GameModeSettings& gms) { return gms.chicken; }
template<> auto settingByType<TagGameModeSettings>(const GameModeSettings& gms) { return gms.tag; }
template<> auto settingByType<StarGameModeSettings>(const GameModeSettings& gms) { return gms.star; }
template<> auto settingByType<DominationGameModeSettings>(const GameModeSettings& gms) { return gms.domination; }
template<> auto settingByType<KingOfTheHillModeSettings>(const GameModeSettings& gms) { return gms.kingofthehill; }
template<> auto settingByType<RaceGameModeSettings>(const GameModeSettings& gms) { return gms.race; }
template<> auto settingByType<FrenzyGameModeSettings>(const GameModeSettings& gms) { return gms.frenzy; }
template<> auto settingByType<SurvivalGameModeSettings>(const GameModeSettings& gms) { return gms.survival; }
template<> auto settingByType<GreedGameModeSettings>(const GameModeSettings& gms) { return gms.greed; }
template<> auto settingByType<HealthGameModeSettings>(const GameModeSettings& gms) { return gms.health; }
template<> auto settingByType<CollectionGameModeSettings>(const GameModeSettings& gms) { return gms.collection; }
template<> auto settingByType<ChaseGameModeSettings>(const GameModeSettings& gms) { return gms.chase; }
template<> auto settingByType<ShyGuyTagGameModeSettings>(const GameModeSettings& gms) { return gms.shyguytag; }
template<> auto settingByType<BossGameModeSettings>(const GameModeSettings& gms) { return gms.boss; }


TEST_CASE_TEMPLATE("Serialization", T,
    ClassicGameModeSettings,
    FragGameModeSettings,
    TimeGameModeSettings,
    JailGameModeSettings,
    CoinGameModeSettings,
    StompGameModeSettings,
    EggGameModeSettings,
    FlagGameModeSettings,
    ChickenGameModeSettings,
    TagGameModeSettings,
    StarGameModeSettings,
    DominationGameModeSettings,
    KingOfTheHillModeSettings,
    RaceGameModeSettings,
    FrenzyGameModeSettings,
    SurvivalGameModeSettings,
    GreedGameModeSettings,
    HealthGameModeSettings,
    CollectionGameModeSettings,
    ChaseGameModeSettings,
    ShyGuyTagGameModeSettings,
    BossGameModeSettings)
{
    const GameModeSettings base;
    // TODO: Modify the defaults

    const std::vector<short> serialized = serializeGMS(keyByType<T>(), base);
    const std::string serializedText = "dummy," + join(serialized);

    char buffer[256] = {};
    strncpy(buffer, serializedText.c_str(), std::min<size_t>(serializedText.size(), 255));
    strtok(buffer, ",\n");

    GameModeSettings out;
    deserializeGMS(keyByType<T>(), out, base);

    const auto expected = settingByType<T>(base);
    const auto actual = settingByType<T>(out);
    CHECK(actual == expected);
}

#pragma once

#include "EyecandyStyles.h"
#include "GameModeSettings.h"
#include "GameplayStyles.h"
#include "GameValues.h"

#include <toml.hpp>

#include <array>
#include <cstdio>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <vector>

namespace tomlconfig {

template <typename E> struct NamedEnum;

template<> struct NamedEnum<Boss> {
    static constexpr auto table = std::to_array<std::pair<Boss, std::string_view>>({
        { Boss::Hammer, "hammer" },
        { Boss::Bomb, "bomb" },
        { Boss::Fire, "fire" },
    });
};
template<> struct NamedEnum<SpawnStyle> {
    static constexpr auto table = std::to_array<std::pair<SpawnStyle, std::string_view>>({
        { SpawnStyle::Instant, "instant" },
        { SpawnStyle::Door, "door" },
        { SpawnStyle::Swirl, "swirl" },
    });
};
template<> struct NamedEnum<AwardStyle> {
    static constexpr auto table = std::to_array<std::pair<AwardStyle, std::string_view>>({
        { AwardStyle::None, "none" },
        { AwardStyle::Fireworks, "fireworks" },
        { AwardStyle::Swirl, "swirl" },
        { AwardStyle::Halo, "halo" },
        { AwardStyle::Souls, "souls" },
        { AwardStyle::Text, "text" },
    });
};
template<> struct NamedEnum<TeamCollisionStyle> {
    static constexpr auto table = std::to_array<std::pair<TeamCollisionStyle, std::string_view>>({
        { TeamCollisionStyle::Off, "off" },
        { TeamCollisionStyle::Assist, "assist" },
        { TeamCollisionStyle::On, "on" },
    });
};
template<> struct NamedEnum<ScoreboardStyle> {
    static constexpr auto table = std::to_array<std::pair<ScoreboardStyle, std::string_view>>({
        { ScoreboardStyle::Top, "top" },
        { ScoreboardStyle::Bottom, "bottom" },
        { ScoreboardStyle::Corners, "corners" },
    });
};
template<> struct NamedEnum<TournamentControlStyle> {
    static constexpr auto table = std::to_array<std::pair<TournamentControlStyle, std::string_view>>({
        { TournamentControlStyle::All, "all" },
        { TournamentControlStyle::GameWinner, "game_winner" },
        { TournamentControlStyle::GameLoser, "game_loser" },
        { TournamentControlStyle::LeadingTeams, "leading_teams" },
        { TournamentControlStyle::TrailingTeams, "trailing_teams" },
        { TournamentControlStyle::Random, "random" },
        { TournamentControlStyle::RandomLoser, "random_loser" },
        { TournamentControlStyle::RoundRobin, "round_robin" },
    });
};
template<> struct NamedEnum<DeathStyle> {
    static constexpr auto table = std::to_array<std::pair<DeathStyle, std::string_view>>({
        { DeathStyle::Respawn, "respawn" },
        { DeathStyle::Shield, "shield" },
    });
};
template<> struct NamedEnum<ScoringStyle> {
    static constexpr auto table = std::to_array<std::pair<ScoringStyle, std::string_view>>({
        { ScoringStyle::AllKills, "all_kills" },
        { ScoringStyle::PushOnly, "push_only" },
    });
};
template<> struct NamedEnum<JailStyle> {
    static constexpr auto table = std::to_array<std::pair<JailStyle, std::string_view>>({
        { JailStyle::Classic, "classic" },
        { JailStyle::Owned, "owned" },
        { JailStyle::FreeForAll, "free_for_all" },
    });
};
template<> struct NamedEnum<ShieldStyle> {
    static constexpr auto table = std::to_array<std::pair<ShieldStyle, std::string_view>>({
        { ShieldStyle::NoShield, "no_shield" },
        { ShieldStyle::Soft, "soft" },
        { ShieldStyle::SoftWithStomp, "soft_with_stomp" },
        { ShieldStyle::Hard, "hard" },
    });
};
template<> struct NamedEnum<StarStyle> {
    static constexpr auto table = std::to_array<std::pair<StarStyle, std::string_view>>({
        { StarStyle::Ztar, "ztar" },
        { StarStyle::Shine, "shine" },
        { StarStyle::Multi, "multi" },
        { StarStyle::Random, "random" },
    });
};
template<> struct NamedEnum<WarpLockStyle> {
    static constexpr auto table = std::to_array<std::pair<WarpLockStyle, std::string_view>>({
        { WarpLockStyle::EntranceOnly, "entrance_only" },
        { WarpLockStyle::ExitOnly, "exit_only" },
        { WarpLockStyle::EntranceAndExit, "entrance_and_exit" },
        { WarpLockStyle::EntireConnection, "entire_connection" },
        { WarpLockStyle::AllWarps, "all_warps" },
    });
};
template<> struct NamedEnum<BoomerangStyle> {
    static constexpr auto table = std::to_array<std::pair<BoomerangStyle, std::string_view>>({
        { BoomerangStyle::Flat, "flat" },
        { BoomerangStyle::SMB3, "smb3" },
        { BoomerangStyle::Zelda, "zelda" },
        { BoomerangStyle::Random, "random" },
    });
};
template<> struct NamedEnum<MysteryMushroomSwapStyle> {
    static constexpr auto table = std::to_array<std::pair<MysteryMushroomSwapStyle, std::string_view>>({
        { MysteryMushroomSwapStyle::Walk, "walk" },
        { MysteryMushroomSwapStyle::Blink, "blink" },
        { MysteryMushroomSwapStyle::Instant, "instant" },
    });
};
template<> struct NamedEnum<BonusWheelStyle> {
    static constexpr auto table = std::to_array<std::pair<BonusWheelStyle, std::string_view>>({
        { BonusWheelStyle::Off, "off" },
        { BonusWheelStyle::TournamentWin, "tournament_win" },
        { BonusWheelStyle::EveryGame, "every_game" },
    });
};

template<> struct NamedEnum<PowerupDropRateStyle> {
    static constexpr auto table = std::to_array<std::pair<PowerupDropRateStyle, std::string_view>>({
        { PowerupDropRateStyle::UseMap, "use_map" },
        { PowerupDropRateStyle::UseGame, "use_game" },
        { PowerupDropRateStyle::SimpleAverage, "simple_average" },
        { PowerupDropRateStyle::WeightedAverage, "weighted_average" },
    });
};


template <typename E>
constexpr std::string_view as_str(E value) {
    for (auto& [e, name] : NamedEnum<E>::table)
        if (e == value) return name;
    return {};
}

template <typename E>
constexpr std::optional<E> from_str(std::string_view s) {
    for (auto& [e, name] : NamedEnum<E>::table)
        if (name == s) return e;
    return std::nullopt;
}


template <typename Struct, typename T>
struct Field {
    std::string_view name;
    T Struct::* member;
};

template <typename S, typename T>
constexpr Field<S, T> field(std::string_view name, T S::* member) {
    return {name, member};
}


template <typename S> struct NamedStruct;

template<> struct NamedStruct<ClassicGameModeSettings> {
    static constexpr auto table = std::make_tuple(
        field("death_style", &ClassicGameModeSettings::style),
        field("scoring_style", &ClassicGameModeSettings::scoring));
};
template<> struct NamedStruct<FragGameModeSettings> {
    static constexpr auto table = std::make_tuple(
        field("death_style", &FragGameModeSettings::style),
        field("scoring_style", &FragGameModeSettings::scoring));
};
template<> struct NamedStruct<TimeGameModeSettings> {
    static constexpr auto table = std::make_tuple(
        field("death_style", &TimeGameModeSettings::style),
        field("scoring_style", &TimeGameModeSettings::scoring),
        field("percent_extra_time", &TimeGameModeSettings::percentextratime));
};
template<> struct NamedStruct<JailGameModeSettings> {
    static constexpr auto table = std::make_tuple(
        field("style", &JailGameModeSettings::style),
        field("tagfree", &JailGameModeSettings::tagfree),
        field("timetofree", &JailGameModeSettings::timetofree),
        field("percentkey", &JailGameModeSettings::percentkey));
};
template<> struct NamedStruct<CoinGameModeSettings> {
    static constexpr auto table = std::make_tuple(
        field("penalty", &CoinGameModeSettings::penalty),
        field("quantity", &CoinGameModeSettings::quantity),
        field("percentextracoin", &CoinGameModeSettings::percentextracoin));
};
template<> struct NamedStruct<StompGameModeSettings> {
    static constexpr auto table = std::make_tuple(
        field("rate", &StompGameModeSettings::rate),
        field("enemyweight", &StompGameModeSettings::enemyweight));
};
template<> struct NamedStruct<EggGameModeSettings> {
    static constexpr auto table = std::make_tuple(
        field("eggs", &EggGameModeSettings::eggs),
        field("yoshis", &EggGameModeSettings::yoshis),
        field("explode", &EggGameModeSettings::explode));
};
template<> struct NamedStruct<FlagGameModeSettings> {
    static constexpr auto table = std::make_tuple(
        field("speed", &FlagGameModeSettings::speed),
        field("touchreturn", &FlagGameModeSettings::touchreturn),
        field("pointmove", &FlagGameModeSettings::pointmove),
        field("autoreturn", &FlagGameModeSettings::autoreturn),
        field("homescore", &FlagGameModeSettings::homescore),
        field("centerflag", &FlagGameModeSettings::centerflag));
};
template<> struct NamedStruct<ChickenGameModeSettings> {
    static constexpr auto table = std::make_tuple(
        field("usetarget", &ChickenGameModeSettings::usetarget),
        field("glide", &ChickenGameModeSettings::glide));
};
template<> struct NamedStruct<TagGameModeSettings> {
    static constexpr auto table = std::make_tuple(
        field("tagontouch", &TagGameModeSettings::tagontouch));
};
template<> struct NamedStruct<StarGameModeSettings> {
    static constexpr auto table = std::make_tuple(
        field("time", &StarGameModeSettings::time),
        field("shine", &StarGameModeSettings::shine),
        field("percentextratime", &StarGameModeSettings::percentextratime));
};
template<> struct NamedStruct<DominationGameModeSettings> {
    static constexpr auto table = std::make_tuple(
        field("loseondeath", &DominationGameModeSettings::loseondeath),
        field("stealondeath", &DominationGameModeSettings::stealondeath),
        field("relocateondeath", &DominationGameModeSettings::relocateondeath),
        field("relocationfrequency", &DominationGameModeSettings::relocationfrequency),
        field("quantity", &DominationGameModeSettings::quantity));
};
template<> struct NamedStruct<KingOfTheHillModeSettings> {
    static constexpr auto table = std::make_tuple(
        field("areasize", &KingOfTheHillModeSettings::areasize),
        field("relocationfrequency", &KingOfTheHillModeSettings::relocationfrequency),
        field("maxmultiplier", &KingOfTheHillModeSettings::maxmultiplier));
};
template<> struct NamedStruct<RaceGameModeSettings> {
    static constexpr auto table = std::make_tuple(
        field("quantity", &RaceGameModeSettings::quantity),
        field("speed", &RaceGameModeSettings::speed),
        field("penalty", &RaceGameModeSettings::penalty));
};
template<> struct NamedStruct<FrenzyGameModeSettings> {
    static constexpr auto table = std::make_tuple(
        field("quantity", &FrenzyGameModeSettings::quantity),
        field("rate", &FrenzyGameModeSettings::rate),
        field("storedshells", &FrenzyGameModeSettings::storedshells),
        field("powerupweight", &FrenzyGameModeSettings::powerupweight));
};
template<> struct NamedStruct<SurvivalGameModeSettings> {
    static constexpr auto table = std::make_tuple(
        field("enemyweight", &SurvivalGameModeSettings::enemyweight),
        field("density", &SurvivalGameModeSettings::density),
        field("speed", &SurvivalGameModeSettings::speed),
        field("shield", &SurvivalGameModeSettings::shield));
};
template<> struct NamedStruct<GreedGameModeSettings> {
    static constexpr auto table = std::make_tuple(
        field("coinlife", &GreedGameModeSettings::coinlife),
        field("owncoins", &GreedGameModeSettings::owncoins),
        field("multiplier", &GreedGameModeSettings::multiplier),
        field("percentextracoin", &GreedGameModeSettings::percentextracoin));
};
template<> struct NamedStruct<HealthGameModeSettings> {
    static constexpr auto table = std::make_tuple(
        field("startlife", &HealthGameModeSettings::startlife),
        field("maxlife", &HealthGameModeSettings::maxlife),
        field("percentextralife", &HealthGameModeSettings::percentextralife));
};
template<> struct NamedStruct<CollectionGameModeSettings> {
    static constexpr auto table = std::make_tuple(
        field("quantity", &CollectionGameModeSettings::quantity),
        field("rate", &CollectionGameModeSettings::rate),
        field("banktime", &CollectionGameModeSettings::banktime),
        field("cardlife", &CollectionGameModeSettings::cardlife));
};
template<> struct NamedStruct<ChaseGameModeSettings> {
    static constexpr auto table = std::make_tuple(
        field("phantospeed", &ChaseGameModeSettings::phantospeed),
        field("phantoquantity", &ChaseGameModeSettings::phantoquantity));
};
template<> struct NamedStruct<ShyGuyTagGameModeSettings> {
    static constexpr auto table = std::make_tuple(
        field("tagonsuicide", &ShyGuyTagGameModeSettings::tagonsuicide),
        field("tagtransfer", &ShyGuyTagGameModeSettings::tagtransfer),
        field("freetime", &ShyGuyTagGameModeSettings::freetime));
};
template<> struct NamedStruct<BossGameModeSettings> {
    static constexpr auto table = std::make_tuple(
        field("bosstype", &BossGameModeSettings::bosstype),
        field("difficulty", &BossGameModeSettings::difficulty),
        field("hitpoints", &BossGameModeSettings::hitpoints));
};
template<> struct NamedStruct<GameModeSettings> {
    static constexpr auto table = std::make_tuple(
        field("classic", &GameModeSettings::classic),
        field("frag", &GameModeSettings::frag),
        field("time", &GameModeSettings::time),
        field("jail", &GameModeSettings::jail),
        field("coins", &GameModeSettings::coins),
        field("stomp", &GameModeSettings::stomp),
        field("egg", &GameModeSettings::egg),
        field("flag", &GameModeSettings::flag),
        field("chicken", &GameModeSettings::chicken),
        field("tag", &GameModeSettings::tag),
        field("star", &GameModeSettings::star),
        field("domination", &GameModeSettings::domination),
        field("kingofthehill", &GameModeSettings::kingofthehill),
        field("race", &GameModeSettings::race),
        field("frenzy", &GameModeSettings::frenzy),
        field("survival", &GameModeSettings::survival),
        field("greed", &GameModeSettings::greed),
        field("health", &GameModeSettings::health),
        field("collection", &GameModeSettings::collection),
        field("chase", &GameModeSettings::chase),
        field("shyguytag", &GameModeSettings::shyguytag),
        field("boss", &GameModeSettings::boss));
};


template <typename T, typename = void>
struct has_named_enum : std::false_type {};
template <typename E>
struct has_named_enum<E, std::void_t<decltype(NamedEnum<E>::table)>> : std::true_type {};

template <typename T, typename = void>
struct has_named_struct : std::false_type {};
template <typename S>
struct has_named_struct<S, std::void_t<decltype(NamedStruct<S>::table)>> : std::true_type {};

template <typename T, typename = void>
struct is_std_array : std::false_type {};
template <typename T, std::size_t N>
struct is_std_array<std::array<T, N>> : std::true_type {};

template <typename S>
toml::ordered_value to_toml_table(const S& s);

template <typename T>
toml::ordered_value to_toml(const T& value)
{
    if constexpr (std::is_same_v<T, bool>) {
        return toml::ordered_value(value);
    } else if constexpr (std::is_same_v<T, std::string>) {
        return toml::ordered_value(value);
    } else if constexpr (has_named_enum<T>::value) {
        return toml::ordered_value(std::string(as_str(value)));
    } else if constexpr (std::is_arithmetic_v<T>) {
        return toml::ordered_value(value);
    } else if constexpr (is_std_array<T>::value) {
        toml::ordered_array arr;
        for (const auto& element : value)
            arr.push_back(to_toml(element));
        toml::ordered_value result = std::move(arr);
        // arrays of scalars read better on one line
        if constexpr (std::is_arithmetic_v<typename T::value_type>)
            result.as_array_fmt().fmt = toml::array_format::oneline;
        return result;
    } else if constexpr (has_named_struct<T>::value) {
        return to_toml_table(value);
    } else {
        static_assert(!sizeof(T), "Unsupported field type: add a NamedEnum or NamedStruct specialization");
    }
}

template <typename S>
toml::ordered_value to_toml_table(const S& s)
{
    toml::ordered_table table;
    std::apply([&table, &s](const auto&... f) {
        (table.emplace(std::string(f.name), to_toml(s.*(f.member))), ...);
    }, NamedStruct<std::remove_cv_t<S>>::table);
    return toml::ordered_value(std::move(table));
}


template <typename T>
void from_toml(T& value, const toml::value& t);

/// Reads a field named `key` from a toml table if it exists.
template <typename T>
void read_field(T& value, const toml::value& table, std::string_view key)
{
    try {
        const toml::value& item = table.at(std::string(key));
        from_toml(value, item);
    }
    catch (const std::out_of_range&) {
        // missing key: keep the current value
    }
    catch (const std::exception& error) {
        printf("[config][warning] Failed to read field '%s': %s\n", std::string(key).c_str(), error.what());
    }
}

/// Reads the element at `index` from a toml array if it exists.
template <typename T>
void read_field(T& value, const toml::value& array, std::size_t index)
{
    try {
        const toml::value& item = array.at(index);
        from_toml(value, item);
    }
    catch (const std::exception& error) {
        printf("[config][warning] Failed to read element %zu: %s\n", index, error.what());
    }
}

template <typename S>
void from_toml_table(S& s, const toml::value& table)
{
    if (!table.is_table())
        throw std::runtime_error("expected a table");

    std::apply([&s, &table](const auto&... f) {
        (read_field(s.*(f.member), table, f.name), ...);
    }, NamedStruct<std::remove_cv_t<S>>::table);
}

template <typename T>
void from_toml(T& value, const toml::value& t)
{
    if constexpr (std::is_same_v<T, bool>) {
        value = t.as_boolean();
    } else if constexpr (std::is_same_v<T, std::string>) {
        value = t.as_string();
    } else if constexpr (has_named_enum<T>::value) {
        const std::optional<T> parsed = from_str<T>(t.as_string());
        if (!parsed)
            throw std::runtime_error("unknown value '" + t.as_string() + "'");
        value = *parsed;
    } else if constexpr (std::is_arithmetic_v<T>) {
        value = static_cast<T>(t.as_integer());
    } else if constexpr (std::is_array_v<T>) {
        constexpr std::size_t length = std::extent_v<T>;
        const toml::array& arr = t.as_array();
        if (arr.size() != length)
            throw std::runtime_error("array size mismatch: expected " + std::to_string(length) + " elements");
        for (std::size_t i = 0; i < length; i++)
            from_toml(value[i], arr[i]);
    } else if constexpr (is_std_array<T>::value) {
        const toml::array& arr = t.as_array();
        if (arr.size() != value.size())
            throw std::runtime_error("array size mismatch: expected " + std::to_string(value.size()) + " elements");
        for (std::size_t i = 0; i < value.size(); i++)
            from_toml(value[i], arr[i]);
    } else if constexpr (has_named_struct<T>::value) {
        from_toml_table(value, t);
    } else {
        static_assert(!sizeof(T), "Unsupported field type: add a NamedEnum or NamedStruct specialization");
    }
}


/// A TOML table whose entries map directly to fields of the config struct
template <typename Struct, typename... T>
struct Section {
    std::string_view name;
    std::tuple<Field<Struct, T>...> fields;
};

template <typename Struct, typename... T>
constexpr Section<Struct, T...> section(std::string_view name, Field<Struct, T>... fields)
{
    return {name, {fields...}};
}

// A TOML table that mirrors a single, registered sub-struct field in full
template <typename Struct, typename T>
struct SubStructSection {
    std::string_view name;
    T Struct::* member;
};

template <typename Struct, typename T>
constexpr SubStructSection<Struct, T> struct_section(std::string_view name, T Struct::* member)
{
    return {name, member};
}

template <typename Struct> struct NamedLayout;

template<> struct NamedLayout<CGameConfig> {
    static constexpr auto sections = std::make_tuple(
        section("video",
            field("top_layer", &CGameConfig::toplayer),
            field("frame_limiter", &CGameConfig::framelimiter),
            field("fullscreen", &CGameConfig::fullscreen)),
        section("audio",
            field("music_volume", &CGameConfig::musicvolume),
            field("sound_volume", &CGameConfig::soundvolume),
            field("play_next_music", &CGameConfig::playnextmusic),
            field("announcer", &CGameConfig::announcerpack),
            field("sound_pack", &CGameConfig::soundpack),
            field("game_music_pack", &CGameConfig::gamemusicpack),
            field("world_music_pack", &CGameConfig::worldmusicpack)),
        section("gameplay",
            field("respawn_time", &CGameConfig::respawn),
            field("shield_style", &CGameConfig::shieldstyle),
            field("shield_time", &CGameConfig::shieldtime),
            field("out_of_bounds_time", &CGameConfig::outofboundstime),
            field("suicide_time", &CGameConfig::suicidetime),
            field("warp_lock_style", &CGameConfig::warplockstyle),
            field("warp_lock_time", &CGameConfig::warplocktime),
            field("bot_difficulty", &CGameConfig::cpudifficulty),
            field("point_speed", &CGameConfig::pointspeed),
            field("secrets", &CGameConfig::secretsenabled)),
        section("team",
            field("team_collision_style", &CGameConfig::teamcollision),
            field("team_colors", &CGameConfig::teamcolors),
            field("tournament_control_style", &CGameConfig::tournamentcontrolstyle)),
        section("eyecandy",
            field("spawn_style", &CGameConfig::spawnstyle),
            field("award_style", &CGameConfig::awardstyle),
            field("scoreboard_style", &CGameConfig::scoreboardstyle),
            field("screen_crunch", &CGameConfig::screencrunch),
            field("show_leader_crown", &CGameConfig::showwinningcrown),
            field("start_game_countdown", &CGameConfig::startgamecountdown),
            field("show_gamemode", &CGameConfig::startmodedisplay),
            field("dead_team_notice", &CGameConfig::deadteamnotice)),
        section("weapons",
            field("fireball_ttl", &CGameConfig::fireballttl),
            field("feather_jumps", &CGameConfig::featherjumps),
            field("boomerang_style", &CGameConfig::boomerangstyle),
            field("boomerang_life", &CGameConfig::boomeranglife),
            field("shell_ttl", &CGameConfig::shellttl),
            field("wand_freeze_time", &CGameConfig::wandfreezetime),
            field("hammer_ttl", &CGameConfig::hammerttl),
            field("hammer_delay", &CGameConfig::hammerdelay),
            field("hammer_one_kill", &CGameConfig::hammerpower),
            field("blue_block_ttl", &CGameConfig::blueblockttl),
            field("red_block_ttl", &CGameConfig::redblockttl),
            field("gray_block_ttl", &CGameConfig::grayblockttl)),
        section("weapon_limits",
            field("fireball_limit", &CGameConfig::fireballlimit),
            field("hammer_limit", &CGameConfig::hammerlimit),
            field("boomerang_limit", &CGameConfig::boomeranglimit),
            field("feather_limit", &CGameConfig::featherlimit),
            field("leaf_limit", &CGameConfig::leaflimit),
            field("pwings_limit", &CGameConfig::pwingslimit),
            field("tanooki_limit", &CGameConfig::tanookilimit),
            field("bombs_limit", &CGameConfig::bombslimit),
            field("wand_limit", &CGameConfig::wandlimit)),
        section("powerups",
            field("stored_powerup_delay", &CGameConfig::storedpowerupdelay),
            field("item_respawn_time", &CGameConfig::itemrespawntime),
            field("hidden_block_respawn", &CGameConfig::hiddenblockrespawn),
            field("swap_style", &CGameConfig::swapstyle),
            field("bonus_wheel_style", &CGameConfig::bonuswheel),
            field("bonus_keep_powerup", &CGameConfig::keeppowerup)),
        section("powerup_rates",
            field("drop_rate_style", &CGameConfig::overridepowerupsettings),
            field("selected_preset", &CGameConfig::poweruppreset),
            field("presets", &CGameConfig::allPowerupPresets)),
        struct_section("gamemode_settings", &CGameConfig::gamemodemenusettings)
    );
};


template <typename Struct>
toml::ordered_value make_config_document(const Struct& config)
{
    toml::ordered_table root;
    std::apply([&root, &config](const auto&... sec) {
        auto add_section = [&](const auto& sec) {
            if constexpr (requires { sec.fields; }) {
                toml::ordered_table table;
                std::apply([&table, &config](const auto&... f) {
                    (table.emplace(std::string(f.name), to_toml(config.*(f.member))), ...);
                }, sec.fields);
                root.emplace(std::string(sec.name), toml::ordered_value(std::move(table)));
            } else {
                toml::ordered_value value = to_toml_table(config.*(sec.member));
                if (value.is_table())
                    value.as_table_fmt().fmt = toml::table_format::implicit;
                root.emplace(std::string(sec.name), std::move(value));
            }
        };
        (add_section(sec), ...);
    }, NamedLayout<std::remove_cv_t<Struct>>::sections);
    return toml::ordered_value(std::move(root));
}


template <typename Struct>
void apply_config_document(Struct& config, const toml::value& document)
{
    if (!document.is_table())
        return;

    std::apply([&config, &document](const auto&... sec) {
        auto read_section = [&](const auto& sec) {
            if (!document.contains(std::string(sec.name)))
                return;
            const toml::value& table = document.at(std::string(sec.name));

            if constexpr (requires { sec.fields; }) {
                std::apply([&table, &config](const auto&... f) {
                    (read_field(config.*(f.member), table, f.name), ...);
                }, sec.fields);
            } else {
                if (table.is_table())
                    from_toml(config.*(sec.member), table);
            }
        };
        (read_section(sec), ...);
    }, NamedLayout<std::remove_cv_t<Struct>>::sections);
}

} // namespace tomlconfig

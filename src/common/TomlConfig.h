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


struct AudioPackNames {
    std::string announcer;
    std::string sound_pack;
    std::string game_music;
    std::string world_music;

    bool operator==(const AudioPackNames&) const = default;
};

inline toml::ordered_value make_config_document(const CGameConfig& config, const AudioPackNames& pack_names)
{
    std::vector<toml::ordered_value> toml_powerup_presets;
    for (size_t preset = 0; preset < NUM_POWERUP_PRESETS; preset++) {
        toml::ordered_value val = std::vector<short>(config.allPowerupPresets[preset], config.allPowerupPresets[preset] + NUM_POWERUPS);
        val.as_array_fmt().fmt = toml::array_format::oneline;
        toml_powerup_presets.emplace_back(std::move(val));
    }

    toml::ordered_value toml_gamemode_settings = to_toml_table(config.gamemodemenusettings);
    // The table only contains sub-tables, so suppress its own header
    toml_gamemode_settings.as_table_fmt().fmt = toml::table_format::implicit;

    return toml::ordered_value(toml::ordered_table {
        { "video", toml::table {
            { "top_layer", config.toplayer },
            { "frame_limiter", config.framelimiter },
            { "fullscreen", config.fullscreen },
        }},
        { "audio", toml::table {
            { "music_volume", config.musicvolume },
            { "sound_volume", config.soundvolume },
            { "play_next_music", config.playnextmusic },
            { "announcer", pack_names.announcer },
            { "sound_pack", pack_names.sound_pack },
            { "game_music_pack", pack_names.game_music },
            { "world_music_pack", pack_names.world_music },
        }},
        { "gameplay", toml::table {
            { "respawn_time", config.respawn },
            { "shield_style", tomlconfig::as_str(config.shieldstyle) },
            { "shield_time", config.shieldtime },
            { "out_of_bounds_time", config.outofboundstime },
            { "suicide_time", config.suicidetime },
            { "warp_lock_style", tomlconfig::as_str(config.warplockstyle) },
            { "warp_lock_time", config.warplocktime },
            { "bot_difficulty", config.cpudifficulty },
            { "point_speed", config.pointspeed },
            { "secrets", config.secretsenabled },
        }},
        { "team", toml::table {
            { "team_collision_style", tomlconfig::as_str(config.teamcollision) },
            { "team_colors", config.teamcolors },
            { "tournament_control_style", tomlconfig::as_str(config.tournamentcontrolstyle) },
        }},
        { "eyecandy", toml::table {
            { "spawn_style", tomlconfig::as_str(config.spawnstyle) },
            { "award_style", tomlconfig::as_str(config.awardstyle) },
            { "scoreboard_style", tomlconfig::as_str(config.scoreboardstyle) },
            { "screen_crunch", config.screencrunch },
            { "show_leader_crown", config.showwinningcrown },
            { "start_game_countdown", config.startgamecountdown },
            { "show_gamemode", config.startmodedisplay },
            { "dead_team_notice", config.deadteamnotice },
        }},
        { "weapons", toml::table {
            { "fireball_ttl", config.fireballttl },
            { "feather_jumps", config.featherjumps },
            { "boomerang_style", tomlconfig::as_str(config.boomerangstyle) },
            { "boomerang_life", config.boomeranglife },
            { "shell_ttl", config.shellttl },
            { "wand_freeze_time", config.wandfreezetime },
            { "hammer_ttl", config.hammerttl },
            { "hammer_delay", config.hammerdelay },
            { "hammer_one_kill", config.hammerpower },
            { "blue_block_ttl", config.blueblockttl },
            { "red_block_ttl", config.redblockttl },
            { "gray_block_ttl", config.grayblockttl },
        }},
        { "weapon_limits", toml::table {
            { "fireball_limit", config.fireballlimit },
            { "hammer_limit", config.hammerlimit },
            { "boomerang_limit", config.boomeranglimit },
            { "feather_limit", config.featherlimit },
            { "leaf_limit", config.leaflimit },
            { "pwings_limit", config.pwingslimit },
            { "tanooki_limit", config.tanookilimit },
            { "bombs_limit", config.bombslimit },
            { "wand_limit", config.wandlimit },
        }},
        { "powerups", toml::table {
            { "stored_powerup_delay", config.storedpowerupdelay },
            { "item_respawn_time", config.itemrespawntime },
            { "hidden_block_respawn", config.hiddenblockrespawn },
            { "swap_style", tomlconfig::as_str(config.swapstyle) },
            { "bonus_wheel_style", tomlconfig::as_str(config.bonuswheel) },
            { "bonus_keep_powerup", config.keeppowerup },
        }},
        { "powerup_rates", toml::ordered_table {
            { "drop_rate_style", tomlconfig::as_str(config.overridepowerupsettings) },
            { "selected_preset", config.poweruppreset },
            { "presets", toml_powerup_presets },
        }},
        { "gamemode_settings", toml_gamemode_settings },
    });
}

inline void apply_config_document(CGameConfig& config, const toml::value& document, AudioPackNames& pack_names)
{
    if (document.contains("video")) {
        const toml::value& video = document.at("video");
        read_field(config.toplayer, video, "top_layer");
        read_field(config.framelimiter, video, "frame_limiter");
        read_field(config.fullscreen, video, "fullscreen");
    }

    if (document.contains("audio")) {
        const toml::value& audio = document.at("audio");
        read_field(config.musicvolume, audio, "music_volume");
        read_field(config.soundvolume, audio, "sound_volume");
        read_field(config.playnextmusic, audio, "play_next_music");

        read_field(pack_names.announcer, audio, "announcer");
        read_field(pack_names.sound_pack, audio, "sound_pack");
        read_field(pack_names.game_music, audio, "game_music_pack");
        read_field(pack_names.world_music, audio, "world_music_pack");
    }

    if (document.contains("gameplay")) {
        const toml::value& gameplay = document.at("gameplay");
        read_field(config.respawn, gameplay, "respawn_time");
        read_field(config.shieldstyle, gameplay, "shield_style");
        read_field(config.shieldtime, gameplay, "shield_time");
        read_field(config.outofboundstime, gameplay, "out_of_bounds_time");
        read_field(config.suicidetime, gameplay, "suicide_time");
        read_field(config.warplockstyle, gameplay, "warp_lock_style");
        read_field(config.warplocktime, gameplay, "warp_lock_time");
        read_field(config.cpudifficulty, gameplay, "bot_difficulty");
        read_field(config.pointspeed, gameplay, "point_speed");
        read_field(config.secretsenabled, gameplay, "secrets");
    }

    if (document.contains("team")) {
        const toml::value& team = document.at("team");
        read_field(config.teamcollision, team, "team_collision_style");
        read_field(config.teamcolors, team, "team_colors");
        read_field(config.tournamentcontrolstyle, team, "tournament_control_style");
    }

    if (document.contains("eyecandy")) {
        const toml::value& eyecandy = document.at("eyecandy");
        read_field(config.spawnstyle, eyecandy, "spawn_style");
        read_field(config.awardstyle, eyecandy, "award_style");
        read_field(config.scoreboardstyle, eyecandy, "scoreboard_style");
        read_field(config.screencrunch, eyecandy, "screen_crunch");
        read_field(config.showwinningcrown, eyecandy, "show_leader_crown");
        read_field(config.startgamecountdown, eyecandy, "start_game_countdown");
        read_field(config.startmodedisplay, eyecandy, "show_gamemode");
        read_field(config.deadteamnotice, eyecandy, "dead_team_notice");
    }

    if (document.contains("weapons")) {
        const toml::value& weapons = document.at("weapons");
        read_field(config.fireballttl, weapons, "fireball_ttl");
        read_field(config.featherjumps, weapons, "feather_jumps");
        read_field(config.boomerangstyle, weapons, "boomerang_style");
        read_field(config.boomeranglife, weapons, "boomerang_life");
        read_field(config.shellttl, weapons, "shell_ttl");
        read_field(config.wandfreezetime, weapons, "wand_freeze_time");
        read_field(config.hammerttl, weapons, "hammer_ttl");
        read_field(config.hammerdelay, weapons, "hammer_delay");
        read_field(config.hammerpower, weapons, "hammer_one_kill");
        read_field(config.blueblockttl, weapons, "blue_block_ttl");
        read_field(config.redblockttl, weapons, "red_block_ttl");
        read_field(config.grayblockttl, weapons, "gray_block_ttl");
    }

    if (document.contains("weapon_limits")) {
        const toml::value& weapon_limits = document.at("weapon_limits");
        read_field(config.fireballlimit, weapon_limits, "fireball_limit");
        read_field(config.hammerlimit, weapon_limits, "hammer_limit");
        read_field(config.boomeranglimit, weapon_limits, "boomerang_limit");
        read_field(config.featherlimit, weapon_limits, "feather_limit");
        read_field(config.leaflimit, weapon_limits, "leaf_limit");
        read_field(config.pwingslimit, weapon_limits, "pwings_limit");
        read_field(config.tanookilimit, weapon_limits, "tanooki_limit");
        read_field(config.bombslimit, weapon_limits, "bombs_limit");
        read_field(config.wandlimit, weapon_limits, "wand_limit");
    }

    if (document.contains("powerups")) {
        const toml::value& powerups = document.at("powerups");
        read_field(config.storedpowerupdelay, powerups, "stored_powerup_delay");
        read_field(config.itemrespawntime, powerups, "item_respawn_time");
        read_field(config.hiddenblockrespawn, powerups, "hidden_block_respawn");
        read_field(config.swapstyle, powerups, "swap_style");
        read_field(config.bonuswheel, powerups, "bonus_wheel_style");
        read_field(config.keeppowerup, powerups, "bonus_keep_powerup");
    }

    if (document.contains("powerup_rates")) {
        const toml::value& powerup_rates = document.at("powerup_rates");
        read_field(config.overridepowerupsettings, powerup_rates, "drop_rate_style");
        read_field(config.poweruppreset, powerup_rates, "selected_preset");

        if (powerup_rates.contains("presets") && powerup_rates.at("presets").is_array()) {
            const toml::array& presets = powerup_rates.at("presets").as_array();
            for (size_t preset = 0; preset < NUM_POWERUP_PRESETS && preset < presets.size(); preset++)
                read_field(config.allPowerupPresets[preset], powerup_rates.at("presets"), preset);
        }
    }

    read_field(config.gamemodemenusettings, document, "gamemode_settings");
}

} // namespace tomlconfig

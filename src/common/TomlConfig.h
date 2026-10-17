#pragma once

#include "EyecandyStyles.h"
#include "GameModeSettings.h"
#include "GameplayStyles.h"

#include <toml.hpp>

#include <array>
#include <optional>
#include <string_view>
#include <tuple>

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

} // namespace tomlconfig

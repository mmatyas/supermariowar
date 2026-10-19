#pragma once

#include "EyecandyStyles.h"
#include "GameModeSettings.h"
#include "GameValues.h"
#include "GameplayStyles.h"
#include "MatchTypes.h"

#include <array>
#include <optional>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>

// This file acts as the single "source of truth" for string <-> value conversions
// in the config file. Enjoy the template magic.


namespace config {

// Enums

template <typename E> struct NamedEnum;

template<> struct NamedEnum<Boss> {
    static constexpr auto table = std::to_array<std::pair<Boss, std::string_view>>({
        { Boss::Hammer, "hammer" },
        { Boss::Bomb, "bomb" },
        { Boss::Fire, "fire" },
    });
};
template<> struct NamedEnum<PlayerSlot> {
    static constexpr auto table = std::to_array<std::pair<PlayerSlot, std::string_view>>({
        { PlayerSlot::Empty, "empty" },
        { PlayerSlot::Human, "human" },
        { PlayerSlot::Cpu, "cpu" },
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


// Structs

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
        field("tag_free", &JailGameModeSettings::tagfree),
        field("time_to_free", &JailGameModeSettings::timetofree),
        field("percent_key", &JailGameModeSettings::percentkey));
};
template<> struct NamedStruct<CoinGameModeSettings> {
    static constexpr auto table = std::make_tuple(
        field("penalty", &CoinGameModeSettings::penalty),
        field("quantity", &CoinGameModeSettings::quantity),
        field("percent_extra_coin", &CoinGameModeSettings::percentextracoin));
};
template<> struct NamedStruct<StompGameModeSettings> {
    static constexpr auto table = std::make_tuple(
        field("rate", &StompGameModeSettings::rate),
        field("enemy_weight", &StompGameModeSettings::enemyweight));
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
        field("touch_return", &FlagGameModeSettings::touchreturn),
        field("point_move", &FlagGameModeSettings::pointmove),
        field("auto_return", &FlagGameModeSettings::autoreturn),
        field("home_score", &FlagGameModeSettings::homescore),
        field("center_flag", &FlagGameModeSettings::centerflag));
};
template<> struct NamedStruct<ChickenGameModeSettings> {
    static constexpr auto table = std::make_tuple(
        field("use_target", &ChickenGameModeSettings::usetarget),
        field("glide", &ChickenGameModeSettings::glide));
};
template<> struct NamedStruct<TagGameModeSettings> {
    static constexpr auto table = std::make_tuple(
        field("tag_on_touch", &TagGameModeSettings::tagontouch));
};
template<> struct NamedStruct<StarGameModeSettings> {
    static constexpr auto table = std::make_tuple(
        field("time", &StarGameModeSettings::time),
        field("shine", &StarGameModeSettings::shine),
        field("percent_extra_time", &StarGameModeSettings::percentextratime));
};
template<> struct NamedStruct<DominationGameModeSettings> {
    static constexpr auto table = std::make_tuple(
        field("lose_on_death", &DominationGameModeSettings::loseondeath),
        field("steal_on_death", &DominationGameModeSettings::stealondeath),
        field("relocate_on_death", &DominationGameModeSettings::relocateondeath),
        field("relocation_frequency", &DominationGameModeSettings::relocationfrequency),
        field("quantity", &DominationGameModeSettings::quantity));
};
template<> struct NamedStruct<KingOfTheHillModeSettings> {
    static constexpr auto table = std::make_tuple(
        field("area_size", &KingOfTheHillModeSettings::areasize),
        field("relocation_frequency", &KingOfTheHillModeSettings::relocationfrequency),
        field("max_multiplier", &KingOfTheHillModeSettings::maxmultiplier));
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
        field("stored_shells", &FrenzyGameModeSettings::storedshells),
        field("powerup_weight", &FrenzyGameModeSettings::powerupweight));
};
template<> struct NamedStruct<SurvivalGameModeSettings> {
    static constexpr auto table = std::make_tuple(
        field("enemy_weight", &SurvivalGameModeSettings::enemyweight),
        field("density", &SurvivalGameModeSettings::density),
        field("speed", &SurvivalGameModeSettings::speed),
        field("shield", &SurvivalGameModeSettings::shield));
};
template<> struct NamedStruct<GreedGameModeSettings> {
    static constexpr auto table = std::make_tuple(
        field("coinlife", &GreedGameModeSettings::coinlife),
        field("owncoins", &GreedGameModeSettings::owncoins),
        field("multiplier", &GreedGameModeSettings::multiplier),
        field("percent_extra_coin", &GreedGameModeSettings::percentextracoin));
};
template<> struct NamedStruct<HealthGameModeSettings> {
    static constexpr auto table = std::make_tuple(
        field("start_life", &HealthGameModeSettings::startlife),
        field("max_life", &HealthGameModeSettings::maxlife),
        field("percent_extra_life", &HealthGameModeSettings::percentextralife));
};
template<> struct NamedStruct<CollectionGameModeSettings> {
    static constexpr auto table = std::make_tuple(
        field("quantity", &CollectionGameModeSettings::quantity),
        field("rate", &CollectionGameModeSettings::rate),
        field("bank_time", &CollectionGameModeSettings::banktime),
        field("card_life", &CollectionGameModeSettings::cardlife));
};
template<> struct NamedStruct<ChaseGameModeSettings> {
    static constexpr auto table = std::make_tuple(
        field("phanto_speed", &ChaseGameModeSettings::phantospeed),
        field("phanto_quantity", &ChaseGameModeSettings::phantoquantity));
};
template<> struct NamedStruct<ShyGuyTagGameModeSettings> {
    static constexpr auto table = std::make_tuple(
        field("tag_on_suicide", &ShyGuyTagGameModeSettings::tagonsuicide),
        field("tag_transfer", &ShyGuyTagGameModeSettings::tagtransfer),
        field("free_time", &ShyGuyTagGameModeSettings::freetime));
};
template<> struct NamedStruct<BossGameModeSettings> {
    static constexpr auto table = std::make_tuple(
        field("boss_type", &BossGameModeSettings::bosstype),
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
template<> struct NamedStruct<GameModeGoals> {
    static constexpr auto table = std::make_tuple(
        field("classic", &GameModeGoals::classic),
        field("frag", &GameModeGoals::frag),
        field("time", &GameModeGoals::time),
        field("jail", &GameModeGoals::jail),
        field("coins", &GameModeGoals::coins),
        field("stomp", &GameModeGoals::stomp),
        field("egg", &GameModeGoals::egg),
        field("flag", &GameModeGoals::flag),
        field("chicken", &GameModeGoals::chicken),
        field("tag", &GameModeGoals::tag),
        field("star", &GameModeGoals::star),
        field("domination", &GameModeGoals::domination),
        field("kingofthehill", &GameModeGoals::kingofthehill),
        field("race", &GameModeGoals::race),
        field("owned", &GameModeGoals::owned),
        field("frenzy", &GameModeGoals::frenzy),
        field("survival", &GameModeGoals::survival),
        field("greed", &GameModeGoals::greed),
        field("health", &GameModeGoals::health),
        field("collection", &GameModeGoals::collection),
        field("chase", &GameModeGoals::chase),
        field("shyguytag", &GameModeGoals::shyguytag),
        field("boss", &GameModeGoals::boss));
};


// Powerup droprate presets

constexpr std::array<std::string_view, NUM_POWERUPS> POWERUP_DROPRATE_KEYS_ORDERED = {
    "poison",
    "1up",
    "2up",
    "3up",
    "5up",
    "flower",
    "star",
    "clock",
    "bobomb",
    "pow",
    "bbill",
    "hammer",
    "gshell",
    "rshell",
    "spiky",
    "bshell",
    "mod",
    "feather",
    "mystery",
    "boomerang",
    "tanooki",
    "icewand",
    "podobo",
    "bomb",
    "leaf",
    "pwing",
};

template <typename T>
inline constexpr bool is_powerup_rate_presets = std::is_same_v<
    T, std::array<std::array<short, NUM_POWERUPS>, NUM_POWERUP_PRESETS>>;


// The game config itself

/// Fields that do not live in CGameConfig
struct Bridge {
    std::string announcerpack;
    std::string soundpack;
    std::string gamemusicpack;
    std::string worldmusicpack;
    std::string menugraphicspack;
    std::string worldgraphicspack;
    std::string gamegraphicspack;
    std::array<std::string, MAX_PLAYERS> skins;
    std::array<PlayerSlot, MAX_PLAYERS> player_slots;
    GameModeGoals gamemodegoals;
};

/// A group whose entries map directly to fields of the config struct;
/// the fields may belong to the config or to the Bridge (see field_value)
template <typename... F>
struct Section {
    std::string_view name;
    std::tuple<F...> fields;
};
template <typename... F>
constexpr Section<F...> section(std::string_view name, F... fields) {
    return {name, {fields...}};
}

// A TOML table that mirrors a single, registered sub-struct field in full
template <typename Struct, typename T>
struct NestedStructSection {
    std::string_view name;
    T Struct::* member;
};

template <typename Struct, typename T>
constexpr NestedStructSection<Struct, T> struct_section(std::string_view name, T Struct::* member) {
    return {name, member};
}

template <typename Struct> struct NamedLayout;

template<> struct NamedLayout<CGameConfig> {
    static constexpr auto sections = std::make_tuple(
        section("video",
            field("top_layer", &CGameConfig::toplayer),
            field("frame_limiter", &CGameConfig::framelimiter),
            field("fullscreen", &CGameConfig::fullscreen),
            field("menu_pack", &Bridge::menugraphicspack),
            field("world_pack", &Bridge::worldgraphicspack),
            field("game_pack", &Bridge::gamegraphicspack)),
        section("audio",
            field("music_volume", &CGameConfig::musicvolume),
            field("sound_volume", &CGameConfig::soundvolume),
            field("play_next_music", &CGameConfig::playnextmusic),
            field("announcer", &Bridge::announcerpack),
            field("sound_pack", &Bridge::soundpack),
            field("game_music_pack", &Bridge::gamemusicpack),
            field("world_music_pack", &Bridge::worldmusicpack)),
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
        section("players",
            field("skins", &Bridge::skins),
            field("player_slots", &Bridge::player_slots)),
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
        struct_section("gamemode_settings", &CGameConfig::gamemodemenusettings),
        struct_section("gamemode_goals", &Bridge::gamemodegoals)
    );
};


template <typename Ptr> struct member_ptr_owner;
template <typename T, typename Owner> struct member_ptr_owner<T Owner::*> {
    using type = Owner;
};

/// Resolves a field's pointer-to-member against the object that owns it,
/// which is either the config or the Bridge (see Bridge).
template <typename Ptr, typename Config, typename Bridge>
const auto& field_value(Ptr member, const Config& config, const Bridge& bridge)
{
    if constexpr (std::is_same_v<typename member_ptr_owner<Ptr>::type, Config>) {
        return config.*member;
    } else {
        return bridge.*member;
    }
}
template <typename Ptr, typename Config, typename Bridge>
auto& field_value(Ptr member, Config& config, Bridge& bridge)
{
    if constexpr (std::is_same_v<typename member_ptr_owner<Ptr>::type, Config>) {
        return config.*member;
    } else {
        return bridge.*member;
    }
}


// Some marker types

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

} // namespace config

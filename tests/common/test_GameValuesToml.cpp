#include "doctest.h"

#include "TomlConfig.h"

#include <string>

SDL_Surface* screen = nullptr;
SDL_Surface* blitdest = nullptr;
short x_shake = 0;
short y_shake = 0;


namespace {

// Fill all serializable CGameConfig fields with distinctive non-default values so
// that a dropped and a coincidentally-matching field do not cancel each other out.
CGameConfig makePeculiarConfig()
{
    CGameConfig config {}; // zero initialize

    // video
    config.toplayer = true;
    config.framelimiter = 13;
    config.fullscreen = true;
    config.menugraphicspack = "Classical Menu";
    config.worldgraphicspack = "Unbiased World";
    config.gamegraphicspack = "Retro Game";

    // audio
    config.musicvolume = 11;
    config.soundvolume = 11;
    config.playnextmusic = true;
    config.announcerpack = "dude_duds";
    config.soundpack = "retro";
    config.gamemusicpack = "smb3 packs";
    config.worldmusicpack = "Tournament 2026";

    // gameplay
    config.respawn = 11;
    config.shieldstyle = ShieldStyle::Hard;
    config.shieldtime = 11;
    config.outofboundstime = 11;
    config.suicidetime = 11;
    config.warplockstyle = WarpLockStyle::AllWarps;
    config.warplocktime = 11;
    config.cpudifficulty = 11;
    config.pointspeed = 11;
    config.secretsenabled = true;

    // team
    config.teamcollision = TeamCollisionStyle::Assist;
    config.teamcolors = true;
    config.tournamentcontrolstyle = TournamentControlStyle::RoundRobin;

    // eyecandy
    config.spawnstyle = SpawnStyle::Swirl;
    config.awardstyle = AwardStyle::Fireworks;
    config.scoreboardstyle = ScoreboardStyle::Corners;
    config.screencrunch = true;
    config.showwinningcrown = true;
    config.startgamecountdown = true;
    config.startmodedisplay = true;
    config.deadteamnotice = true;

    // weapons
    config.fireballttl = 11;
    config.featherjumps = 11;
    config.boomerangstyle = BoomerangStyle::Zelda;
    config.boomeranglife = 11;
    config.shellttl = 11;
    config.wandfreezetime = 11;
    config.hammerttl = 11;
    config.hammerdelay = 11;
    config.hammerpower = true;
    config.blueblockttl = 11;
    config.redblockttl = 11;
    config.grayblockttl = 11;

    // weapon_limits
    config.fireballlimit = 11;
    config.hammerlimit = 11;
    config.boomeranglimit = 11;
    config.featherlimit = 11;
    config.leaflimit = 11;
    config.pwingslimit = 11;
    config.tanookilimit = 11;
    config.bombslimit = 11;
    config.wandlimit = 11;

    // powerups
    config.storedpowerupdelay = 11;
    config.itemrespawntime = 11;
    config.hiddenblockrespawn = 11;
    config.swapstyle = MysteryMushroomSwapStyle::Blink;
    config.bonuswheel = BonusWheelStyle::EveryGame;
    config.keeppowerup = true;

    // powerup_rates
    config.overridepowerupsettings = PowerupDropRateStyle::WeightedAverage;
    config.poweruppreset = 2;
    for (size_t preset = 0; preset < NUM_POWERUP_PRESETS; preset++) {
        for (size_t powerup = 0; powerup < NUM_POWERUPS; powerup++)
            config.allPowerupPresets[preset][powerup] = static_cast<short>(preset * NUM_POWERUPS + powerup);
    }

    // gamemode_settings
    config.gamemodemenusettings.classic.style = DeathStyle::Shield;
    config.gamemodemenusettings.classic.scoring = ScoringStyle::PushOnly;
    config.gamemodemenusettings.jail.style = JailStyle::Owned;
    config.gamemodemenusettings.jail.timetofree = 11;
    config.gamemodemenusettings.stomp.enemyweight[0] = 9;
    config.gamemodemenusettings.egg.explode = 11;
    config.gamemodemenusettings.flag.speed = 11;
    config.gamemodemenusettings.chicken.glide = true;
    config.gamemodemenusettings.star.shine = StarStyle::Shine;
    config.gamemodemenusettings.boss.bosstype = Boss::Bomb;

    return config;
}

toml::ordered_value make_document(const CGameConfig& config)
{
    return tomlconfig::make_config_document(config);
}

std::string document_round_trip(const CGameConfig& config)
{
    const std::string text = toml::format(make_document(config));
    const toml::value parsed = toml::parse_str(text);

    CGameConfig restored {};
    tomlconfig::apply_config_document(restored, parsed);

    return toml::format(tomlconfig::make_config_document(restored));
}

} // namespace


TEST_CASE("TOML config document round trip with non-default values")
{
    const CGameConfig config = makePeculiarConfig();
    CHECK(document_round_trip(config) == toml::format(make_document(config)));
}

TEST_CASE("TOML config document round trip with defaulted values")
{
    const CGameConfig config {};
    CHECK(document_round_trip(config) == toml::format(make_document(config)));
}

TEST_CASE("TOML config document covers every descriptor field exactly once")
{
    // sections with fields list them one by one; sub-struct sections render a
    // whole registered struct
    std::array<size_t, 3> counts {}; // field sections, fields, sub-struct sections
    std::apply([&counts](const auto&... sec) {
        auto count_section = [&](const auto& sec) {
            if constexpr (requires { sec.fields;}) {
                counts[0]++;
                std::apply([&counts](const auto&... f) {
                    counts[1] += sizeof...(f);
                }, sec.fields);
            } else {
                counts[2]++;
            }
        };
        (count_section(sec), ...);
    }, tomlconfig::NamedLayout<CGameConfig>::sections);

    CHECK(counts[0] == 9);
    CHECK(counts[1] == 64);
    CHECK(counts[2] == 1);
}

TEST_CASE("TOML config document values map to the documented TOML type")
{
    const std::string text = toml::format(make_document(makePeculiarConfig()));
    const toml::value document = toml::parse_str(text);

    // enums are stored by their canonical string names
    const toml::value& gameplay = document.at("gameplay");
    CHECK(gameplay.at("shield_style").as_string() == "hard");
    CHECK(document.at("weapons").at("boomerang_style").as_string() == "zelda");
    CHECK(document.at("gamemode_settings").at("boss").at("bosstype").as_string() == "bomb");

    // booleans stay booleans while other fields stay integers
    CHECK(document.at("powerups").at("bonus_keep_powerup").is_boolean());
    CHECK(document.at("powerup_rates").at("presets").is_array());
}

TEST_CASE("Applying a broken TOML document keeps the existing values")
{
    const toml::value document = toml::parse_str(R"(
        weapons = { fireball_ttl = "fff" }

        [gameplay]
        respon_time = 99
        secrets = true

        [video]
        full_screen = "yes"

        [gamemode_settings.classic]
        death_style = "made_up"
        scoring_style = "push_only"
    )");

    const CGameConfig config {};
    CGameConfig restored {};
    tomlconfig::apply_config_document(restored, document);

    // wrong value type: kept as-is
    CHECK(restored.fireballttl == config.fireballttl);
    // wrong key name: kept as-is
    CHECK(restored.fullscreen == config.fullscreen);
    CHECK(restored.respawn == config.respawn);
    // valid keys of valid sections are still applied
    CHECK(restored.secretsenabled == true);
    CHECK(restored.gamemodemenusettings.classic.scoring == ScoringStyle::PushOnly);
    // unknown enum values fall back to the current value
    CHECK(restored.gamemodemenusettings.classic.style == config.gamemodemenusettings.classic.style);
}

TEST_CASE("Applying a TOML document without chapters keeps the existing values")
{
    const toml::value document = toml::parse_str(R"(
        unrelated_field = 5
    )");

    CGameConfig restored {};
    tomlconfig::apply_config_document(restored, document);

    CHECK(restored.fireballttl == CGameConfig {}.fireballttl);
    CHECK(restored.gamemodemenusettings.chicken.usetarget == CGameConfig {}.gamemodemenusettings.chicken.usetarget);
}

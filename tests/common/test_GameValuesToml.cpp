#include "doctest.h"

#include "TomlConfig.h"

#include <string>

SDL_Surface* screen = nullptr;
SDL_Surface* blitdest = nullptr;
short x_shake = 0;
short y_shake = 0;


namespace {

// Fill all serializable CGameConfig and Bridge fields with distinctive non-default
// values so that a dropped and a coincidentally-matching field do not cancel each
// other out.
std::pair<CGameConfig, tomlconfig::Bridge> makePeculiar()
{
    std::pair<CGameConfig, tomlconfig::Bridge> result {CGameConfig {}, tomlconfig::Bridge {}};
    CGameConfig& config = result.first;
    tomlconfig::Bridge& bridge = result.second;

    // video
    config.toplayer = true;
    config.framelimiter = 13;
    config.fullscreen = true;
    bridge.menugraphicspack = "Classical Menu";
    bridge.worldgraphicspack = "Unbiased World";
    bridge.gamegraphicspack = "Retro Game";

    // audio
    config.musicvolume = 11;
    config.soundvolume = 11;
    config.playnextmusic = true;
    bridge.announcerpack = "dude_duds";
    bridge.soundpack = "retro";
    bridge.gamemusicpack = "smb3 packs";
    bridge.worldmusicpack = "Tournament 2026";

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

    // players
    bridge.skins[0] = "toad";
    bridge.skins[1] = ""; // random
    bridge.skins[2] = "fire";
    bridge.skins[3] = "kid";
    bridge.player_slots[0] = PlayerType::Human;
    bridge.player_slots[1] = PlayerType::CPU;
    bridge.player_slots[2] = PlayerType::CPU;
    bridge.player_slots[3] = PlayerType::Empty;

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

    // gamemode_goals
    bridge.gamemodegoals.classic = 11;
    bridge.gamemodegoals.frag = 12;
    bridge.gamemodegoals.time = 13;
    bridge.gamemodegoals.jail = 14;
    bridge.gamemodegoals.coins = 15;
    bridge.gamemodegoals.stomp = 16;
    bridge.gamemodegoals.egg = 17;
    bridge.gamemodegoals.flag = 18;
    bridge.gamemodegoals.chicken = 19;
    bridge.gamemodegoals.tag = 20;
    bridge.gamemodegoals.star = 21;
    bridge.gamemodegoals.domination = 22;
    bridge.gamemodegoals.kingofthehill = 23;
    bridge.gamemodegoals.race = 24;
    bridge.gamemodegoals.owned = 25;
    bridge.gamemodegoals.frenzy = 26;
    bridge.gamemodegoals.survival = 27;
    bridge.gamemodegoals.greed = 28;
    bridge.gamemodegoals.health = 29;
    bridge.gamemodegoals.collection = 30;
    bridge.gamemodegoals.chase = 31;
    bridge.gamemodegoals.shyguytag = 32;
    bridge.gamemodegoals.boss = 33;

    return result;
}

toml::ordered_value make_document(const CGameConfig& config, const tomlconfig::Bridge& bridge)
{
    return tomlconfig::make_config_document(config, bridge);
}

std::string document_round_trip(const CGameConfig& config, const tomlconfig::Bridge& bridge)
{
    const std::string text = toml::format(make_document(config, bridge));
    const toml::value parsed = toml::parse_str(text);

    CGameConfig restored {};
    tomlconfig::Bridge restored_bridge {};
    tomlconfig::apply_config_document(restored, restored_bridge, parsed);

    return toml::format(tomlconfig::make_config_document(restored, restored_bridge));
}

} // namespace


TEST_CASE("TOML config document round trip with non-default values")
{
    const auto [config, bridge] = makePeculiar();
    CHECK(document_round_trip(config, bridge) == toml::format(make_document(config, bridge)));
}

TEST_CASE("TOML config document round trip with defaulted values")
{
    const CGameConfig config {};
    const tomlconfig::Bridge bridge {};
    CHECK(document_round_trip(config, bridge) == toml::format(make_document(config, bridge)));
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

    CHECK(counts[0] == 10);
    CHECK(counts[1] == 66);
    CHECK(counts[2] == 2); // gamemode_settings, gamemode_goals
}

TEST_CASE("TOML config document values map to the documented TOML type")
{
    const auto [config, bridge] = makePeculiar();
    const auto document = make_document(config, bridge);

    // enums are stored by their canonical string names
    const toml::value& gameplay = document.at("gameplay");
    CHECK(gameplay.at("shield_style").as_string() == "hard");
    CHECK(document.at("weapons").at("boomerang_style").as_string() == "zelda");
    CHECK(document.at("gamemode_settings").at("boss").at("bosstype").as_string() == "bomb");

    // booleans stay booleans while other fields stay integers
    CHECK(document.at("powerups").at("bonus_keep_powerup").is_boolean());
    CHECK(document.at("powerup_rates").at("presets").is_array());

    // player skins are stored as names, empty string selects a random skin
    CHECK(document.at("players").at("skins").is_array());
    CHECK(document.at("players").at("skins").as_array().at(1).as_string().empty());

    // player types are stored as strings
    CHECK(document.at("players").at("player_slots").as_array().at(2).as_string() == "cpu");

    // the per-mode goals live in their own table
    CHECK(document.at("gamemode_goals").at("classic").as_integer() == 11);
    CHECK(document.at("gamemode_goals").at("flag").as_integer() == 18);
    CHECK(document.at("gamemode_goals").at("race").as_integer() == 24);
    CHECK(document.at("gamemode_goals").at("frenzy").as_integer() == 26);
    CHECK(document.at("gamemode_goals").at("owned").as_integer() == 25);
    CHECK(document.at("gamemode_goals").at("boss").as_integer() == 33);
}

TEST_CASE("Applying a TOML document fills the per-mode goals")
{
    const toml::value document = toml::parse_str(R"(
        [gamemode_goals]
        classic = 7
        chase = 3
    )");

    CGameConfig restored {};
    tomlconfig::Bridge restored_bridge {};
    tomlconfig::apply_config_document(restored, restored_bridge, document);

    // goals from the table fill their members,
    // the remaining goals keep their defaults
    CHECK(restored_bridge.gamemodegoals.classic == 7);
    CHECK(restored_bridge.gamemodegoals.chase == 3);
    CHECK(restored_bridge.gamemodegoals.frag == GameModeGoals {}.frag);
    CHECK(restored_bridge.gamemodegoals.boss == GameModeGoals {}.boss);
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

        [players]
        skins = ["toad", "peach"]
        player_slots = ["human", "nope"]

        [gamemode_settings.classic]
        death_style = "made_up"
        scoring_style = "push_only"
        [gamemode_goals]
        classic = "nope"
    )");

    const CGameConfig config {};
    CGameConfig restored {};
    tomlconfig::Bridge restored_bridge {};
    tomlconfig::apply_config_document(restored, restored_bridge, document);

    // wrong value type: kept as-is
    CHECK(restored.fireballttl == config.fireballttl);
    // wrong key name: kept as-is
    CHECK(restored.fullscreen == config.fullscreen);
    CHECK(restored.respawn == config.respawn);
    // wrong array length: kept as-is
    CHECK(restored_bridge.skins == tomlconfig::Bridge {}.skins);
    // wrong value inside the array: kept as-is
    CHECK(restored_bridge.player_slots == tomlconfig::Bridge {}.player_slots);
    // valid keys of valid sections are still applied
    CHECK(restored.secretsenabled == true);
    CHECK(restored.gamemodemenusettings.classic.scoring == ScoringStyle::PushOnly);
    // unknown enum values fall back to the current value
    CHECK(restored.gamemodemenusettings.classic.style == config.gamemodemenusettings.classic.style);
    // wrong goal value type: kept as-is
    CHECK(restored_bridge.gamemodegoals.classic == GameModeGoals {}.classic);
}

TEST_CASE("Applying a TOML document without chapters keeps the existing values")
{
    const toml::value document = toml::parse_str(R"(
        unrelated_field = 5
    )");

    CGameConfig restored {};
    tomlconfig::Bridge restored_bridge {};
    tomlconfig::apply_config_document(restored, restored_bridge, document);

    CHECK(restored.fireballttl == CGameConfig {}.fireballttl);
    CHECK(restored.gamemodemenusettings.chicken.usetarget == CGameConfig {}.gamemodemenusettings.chicken.usetarget);
}

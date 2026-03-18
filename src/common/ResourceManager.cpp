#include "ResourceManager.h"

#include "FileList.h"
#include "Game.h"
#include "GameValues.h"
#include "TilesetManager.h"
#include "path.h"
#include "gfx/Color.h"

#include <filesystem>

namespace fs = std::filesystem;

extern SkinList *skinlist;
extern GraphicsList *menugraphicspacklist;
extern GraphicsList *worldgraphicspacklist;
extern GraphicsList *gamegraphicspacklist;
extern SoundsList *soundpacklist;

extern CTilesetManager  *g_tilesetmanager;

extern CGameValues game_values;


bool CResourceManager::LoadMenuSkin(short playerID, short skinID, short colorID, bool fLoadBothDirections)
{
    return gfx_loadmenuskin(spr_player[playerID], skinlist->at(skinID).path.string(), colors::MAGENTA, colorID, fLoadBothDirections);
}

bool CResourceManager::LoadMenuSkin(short playerID, const std::string& filename, short colorID, bool fLoadBothDirections)
{
    return gfx_loadmenuskin(spr_player[playerID], filename, colors::MAGENTA, colorID, fLoadBothDirections);
}

bool CResourceManager::LoadFullSkin(SpriteStrip& sprites, const std::string& filename, short colorID)
{
    return gfx_loadfullskin(sprites, filename, colors::MAGENTA, colorID);
}

bool CResourceManager::LoadFullSkin(SpriteStrip& sprites, short skinID, short colorID)
{
    return LoadFullSkin(sprites, skinlist->at(skinID).path.string(), colorID);
}

void CResourceManager::loadAllSprites() {
    const fs::path graphicspack = gamegraphicspacklist->currentPath();
    const auto builder = [&graphicspack](std::string_view relpath) {
        return SpriteBuilder(convertPath(relpath, graphicspack));
    };

    std::string shyguyPath = convertPath("gfx/packs/modeskins/shyguy.png", graphicspack);
    if (!FileExists(shyguyPath))
        shyguyPath = convertPath("gfx/packs/modeskins/shyguy.bmp", graphicspack);

    std::string chickenPath = convertPath("gfx/packs/modeskins/chicken.png", graphicspack);
    if (!FileExists(chickenPath))
        chickenPath = convertPath("gfx/packs/modeskins/chicken.bmp", graphicspack);

    std::string bobombPath = convertPath("gfx/packs/modeskins/bobomb.png", graphicspack);
    if (!FileExists(bobombPath))
        bobombPath = convertPath("gfx/packs/modeskins/bobomb.bmp", graphicspack);

    //Just load menu skins for now (just standing right sprite)
    for (short k = 0; k < MAX_PLAYERS; k++) {
        LoadFullSkin(spr_shyguy[k], shyguyPath, k);
        LoadFullSkin(spr_chocobo[k], chickenPath, k);
        LoadFullSkin(spr_bobomb[k], bobombPath, k);
    }

    menu_survival = builder("gfx/packs/modeobjects/menu_survival.png").create();
    menu_stomp = builder("gfx/packs/modeobjects/menu_stomp.png").create();
    menu_egg = builder("gfx/packs/modeobjects/menu_egg.png").create();

    spr_clouds = builder("gfx/packs/eyecandy/cloud.png").withAlpha(255).withWrapping().create();
    spr_ghosts = builder("gfx/packs/eyecandy/ghost.png").withAlpha(128).withWrapping().create();
    spr_fish = builder("gfx/packs/eyecandy/fish.png").withAlpha(128).withWrapping().create();
    spr_leaves = builder("gfx/packs/eyecandy/leaves.png").withWrapping().create();
    spr_snow = builder("gfx/packs/eyecandy/snow.png").withWrapping().create();
    spr_rain = builder("gfx/packs/eyecandy/rain.png").withWrapping().create();

    spr_noteblock = builder("gfx/packs/blocks/noteblock.png").create();
    spr_breakableblock = builder("gfx/packs/blocks/breakableblock.png").create();
    spr_powerupblock = builder("gfx/packs/blocks/powerupblock.png").create();
    spr_donutblock = builder("gfx/packs/blocks/donutblock.png").create();
    spr_flipblock = builder("gfx/packs/blocks/flipblock.png").create();
    spr_bounceblock = builder("gfx/packs/blocks/bounceblock.png").create();
    spr_throwblock = builder("gfx/packs/blocks/throwblock.png").create();
    spr_switchblocks = builder("gfx/packs/blocks/switchblock.png").create();
    spr_viewblock = builder("gfx/packs/blocks/viewblock.png").create();
    spr_weaponbreakableblock = builder("gfx/packs/blocks/weaponbreakableblock.png").create();

    spr_spring = builder("gfx/packs/powerups/spring.png").withWrapping().create();
    spr_spike = builder("gfx/packs/powerups/spike.png").withWrapping().create();
    spr_kuriboshoe = builder("gfx/packs/powerups/kuriboshoe.png").withWrapping().create();
    spr_throwbox = builder("gfx/packs/powerups/throwbox.png").withWrapping().create();

    spr_tileanimation[0] = builder("gfx/packs/tilesets/tile_animation.png").create();
    spr_tileanimation[1] = builder("gfx/packs/tilesets/tile_animation_preview.png").create();
    spr_tileanimation[2] = builder("gfx/packs/tilesets/tile_animation_thumbnail.png").create();

    spr_blocks[0] = builder("gfx/packs/tilesets/blocks.png").create();
    spr_blocks[1] = builder("gfx/packs/tilesets/blocks_preview.png").create();
    spr_blocks[2] = builder("gfx/packs/tilesets/blocks_thumbnail.png").create();

    spr_unknowntile[0] = builder("gfx/packs/tilesets/unknown_tile.png").create();
    spr_unknowntile[1] = builder("gfx/packs/tilesets/unknown_tile_preview.png").create();
    spr_unknowntile[2] = builder("gfx/packs/tilesets/unknown_tile_thumbnail.png").create();

    spr_brokenyellowblock = builder("gfx/packs/eyecandy/brokenyellowblock.png").withWrapping().create();
    spr_brokenflipblock = builder("gfx/packs/eyecandy/brokenflipblock.png").withWrapping().create();
    spr_brokenblueblock = builder("gfx/packs/eyecandy/brokenblueblock.png").withWrapping().create();
    spr_brokengrayblock = builder("gfx/packs/eyecandy/brokengrayblock.png").withWrapping().create();

    spr_brokeniceblock = builder("gfx/packs/eyecandy/icecube.png").withWrapping().create();
    spr_iceblock = builder("gfx/packs/eyecandy/iceblock.png").withWrapping().create();

    spr_tanooki = builder("gfx/packs/powerups/tanooki.png").withWrapping().create();
    spr_statue = builder("gfx/packs/projectiles/statue.png").withWrapping().create();
    spr_starpowerup = builder("gfx/packs/powerups/starpowerup.png").withWrapping().create();
    spr_1uppowerup = builder("gfx/packs/powerups/1uppowerup.png").withWrapping().create();
    spr_2uppowerup = builder("gfx/packs/powerups/2uppowerup.png").withWrapping().create();
    spr_3uppowerup = builder("gfx/packs/powerups/3uppowerup.png").withWrapping().create();
    spr_5uppowerup = builder("gfx/packs/powerups/5uppowerup.png").withWrapping().create();
    spr_firepowerup = builder("gfx/packs/powerups/fireflower.png").withWrapping().create();
    spr_hammerpowerup = builder("gfx/packs/powerups/hammerpowerup.png").withWrapping().create();
    spr_icewandpowerup = builder("gfx/packs/powerups/icewandpowerup.png").withWrapping().create();
    spr_podobopowerup = builder("gfx/packs/powerups/podobopowerup.png").withWrapping().create();
    spr_poisonpowerup = builder("gfx/packs/powerups/poisonpowerup.png").withWrapping().create();
    spr_mysterymushroompowerup = builder("gfx/packs/powerups/mysterymushroom.png").withWrapping().create();
    spr_boomerangpowerup = builder("gfx/packs/powerups/boomerangpowerup.png").withWrapping().create();
    spr_clockpowerup = builder("gfx/packs/powerups/clockpowerup.png").withWrapping().create();
    spr_bobombpowerup = builder("gfx/packs/powerups/bobombpowerup.png").withWrapping().create();
    spr_powpowerup = builder("gfx/packs/powerups/powpowerup.png").withWrapping().create();
    spr_modpowerup = builder("gfx/packs/powerups/modpowerup.png").withWrapping().create();
    spr_bulletbillpowerup = builder("gfx/packs/powerups/bulletbillpowerup.png").withWrapping().create();
    spr_featherpowerup = builder("gfx/packs/powerups/featherpowerup.png").withWrapping().create();
    spr_leafpowerup = builder("gfx/packs/powerups/leafpowerup.png").withWrapping().create();
    spr_bombpowerup = builder("gfx/packs/powerups/bombpowerup.png").withWrapping().create();
    spr_pwingspowerup = builder("gfx/packs/powerups/pwings.png").withWrapping().create();

    spr_extraheartpowerup = builder("gfx/packs/powerups/heartpowerup.png").withWrapping().create();
    spr_extratimepowerup = builder("gfx/packs/powerups/extratimepowerup.png").withWrapping().create();
    spr_jailkeypowerup = builder("gfx/packs/powerups/jailkeypowerup.png").withWrapping().create();

    spr_secret1 = builder("gfx/packs/powerups/secret1.png").withWrapping().create();
    spr_secret2 = builder("gfx/packs/powerups/secret2.png").withWrapping().create();
    spr_secret3 = builder("gfx/packs/powerups/secret3.png").withWrapping().create();
    spr_secret4 = builder("gfx/packs/powerups/secret4.png").withWrapping().create();

    spr_shade[0] = builder("gfx/packs/eyecandy/shade1.png").withAlpha(64).create();
    spr_shade[1] = builder("gfx/packs/eyecandy/shade2.png").withAlpha(64).create();
    spr_shade[2] = builder("gfx/packs/eyecandy/shade3.png").withAlpha(64).create();
    spr_scorehearts = builder("gfx/packs/menu/score_hearts.png").create();
    spr_scorecards = builder("gfx/packs/menu/score_cards.png").create();
    spr_scorecoins = builder("gfx/packs/menu/score_coins.png").create();

    spr_timershade = builder("gfx/packs/eyecandy/timershade.png").withAlpha(64).create();
    spr_scoretext = builder("gfx/packs/fonts/score.png").create();
    spr_racetext = builder("gfx/packs/fonts/race.png").create();

    spr_crown = builder("gfx/packs/eyecandy/crown.png").withWrapping().create();
    spr_cape = builder("gfx/packs/eyecandy/cape.png").withWrapping().create();
    spr_tail = builder("gfx/packs/eyecandy/tail.png").withWrapping().create();
    spr_wings = builder("gfx/packs/eyecandy/wings.png").withWrapping().create();

    spr_warplock = builder("gfx/packs/eyecandy/warplock.png").create();
    spr_coinsparkle = builder("gfx/packs/eyecandy/coinsparks.png").withWrapping().create();
    spr_shinesparkle = builder("gfx/packs/eyecandy/shinesparks.png").withWrapping().create();
    spr_shellbounce = builder("gfx/packs/eyecandy/shellbounce.png").withWrapping().create();
    spr_superstomp = builder("gfx/packs/eyecandy/supersmash.png").withWrapping().create();

    spr_egg = builder("gfx/packs/modeobjects/egg.png").withWrapping().create();
    spr_eggnumbers = builder("gfx/packs/modeobjects/eggnumbers.png").withWrapping().create();
    spr_star = builder("gfx/packs/modeobjects/star.png").withWrapping().create();
    spr_flags = builder("gfx/packs/modeobjects/flags.png").withWrapping().create();
    spr_frenzycards = builder("gfx/packs/modeobjects/frenzycards.png").withWrapping().create();
    spr_collectcards = builder("gfx/packs/modeobjects/collectcards.png").withWrapping().create();

    spr_yoshi = builder("gfx/packs/modeobjects/yoshi.png").withWrapping().create();
    spr_coin = builder("gfx/packs/modeobjects/coin.png").withWrapping().create();
    spr_thwomp = builder("gfx/packs/modeobjects/thwomp.png").withWrapping().create();
    spr_podobo = builder("gfx/packs/modeobjects/podobo.png").create();
    spr_bowserfire = builder("gfx/packs/modeobjects/bowserfire.png").create();
    spr_areas = builder("gfx/packs/modeobjects/areas.png").create();
    spr_kingofthehillarea = builder("gfx/packs/modeobjects/kingofthehill.png").withAlpha(128).create();
    spr_jail = builder("gfx/packs/modeobjects/jail.png").withAlpha(160).withWrapping().create();
    spr_goomba = builder("gfx/packs/modeobjects/goomba.png").withWrapping().create();
    spr_goombadead = builder("gfx/packs/eyecandy/goombadead.png").withWrapping().create();
    spr_goombadeadflying = builder("gfx/packs/eyecandy/goombadeadflying.png").withWrapping().create();
    spr_koopa = builder("gfx/packs/modeobjects/koopa.png").withWrapping().create();
    spr_buzzybeetle = builder("gfx/packs/modeobjects/buzzybeetle.png").withWrapping().create();
    spr_spiny = builder("gfx/packs/modeobjects/spiny.png").withWrapping().create();
    spr_paragoomba = builder("gfx/packs/modeobjects/paragoomba.png").withWrapping().create();
    spr_parakoopa = builder("gfx/packs/modeobjects/parakoopa.png").withWrapping().create();
    spr_redparakoopa = builder("gfx/packs/modeobjects/redparakoopa.png").withWrapping().create();
    spr_redkoopa = builder("gfx/packs/modeobjects/redkoopa.png").withWrapping().create();
    spr_cheepcheep = builder("gfx/packs/modeobjects/cheepcheep.png").withWrapping().create();
    spr_cheepcheepdead = builder("gfx/packs/eyecandy/cheepcheepdead.png").withWrapping().create();

    spr_sledgebrothers = builder("gfx/packs/modeobjects/sledgebrothers.png").withWrapping().create();
    spr_sledgebrothersdead = builder("gfx/packs/eyecandy/sledgebrothersdead.png").withWrapping().create();

    spr_bulletbill = builder("gfx/packs/projectiles/bulletbill.png").create();
    spr_bulletbilldead = builder("gfx/packs/eyecandy/bulletbilldead.png").create();
    spr_chicken = builder("gfx/packs/modeobjects/chicken.png").withAlpha(160).withWrapping().create();
    spr_racegoal = builder("gfx/packs/modeobjects/racegoal.png").create();
    spr_pipegamebonus = builder("gfx/packs/modeobjects/pipeminigamebonuses.png").withWrapping().create();

    spr_phanto = builder("gfx/packs/modeobjects/phanto.png").withWrapping().create();
    spr_phantokey = builder("gfx/packs/modeobjects/key.png").withWrapping().create();

    spr_bonuschest = builder("gfx/packs/modeobjects/bonuschest.png").withWrapping().create();
    spr_teleportstar = builder("gfx/packs/eyecandy/teleportstar.png").create();

    spr_fireball = builder("gfx/packs/projectiles/fireball.png").withWrapping().create();
    spr_hammer = builder("gfx/packs/projectiles/hammer.png").withWrapping().create();
    spr_iceblast = builder("gfx/packs/projectiles/wandblast.png").withWrapping().create();
    spr_boomerang = builder("gfx/packs/projectiles/boomerang.png").withWrapping().create();
    spr_shell = builder("gfx/packs/projectiles/shell.png").withWrapping().create();
    spr_shelldead = builder("gfx/packs/eyecandy/shelldead.png").withWrapping().create();
    spr_blueblock = builder("gfx/packs/projectiles/throwblock.png").withWrapping().create();
    spr_bomb = builder("gfx/packs/projectiles/bomb.png").withWrapping().create();

    spr_superfireball = builder("gfx/packs/modeobjects/superfire.png").withWrapping().create();
    spr_sledgehammer = builder("gfx/packs/modeobjects/sledgehammer.png").withWrapping().create();

    spr_hazard_fireball[0] = builder("gfx/packs/hazards/fireball.png").withWrapping().create();
    spr_hazard_fireball[1] = builder("gfx/packs/hazards/fireball_preview.png").withWrapping().create();
    spr_hazard_fireball[2] = builder("gfx/packs/hazards/fireball_thumbnail.png").withWrapping().create();

    spr_hazard_rotodisc[0] = builder("gfx/packs/hazards/rotodisc.png").withWrapping().create();
    spr_hazard_rotodisc[1] = builder("gfx/packs/hazards/rotodisc_preview.png").withWrapping().create();
    spr_hazard_rotodisc[2] = builder("gfx/packs/hazards/rotodisc_thumbnail.png").withWrapping().create();

    spr_hazard_bulletbill[0] = builder("gfx/packs/hazards/bulletbill.png").create();
    spr_hazard_bulletbill[1] = builder("gfx/packs/hazards/bulletbill_preview.png").create();
    spr_hazard_bulletbill[2] = builder("gfx/packs/hazards/bulletbill_thumbnail.png").create();

    spr_hazard_flame[0] = builder("gfx/packs/hazards/flame.png").withWrapping().create();
    spr_hazard_flame[1] = builder("gfx/packs/hazards/flame_preview.png").withWrapping().create();
    spr_hazard_flame[2] = builder("gfx/packs/hazards/flame_thumbnail.png").withWrapping().create();

    spr_hazard_pirhanaplant[0] = builder("gfx/packs/hazards/pirhanaplant.png").withWrapping().create();
    spr_hazard_pirhanaplant[1] = builder("gfx/packs/hazards/pirhanaplant_preview.png").withWrapping().create();
    spr_hazard_pirhanaplant[2] = builder("gfx/packs/hazards/pirhanaplant_thumbnail.png").withWrapping().create();

    spr_hazard_bulletbilldead = builder("gfx/packs/hazards/bulletbilldead.png").create();

    spr_fireballexplosion = builder("gfx/packs/eyecandy/fireballexplosion.png").withAlpha(160).withWrapping().create();
    spr_frictionsmoke = builder("gfx/packs/eyecandy/frictionsmoke.png").withAlpha(160).withWrapping().create();
    spr_bobombsmoke = builder("gfx/packs/eyecandy/bobombsmoke.png").withAlpha(160).withWrapping().create();
    spr_explosion = builder("gfx/packs/eyecandy/explosion.png").withWrapping().create();
    spr_burnup = builder("gfx/packs/eyecandy/burnup.png").withAlpha(192).withWrapping().create();
    spr_fireworks = builder("gfx/packs/eyecandy/fireworks.png").withWrapping().create();
    spr_poof = builder("gfx/packs/eyecandy/poof.png").withWrapping().create();

    spr_spawnsmoke = builder("gfx/packs/eyecandy/spawnsmoke.png").withAlpha(128).withWrapping().create();
    spr_spawndoor = builder("gfx/packs/eyecandy/spawndoor.png").withWrapping().create();

    spr_bonus = builder("gfx/packs/eyecandy/bonus.png").withWrapping().create();
    spr_extralife = builder("gfx/packs/eyecandy/extralife.png").withWrapping().create();

    spr_windmeter = builder("gfx/packs/eyecandy/wind_meter.png").withAlpha(192).withWrapping().create();
    spr_overlayhole = builder("gfx/packs/eyecandy/overlayholes.png").withColorKey(RGB {0, 255, 0}).withWrapping().create();

    spr_award = builder("gfx/packs/awards/killsinrow.png").withAlpha(128).withWrapping().create();
    spr_awardsolid = builder("gfx/packs/awards/killsinrow.png").withWrapping().create();
    spr_awardsouls = builder("gfx/packs/awards/souls.png").withWrapping().create();
    spr_awardsoulspawn = builder("gfx/packs/awards/soulspawn.png").withWrapping().create();

    spr_awardkillsinrow = builder("gfx/packs/awards/killsinrownumbers.png").withWrapping().create();

    spr_flagbases = builder("gfx/packs/modeobjects/flagbases.png").withAlpha(160).withWrapping().create();
    spr_ownedtags = builder("gfx/packs/modeobjects/ownedtags.png").withAlpha(160).withWrapping().create();

    spr_storedpowerupsmall = builder("gfx/packs/powerups/small.png").withWrapping().create();
    spr_storedpoweruplarge = builder("gfx/packs/powerups/large.png").create();
    spr_powerupselector = builder("gfx/packs/awards/award.png").create();

    spr_abovearrows = builder("gfx/packs/eyecandy/abovearrows.png").withWrapping().create();
}

void CResourceManager::loadMenuGraphics()
{
    const fs::path graphicspack = menugraphicspacklist->currentPath();
    const auto builder = [&graphicspack](std::string_view relpath) {
        return SpriteBuilder(convertPath(relpath, graphicspack));
    };

    menu_shade = builder("gfx/packs/menu/menu_shade.png").withAlpha(App::menuTransparency).withoutColorKey().create();

    spr_scoreboard = builder("gfx/packs/menu/scoreboard.png").create();
    menu_slider_bar = builder("gfx/packs/menu/menu_slider_bar.png").create();
    menu_plain_field = builder("gfx/packs/menu/menu_plain_field.png").create();
    menu_player_select = builder("gfx/packs/menu/menu_player_select.png").create();
    menu_dialog = builder("gfx/packs/menu/menu_dialog.png").create();
    menu_map_filter = builder("gfx/packs/menu/menu_map_filter.png").create();
    menu_match_select = builder("gfx/packs/menu/menu_match_select.png").create();

    menu_verticalarrows = builder("gfx/packs/menu/menu_vertical_arrows.png").create();

    menu_mode_small = builder("gfx/packs/menu/menu_mode_small.png").create();
    menu_mode_large = builder("gfx/packs/menu/menu_mode_large.png").create();

    spr_dialog = builder("gfx/packs/menu/dialog.png").create();
    spr_dialogbutton = builder("gfx/packs/menu/dialog_button.png").create();
    spr_tournament_background = builder("gfx/packs/menu/tournament_background.png").create();
    spr_tournament_powerup_splash = builder("gfx/packs/menu/tournament_powerup_splash.png").create();
    spr_player_select_background = builder("gfx/packs/menu/player_select_background.png").create();
    spr_player_select_ready = builder("gfx/packs/menu/player_select_ready.png").create();
    //spr_ipfield = builder("gfx/packs/menu/menu_ipfield.png").create();
    spr_selectfield = builder("gfx/packs/menu/menu_selectfield.png").create();
    spr_selectfielddisabled = builder("gfx/packs/menu/menu_selectfield_disabled.png").create();
    spr_map_filter_icons = builder("gfx/packs/menu/menu_map_flags.png").create();
    spr_tour_markers = builder("gfx/packs/menu/tour_markers.png").create();
    spr_menu_boxed_numbers = builder("gfx/packs/menu/menu_boxed_numbers.png").create();
    spr_countdown_numbers = builder("gfx/packs/menu/game_countdown_numbers.png").create();
    spr_thumbnail_warps[0] = builder("gfx/packs/menu/menu_warp_preview.png").create();
    spr_thumbnail_warps[1] = builder("gfx/packs/menu/menu_warp_thumbnail.png").create();
    spr_thumbnail_mapitems[0] = builder("gfx/packs/menu/menu_mapitems_preview.png").create();
    spr_thumbnail_mapitems[1] = builder("gfx/packs/menu/menu_mapitems_thumbnail.png").create();

    spr_announcementicons = builder("gfx/packs/menu/menu_announcement_icons.png").create();

    spr_platformstarttile = builder("gfx/leveleditor/leveleditor_platformstarttile.png").withAlpha(64).withWrapping().create();
    spr_platformendtile = builder("gfx/leveleditor/leveleditor_selectedtile.png").withAlpha(64).withWrapping().create();
    spr_platformpath = builder("gfx/leveleditor/leveleditor_platform_path.png").withAlpha(128).withWrapping().create();
}

void CResourceManager::loadWorldGraphics()
{
    const fs::path graphicspack = worldgraphicspacklist->currentPath();
    const auto builder = [&graphicspack](std::string_view relpath) {
        return SpriteBuilder(convertPath(relpath, graphicspack));
    };

    spr_worldbackground[0] = builder("gfx/packs/world/world_background.png").create();
    spr_worldbackground[1] = builder("gfx/packs/world/preview/world_background.png").create();

    spr_worldforeground[0] = builder("gfx/packs/world/world_foreground.png").create();
    spr_worldforeground[1] = builder("gfx/packs/world/preview/world_foreground.png").create();

    spr_worldforegroundspecial[0] = builder("gfx/packs/world/world_foreground_special.png").create();
    spr_worldforegroundspecial[1] = builder("gfx/packs/world/preview/world_foreground_special.png").create();

    spr_worldpaths[0] = builder("gfx/packs/world/world_paths.png").create();
    spr_worldpaths[1] = builder("gfx/packs/world/preview/world_paths.png").create();

    spr_worldvehicle[0] = builder("gfx/packs/world/world_vehicles.png").create();
    spr_worldvehicle[1] = builder("gfx/packs/world/preview/world_vehicles.png").create();

    spr_worlditems = builder("gfx/packs/world/world_powerups.png").create();
    spr_worlditempopup = builder("gfx/packs/world/world_item_popup.png").create();
    spr_worlditemssmall = builder("gfx/packs/world/world_powerupssmall.png").create();
    spr_worlditemsplace = builder("gfx/packs/world/world_bonusplace.png").create();
    spr_worldbonushouse = builder("gfx/packs/world/world_bonushouse.png").create();
}

void CResourceManager::loadGameGraphics()
{
    std::string graphicspack = gamegraphicspacklist->currentPath().string();

    g_tilesetmanager->init(graphicspack);

    bool loadok = true;
    loadok &= game_font_small.init(convertPath("gfx/packs/fonts/font_small.png", graphicspack));
    loadok &= game_font_large.init(convertPath("gfx/packs/fonts/font_large.png", graphicspack));
    if (!loadok)
        throw "ERROR: error loading the fonts!";

    loadAllSprites();
}

void CResourceManager::loadStartGraphics()
{
    const fs::path graphicspack = menugraphicspacklist->currentPath();
    const auto builder = [&graphicspack](std::string_view relpath) {
        return SpriteBuilder(convertPath(relpath, graphicspack));
    };

    bool loadok = true;
    loadok &= menu_font_small.init(convertPath("gfx/packs/menu/menu_font_small.png", graphicspack));
    loadok &= menu_font_large.init(convertPath("gfx/packs/menu/menu_font_large.png", graphicspack));
    if (!loadok)
        throw "ERROR: error loading the fonts!";

    //load basic stuff
    menu_backdrop = builder("gfx/packs/menu/menu_background.png").withoutColorKey().create();
    menu_smw = builder("gfx/packs/menu/menu_smw.png").create();
    menu_version = builder("gfx/packs/menu/menu_version.png").create();
}

void CResourceManager::loadAllGraphics()
{
    const fs::path graphicspack = gamegraphicspacklist->currentPath();
    const auto builder = [&graphicspack](std::string_view relpath) {
        return SpriteBuilder(convertPath(relpath, graphicspack));
    };

    for (size_t player = 0; player < MAX_PLAYERS; player++) {
        for (size_t frame = 0; frame < PGFX_LAST; frame++) {
            spr_player[player][frame].SetWrap(true);
            spr_shyguy[player][frame].SetWrap(true);
            spr_chocobo[player][frame].SetWrap(true);
            spr_bobomb[player][frame].SetWrap(true);
        }
    }

    loadMenuGraphics();
    loadWorldGraphics();
    loadGameGraphics();

    spr_backmap[0] = builder("gfx/packs/backgrounds/Land_Classic.png").withoutColorKey().create();
    spr_backmap[1] = builder("gfx/packs/backgrounds/Land_Classic.png").withoutColorKey().create();
    spr_frontmap[0] = builder("gfx/packs/backgrounds/Land_Classic.png").withoutColorKey().create();
    spr_frontmap[1] = builder("gfx/packs/backgrounds/Land_Classic.png").withoutColorKey().create();

    spr_overlay = builder("gfx/packs/menu/menu_shade.png").create();
}

bool CResourceManager::LoadGameSounds()
{
    game_values.soundcapable = false;

    if (!sfx_canPlayAudio())
        return false;

    std::string soundpack = soundpacklist->currentPath().string();

    sfx_mip.init(convertPath("sfx/packs/mip.wav", soundpack));
    sfx_deathsound.init(convertPath("sfx/packs/death.wav", soundpack));
    sfx_jump.init(convertPath("sfx/packs/jump.wav", soundpack));
    sfx_skid.init(convertPath("sfx/packs/skid.wav", soundpack));
    sfx_capejump.init(convertPath("sfx/packs/capejump.wav", soundpack));
    sfx_invinciblemusic.init(convertPath("sfx/packs/invincible.wav", soundpack));
    sfx_extraguysound.init(convertPath("sfx/packs/1up.wav", soundpack));
    sfx_sprout.init(convertPath("sfx/packs/sprout.wav", soundpack));
    sfx_collectpowerup.init(convertPath("sfx/packs/collectpowerup.wav", soundpack));
    sfx_collectfeather.init(convertPath("sfx/packs/feather.wav", soundpack));
    sfx_tailspin.init(convertPath("sfx/packs/tail.wav", soundpack));
    sfx_storepowerup.init(convertPath("sfx/packs/storeitem.wav", soundpack));
    sfx_breakblock.init(convertPath("sfx/packs/breakblock.wav", soundpack));
    sfx_bump.init(convertPath("sfx/packs/bump.wav", soundpack));
    sfx_coin.init(convertPath("sfx/packs/coin.wav", soundpack));
    sfx_fireball.init(convertPath("sfx/packs/fireball.wav", soundpack));
    sfx_springjump.init(convertPath("sfx/packs/springjump.wav", soundpack));
    sfx_timewarning.init(convertPath("sfx/packs/timewarning.wav", soundpack));
    sfx_hit.init(convertPath("sfx/packs/hit.wav", soundpack));
    sfx_chicken.init(convertPath("sfx/packs/chicken.wav", soundpack));
    sfx_transform.init(convertPath("sfx/packs/transform.wav", soundpack));
    sfx_yoshi.init(convertPath("sfx/packs/yoshi.wav", soundpack));
    sfx_pause.init(convertPath("sfx/packs/pause.wav", soundpack));
    sfx_bobombsound.init(convertPath("sfx/packs/bob-omb.wav", soundpack));
    sfx_areatag.init(convertPath("sfx/packs/dcoin.wav", soundpack));
    sfx_cannon.init(convertPath("sfx/packs/cannon.wav", soundpack));
    sfx_burnup.init(convertPath("sfx/packs/burnup.wav", soundpack));
    sfx_pipe.init(convertPath("sfx/packs/warp.wav", soundpack));
    sfx_thunder.init(convertPath("sfx/packs/thunder.wav", soundpack));
    sfx_slowdownmusic.init(convertPath("sfx/packs/clock.wav", soundpack));
    sfx_flyingsound.init(convertPath("sfx/packs/slowdown.wav", soundpack));
    sfx_storedpowerupsound.init(convertPath("sfx/packs/storedpowerup.wav", soundpack));
    sfx_kicksound.init(convertPath("sfx/packs/kick.wav", soundpack));
    sfx_racesound.init(convertPath("sfx/packs/race.wav", soundpack));
    sfx_bulletbillsound.init(convertPath("sfx/packs/bulletbill.wav", soundpack));
    sfx_boomerang.init(convertPath("sfx/packs/boomerang.wav", soundpack));
    sfx_spit.init(convertPath("sfx/packs/spit.wav", soundpack));
    sfx_starwarning.init(convertPath("sfx/packs/starwarning.wav", soundpack));
    sfx_powerdown.init(convertPath("sfx/packs/powerdown.wav", soundpack));
    sfx_switchpress.init(convertPath("sfx/packs/switchpress.wav", soundpack));
    sfx_superspring.init(convertPath("sfx/packs/superspring.wav", soundpack));
    sfx_stun.init(convertPath("sfx/packs/stun.wav", soundpack));
    sfx_inventory.init(convertPath("sfx/packs/inventory.wav", soundpack));
    sfx_worldmove.init(convertPath("sfx/packs/mapmove.wav", soundpack));
    sfx_treasurechest.init(convertPath("sfx/packs/treasurechest.wav", soundpack));
    sfx_flamecannon.init(convertPath("sfx/packs/flamecannon.wav", soundpack));
    sfx_wand.init(convertPath("sfx/packs/wand.wav", soundpack));
    sfx_enterstage.init(convertPath("sfx/packs/enter-stage.wav", soundpack));
    sfx_gameover.init(convertPath("sfx/packs/gameover.wav", soundpack));
    sfx_pickup.init(convertPath("sfx/packs/pickup.wav", soundpack));

    game_values.soundcapable = true;
    return true;
}

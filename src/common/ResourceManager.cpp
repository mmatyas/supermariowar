#include "ResourceManager.h"

#include "FileList.h"
#include "Game.h"
#include "GameValues.h"
#include "TilesetManager.h"
#include "path.h"
#include "gfx/Color.h"

#include <iostream>

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
    try {
        spr_player[playerID] = gfx_loadmenuskin(skinlist->at(skinID).path, colorID, fLoadBothDirections);
        return true;
    } catch (const std::string& what) {
        std::cout << "ERROR: " << what << std::endl;
        return false;
    }
}

bool CResourceManager::LoadMenuSkin(short playerID, const fs::path& filename, short colorID, bool fLoadBothDirections)
{
    try {
        spr_player[playerID] = gfx_loadmenuskin(filename, colorID, fLoadBothDirections);
        return true;
    } catch (const std::string& what) {
        std::cout << "ERROR: " << what << std::endl;
        return false;
    }
}

SpriteStrip CResourceManager::LoadFullSkin(const fs::path& path, short colorID)
{
    return gfx_loadfullskin(path, colorID);
}

SpriteStrip CResourceManager::LoadFullSkin(short skinID, short colorID)
{
    return gfx_loadfullskin(skinlist->at(skinID).path, colorID);
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
        spr_shyguy[k] = LoadFullSkin(shyguyPath, k);
        spr_chocobo[k] = LoadFullSkin(chickenPath, k);
        spr_bobomb[k] = LoadFullSkin(bobombPath, k);
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

    const fs::path soundpack = soundpacklist->currentPath();
    const auto open = [&soundpack](std::string_view relpath) {
        return sfxSound(convertPath(relpath, soundpack));
    };

    sfx_mip = open("sfx/packs/mip.wav");
    sfx_deathsound = open("sfx/packs/death.wav");
    sfx_jump = open("sfx/packs/jump.wav");
    sfx_skid = open("sfx/packs/skid.wav");
    sfx_capejump = open("sfx/packs/capejump.wav");
    sfx_invinciblemusic = open("sfx/packs/invincible.wav");
    sfx_extraguysound = open("sfx/packs/1up.wav");
    sfx_sprout = open("sfx/packs/sprout.wav");
    sfx_collectpowerup = open("sfx/packs/collectpowerup.wav");
    sfx_collectfeather = open("sfx/packs/feather.wav");
    sfx_tailspin = open("sfx/packs/tail.wav");
    sfx_storepowerup = open("sfx/packs/storeitem.wav");
    sfx_breakblock = open("sfx/packs/breakblock.wav");
    sfx_bump = open("sfx/packs/bump.wav");
    sfx_coin = open("sfx/packs/coin.wav");
    sfx_fireball = open("sfx/packs/fireball.wav");
    sfx_springjump = open("sfx/packs/springjump.wav");
    sfx_timewarning = open("sfx/packs/timewarning.wav");
    sfx_hit = open("sfx/packs/hit.wav");
    sfx_chicken = open("sfx/packs/chicken.wav");
    sfx_transform = open("sfx/packs/transform.wav");
    sfx_yoshi = open("sfx/packs/yoshi.wav");
    sfx_pause = open("sfx/packs/pause.wav");
    sfx_bobombsound = open("sfx/packs/bob-omb.wav");
    sfx_areatag = open("sfx/packs/dcoin.wav");
    sfx_cannon = open("sfx/packs/cannon.wav");
    sfx_burnup = open("sfx/packs/burnup.wav");
    sfx_pipe = open("sfx/packs/warp.wav");
    sfx_thunder = open("sfx/packs/thunder.wav");
    sfx_slowdownmusic = open("sfx/packs/clock.wav");
    sfx_flyingsound = open("sfx/packs/slowdown.wav");
    sfx_storedpowerupsound = open("sfx/packs/storedpowerup.wav");
    sfx_kicksound = open("sfx/packs/kick.wav");
    sfx_racesound = open("sfx/packs/race.wav");
    sfx_bulletbillsound = open("sfx/packs/bulletbill.wav");
    sfx_boomerang = open("sfx/packs/boomerang.wav");
    sfx_spit = open("sfx/packs/spit.wav");
    sfx_starwarning = open("sfx/packs/starwarning.wav");
    sfx_powerdown = open("sfx/packs/powerdown.wav");
    sfx_switchpress = open("sfx/packs/switchpress.wav");
    sfx_superspring = open("sfx/packs/superspring.wav");
    sfx_stun = open("sfx/packs/stun.wav");
    sfx_inventory = open("sfx/packs/inventory.wav");
    sfx_worldmove = open("sfx/packs/mapmove.wav");
    sfx_treasurechest = open("sfx/packs/treasurechest.wav");
    sfx_flamecannon = open("sfx/packs/flamecannon.wav");
    sfx_wand = open("sfx/packs/wand.wav");
    sfx_enterstage = open("sfx/packs/enter-stage.wav");
    sfx_gameover = open("sfx/packs/gameover.wav");
    sfx_pickup = open("sfx/packs/pickup.wav");

    game_values.soundcapable = true;
    return true;
}

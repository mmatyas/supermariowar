#include "ResourceManager.h"

#include "FileList.h"
#include "Game.h"
#include "GameValues.h"
#include "TilesetManager.h"

extern SkinList *skinlist;
extern GraphicsList *menugraphicspacklist;
extern GraphicsList *worldgraphicspacklist;
extern GraphicsList *gamegraphicspacklist;
extern SoundsList *soundpacklist;

extern CTilesetManager  *g_tilesetmanager;

extern CGameValues game_values;

extern CGame* smw;

extern void _load_drawmsg(const std::string& f);
extern void _load_waitforkey();

bool CResourceManager::LoadMenuSkin(short playerID, short skinID, short colorID, bool fLoadBothDirections)
{
    return gfx_loadmenuskin(spr_player[playerID], skinlist->GetIndex(skinID), 255, 0, 255, colorID, fLoadBothDirections);
}

bool CResourceManager::LoadMenuSkin(short playerID, const std::string& filename, short colorID, bool fLoadBothDirections)
{
    return gfx_loadmenuskin(spr_player[playerID], filename, 255, 0, 255, colorID, fLoadBothDirections);
}

bool CResourceManager::LoadFullSkin(gfxSprite ** sprites, const std::string& filename, short colorID)
{
    return gfx_loadfullskin(sprites, filename, 255, 0, 255, colorID);
}

bool CResourceManager::LoadFullSkin(gfxSprite ** sprites, short skinID, short colorID)
{
    return LoadFullSkin(sprites, skinlist->GetIndex(skinID), colorID);
}

void CResourceManager::LoadAllSprites() {
    const char * graphicspack = gamegraphicspacklist->current_name();

    //Just load menu skins for now (just standing right sprite)
    for (short k = 0; k < MAX_PLAYERS; k++) {
        //LoadMenuSkin(k, game_values.skinids[k], game_values.colorids[k], false);
        LoadFullSkin(spr_shyguy[k], convertPath("gfx/packs/modeskins/shyguy.bmp", graphicspack), k);
        LoadFullSkin(spr_chocobo[k], convertPath("gfx/packs/modeskins/chicken.bmp", graphicspack), k);
        LoadFullSkin(spr_bobomb[k], convertPath("gfx/packs/modeskins/bobomb.bmp", graphicspack), k);
    }

    gfx_loadimage(&menu_survival, convertPath("gfx/packs/modeobjects/menu_survival.png", graphicspack), false);
    gfx_loadimage(&menu_stomp, convertPath("gfx/packs/modeobjects/menu_stomp.png", graphicspack), false);
    gfx_loadimage(&menu_egg, convertPath("gfx/packs/modeobjects/menu_egg.png", graphicspack), false);

    gfx_loadimage(&spr_clouds, convertPath("gfx/packs/eyecandy/cloud.png", graphicspack), 255, true, true);
    gfx_loadimage(&spr_ghosts, convertPath("gfx/packs/eyecandy/ghost.png", graphicspack), 128, true, true);
    gfx_loadimage(&spr_fish, convertPath("gfx/packs/eyecandy/fish.png", graphicspack), 128, true, true);
    gfx_loadimage(&spr_leaves, convertPath("gfx/packs/eyecandy/leaves.png", graphicspack), true);
    gfx_loadimage(&spr_snow, convertPath("gfx/packs/eyecandy/snow.png", graphicspack), true);
    gfx_loadimage(&spr_rain, convertPath("gfx/packs/eyecandy/rain.png", graphicspack), true);

    gfx_loadimage(&spr_noteblock, convertPath("gfx/packs/blocks/noteblock.png", graphicspack), false);
    gfx_loadimage(&spr_breakableblock, convertPath("gfx/packs/blocks/breakableblock.png", graphicspack), false);
    gfx_loadimage(&spr_powerupblock, convertPath("gfx/packs/blocks/powerupblock.png", graphicspack), false);
    gfx_loadimage(&spr_donutblock, convertPath("gfx/packs/blocks/donutblock.png", graphicspack), false);
    gfx_loadimage(&spr_flipblock, convertPath("gfx/packs/blocks/flipblock.png", graphicspack), false);
    gfx_loadimage(&spr_bounceblock, convertPath("gfx/packs/blocks/bounceblock.png", graphicspack), false);
    gfx_loadimage(&spr_throwblock, convertPath("gfx/packs/blocks/throwblock.png", graphicspack), false);
    gfx_loadimage(&spr_switchblocks, convertPath("gfx/packs/blocks/switchblock.png", graphicspack), false);
    gfx_loadimage(&spr_viewblock, convertPath("gfx/packs/blocks/viewblock.png", graphicspack), false);
    gfx_loadimage(&spr_weaponbreakableblock, convertPath("gfx/packs/blocks/weaponbreakableblock.png", graphicspack), false);

    gfx_loadimage(&spr_spring, convertPath("gfx/packs/powerups/spring.png", graphicspack), true);
    gfx_loadimage(&spr_spike, convertPath("gfx/packs/powerups/spike.png", graphicspack), true);
    gfx_loadimage(&spr_kuriboshoe, convertPath("gfx/packs/powerups/kuriboshoe.png", graphicspack), true);
    gfx_loadimage(&spr_throwbox, convertPath("gfx/packs/powerups/throwbox.png", graphicspack), true);

    gfx_loadimage(&spr_tileanimation[0], convertPath("gfx/packs/tilesets/tile_animation.png", graphicspack), false);
    gfx_loadimage(&spr_tileanimation[1], convertPath("gfx/packs/tilesets/tile_animation_preview.png", graphicspack), false);
    gfx_loadimage(&spr_tileanimation[2], convertPath("gfx/packs/tilesets/tile_animation_thumbnail.png", graphicspack), false);

    gfx_loadimage(&spr_blocks[0], convertPath("gfx/packs/tilesets/blocks.png", graphicspack), false);
    gfx_loadimage(&spr_blocks[1], convertPath("gfx/packs/tilesets/blocks_preview.png", graphicspack), false);
    gfx_loadimage(&spr_blocks[2], convertPath("gfx/packs/tilesets/blocks_thumbnail.png", graphicspack), false);

    gfx_loadimage(&spr_unknowntile[0], convertPath("gfx/packs/tilesets/unknown_tile.png", graphicspack), false);
    gfx_loadimage(&spr_unknowntile[1], convertPath("gfx/packs/tilesets/unknown_tile_preview.png", graphicspack), false);
    gfx_loadimage(&spr_unknowntile[2], convertPath("gfx/packs/tilesets/unknown_tile_thumbnail.png", graphicspack), false);

    gfx_loadimage(&spr_brokenyellowblock, convertPath("gfx/packs/eyecandy/brokenyellowblock.png", graphicspack), true);
    gfx_loadimage(&spr_brokenflipblock, convertPath("gfx/packs/eyecandy/brokenflipblock.png", graphicspack), true);
    gfx_loadimage(&spr_brokenblueblock, convertPath("gfx/packs/eyecandy/brokenblueblock.png", graphicspack), true);
    gfx_loadimage(&spr_brokengrayblock, convertPath("gfx/packs/eyecandy/brokengrayblock.png", graphicspack), true);

    gfx_loadimage(&spr_brokeniceblock, convertPath("gfx/packs/eyecandy/icecube.png", graphicspack), true);
    gfx_loadimage(&spr_iceblock, convertPath("gfx/packs/eyecandy/iceblock.png", graphicspack), true);

    gfx_loadimage(&spr_tanooki, convertPath("gfx/packs/powerups/tanooki.png", graphicspack), true);
    gfx_loadimage(&spr_statue, convertPath("gfx/packs/projectiles/statue.png", graphicspack), true);
    gfx_loadimage(&spr_starpowerup, convertPath("gfx/packs/powerups/starpowerup.png", graphicspack), true);
    gfx_loadimage(&spr_1uppowerup, convertPath("gfx/packs/powerups/1uppowerup.png", graphicspack), true);
    gfx_loadimage(&spr_2uppowerup, convertPath("gfx/packs/powerups/2uppowerup.png", graphicspack), true);
    gfx_loadimage(&spr_3uppowerup, convertPath("gfx/packs/powerups/3uppowerup.png", graphicspack), true);
    gfx_loadimage(&spr_5uppowerup, convertPath("gfx/packs/powerups/5uppowerup.png", graphicspack), true);
    gfx_loadimage(&spr_firepowerup, convertPath("gfx/packs/powerups/fireflower.png", graphicspack), true);
    gfx_loadimage(&spr_hammerpowerup, convertPath("gfx/packs/powerups/hammerpowerup.png", graphicspack), true);
    gfx_loadimage(&spr_icewandpowerup, convertPath("gfx/packs/powerups/icewandpowerup.png", graphicspack), true);
    gfx_loadimage(&spr_podobopowerup, convertPath("gfx/packs/powerups/podobopowerup.png", graphicspack), true);
    gfx_loadimage(&spr_poisonpowerup, convertPath("gfx/packs/powerups/poisonpowerup.png", graphicspack), true);
    gfx_loadimage(&spr_mysterymushroompowerup, convertPath("gfx/packs/powerups/mysterymushroom.png", graphicspack), true);
    gfx_loadimage(&spr_boomerangpowerup, convertPath("gfx/packs/powerups/boomerangpowerup.png", graphicspack), true);
    gfx_loadimage(&spr_clockpowerup, convertPath("gfx/packs/powerups/clockpowerup.png", graphicspack), true);
    gfx_loadimage(&spr_bobombpowerup, convertPath("gfx/packs/powerups/bobombpowerup.png", graphicspack), true);
    gfx_loadimage(&spr_powpowerup, convertPath("gfx/packs/powerups/powpowerup.png", graphicspack), true);
    gfx_loadimage(&spr_modpowerup, convertPath("gfx/packs/powerups/modpowerup.png", graphicspack), true);
    gfx_loadimage(&spr_bulletbillpowerup, convertPath("gfx/packs/powerups/bulletbillpowerup.png", graphicspack), true);
    gfx_loadimage(&spr_featherpowerup, convertPath("gfx/packs/powerups/featherpowerup.png", graphicspack), true);
    gfx_loadimage(&spr_leafpowerup, convertPath("gfx/packs/powerups/leafpowerup.png", graphicspack), true);
    gfx_loadimage(&spr_bombpowerup, convertPath("gfx/packs/powerups/bombpowerup.png", graphicspack), true);
    gfx_loadimage(&spr_pwingspowerup, convertPath("gfx/packs/powerups/pwings.png", graphicspack), true);

    gfx_loadimage(&spr_extraheartpowerup, convertPath("gfx/packs/powerups/heartpowerup.png", graphicspack), true);
    gfx_loadimage(&spr_extratimepowerup, convertPath("gfx/packs/powerups/extratimepowerup.png", graphicspack), true);
    gfx_loadimage(&spr_jailkeypowerup, convertPath("gfx/packs/powerups/jailkeypowerup.png", graphicspack), true);

    gfx_loadimage(&spr_secret1, convertPath("gfx/packs/powerups/secret1.png", graphicspack), true);
    gfx_loadimage(&spr_secret2, convertPath("gfx/packs/powerups/secret2.png", graphicspack), true);
    gfx_loadimage(&spr_secret3, convertPath("gfx/packs/powerups/secret3.png", graphicspack), true);
    gfx_loadimage(&spr_secret4, convertPath("gfx/packs/powerups/secret4.png", graphicspack), true);

    gfx_loadimage(&spr_shade[0], convertPath("gfx/packs/eyecandy/shade1.png", graphicspack), 64, false, true);
    gfx_loadimage(&spr_shade[1], convertPath("gfx/packs/eyecandy/shade2.png", graphicspack), 64, false, true);
    gfx_loadimage(&spr_shade[2], convertPath("gfx/packs/eyecandy/shade3.png", graphicspack), 64, false, true);
    gfx_loadimage(&spr_scorehearts, convertPath("gfx/packs/menu/score_hearts.png", graphicspack), false);
    gfx_loadimage(&spr_scorecards, convertPath("gfx/packs/menu/score_cards.png", graphicspack), false);
    gfx_loadimage(&spr_scorecoins, convertPath("gfx/packs/menu/score_coins.png", graphicspack), false);

    gfx_loadimage(&spr_timershade, convertPath("gfx/packs/eyecandy/timershade.png", graphicspack), 64, false, true);
    gfx_loadimage(&spr_scoretext, convertPath("gfx/packs/fonts/score.png", graphicspack), false);
    gfx_loadimage(&spr_racetext, convertPath("gfx/packs/fonts/race.png", graphicspack), false);

    gfx_loadimage(&spr_crown, convertPath("gfx/packs/eyecandy/crown.png", graphicspack), true);
    gfx_loadimage(&spr_cape, convertPath("gfx/packs/eyecandy/cape.png", graphicspack), true);
    gfx_loadimage(&spr_tail, convertPath("gfx/packs/eyecandy/tail.png", graphicspack), true);
    gfx_loadimage(&spr_wings, convertPath("gfx/packs/eyecandy/wings.png", graphicspack), true);

    gfx_loadimage(&spr_warplock, convertPath("gfx/packs/eyecandy/warplock.png", graphicspack), false);
    gfx_loadimage(&spr_coinsparkle, convertPath("gfx/packs/eyecandy/coinsparks.png", graphicspack), true);
    gfx_loadimage(&spr_shinesparkle, convertPath("gfx/packs/eyecandy/shinesparks.png", graphicspack), true);
    gfx_loadimage(&spr_shellbounce, convertPath("gfx/packs/eyecandy/shellbounce.png", graphicspack), true);
    gfx_loadimage(&spr_superstomp, convertPath("gfx/packs/eyecandy/supersmash.png", graphicspack), true);

    gfx_loadimage(&spr_egg, convertPath("gfx/packs/modeobjects/egg.png", graphicspack), true);
    gfx_loadimage(&spr_eggnumbers, convertPath("gfx/packs/modeobjects/eggnumbers.png", graphicspack), true);
    gfx_loadimage(&spr_star, convertPath("gfx/packs/modeobjects/star.png", graphicspack), true);
    gfx_loadimage(&spr_flags, convertPath("gfx/packs/modeobjects/flags.png", graphicspack), true);
    gfx_loadimage(&spr_frenzycards, convertPath("gfx/packs/modeobjects/frenzycards.png", graphicspack), true);
    gfx_loadimage(&spr_collectcards, convertPath("gfx/packs/modeobjects/collectcards.png", graphicspack), true);

    gfx_loadimage(&spr_yoshi, convertPath("gfx/packs/modeobjects/yoshi.png", graphicspack), true);
    gfx_loadimage(&spr_coin, convertPath("gfx/packs/modeobjects/coin.png", graphicspack), true);
    gfx_loadimage(&spr_thwomp, convertPath("gfx/packs/modeobjects/thwomp.png", graphicspack), true);
    gfx_loadimage(&spr_podobo, convertPath("gfx/packs/modeobjects/podobo.png", graphicspack), false);
    gfx_loadimage(&spr_bowserfire, convertPath("gfx/packs/modeobjects/bowserfire.png", graphicspack), false);
    gfx_loadimage(&spr_areas, convertPath("gfx/packs/modeobjects/areas.png", graphicspack), false);
    gfx_loadimage(&spr_kingofthehillarea, convertPath("gfx/packs/modeobjects/kingofthehill.png", graphicspack), 128, false, true);
    gfx_loadimage(&spr_jail, convertPath("gfx/packs/modeobjects/jail.png", graphicspack), 160, true, true);
    gfx_loadimage(&spr_goomba, convertPath("gfx/packs/modeobjects/goomba.png", graphicspack), true);
    gfx_loadimage(&spr_goombadead, convertPath("gfx/packs/eyecandy/goombadead.png", graphicspack), true);
    gfx_loadimage(&spr_goombadeadflying, convertPath("gfx/packs/eyecandy/goombadeadflying.png", graphicspack), true);
    gfx_loadimage(&spr_koopa, convertPath("gfx/packs/modeobjects/koopa.png", graphicspack), true);
    gfx_loadimage(&spr_buzzybeetle, convertPath("gfx/packs/modeobjects/buzzybeetle.png", graphicspack), true);
    gfx_loadimage(&spr_spiny, convertPath("gfx/packs/modeobjects/spiny.png", graphicspack), true);
    gfx_loadimage(&spr_paragoomba, convertPath("gfx/packs/modeobjects/paragoomba.png", graphicspack), true);
    gfx_loadimage(&spr_parakoopa, convertPath("gfx/packs/modeobjects/parakoopa.png", graphicspack), true);
    gfx_loadimage(&spr_redparakoopa, convertPath("gfx/packs/modeobjects/redparakoopa.png", graphicspack), true);
    gfx_loadimage(&spr_redkoopa, convertPath("gfx/packs/modeobjects/redkoopa.png", graphicspack), true);
    gfx_loadimage(&spr_cheepcheep, convertPath("gfx/packs/modeobjects/cheepcheep.png", graphicspack), true);
    gfx_loadimage(&spr_cheepcheepdead, convertPath("gfx/packs/eyecandy/cheepcheepdead.png", graphicspack), true);

    gfx_loadimage(&spr_sledgebrothers, convertPath("gfx/packs/modeobjects/sledgebrothers.png", graphicspack), true);
    gfx_loadimage(&spr_sledgebrothersdead, convertPath("gfx/packs/eyecandy/sledgebrothersdead.png", graphicspack), true);

    gfx_loadimage(&spr_bulletbill, convertPath("gfx/packs/projectiles/bulletbill.png", graphicspack), false);
    gfx_loadimage(&spr_bulletbilldead, convertPath("gfx/packs/eyecandy/bulletbilldead.png", graphicspack), false);
    gfx_loadimage(&spr_chicken, convertPath("gfx/packs/modeobjects/chicken.png", graphicspack), 160, true, true);
    gfx_loadimage(&spr_racegoal, convertPath("gfx/packs/modeobjects/racegoal.png", graphicspack), false);
    gfx_loadimage(&spr_pipegamebonus, convertPath("gfx/packs/modeobjects/pipeminigamebonuses.png", graphicspack), true);

    gfx_loadimage(&spr_phanto, convertPath("gfx/packs/modeobjects/phanto.png", graphicspack), true);
    gfx_loadimage(&spr_phantokey, convertPath("gfx/packs/modeobjects/key.png", graphicspack), true);

    gfx_loadimage(&spr_bonuschest, convertPath("gfx/packs/modeobjects/bonuschest.png", graphicspack), true);
    gfx_loadimage(&spr_teleportstar, convertPath("gfx/packs/eyecandy/teleportstar.png", graphicspack), false);

    gfx_loadimage(&spr_fireball, convertPath("gfx/packs/projectiles/fireball.png", graphicspack), true);
    gfx_loadimage(&spr_hammer, convertPath("gfx/packs/projectiles/hammer.png", graphicspack), true);
    gfx_loadimage(&spr_iceblast, convertPath("gfx/packs/projectiles/wandblast.png", graphicspack), true);
    gfx_loadimage(&spr_boomerang, convertPath("gfx/packs/projectiles/boomerang.png", graphicspack), true);
    gfx_loadimage(&spr_shell, convertPath("gfx/packs/projectiles/shell.png", graphicspack), true);
    gfx_loadimage(&spr_shelldead, convertPath("gfx/packs/eyecandy/shelldead.png", graphicspack), true);
    gfx_loadimage(&spr_blueblock, convertPath("gfx/packs/projectiles/throwblock.png", graphicspack), true);
    gfx_loadimage(&spr_bomb, convertPath("gfx/packs/projectiles/bomb.png", graphicspack), true);

    gfx_loadimage(&spr_superfireball, convertPath("gfx/packs/modeobjects/superfire.png", graphicspack), true);
    gfx_loadimage(&spr_sledgehammer, convertPath("gfx/packs/modeobjects/sledgehammer.png", graphicspack), true);

    gfx_loadimage(&spr_hazard_fireball[0], convertPath("gfx/packs/hazards/fireball.png", graphicspack), true);
    gfx_loadimage(&spr_hazard_fireball[1], convertPath("gfx/packs/hazards/fireball_preview.png", graphicspack), true);
    gfx_loadimage(&spr_hazard_fireball[2], convertPath("gfx/packs/hazards/fireball_thumbnail.png", graphicspack), true);

    gfx_loadimage(&spr_hazard_rotodisc[0], convertPath("gfx/packs/hazards/rotodisc.png", graphicspack), true);
    gfx_loadimage(&spr_hazard_rotodisc[1], convertPath("gfx/packs/hazards/rotodisc_preview.png", graphicspack), true);
    gfx_loadimage(&spr_hazard_rotodisc[2], convertPath("gfx/packs/hazards/rotodisc_thumbnail.png", graphicspack), true);

    gfx_loadimage(&spr_hazard_bulletbill[0], convertPath("gfx/packs/hazards/bulletbill.png", graphicspack), false);
    gfx_loadimage(&spr_hazard_bulletbill[1], convertPath("gfx/packs/hazards/bulletbill_preview.png", graphicspack), false);
    gfx_loadimage(&spr_hazard_bulletbill[2], convertPath("gfx/packs/hazards/bulletbill_thumbnail.png", graphicspack), false);

    gfx_loadimage(&spr_hazard_flame[0], convertPath("gfx/packs/hazards/flame.png", graphicspack), true);
    gfx_loadimage(&spr_hazard_flame[1], convertPath("gfx/packs/hazards/flame_preview.png", graphicspack), true);
    gfx_loadimage(&spr_hazard_flame[2], convertPath("gfx/packs/hazards/flame_thumbnail.png", graphicspack), true);

    gfx_loadimage(&spr_hazard_pirhanaplant[0], convertPath("gfx/packs/hazards/pirhanaplant.png", graphicspack), true);
    gfx_loadimage(&spr_hazard_pirhanaplant[1], convertPath("gfx/packs/hazards/pirhanaplant_preview.png", graphicspack), true);
    gfx_loadimage(&spr_hazard_pirhanaplant[2], convertPath("gfx/packs/hazards/pirhanaplant_thumbnail.png", graphicspack), true);

    gfx_loadimage(&spr_hazard_bulletbilldead, convertPath("gfx/packs/hazards/bulletbilldead.png", graphicspack), false);

    gfx_loadimage(&spr_fireballexplosion, convertPath("gfx/packs/eyecandy/fireballexplosion.png", graphicspack), 160, true, true);
    gfx_loadimage(&spr_frictionsmoke, convertPath("gfx/packs/eyecandy/frictionsmoke.png", graphicspack), 160, true, true);
    gfx_loadimage(&spr_bobombsmoke, convertPath("gfx/packs/eyecandy/bobombsmoke.png", graphicspack), 160, true, true);
    gfx_loadimage(&spr_explosion, convertPath("gfx/packs/eyecandy/explosion.png", graphicspack), true);
    gfx_loadimage(&spr_burnup, convertPath("gfx/packs/eyecandy/burnup.png", graphicspack), 192, true, true);
    gfx_loadimage(&spr_fireworks, convertPath("gfx/packs/eyecandy/fireworks.png", graphicspack), true);
    gfx_loadimage(&spr_poof, convertPath("gfx/packs/eyecandy/poof.png", graphicspack), true);

    gfx_loadimage(&spr_spawnsmoke, convertPath("gfx/packs/eyecandy/spawnsmoke.png", graphicspack), 128, true, true);
    gfx_loadimage(&spr_spawndoor, convertPath("gfx/packs/eyecandy/spawndoor.png", graphicspack), true);

    gfx_loadimage(&spr_bonus, convertPath("gfx/packs/eyecandy/bonus.png", graphicspack), true);
    gfx_loadimage(&spr_extralife, convertPath("gfx/packs/eyecandy/extralife.png", graphicspack), true);

    gfx_loadimage(&spr_windmeter, convertPath("gfx/packs/eyecandy/wind_meter.png", graphicspack), 192, true, true);
    gfx_loadimage(&spr_overlayhole, convertPath("gfx/packs/eyecandy/overlayholes.png", graphicspack), 0, 255, 0, true, true);

    gfx_loadimage(&spr_award, convertPath("gfx/packs/awards/killsinrow.png", graphicspack), 128, true, true);
    gfx_loadimage(&spr_awardsolid, convertPath("gfx/packs/awards/killsinrow.png", graphicspack), true);
    gfx_loadimage(&spr_awardsouls, convertPath("gfx/packs/awards/souls.png", graphicspack), true);
    gfx_loadimage(&spr_awardsoulspawn, convertPath("gfx/packs/awards/soulspawn.png", graphicspack), true);

    gfx_loadimage(&spr_awardkillsinrow, convertPath("gfx/packs/awards/killsinrownumbers.png", graphicspack), true);

    //gfx_loadteamcoloredimage(&spr_flagbases, convertPath("gfx/packs/modeobjects/flagbases.png", graphicspack), 255, 0, 255, 160, false, false);
    gfx_loadimage(&spr_flagbases, convertPath("gfx/packs/modeobjects/flagbases.png", graphicspack), 160, true, true);
    gfx_loadimage(&spr_ownedtags, convertPath("gfx/packs/modeobjects/ownedtags.png", graphicspack), 160, true, true);

    gfx_loadimage(&spr_storedpowerupsmall, convertPath("gfx/packs/powerups/small.png", graphicspack), true);
    gfx_loadimage(&spr_storedpoweruplarge, convertPath("gfx/packs/powerups/large.png", graphicspack), false);
    gfx_loadimage(&spr_powerupselector, convertPath("gfx/packs/awards/award.png", graphicspack), false);

    gfx_loadimage(&spr_abovearrows, convertPath("gfx/packs/eyecandy/abovearrows.png", graphicspack), true);
}

bool CResourceManager::LoadMenuGraphics()
{
    const char * graphicspack = menugraphicspacklist->current_name();

    gfx_loadimagenocolorkey(&menu_shade, convertPath("gfx/packs/menu/menu_shade.png", graphicspack));
    menu_shade.setalpha(smw->MenuTransparency);

    gfx_loadimage(&spr_scoreboard, convertPath("gfx/packs/menu/scoreboard.png", graphicspack), false);
    gfx_loadimage(&menu_slider_bar, convertPath("gfx/packs/menu/menu_slider_bar.png", graphicspack), false);
    gfx_loadimage(&menu_plain_field, convertPath("gfx/packs/menu/menu_plain_field.png", graphicspack), false);
    gfx_loadimage(&menu_player_select, convertPath("gfx/packs/menu/menu_player_select.png", graphicspack), false);
    gfx_loadimage(&menu_dialog, convertPath("gfx/packs/menu/menu_dialog.png", graphicspack), false);
    gfx_loadimage(&menu_map_filter, convertPath("gfx/packs/menu/menu_map_filter.png", graphicspack), false);
    gfx_loadimage(&menu_match_select, convertPath("gfx/packs/menu/menu_match_select.png", graphicspack), false);

    gfx_loadimage(&menu_verticalarrows, convertPath("gfx/packs/menu/menu_vertical_arrows.png", graphicspack), false);

    gfx_loadimage(&menu_mode_small, convertPath("gfx/packs/menu/menu_mode_small.png", graphicspack), false);
    gfx_loadimage(&menu_mode_large, convertPath("gfx/packs/menu/menu_mode_large.png", graphicspack), false);

    gfx_loadimage(&spr_dialog, convertPath("gfx/packs/menu/dialog.png", graphicspack), false);
    gfx_loadimage(&spr_dialogbutton, convertPath("gfx/packs/menu/dialog_button.png", graphicspack), false);
    gfx_loadimage(&spr_tournament_background, convertPath("gfx/packs/menu/tournament_background.png", graphicspack), false);
    gfx_loadimage(&spr_tournament_powerup_splash, convertPath("gfx/packs/menu/tournament_powerup_splash.png", graphicspack), false);
    gfx_loadimage(&spr_player_select_background, convertPath("gfx/packs/menu/player_select_background.png", graphicspack), false);
    gfx_loadimage(&spr_player_select_ready, convertPath("gfx/packs/menu/player_select_ready.png", graphicspack), false);
    //gfx_loadimage(&spr_ipfield, convertPath("gfx/packs/menu/menu_ipfield.png", graphicspack), false);
    gfx_loadimage(&spr_selectfield, convertPath("gfx/packs/menu/menu_selectfield.png", graphicspack), false);
    gfx_loadimage(&spr_selectfielddisabled, convertPath("gfx/packs/menu/menu_selectfield_disabled.png", graphicspack), false);
    gfx_loadimage(&spr_map_filter_icons, convertPath("gfx/packs/menu/menu_map_flags.png", graphicspack), false);
    gfx_loadimage(&spr_tour_markers, convertPath("gfx/packs/menu/tour_markers.png", graphicspack), false);
    gfx_loadimage(&spr_menu_boxed_numbers, convertPath("gfx/packs/menu/menu_boxed_numbers.png", graphicspack), false);
    gfx_loadimage(&spr_countdown_numbers, convertPath("gfx/packs/menu/game_countdown_numbers.png", graphicspack), false);
    gfx_loadimage(&spr_thumbnail_warps[0], convertPath("gfx/packs/menu/menu_warp_preview.png", graphicspack), false);
    gfx_loadimage(&spr_thumbnail_warps[1], convertPath("gfx/packs/menu/menu_warp_thumbnail.png", graphicspack), false);
    gfx_loadimage(&spr_thumbnail_mapitems[0], convertPath("gfx/packs/menu/menu_mapitems_preview.png", graphicspack), false);
    gfx_loadimage(&spr_thumbnail_mapitems[1], convertPath("gfx/packs/menu/menu_mapitems_thumbnail.png", graphicspack), false);

    gfx_loadimage(&spr_announcementicons, convertPath("gfx/packs/menu/menu_announcement_icons.png", graphicspack), false);

    gfx_loadimage(&spr_platformstarttile, convertPath("gfx/leveleditor/leveleditor_platformstarttile.png"), 64, true, true);
    gfx_loadimage(&spr_platformendtile, convertPath("gfx/leveleditor/leveleditor_selectedtile.png"), 64, true, true);
    gfx_loadimage(&spr_platformpath, convertPath("gfx/leveleditor/leveleditor_platform_path.png"), 128, true, true);

    return true;
}

bool CResourceManager::LoadWorldGraphics()
{
    const char * graphicspack = worldgraphicspacklist->current_name();

    gfx_loadimage(&spr_worldbackground[0], convertPath("gfx/packs/world/world_background.png", graphicspack), false, false);
    gfx_loadimage(&spr_worldbackground[1], convertPath("gfx/packs/world/preview/world_background.png", graphicspack), false, false);

    gfx_loadimage(&spr_worldforeground[0], convertPath("gfx/packs/world/world_foreground.png", graphicspack), false, false);
    gfx_loadimage(&spr_worldforeground[1], convertPath("gfx/packs/world/preview/world_foreground.png", graphicspack), false, false);

    gfx_loadimage(&spr_worldforegroundspecial[0], convertPath("gfx/packs/world/world_foreground_special.png", graphicspack), false, false);
    gfx_loadimage(&spr_worldforegroundspecial[1], convertPath("gfx/packs/world/preview/world_foreground_special.png", graphicspack), false, false);

    gfx_loadimage(&spr_worldpaths[0], convertPath("gfx/packs/world/world_paths.png", graphicspack), false, false);
    gfx_loadimage(&spr_worldpaths[1], convertPath("gfx/packs/world/preview/world_paths.png", graphicspack), false, false);

    gfx_loadimage(&spr_worldvehicle[0], convertPath("gfx/packs/world/world_vehicles.png", graphicspack), false);
    gfx_loadimage(&spr_worldvehicle[1], convertPath("gfx/packs/world/preview/world_vehicles.png", graphicspack), false);

    gfx_loadimage(&spr_worlditems, convertPath("gfx/packs/world/world_powerups.png", graphicspack), false);
    gfx_loadimage(&spr_worlditempopup, convertPath("gfx/packs/world/world_item_popup.png", graphicspack), false);
    gfx_loadimage(&spr_worlditemssmall, convertPath("gfx/packs/world/world_powerupssmall.png", graphicspack), false);
    gfx_loadimage(&spr_worlditemsplace, convertPath("gfx/packs/world/world_bonusplace.png", graphicspack), false);
    gfx_loadimage(&spr_worldbonushouse, convertPath("gfx/packs/world/world_bonushouse.png", graphicspack), false);

    return true;
}

bool CResourceManager::LoadGameGraphics()
{
    const char * graphicspack = gamegraphicspacklist->current_name();

    g_tilesetmanager->Init(graphicspack);

    bool loadok = true;
    loadok &= game_font_small.init(convertPath("gfx/packs/fonts/font_small.png", graphicspack));
    loadok &= game_font_large.init(convertPath("gfx/packs/fonts/font_large.png", graphicspack));

    if (!loadok) {
        _load_drawmsg("ERROR: error loading the fonts!\n");
        _load_waitforkey();
        return false;
    }

    LoadAllSprites();

    return true;
}

void CResourceManager::LoadAllGraphics()
{

    short k, j;
    for (k = 0; k < MAX_PLAYERS; k++) {
        spr_player[k] = new gfxSprite * [PGFX_LAST];
        spr_shyguy[k] = new gfxSprite * [PGFX_LAST];
        spr_chocobo[k] = new gfxSprite * [PGFX_LAST];
        spr_bobomb[k] = new gfxSprite * [PGFX_LAST];

        for (j = 0; j < PGFX_LAST; j++) {
            spr_player[k][j] = new gfxSprite();
            spr_shyguy[k][j] = new gfxSprite();
            spr_chocobo[k][j] = new gfxSprite();
            spr_bobomb[k][j] = new gfxSprite();

            spr_player[k][j]->SetWrap(true);
            spr_shyguy[k][j]->SetWrap(true);
            spr_chocobo[k][j]->SetWrap(true);
            spr_bobomb[k][j]->SetWrap(true);
        }
    }

    LoadMenuGraphics();
    LoadWorldGraphics();
    LoadGameGraphics();

    gfx_loadimagenocolorkey(&spr_backmap[0], convertPath("gfx/packs/backgrounds/Land_Classic.png", gamegraphicspacklist->current_name()));
    gfx_loadimagenocolorkey(&spr_backmap[1], convertPath("gfx/packs/backgrounds/Land_Classic.png", gamegraphicspacklist->current_name()));
    gfx_loadimagenocolorkey(&spr_frontmap[0], convertPath("gfx/packs/backgrounds/Land_Classic.png", gamegraphicspacklist->current_name()));
    gfx_loadimagenocolorkey(&spr_frontmap[1], convertPath("gfx/packs/backgrounds/Land_Classic.png", gamegraphicspacklist->current_name()));

    gfx_loadimage(&spr_overlay, convertPath("gfx/packs/menu/menu_shade.png", gamegraphicspacklist->current_name()), false, false);
}

bool CResourceManager::LoadGameSounds()
{
    game_values.soundcapable = false;

#if !defined(_XBOX) && !defined(__EMSCRIPTEN__)  //xbox and emscripten has sound capabilities
    int frequency, channels;
    Uint16 format;

    if (0 == Mix_QuerySpec(&frequency, &format, &channels))
        return false;
#endif

    const char * soundpack = soundpacklist->current_name();

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

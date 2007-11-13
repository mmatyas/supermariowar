#ifdef _XBOX
	#include <xtl.h>
#endif

#ifdef _WIN32
	#ifndef _XBOX
		#define WIN32_LEAN_AND_MEAN
		#include <windows.h>
	#endif
#endif

#include "global.h"
#include "time.h"
#include <string>
#include <iostream>
using std::cout;
using std::endl;
using std::string;

extern bool g_fLoadMessages;

extern bool gfx_createmenuskin(gfxSprite ** gSprites, const std::string& filename, Uint8 r, Uint8 g, Uint8 b, short colorScheme, bool fLoadBothDirections);
extern bool gfx_createfullskin(gfxSprite ** gSprites, const std::string& filename, Uint8 r, Uint8 g, Uint8 b, short colorScheme);

bool LoadMenuSkin(short playerID, short skinID, short colorID, bool fLoadBothDirections);
bool LoadFullSkin(gfxSprite ** sprites, const std::string&  filename, short colorID);

extern CEyecandyContainer eyecandyfront;
extern Uint8 GetScreenBackgroundFade();

//-----------------------------------------------------------------------------
// THE LOAD UP SEQUENCE + SPLASH SCREEN
//-----------------------------------------------------------------------------
//that's a bunch of ugly code, maybe i'll throw it out again

bool LoadStartGraphics()
{
	const char * graphicspack = menugraphicspacklist.current_name();

	bool loadok = true;

	loadok &= menu_font_small.init(convertPath("gfx/packs/menu/menu_font_small.png", graphicspack));
	loadok &= menu_font_large.init(convertPath("gfx/packs/menu/menu_font_large.png", graphicspack));
	
	if(!loadok)
	{
		_load_drawmsg("ERROR: error loading the fonts!\n");
		_load_waitforkey();
		return false;
	}

	//load basic stuff
	_load_gfx(menu_backdrop, convertPath("gfx/packs/menu/menu_background.png", graphicspack));
	_load_gfxck(menu_smw, convertPath("gfx/packs/menu/menu_smw.png", graphicspack));
	_load_gfxck(menu_version, convertPath("gfx/packs/menu/menu_version.png", graphicspack));

	return true;
}

bool LoadMenuGraphics()
{
	const char * graphicspack = menugraphicspacklist.current_name();
	_load_gfx(menu_shade, convertPath("gfx/packs/menu/menu_shade.png", graphicspack));
	menu_shade.setalpha(GetScreenBackgroundFade());

	_load_gfxck(spr_scoreboard, convertPath("gfx/packs/menu/scoreboard.png", graphicspack));
	_load_gfxck(menu_slider_bar, convertPath("gfx/packs/menu/menu_slider_bar.png", graphicspack));
	_load_gfxck(menu_plain_field, convertPath("gfx/packs/menu/menu_plain_field.png", graphicspack));
	_load_gfxck(menu_player_select, convertPath("gfx/packs/menu/menu_player_select.png", graphicspack));
	_load_gfxck(menu_dialog, convertPath("gfx/packs/menu/menu_dialog.png", graphicspack));
	_load_gfxck(menu_map_filter, convertPath("gfx/packs/menu/menu_map_filter.png", graphicspack));
	_load_gfxck(menu_match_select, convertPath("gfx/packs/menu/menu_match_select.png", graphicspack));

	_load_gfxck(menu_mode_small, convertPath("gfx/packs/menu/menu_mode_small.png", graphicspack));
	_load_gfxck(menu_mode_large, convertPath("gfx/packs/menu/menu_mode_large.png", graphicspack));

	_load_gfxck(spr_dialog, convertPath("gfx/packs/menu/dialog.png", graphicspack));
	_load_gfxck(spr_dialogbutton, convertPath("gfx/packs/menu/dialog_button.png", graphicspack));
	_load_gfxck(spr_tournament_background, convertPath("gfx/packs/menu/tournament_background.png", graphicspack));
	_load_gfxck(spr_tournament_powerup_splash, convertPath("gfx/packs/menu/tournament_powerup_splash.png", graphicspack));
	_load_gfxck(spr_player_select_background, convertPath("gfx/packs/menu/player_select_background.png", graphicspack));
	_load_gfxck(spr_player_select_ready, convertPath("gfx/packs/menu/player_select_ready.png", graphicspack));
	_load_gfxck(spr_ipfield, convertPath("gfx/packs/menu/menu_ipfield.png", graphicspack));
	_load_gfxck(spr_selectfield, convertPath("gfx/packs/menu/menu_selectfield.png", graphicspack));
	_load_gfxck(spr_selectfielddisabled, convertPath("gfx/packs/menu/menu_selectfield_disabled.png", graphicspack));
	_load_gfxck(spr_map_filter_icons, convertPath("gfx/packs/menu/menu_map_flags.png", graphicspack));
	_load_gfxck(spr_tour_markers, convertPath("gfx/packs/menu/tour_markers.png", graphicspack)); 
	_load_gfxck(spr_menu_boxed_numbers, convertPath("gfx/packs/menu/menu_boxed_numbers.png", graphicspack));
	_load_gfxa(spr_thumbnail_platformarrows, convertPath("gfx/packs/menu/menu_platform_arrows.png", graphicspack), 128);
	_load_gfxck(spr_thumbnail_warps[0], convertPath("gfx/packs/menu/menu_warp_preview.png", graphicspack));
	_load_gfxck(spr_thumbnail_warps[1], convertPath("gfx/packs/menu/menu_warp_thumbnail.png", graphicspack));
	_load_gfxck(spr_thumbnail_mapitems[0], convertPath("gfx/packs/menu/menu_mapitems_preview.png", graphicspack));
	_load_gfxck(spr_thumbnail_mapitems[1], convertPath("gfx/packs/menu/menu_mapitems_thumbnail.png", graphicspack));

	_load_gfxck(spr_worldbackground[0], convertPath("gfx/packs/world/world_background.png", graphicspack));
	_load_gfxck(spr_worldbackground[1], convertPath("gfx/packs/world/preview/world_background.png", graphicspack));

	_load_gfxck(spr_worldforeground[0], convertPath("gfx/packs/world/world_foreground.png", graphicspack));
	_load_gfxck(spr_worldforeground[1], convertPath("gfx/packs/world/preview/world_foreground.png", graphicspack));

	_load_gfxck(spr_worldforegroundspecial[0], convertPath("gfx/packs/world/world_foreground_special.png", graphicspack));
	_load_gfxck(spr_worldforegroundspecial[1], convertPath("gfx/packs/world/preview/world_foreground_special.png", graphicspack));

	_load_gfxck(spr_worldpaths[0], convertPath("gfx/packs/world/world_paths.png", graphicspack));
	_load_gfxck(spr_worldpaths[1], convertPath("gfx/packs/world/preview/world_paths.png", graphicspack));

	_load_gfxck(spr_worldvehicle[0], convertPath("gfx/packs/world/world_vehicles.png", graphicspack));
	_load_gfxck(spr_worldvehicle[1], convertPath("gfx/packs/world/preview/world_vehicles.png", graphicspack));

	_load_gfxck(spr_worlditems, convertPath("gfx/packs/world/world_powerups.png", graphicspack));
	_load_gfxck(spr_worlditempopup, convertPath("gfx/packs/world/world_item_popup.png", graphicspack));
	_load_gfxck(spr_worlditemssmall, convertPath("gfx/packs/world/world_powerupssmall.png", graphicspack));
	_load_gfxck(spr_worlditemsplace, convertPath("gfx/packs/world/world_bonusplace.png", graphicspack));
	_load_gfxck(spr_worldbonushouse, convertPath("gfx/packs/world/world_bonushouse.png", graphicspack));

	return true;
}

bool LoadGameGraphics()
{
	const char * graphicspack = gamegraphicspacklist.current_name();

	bool loadok = true;
	loadok &= game_font_small.init(convertPath("gfx/packs/fonts/font_small.png", graphicspack));
	loadok &= game_font_large.init(convertPath("gfx/packs/fonts/font_large.png", graphicspack));
	
	if(!loadok)
	{
		_load_drawmsg("ERROR: error loading the fonts!\n");
		_load_waitforkey();
		return false;
	}

	//Just load menu skins for now (just standing right sprite)
	for(short k = 0; k < 4; k++)
	{
		//LoadMenuSkin(k, game_values.skinids[k], game_values.colorids[k], false);
		LoadFullSkin(spr_chocobo[k], convertPath("gfx/packs/modeskins/chicken.bmp", graphicspack), k);
		LoadFullSkin(spr_bobomb[k], convertPath("gfx/packs/modeskins/bobomb.bmp", graphicspack), k);
	}

	_load_gfxck(menu_survival, convertPath("gfx/packs/modeobjects/menu_survival.png", graphicspack));
	_load_gfxck(menu_stomp, convertPath("gfx/packs/modeobjects/menu_stomp.png", graphicspack));

	_load_gfxa(spr_clouds[0], convertPath("gfx/packs/eyecandy/cloud1.png", graphicspack), 255);
	_load_gfxa(spr_clouds[1], convertPath("gfx/packs/eyecandy/cloud2.png", graphicspack), 255);
	spr_clouds[0].SetWrap(true);
	spr_clouds[1].SetWrap(true);

	_load_gfxa(spr_ghosts[0], convertPath("gfx/packs/eyecandy/ghost1.png", graphicspack), 128);
	_load_gfxa(spr_ghosts[1], convertPath("gfx/packs/eyecandy/ghost2.png", graphicspack), 128);
	_load_gfxa(spr_ghosts[2], convertPath("gfx/packs/eyecandy/ghost3.png", graphicspack), 128);
	spr_ghosts[0].SetWrap(true);
	spr_ghosts[1].SetWrap(true);
	spr_ghosts[2].SetWrap(true);

	_load_gfxck(spr_noteblock, convertPath("gfx/packs/blocks/noteblock.png", graphicspack));
	_load_gfxck(spr_breakableblock, convertPath("gfx/packs/blocks/breakableblock.png", graphicspack));
	_load_gfxck(spr_powerupblock, convertPath("gfx/packs/blocks/powerupblock.png", graphicspack));
	_load_gfxck(spr_donutblock, convertPath("gfx/packs/blocks/donutblock.png", graphicspack));
	_load_gfxck(spr_flipblock, convertPath("gfx/packs/blocks/flipblock.png", graphicspack));
	_load_gfxck(spr_bounceblock, convertPath("gfx/packs/blocks/bounceblock.png", graphicspack));
	_load_gfxck(spr_throwblock, convertPath("gfx/packs/blocks/throwblock.png", graphicspack));
	_load_gfxck(spr_switchblocks, convertPath("gfx/packs/blocks/switchblock.png", graphicspack));
	_load_gfxck(spr_viewblock, convertPath("gfx/packs/blocks/viewblock.png", graphicspack));
	
	_load_gfxck(spr_spring, convertPath("gfx/packs/powerups/spring.png", graphicspack));
	_load_gfxck(spr_spike, convertPath("gfx/packs/powerups/spike.png", graphicspack));
	_load_gfxck(spr_kuriboshoe, convertPath("gfx/packs/powerups/kuriboshoe.png", graphicspack));
	spr_spring.SetWrap(true);
	spr_spike.SetWrap(true);
	spr_kuriboshoe.SetWrap(true);

	_load_gfxck(spr_tileanimation[0], convertPath("gfx/packs/eyecandy/tile_animation.png", graphicspack));
	_load_gfxck(spr_tileanimation[1], convertPath("gfx/packs/eyecandy/tile_animation_preview.png", graphicspack));
	_load_gfxck(spr_tileanimation[2], convertPath("gfx/packs/eyecandy/tile_animation_thumbnail.png", graphicspack));

	_load_gfxck(spr_blocks[0], convertPath("gfx/packs/blocks.png", graphicspack));
	_load_gfxck(spr_blocks[1], convertPath("gfx/packs/blocks_preview.png", graphicspack));
	_load_gfxck(spr_blocks[2], convertPath("gfx/packs/blocks_thumbnail.png", graphicspack));
	
	_load_gfxck(spr_unknowntile[0], convertPath("gfx/packs/unknown_tile.png", graphicspack));
	_load_gfxck(spr_unknowntile[1], convertPath("gfx/packs/unknown_tile_preview.png", graphicspack));
	_load_gfxck(spr_unknowntile[2], convertPath("gfx/packs/unknown_tile_thumbnail.png", graphicspack));
	
	_load_gfxck(spr_brokenyellowblock, convertPath("gfx/packs/eyecandy/brokenyellowblock.png", graphicspack));
	_load_gfxck(spr_brokenflipblock, convertPath("gfx/packs/eyecandy/brokenflipblock.png", graphicspack));
	_load_gfxck(spr_brokenblueblock, convertPath("gfx/packs/eyecandy/brokenblueblock.png", graphicspack));
	spr_brokenyellowblock.SetWrap(true);
	spr_brokenflipblock.SetWrap(true);
	spr_brokenblueblock.SetWrap(true);

	_load_gfxck(spr_tanooki, convertPath("gfx/packs/powerups/tanooki.png", graphicspack));
	_load_gfxck(spr_statue, convertPath("gfx/packs/projectiles/statue.png", graphicspack));
	_load_gfxck(spr_starpowerup, convertPath("gfx/packs/powerups/starpowerup.png", graphicspack));
	_load_gfxck(spr_1uppowerup, convertPath("gfx/packs/powerups/1uppowerup.png", graphicspack));
	_load_gfxck(spr_2uppowerup, convertPath("gfx/packs/powerups/2uppowerup.png", graphicspack));
	_load_gfxck(spr_3uppowerup, convertPath("gfx/packs/powerups/3uppowerup.png", graphicspack));
	_load_gfxck(spr_5uppowerup, convertPath("gfx/packs/powerups/5uppowerup.png", graphicspack));
	_load_gfxck(spr_firepowerup, convertPath("gfx/packs/powerups/fireflower.png", graphicspack));
	_load_gfxck(spr_hammerpowerup, convertPath("gfx/packs/powerups/hammerpowerup.png", graphicspack));
	_load_gfxck(spr_sledgehammerpowerup, convertPath("gfx/packs/powerups/sledgehammerpowerup.png", graphicspack));
	_load_gfxck(spr_podobopowerup, convertPath("gfx/packs/powerups/podobopowerup.png", graphicspack));
	_load_gfxck(spr_poisonpowerup, convertPath("gfx/packs/powerups/poisonpowerup.png", graphicspack));
	_load_gfxck(spr_mysterymushroompowerup, convertPath("gfx/packs/powerups/mysterymushroom.png", graphicspack));
	_load_gfxck(spr_boomerangpowerup, convertPath("gfx/packs/powerups/boomerangpowerup.png", graphicspack));
	_load_gfxck(spr_clockpowerup, convertPath("gfx/packs/powerups/clockpowerup.png", graphicspack));
	_load_gfxck(spr_bobombpowerup, convertPath("gfx/packs/powerups/bobombpowerup.png", graphicspack));
	_load_gfxck(spr_powpowerup, convertPath("gfx/packs/powerups/powpowerup.png", graphicspack));
	_load_gfxck(spr_modpowerup, convertPath("gfx/packs/powerups/modpowerup.png", graphicspack));
	_load_gfxck(spr_bulletbillpowerup, convertPath("gfx/packs/powerups/bulletbillpowerup.png", graphicspack));
	_load_gfxck(spr_featherpowerup, convertPath("gfx/packs/powerups/featherpowerup.png", graphicspack));
	_load_gfxck(spr_leafpowerup, convertPath("gfx/packs/powerups/leafpowerup.png", graphicspack));
	_load_gfxck(spr_bombpowerup, convertPath("gfx/packs/powerups/bombpowerup.png", graphicspack));
	_load_gfxck(spr_pwingspowerup, convertPath("gfx/packs/powerups/pwings.png", graphicspack));
	spr_tanooki.SetWrap(true);
	spr_statue.SetWrap(true);
	spr_starpowerup.SetWrap(true);
	spr_1uppowerup.SetWrap(true);
	spr_2uppowerup.SetWrap(true);
	spr_3uppowerup.SetWrap(true);
	spr_5uppowerup.SetWrap(true);
	spr_firepowerup.SetWrap(true);
	spr_hammerpowerup.SetWrap(true);
	spr_sledgehammerpowerup.SetWrap(true);
	spr_podobopowerup.SetWrap(true);
	spr_poisonpowerup.SetWrap(true);
	spr_clockpowerup.SetWrap(true);
	spr_bobombpowerup.SetWrap(true);
	spr_powpowerup.SetWrap(true);
	spr_modpowerup.SetWrap(true);
	spr_bulletbillpowerup.SetWrap(true);
	spr_featherpowerup.SetWrap(true);
	spr_leafpowerup.SetWrap(true);
	spr_mysterymushroompowerup.SetWrap(true);
	spr_boomerangpowerup.SetWrap(true);
	spr_pwingspowerup.SetWrap(true);

	_load_gfxa(spr_shade[0], convertPath("gfx/packs/eyecandy/shade1.png", graphicspack), 64);
	_load_gfxa(spr_shade[1], convertPath("gfx/packs/eyecandy/shade2.png", graphicspack), 64);
	_load_gfxa(spr_shade[2], convertPath("gfx/packs/eyecandy/shade3.png", graphicspack), 64);
	_load_gfxa(spr_timershade, convertPath("gfx/packs/eyecandy/timershade.png", graphicspack), 64);
	_load_gfxck(spr_spawneggs, convertPath("gfx/packs/eyecandy/spawneggs.png", graphicspack));
	_load_gfxck(spr_scoretext, convertPath("gfx/packs/fonts/score.png", graphicspack));
	_load_gfxck(spr_racetext, convertPath("gfx/packs/fonts/race.png", graphicspack));

	_load_gfxck(spr_crown, convertPath("gfx/packs/eyecandy/crown.png", graphicspack));
	_load_gfxck(spr_cape, convertPath("gfx/packs/eyecandy/cape.png", graphicspack));
	_load_gfxck(spr_tail, convertPath("gfx/packs/eyecandy/tail.png", graphicspack));
	_load_gfxck(spr_warplock, convertPath("gfx/packs/eyecandy/warplock.png", graphicspack));
	_load_gfxck(spr_coinsparkle, convertPath("gfx/packs/eyecandy/coinsparks.png", graphicspack));
	_load_gfxck(spr_shinesparkle, convertPath("gfx/packs/eyecandy/shinesparks.png", graphicspack));
	_load_gfxck(spr_shellbounce, convertPath("gfx/packs/eyecandy/shellbounce.png", graphicspack));
	_load_gfxck(spr_superstomp, convertPath("gfx/packs/eyecandy/supersmash.png", graphicspack));
	spr_crown.SetWrap(true); // Wrap crown for players wearing crown during game
	spr_cape.SetWrap(true);
	spr_tail.SetWrap(true);
	spr_shellbounce.SetWrap(true);

	_load_gfxck(spr_egg, convertPath("gfx/packs/modeobjects/egg.png", graphicspack));
	_load_gfxck(spr_star, convertPath("gfx/packs/modeobjects/star.png", graphicspack));
	_load_gfxck(spr_flags, convertPath("gfx/packs/modeobjects/flags.png", graphicspack));
	_load_gfxck(spr_frenzycards, convertPath("gfx/packs/modeobjects/frenzycards.png", graphicspack));
	spr_egg.SetWrap(true);
	spr_star.SetWrap(true);
	spr_flags.SetWrap(true);
	spr_frenzycards.SetWrap(true);
	
	_load_gfxck(spr_yoshi, convertPath("gfx/packs/modeobjects/yoshi.png", graphicspack));
	_load_gfxck(spr_coin, convertPath("gfx/packs/modeobjects/coin.png", graphicspack));
	_load_gfxck(spr_thwomp, convertPath("gfx/packs/modeobjects/thwomp.png", graphicspack));
	_load_gfxck(spr_podobo, convertPath("gfx/packs/modeobjects/podobo.png", graphicspack));
	_load_gfxck(spr_bowserfire, convertPath("gfx/packs/modeobjects/bowserfire.png", graphicspack));
	_load_gfxck(spr_areas, convertPath("gfx/packs/modeobjects/areas.png", graphicspack));
	_load_gfxa(spr_kingofthehillarea, convertPath("gfx/packs/modeobjects/kingofthehill.png", graphicspack), 128);
	_load_gfxa(spr_jail, convertPath("gfx/packs/modeobjects/jail.png", graphicspack), 160);
	_load_gfxck(spr_goomba, convertPath("gfx/packs/modeobjects/goomba.png", graphicspack));
	_load_gfxck(spr_goombadead, convertPath("gfx/packs/eyecandy/goombadead.png", graphicspack));
	_load_gfxck(spr_goombadeadflying, convertPath("gfx/packs/eyecandy/goombadeadflying.png", graphicspack));
	_load_gfxck(spr_koopa, convertPath("gfx/packs/modeobjects/koopa.png", graphicspack));
	_load_gfxck(spr_sledgebrothers, convertPath("gfx/packs/modeobjects/sledgebrothers.png", graphicspack));
	_load_gfxck(spr_sledgebrothersdead, convertPath("gfx/packs/eyecandy/sledgebrothersdead.png", graphicspack));
	_load_gfxck(spr_redkoopa, convertPath("gfx/packs/modeobjects/redkoopa.png", graphicspack));
	_load_gfxck(spr_cheepcheep, convertPath("gfx/packs/modeobjects/cheepcheep.png", graphicspack));
	_load_gfxck(spr_cheepcheepdead, convertPath("gfx/packs/eyecandy/cheepcheepdead.png", graphicspack));
	_load_gfxck(spr_bulletbill, convertPath("gfx/packs/projectiles/bulletbill.png", graphicspack));
	_load_gfxck(spr_bulletbilldead, convertPath("gfx/packs/eyecandy/bulletbilldead.png", graphicspack));
	_load_gfxa(spr_chicken, convertPath("gfx/packs/modeobjects/chicken.png", graphicspack), 160);
	_load_gfxck(spr_racegoal, convertPath("gfx/packs/modeobjects/racegoal.png", graphicspack));

	_load_gfxck(spr_bonuschest, convertPath("gfx/packs/modeobjects/bonuschest.png", graphicspack));
	_load_gfxck(spr_teleportstar, convertPath("gfx/packs/eyecandy/teleportstar.png", graphicspack));
	spr_bonuschest.SetWrap(true);

	spr_yoshi.SetWrap(true);
	spr_coin.SetWrap(true);
	spr_thwomp.SetWrap(true);
	spr_jail.SetWrap(true);
	spr_goomba.SetWrap(true);
	spr_goombadead.SetWrap(true);
	spr_goombadeadflying.SetWrap(true);
	spr_koopa.SetWrap(true);
	spr_redkoopa.SetWrap(true);
	spr_sledgebrothers.SetWrap(true);
	spr_sledgebrothersdead.SetWrap(true);
	spr_cheepcheep.SetWrap(true);
	spr_cheepcheepdead.SetWrap(true);
	spr_chicken.SetWrap(true);
	
	_load_gfxck(spr_fireball, convertPath("gfx/packs/projectiles/fireball.png", graphicspack));
	_load_gfxck(spr_superfireball, convertPath("gfx/packs/projectiles/superfire.png", graphicspack));
	_load_gfxck(spr_hammer, convertPath("gfx/packs/projectiles/hammer.png", graphicspack));
	_load_gfxck(spr_sledgehammer, convertPath("gfx/packs/projectiles/sledgehammer.png", graphicspack));
	_load_gfxck(spr_boomerang, convertPath("gfx/packs/projectiles/boomerang.png", graphicspack));
	_load_gfxck(spr_shell, convertPath("gfx/packs/projectiles/shell.png", graphicspack));
	_load_gfxck(spr_shelldead, convertPath("gfx/packs/eyecandy/shelldead.png", graphicspack));
	_load_gfxck(spr_blueblock, convertPath("gfx/packs/projectiles/throwblock.png", graphicspack));
	_load_gfxck(spr_bomb, convertPath("gfx/packs/projectiles/bomb.png", graphicspack));
	spr_fireball.SetWrap(true);
	spr_superfireball.SetWrap(true);
	spr_hammer.SetWrap(true);
	spr_sledgehammer.SetWrap(true);
	spr_boomerang.SetWrap(true);
	spr_shell.SetWrap(true);
	spr_shelldead.SetWrap(true);
	spr_blueblock.SetWrap(true);
	spr_bomb.SetWrap(true);
	
	_load_gfxa(spr_fireballexplosion, convertPath("gfx/packs/eyecandy/fireballexplosion.png", graphicspack), 160);
	_load_gfxa(spr_frictionsmoke, convertPath("gfx/packs/eyecandy/frictionsmoke.png", graphicspack), 160);
	_load_gfxa(spr_bobombsmoke, convertPath("gfx/packs/eyecandy/bobombsmoke.png", graphicspack), 160);
	_load_gfxck(spr_explosion, convertPath("gfx/packs/eyecandy/explosion.png", graphicspack));
	_load_gfxa(spr_burnup, convertPath("gfx/packs/eyecandy/burnup.png", graphicspack), 192);

	spr_fireballexplosion.SetWrap(true);
	spr_frictionsmoke.SetWrap(true);
	spr_bobombsmoke.SetWrap(true);
	spr_explosion.SetWrap(true);
	spr_burnup.SetWrap(true);

	_load_gfxa(spr_spawnsmoke[0], convertPath("gfx/packs/eyecandy/spawnsmoke1.png", graphicspack), 128);
	_load_gfxa(spr_spawnsmoke[1], convertPath("gfx/packs/eyecandy/spawnsmoke2.png", graphicspack), 128);
	_load_gfxa(spr_spawnsmoke[2], convertPath("gfx/packs/eyecandy/spawnsmoke3.png", graphicspack), 128);
	_load_gfxa(spr_spawnsmoke[3], convertPath("gfx/packs/eyecandy/spawnsmoke4.png", graphicspack), 128);

	spr_spawnsmoke[0].SetWrap(true);
	spr_spawnsmoke[1].SetWrap(true);
	spr_spawnsmoke[2].SetWrap(true);
	spr_spawnsmoke[3].SetWrap(true);

	_load_gfxck(spr_spawndoor[0], convertPath("gfx/packs/eyecandy/spawndoor1.png", graphicspack));
	_load_gfxck(spr_spawndoor[1], convertPath("gfx/packs/eyecandy/spawndoor2.png", graphicspack));
	_load_gfxck(spr_spawndoor[2], convertPath("gfx/packs/eyecandy/spawndoor3.png", graphicspack));
	_load_gfxck(spr_spawndoor[3], convertPath("gfx/packs/eyecandy/spawndoor4.png", graphicspack));

	spr_spawndoor[0].SetWrap(true);
	spr_spawndoor[1].SetWrap(true);
	spr_spawndoor[2].SetWrap(true);
	spr_spawndoor[3].SetWrap(true);

	_load_gfxck(spr_bonus, convertPath("gfx/packs/eyecandy/bonus.png", graphicspack));
	_load_gfxck(spr_extralife, convertPath("gfx/packs/eyecandy/extralife.png", graphicspack));
	spr_bonus.SetWrap(true);
	spr_extralife.SetWrap(true);

	_load_gfxa(spr_award, convertPath("gfx/packs/awards/killsinrow.png", graphicspack), 128);
	_load_gfxck(spr_awardsolid, convertPath("gfx/packs/awards/killsinrow.png", graphicspack));
	_load_gfxck(spr_awardsouls, convertPath("gfx/packs/awards/souls.png", graphicspack));
	_load_gfxck(spr_awardsoulspawn, convertPath("gfx/packs/awards/soulspawn.png", graphicspack));

	spr_award.SetWrap(true);
	spr_awardsolid.SetWrap(true);
	spr_awardsouls.SetWrap(true);
	spr_awardsoulspawn.SetWrap(true);

	_load_gfxck(spr_awardkillsinrow[0], convertPath("gfx/packs/awards/killsinrownumbers1.png", graphicspack));
	_load_gfxck(spr_awardkillsinrow[1], convertPath("gfx/packs/awards/killsinrownumbers2.png", graphicspack));
	_load_gfxck(spr_awardkillsinrow[2], convertPath("gfx/packs/awards/killsinrownumbers3.png", graphicspack));
	_load_gfxck(spr_awardkillsinrow[3], convertPath("gfx/packs/awards/killsinrownumbers4.png", graphicspack));

	spr_awardkillsinrow[0].SetWrap(true);
	spr_awardkillsinrow[1].SetWrap(true);
	spr_awardkillsinrow[2].SetWrap(true);
	spr_awardkillsinrow[3].SetWrap(true);

	_load_gfxa(spr_flagbases, convertPath("gfx/packs/modeobjects/flagbases.png", graphicspack), 160);
	_load_gfxa(spr_ownedtags, convertPath("gfx/packs/modeobjects/ownedtags.png", graphicspack), 160);
	spr_flagbases.SetWrap(true);
	spr_ownedtags.SetWrap(true);

	_load_gfxck(spr_storedpowerupsmall, convertPath("gfx/packs/powerups/small.png", graphicspack));
	_load_gfxck(spr_storedpoweruplarge, convertPath("gfx/packs/powerups/large.png", graphicspack));
	_load_gfxck(spr_powerupselector, convertPath("gfx/packs/awards/award.png", graphicspack));
	spr_storedpowerupsmall.SetWrap(true);

	_load_gfxck(spr_abovearrows, convertPath("gfx/packs/eyecandy/abovearrows.png", graphicspack));
	spr_abovearrows.SetWrap(true);

	return true;
}

bool LoadGameSounds()
{
	game_values.soundcapable = false;

#ifndef _XBOX  //xbox has sound capabilities
	int frequency, channels;
	Uint16 format;
	
	if(0 == Mix_QuerySpec(&frequency, &format, &channels))
		return false;
#endif

	const char * soundpack = soundpacklist.current_name();

	_load_sfx(sfx_mip, convertPath("sfx/packs/mip.wav", soundpack));
	_load_sfx(sfx_deathsound, convertPath("sfx/packs/death.wav", soundpack));
	_load_sfx(sfx_jump, convertPath("sfx/packs/jump.wav", soundpack));
	_load_sfx(sfx_skid, convertPath("sfx/packs/skid.wav", soundpack));
	_load_sfx(sfx_capejump, convertPath("sfx/packs/capejump.wav", soundpack));
	_load_sfx(sfx_invinciblemusic, convertPath("sfx/packs/invincible.wav", soundpack));
	_load_sfx(sfx_extraguysound, convertPath("sfx/packs/1up.wav", soundpack));
	_load_sfx(sfx_sprout, convertPath("sfx/packs/sprout.wav", soundpack));
	_load_sfx(sfx_collectpowerup, convertPath("sfx/packs/collectpowerup.wav", soundpack));
	_load_sfx(sfx_collectfeather, convertPath("sfx/packs/feather.wav", soundpack));
	_load_sfx(sfx_tailspin, convertPath("sfx/packs/tail.wav", soundpack));
	_load_sfx(sfx_storepowerup, convertPath("sfx/packs/storeitem.wav", soundpack));
	_load_sfx(sfx_breakblock, convertPath("sfx/packs/breakblock.wav", soundpack));
	_load_sfx(sfx_bump, convertPath("sfx/packs/bump.wav", soundpack));
	_load_sfx(sfx_coin, convertPath("sfx/packs/coin.wav", soundpack));
	_load_sfx(sfx_fireball, convertPath("sfx/packs/fireball.wav", soundpack));
	_load_sfx(sfx_springjump, convertPath("sfx/packs/springjump.wav", soundpack));
	_load_sfx(sfx_timewarning, convertPath("sfx/packs/timewarning.wav", soundpack));
	_load_sfx(sfx_hit, convertPath("sfx/packs/hit.wav", soundpack));
	_load_sfx(sfx_chicken, convertPath("sfx/packs/chicken.wav", soundpack));
	_load_sfx(sfx_transform, convertPath("sfx/packs/transform.wav", soundpack));
	_load_sfx(sfx_yoshi, convertPath("sfx/packs/yoshi.wav", soundpack));
	_load_sfx(sfx_pause, convertPath("sfx/packs/pause.wav", soundpack));
	_load_sfx(sfx_bobombsound, convertPath("sfx/packs/bob-omb.wav", soundpack));
	_load_sfx(sfx_areatag, convertPath("sfx/packs/dcoin.wav", soundpack));
	_load_sfx(sfx_cannon, convertPath("sfx/packs/cannon.wav", soundpack));
	_load_sfx(sfx_burnup, convertPath("sfx/packs/burnup.wav", soundpack));
	_load_sfx(sfx_pipe, convertPath("sfx/packs/warp.wav", soundpack));
	_load_sfx(sfx_thunder, convertPath("sfx/packs/thunder.wav", soundpack));
	_load_sfx(sfx_slowdownmusic, convertPath("sfx/packs/clock.wav", soundpack));
	_load_sfx(sfx_flyingsound, convertPath("sfx/packs/slowdown.wav", soundpack));
	_load_sfx(sfx_storedpowerupsound, convertPath("sfx/packs/storedpowerup.wav", soundpack));
	_load_sfx(sfx_kicksound, convertPath("sfx/packs/kick.wav", soundpack));
	_load_sfx(sfx_racesound, convertPath("sfx/packs/race.wav", soundpack));
	_load_sfx(sfx_bulletbillsound, convertPath("sfx/packs/bulletbill.wav", soundpack));
	_load_sfx(sfx_boomerang, convertPath("sfx/packs/boomerang.wav", soundpack));
	_load_sfx(sfx_spit, convertPath("sfx/packs/spit.wav", soundpack));
	_load_sfx(sfx_starwarning, convertPath("sfx/packs/starwarning.wav", soundpack));
	_load_sfx(sfx_powerdown, convertPath("sfx/packs/powerdown.wav", soundpack));
	_load_sfx(sfx_switchpress, convertPath("sfx/packs/switchpress.wav", soundpack));
	_load_sfx(sfx_superspring, convertPath("sfx/packs/superspring.wav", soundpack));
	_load_sfx(sfx_gameover, convertPath("sfx/packs/gameover.wav", soundpack));
	_load_sfx(sfx_stun, convertPath("sfx/packs/stun.wav", soundpack));
	_load_sfx(sfx_inventory, convertPath("sfx/packs/inventory.wav", soundpack));
	_load_sfx(sfx_worldmove, convertPath("sfx/packs/mapmove.wav", soundpack));
	_load_sfx(sfx_treasurechest, convertPath("sfx/packs/treasurechest.wav", soundpack));

	game_values.soundcapable = true;
	return true;
}

bool LoadAndSplashScreen()
{
	LoadStartGraphics();

	gfxSprite menu_dpi_logo;
	_load_gfx(menu_dpi_logo, convertPath("gfx/packs/menu/splash_72dpi.png", menugraphicspacklist.current_name()));

	gfxSprite menu_contest_winners;
	_load_gfx(menu_contest_winners, convertPath("gfx/packs/menu/splash_contest_winners.png", menugraphicspacklist.current_name()));

	gfxSprite menu_credits;
	_load_gfxck(menu_credits, convertPath("gfx/packs/menu/splash_credits.png", menugraphicspacklist.current_name()));

	int alpha = 0;
	int state = 0;
	int timer = 120;

	char * contributors[] = {
	"no_shorty", "redfalcon", "no_human", "dschingis", "funvill",
	"matsche", "aeroflare", "Tymoe", "David Olofson", "scoti",
	"affeOHNEwaffe", "mademan", "mario piuk", "yvoo", "DNightmare", 
	"Armen", "vvill", "zio tiok", "Donny Viszneki","alex-rus07", "JacobTheHero",
	"Stephan Peijnik", "ventuz", "Stefan Schury", "riahc3",
	"jinkies.uk", "devguy", "Alexis Morrissette", "Bidbood", "Quinn Storm",
	"Flexserve", "Xijar", "j rok", "nitsuja", "4matsy", "leftyfb",
	"Brian Porter", "Brandon Stansbury", "Brian Smith", "Caesar Cypher",
	"Chris Schager", "Alex Arnot", "Jason Lumbert", "Kevin King",
	"Fernando Marquez", "DrJones", "SleepyP", "UltimateNinja9",
	"Alex Brown", "GKi", "mpinger", "neonater", "NMcCoy", "Rogultgot", "Arima",
	"Toadeeboy", "Crapcom", "Tonberry2k", "Jum2004", "Bacon", "Tetra Vega",
	"Young Link", "Blordow", "Gozinzolo", "Ragey", "SaturnEchidna", "Link901",
	"Techokami", "dolorous", "SMW Fan", "Timonator", "YoshiMonarch7", "Tiptup300",
	"Dude", "Justinio", "Matthew Callis", "VenomousNinja",
	"Discrosh", "Ikill4you", "Helix Snake", "tubesteak", "wasabimario", "Lee",
	"Pikawil", "Marioman64", "Peardian", "Bob Ippolito", "Viper Snake", "neavea",
	"Mr.Bob-omb", "milua", "bobmanperson", "DrTek", "somestrangeflea", "nes6502",
	"XPort", "Naphistim", "Chaos", "NiGHTS", "Kutter"};

#define NUM_CONTRIBUTORS ((int)(sizeof(contributors)/sizeof(char*)))

	bool contributorUsed[NUM_CONTRIBUTORS];
	int contributorOrder[NUM_CONTRIBUTORS];

	for(int k = 0; k < NUM_CONTRIBUTORS; k++)
	{
		contributorUsed[k] = false;
	}

	for(int k = 0; k < NUM_CONTRIBUTORS; k++)
	{
		int index = rand() % NUM_CONTRIBUTORS;

		while(contributorUsed[index])
		{
			if(++index >= NUM_CONTRIBUTORS)
				index = 0;
		}

		contributorUsed[index] = true;
		contributorOrder[k] = index;
	}


	SDL_Event event;

	while (true)
	{
		int framestart = SDL_GetTicks();

		game_values.playerInput.ClearPressedKeys(1);

		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{

#ifndef _XBOX
				case SDL_QUIT:
				{
					return false;
				}
				break;
#endif
				case SDL_KEYDOWN:
				{
					switch(event.key.keysym.sym)
					{
						case SDLK_RETURN:
							if(event.key.keysym.mod & (KMOD_LALT | KMOD_RALT))
							{
#ifndef _XBOX
								game_values.fullscreen = !game_values.fullscreen;
								gfx_setresolution(640, 480, game_values.fullscreen);
								blitdest = screen;
#endif
							}
							break;

#ifndef _XBOX
						case SDLK_F4:
							if(event.key.keysym.mod & (KMOD_LALT | KMOD_RALT))
								return false;
							break;
#endif

						default:
							break;
					}
					break;
				}

			default:
				break;
			}

			game_values.playerInput.Update(event, 1);
		}

		for(int iPlayer = 0; iPlayer < 4; iPlayer++)
		{
			if(game_values.playerInput.outputControls[iPlayer].menu_select.fPressed || 
				game_values.playerInput.outputControls[iPlayer].menu_cancel.fPressed ||
				game_values.playerInput.outputControls[iPlayer].menu_random.fPressed)
			{
				if(state <= 6)
				{
					state = 6;
					alpha = 255;
				}
				else
				{
					blitdest = menu_backdrop.getSurface();
					menu_shade.setalpha(GetScreenBackgroundFade());
					menu_shade.draw(0, 0);
					blitdest = screen;

					g_fLoadMessages = false;
					eyecandyfront.clean();

					game_values.playerInput.ResetKeys();

					return true;
				}
			}
		}

		if(state == 0 || state == 3)
		{
			alpha += 4;
			if(alpha >= 255)
			{
				alpha = 255;
				state++;
			}
		}
		else if(state == 1 || state == 4)
		{
			if(--timer <= 0)
			{
				timer = 120;
				state++;
			}
		}
		else if(state == 2 || state == 5)
		{
			alpha -= 4;
			if(alpha <= 0)
			{
				alpha = 0;
				state++;
			}
		}
		else if(state == 6)
		{
			alpha += 5;
			if(alpha >= 255)
			{
				alpha = 255;
				state++;
			}
		}

		SDL_FillRect(screen, NULL, 0x0);

		if(state == 0 || state == 1 || state == 2)
		{
			menu_dpi_logo.setalpha((Uint8)alpha);
			menu_dpi_logo.draw(195, 186);
		}
		else if(state == 3 || state == 4 || state == 5)
		{
			menu_contest_winners.setalpha((Uint8)alpha);
			menu_contest_winners.draw(0, 0);
		}
		else if(state == 6 || state == 7 || state == 8)
		{
			menu_backdrop.setalpha((Uint8)alpha);
			menu_backdrop.draw(0, 0);

			menu_smw.setalpha((Uint8)alpha);
			menu_smw.draw(320 - ((short)menu_smw.getWidth() >> 1), 30);	//smw logo

			menu_version.setalpha((Uint8)alpha);
			menu_version.draw(570, 10);	//smw logo

			menu_font_large.setalpha((Uint8)alpha);
			menu_font_large.drawRightJustified(630, 45, "Alpha");

			menu_credits.setalpha((Uint8)alpha);
			menu_credits.draw(227, 200);
		}

		if(state == 7)
		{
			_load_drawmsg("Loading...");
			menu_font_large.drawCentered(320, 420, "Loading...");
		}
		else if(state == 8)
		{
			_load_drawmsg("Press Any Key To Continue");

			eyecandyfront.cleandeadobjects();
			eyecandyfront.update();
			eyecandyfront.draw();

			static int timer = 60;
			static int index = 0;
			if(++timer >= 60)
			{
				eyecandyfront.add(new EC_GravText(&menu_font_large, 320, 480, contributors[contributorOrder[index]], -8.2f));
				timer = 0;

				if(++index >= NUM_CONTRIBUTORS)
					index = 0;
			}
		}

		SDL_Flip(screen);

		if(state == 7)
		{
			backgroundmusic[2].load(musiclist.GetMusic(1));

			short k, j;
			for(k = 0; k < 4; k++)
			{
				spr_player[k] = new gfxSprite * [PGFX_LAST];
				spr_chocobo[k] = new gfxSprite * [PGFX_LAST];
				spr_bobomb[k] = new gfxSprite * [PGFX_LAST];

				for(j = 0; j < PGFX_LAST; j++)
				{
					spr_player[k][j] = new gfxSprite();
					spr_chocobo[k][j] = new gfxSprite();
					spr_bobomb[k][j] = new gfxSprite();

					spr_player[k][j]->SetWrap(true);
					spr_chocobo[k][j]->SetWrap(true);
					spr_bobomb[k][j]->SetWrap(true);
				}
			}

			LoadMenuGraphics();
			LoadGameGraphics();

			_load_gfx(spr_backmap[0], convertPath("gfx/packs/backgrounds/Land_Classic.png", gamegraphicspacklist.current_name()));
			_load_gfx(spr_backmap[1], convertPath("gfx/packs/backgrounds/Land_Classic.png", gamegraphicspacklist.current_name()));
			_load_gfx(spr_frontmap[0], convertPath("gfx/packs/backgrounds/Land_Classic.png", gamegraphicspacklist.current_name()));
			_load_gfx(spr_frontmap[1], convertPath("gfx/packs/backgrounds/Land_Classic.png", gamegraphicspacklist.current_name()));

			LoadGameSounds();

			if(!game_values.soundcapable)
			{
				game_values.sound = false;
				game_values.music = false;
				game_values.soundvolume = 0;
				game_values.musicvolume = 0;
			}

			//Read the map filter lists
			maplist.ReadFilters();
			maplist.ApplyFilters(game_values.pfFilters);

			ifsoundonplay(sfx_coin);
			state++;
		}


		//Sleep for time just under what we need
		int delay = WAITTIME - SDL_GetTicks() + framestart - 2;

		if(delay > 0)
		{
			if(delay > WAITTIME)
				delay = WAITTIME;

			SDL_Delay(delay);
		}

		//Fine tune wait here
		while(SDL_GetTicks() - framestart < WAITTIME)
			SDL_Delay(0);   //keep framerate constant at 1000/WAITTIME fps

	}

	return true;
}

bool LoadMenuSkin(short playerID, short skinID, short colorID, bool fLoadBothDirections)
{
	return __load_gfxmenuskin(spr_player[playerID], skinlist.GetIndex(skinID), colorID, fLoadBothDirections);
}

bool LoadFullSkin(gfxSprite ** sprites, const std::string& filename, short colorID)
{
	return __load_gfxfullskin(sprites, filename, colorID);
}

bool LoadFullSkin(gfxSprite ** sprites, short skinID, short colorID)
{
	return LoadFullSkin(sprites, skinlist.GetIndex(skinID), colorID);
}


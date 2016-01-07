#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include "gfx/gfxFont.h"
#include "gfx/gfxSprite.h"
#include "GlobalConstants.h"
#include "sfx.h"

#include <string>

class CResourceManager {
public:
	bool LoadFullSkin(gfxSprite ** sprites, short skinID, short colorID);
	bool LoadFullSkin(gfxSprite ** sprites, const std::string& filename, short colorID);

	bool LoadWorldGraphics();
	bool LoadMenuSkin(short playerID, short skinID, short colorID, bool fLoadBothDirections);
	bool LoadMenuSkin(short playerID, const std::string& filename, short colorID, bool fLoadBothDirections);
	void LoadAllGraphics();
	bool LoadMenuGraphics();
	bool LoadGameGraphics();

	bool LoadGameSounds();

private:
	void LoadAllSprites();

public:

	gfxSprite		** spr_player[4];	//all player sprites
	gfxSprite		** spr_shyguy[4];
	gfxSprite		** spr_chocobo[4];
	gfxSprite		** spr_bobomb[4];
	gfxSprite		spr_clouds;
	gfxSprite		spr_ghosts;
	gfxSprite		spr_fish;
	gfxSprite		spr_leaves;
	gfxSprite		spr_snow;
	gfxSprite		spr_rain;

	gfxSprite		spr_background;
	gfxSprite		spr_backmap[2];
	gfxSprite		spr_frontmap[2];
	gfxSprite		spr_overlay;
	gfxSprite		menu_backdrop;

	gfxFont			menu_font_small;
	gfxFont			menu_font_large;
	gfxFont			game_font_small;
	gfxFont			game_font_large;

	gfxSprite		menu_shade;
	gfxSprite		menu_smw;
	gfxSprite		menu_version;
	gfxSprite		menu_plain_field;
	gfxSprite		menu_player_select;
	gfxSprite		menu_map_filter;
	gfxSprite		menu_match_select;
	gfxSprite		menu_dialog;
	gfxSprite		menu_slider_bar;
	gfxSprite		menu_verticalarrows;

	gfxSprite		menu_stomp;
	gfxSprite		menu_survival;
	gfxSprite		menu_egg;

	gfxSprite		menu_mode_small;
	gfxSprite		menu_mode_large;
	gfxSprite		spr_dialog;
	gfxSprite		spr_dialogbutton;
	gfxSprite		spr_tournament_background;
	gfxSprite		spr_tournament_powerup_splash;
	gfxSprite		spr_player_select_background;
	gfxSprite		spr_player_select_ready;
	//gfxSprite		spr_ipfield;
	gfxSprite		spr_selectfield;
	gfxSprite		spr_selectfielddisabled;
	gfxSprite		spr_map_filter_icons;
	gfxSprite		spr_tour_markers;
	gfxSprite		spr_menu_boxed_numbers;
	gfxSprite		spr_countdown_numbers;
	gfxSprite		spr_thumbnail_warps[2];
	gfxSprite		spr_thumbnail_mapitems[2];
	gfxSprite		spr_platformstarttile;
	gfxSprite		spr_platformendtile;
	gfxSprite		spr_platformpath;

	gfxSprite		spr_worldbackground[3];
	gfxSprite		spr_worldforeground[3];
	gfxSprite		spr_worldforegroundspecial[3];
	gfxSprite		spr_worldpaths[3];
	gfxSprite		spr_worldvehicle[3];

	gfxSprite		spr_worlditems;
	gfxSprite		spr_worlditempopup;
	gfxSprite		spr_worlditemssmall;
	gfxSprite		spr_worlditemsplace;
	gfxSprite		spr_worldbonushouse;

	gfxSprite		spr_announcementicons;

	gfxSprite		spr_noteblock;
	gfxSprite		spr_breakableblock;
	gfxSprite		spr_powerupblock;
	gfxSprite		spr_donutblock;
	gfxSprite		spr_flipblock;
	gfxSprite		spr_bounceblock;
	gfxSprite		spr_throwblock;
	gfxSprite		spr_switchblocks;
	gfxSprite		spr_viewblock;
	gfxSprite		spr_weaponbreakableblock;

	gfxSprite		spr_brokenyellowblock;
	gfxSprite		spr_brokenflipblock;
	gfxSprite		spr_brokenblueblock;
	gfxSprite		spr_brokengrayblock;

	gfxSprite		spr_brokeniceblock;
	gfxSprite		spr_iceblock;

	gfxSprite		spr_tileanimation[3];
	gfxSprite		spr_blocks[3];
	gfxSprite		spr_unknowntile[3];

	gfxSprite		spr_starpowerup;
	gfxSprite		spr_1uppowerup;
	gfxSprite		spr_2uppowerup;
	gfxSprite		spr_3uppowerup;
	gfxSprite		spr_5uppowerup;
	gfxSprite		spr_firepowerup;
	gfxSprite		spr_hammerpowerup;
	gfxSprite		spr_icewandpowerup;
	gfxSprite		spr_podobopowerup;
	gfxSprite		spr_poisonpowerup;
	gfxSprite		spr_mysterymushroompowerup;
	gfxSprite		spr_boomerangpowerup;
	gfxSprite		spr_clockpowerup;
	gfxSprite		spr_bobombpowerup;
	gfxSprite		spr_powpowerup;
	gfxSprite		spr_modpowerup;
	gfxSprite		spr_bulletbillpowerup;
	gfxSprite		spr_featherpowerup;
	gfxSprite		spr_leafpowerup;
	gfxSprite		spr_bombpowerup;
	gfxSprite		spr_pwingspowerup;
	gfxSprite       spr_tanooki, spr_statue;
	gfxSprite		spr_extraheartpowerup;
	gfxSprite		spr_extratimepowerup;
	gfxSprite		spr_jailkeypowerup;

	gfxSprite		spr_secret1;
	gfxSprite		spr_secret2;
	gfxSprite		spr_secret3;
	gfxSprite		spr_secret4;

	gfxSprite		spr_shade[3];
	gfxSprite		spr_scorehearts;
	gfxSprite		spr_scorecards;
	gfxSprite		spr_scorecoins;

	gfxSprite		spr_timershade;
	gfxSprite		spr_scoretext;
	gfxSprite		spr_racetext;
	gfxSprite		spr_crown;
	gfxSprite		spr_warplock;
	gfxSprite		spr_cape;
	gfxSprite		spr_tail;
	gfxSprite		spr_wings;
	gfxSprite		spr_coinsparkle;
	gfxSprite		spr_shinesparkle;
	gfxSprite		spr_shellbounce;
	gfxSprite		spr_superstomp;

	gfxSprite		spr_coin;
	gfxSprite		spr_egg;
	gfxSprite		spr_eggnumbers;
	gfxSprite		spr_star;
	gfxSprite		spr_frenzycards;
	gfxSprite		spr_collectcards;
	gfxSprite		spr_flags;
	gfxSprite		spr_yoshi;
	gfxSprite		spr_thwomp;
	gfxSprite		spr_podobo;
	gfxSprite		spr_bowserfire;
	gfxSprite		spr_areas;
	gfxSprite		spr_kingofthehillarea;
	gfxSprite		spr_jail;
	gfxSprite		spr_racegoal;
	gfxSprite		spr_pipegamebonus;
	gfxSprite		spr_chicken;

	gfxSprite		spr_bonuschest;
	gfxSprite		spr_teleportstar;

	gfxSprite		spr_goomba;
	gfxSprite		spr_goombadead;
	gfxSprite		spr_goombadeadflying;
	gfxSprite		spr_koopa;
	gfxSprite		spr_buzzybeetle;
	gfxSprite		spr_spiny;
	gfxSprite		spr_paragoomba;
	gfxSprite		spr_parakoopa;
	gfxSprite		spr_redparakoopa;
	gfxSprite		spr_sledgebrothers;
	gfxSprite		spr_sledgebrothersdead;
	gfxSprite		spr_redkoopa;
	gfxSprite		spr_cheepcheep;
	gfxSprite		spr_cheepcheepdead;
	gfxSprite		spr_bulletbill;
	gfxSprite		spr_bulletbilldead;

	gfxSprite		spr_fireball;
	gfxSprite		spr_hammer;
	gfxSprite		spr_iceblast;
	gfxSprite		spr_boomerang;
	gfxSprite		spr_shell;
	gfxSprite		spr_shelldead;
	gfxSprite		spr_blueblock;
	gfxSprite		spr_spring;
	gfxSprite		spr_spike;
	gfxSprite		spr_bomb;
	gfxSprite		spr_kuriboshoe;
	gfxSprite		spr_throwbox;

	gfxSprite		spr_sledgehammer;
	gfxSprite		spr_superfireball;

	gfxSprite		spr_hazard_fireball[3];
	gfxSprite		spr_hazard_rotodisc[3];
	gfxSprite		spr_hazard_bulletbill[3];
	gfxSprite		spr_hazard_bulletbilldead;
	gfxSprite		spr_hazard_flame[3];
	gfxSprite		spr_hazard_pirhanaplant[3];

	gfxSprite		spr_fireballexplosion;
	gfxSprite		spr_frictionsmoke;
	gfxSprite		spr_bobombsmoke;
	gfxSprite		spr_explosion;
	gfxSprite		spr_burnup;
	gfxSprite		spr_fireworks;
	gfxSprite		spr_poof;

	gfxSprite		spr_spawnsmoke;
	gfxSprite		spr_spawndoor;
	gfxSprite		spr_bonus;
	gfxSprite		spr_extralife;
	gfxSprite		spr_award;
	gfxSprite		spr_awardsolid;
	gfxSprite		spr_awardsouls;
	gfxSprite		spr_awardsoulspawn;
	gfxSprite		spr_awardkillsinrow;
	gfxSprite		spr_flagbases;
	gfxSprite		spr_ownedtags;
	gfxSprite		spr_phanto, spr_phantokey;

	gfxSprite		spr_storedpowerupsmall;
	gfxSprite		spr_storedpoweruplarge;
	gfxSprite		spr_powerupselector;

	gfxSprite		spr_scoreboard;
	gfxSprite		spr_abovearrows;

	gfxSprite		spr_windmeter;
	gfxSprite		spr_overlayhole;

	// Level Editor sprites

	gfxSprite		spr_tiletypes;
	gfxSprite		spr_transparenttiles;
	gfxSprite		spr_backgroundlevel;
	gfxSprite		spr_tilesetlevel;
	gfxSprite		spr_eyecandy;
	gfxSprite		spr_warps[3];
	gfxSprite		spr_selectedtile;
	gfxSprite		spr_nospawntile;
	gfxSprite		spr_noitemspawntile;
	gfxSprite		spr_mapitems[3];
	gfxSprite		spr_powerups;
	gfxSprite		spr_hidden_marker;
	gfxSprite		spr_racegoals;
	gfxSprite		spr_number_icons;


	//------ sfx ------

	sfxSound		sfx_announcer[PANNOUNCER_SOUND_LAST];

	sfxSound		sfx_mip;
	sfxSound		sfx_deathsound;
	sfxSound		sfx_jump;
	sfxSound		sfx_skid;
	sfxSound		sfx_capejump;

	sfxSound		sfx_invinciblemusic;
	sfxSound		sfx_extraguysound;
	sfxSound		sfx_sprout;
	sfxSound		sfx_collectpowerup;
	sfxSound		sfx_collectfeather;
	sfxSound		sfx_storepowerup;
	sfxSound		sfx_tailspin;
	sfxSound		sfx_breakblock;
	sfxSound		sfx_bump;
	sfxSound		sfx_coin;
	sfxSound		sfx_fireball;
	sfxSound		sfx_springjump;
	sfxSound		sfx_timewarning;
	sfxSound		sfx_hit;
	sfxSound		sfx_chicken;
	sfxSound		sfx_transform;
	sfxSound		sfx_yoshi;
	sfxSound		sfx_pause;
	sfxSound		sfx_bobombsound;
	sfxSound		sfx_areatag;
	sfxSound		sfx_cannon;
	sfxSound		sfx_burnup;
	sfxSound		sfx_pipe;
	sfxSound		sfx_thunder;
	sfxSound		sfx_slowdownmusic;
	sfxSound		sfx_flyingsound;
	sfxSound		sfx_storedpowerupsound;
	sfxSound		sfx_kicksound;
	sfxSound		sfx_racesound;
	sfxSound		sfx_bulletbillsound;
	sfxSound		sfx_boomerang;
	sfxSound		sfx_spit;
	sfxSound		sfx_starwarning;
	sfxSound		sfx_powerdown;
	sfxSound		sfx_switchpress;
	sfxSound		sfx_superspring;
	sfxSound		sfx_stun;
	sfxSound		sfx_inventory;
	sfxSound		sfx_worldmove;
	sfxSound		sfx_treasurechest;
	sfxSound		sfx_flamecannon;
	sfxSound		sfx_wand;
	sfxSound		sfx_enterstage;
	sfxSound		sfx_gameover;
	sfxSound		sfx_pickup;

	sfxMusic		backgroundmusic[6];

};

#endif // RESOURCE_MANAGER_H

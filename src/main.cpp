/*----------------------------------------------------------+
| super mario war											|
|															|
| a mario war clone written using the tile based collision	|
| detection technique explained in jnrdev #1				|
|															|
| you can read the tutorial on http://jnrdev.72dpiarmy.net	|
|															|
|															|
| this sourcecode is released under the GPLv2.				|
|															|
|															|
| the code is a bit messy and contains a few things i'm not	|
| really happy with. (collision detection system, ...)      |
| but it works and the game is fun playing :)				|
|															|
|															|
| maybe put all the game relevant stuff in a CGame class	|
| when adding a menu.										|
|															|
|															|
| start:		24.01.2003									|
| last changes:	12.02.2008									|
|															|
|								© 2003-2009 Florian Hufsky  |
|								  florian.hufsky@gmail.com	|
|                                     mtschaffer@gmail.com  |
|								  http://smw.72dpiarmy.com	|
+----------------------------------------------------------*/

/*
TODO

Before Final Release
[ ] CLEAN UP OLD DLLS in SMW DIR!!!


BUG!! Still reports of disappearing map tiles - caused when rRects is used out of bounds causing w and h to be set to 0 - happened with platform with tile using row 960
      I think this was due to using old maps with newer versions of the 1.8 alpha - keep an eye on this, but it might be a non-issue

BUG!! When you kill 2 players/bots rapidly one after another with the star and you have the announcer on, the invincibilty music stops.

BUG!!  World AI needs ability to use stored items -> harder problem than I have time for

[ ] On/Off switch [?] and note blocks and possibly other types of on/off blocks
[ ] Thunderbolt style spawn
[ ] Reverse gravity blue podoboos
[ ] Bomb option in Star mode
[ ] Mariokart-type podium at the end of a tournament/tour/whatever


STUFF TO WATCH OUT FOR IN BETA2
- animated tiles in platforms
- collision detection and behavior of coins, ztar, collection cards


Beta2 BUGS!
[ ] Move resource loading into Init() methods instead of in the constructors
    - this it to help the port to the Wii

[ ] When you made items play the bump sound when they get squished, you forgot to change the flags in capture the flag mode, and the keys in phanto mode to play that sound.
    - Items that get relocated play the "transform" sound and items that don't, play the "bump" sound
[ ] The 0smw world was removed.
    - Huh, it is in my copy.  Is anyone else missing this map?
[ ] POW and MOd blocks don't work on people wearing kuribo's shoes.
    - Yep, awesome huh?
[ ] If you use an ice wand on someone using a kuribo's shoe, the player won't stick out of the shoe anymore.
    - Intentional.  It looked weird to have the player's head sticking out of the ice block.
[X] When you warp with the crown on your head, the crown is seen over the pipe, while Mario is not.
    - Great bug.  This has been around since warps and crowns were originally added.
[ ] Kills will still transfer the tag if "Touch Only" is set in Shy Guy Tag mode.
    - It is working correctly.  The first shyguy is always created by a kill even with "Touch Only" set.
[X] In the Shy Guy Tag menu, the top says "Tag Mode Menu" instead of "Shy Guy Tag Mode Menu".
    - Fixed
[X] Phanto Mode's menu says "Chase Mode Menu".
    - Fixed
[X] You don't have the numbers "5" and "6" shown for options in the world editor help menu.
    - Fixed
[X] When winning in Boxes minigam by having all 5 coins, all players have the death pic on the results.
    - Good catch.  Fixed.
[ ] Minor bug with the platforms. When you have a static non-moving platform placed, the skins will cycle through the standing and jumping poses very fast as shown in the picture below.
[ ] When a reserve powerup is grabbed after you already won a game, the game acts as if you have a poison mushroom in reserve.


Fixed
[X] Finished secret 3 and 4
[X] Fixed bug where you could throw items through the wall on the left map edge
[X] Red Koopas detect platform edges and turn around
[X] Cleaned up areas that used "*TILESIZE" and replaced them with "<< 5" for performance
[X] Fixed bug that crashed world editor if the map was smaller than the screen
[X] Fixed bug in world editor that didn't save worlds correct with boxes or bosses minigames
[X] Fixed bug when placing maps into the tours folder, they are not found in the world editor

Beta 1 Public Release Bugs
[ ] Treasure chests from winning a stage that rewards an item in world mode can be spawned in areas where they fall forever, and can never be opened. (Though this is aesthetic)

[ ] also usually the game does a good job not spawning you inside a platform(killing you) but I lined the bottom of a 
	map with death tiles and it started spawning inside the platform.  edit: also stomp mode has problems with spawning 
	them inside platforms(specifically continuous), only they don't die and still animate-it looks weird.  I guess you 
	have to take into account how much time is spent in the swirly spawn animation-an area can be empty initially, but 
	occupied by the time the animation is done

[ ] Refreshing the map thumbnails hangs half way through on xbox (after icecap map)
    -> Fixed memory leak in thumbnail creation, retest on xbox
	-> Still Hangs :(
	-> Fixed bug where some maps crashed when loaded

[ ] The "thumbnail" sized tiles for the world editor have purple parts in all the foreground objects, including the level markers.
    -> Write script to make these using 0x808080 gray background

Considering Features

[ ] BUG!!! In world mode when you win a boxes game, the ranking at the end of the game is based on lives left, not coins collected.  All players that didn't win should get 2nd place.

[ ] BUG!!! A bug that isn't on the first post is that if you quit a tour in session, the game displays and error message and the program closes. Not sure if this is 100% garunteed.
    -> No repro, need poster to post which tour and which stop this happened with

[ ] Build in release mode and make sure we can still use special maps in world editor

[ ] Get boss settings from forum and integrate them in


Need To Test
[ ] In the above picture, when I get killed on the spike but get the domination square below it, the square stays my color, even if I set it so that all my blocks go away when I die. All of my other ones turn neutral normally when I die like that. 
    -> made fix for this but need to test
[ ] Test controllers on xbox where different tournament control settings are used
	- Test all control cases with teams/bots keyboard/controllers menu/sub menus/bonus wheel/scoreboard and ties for winner/loser tournament ties etc.


Can't Reproduce Bug
[ ] Weird sound problems with the world music.
	-> no repro
[ ] Level editor crash when adding a new line segment platform, drawing only some solid on top tiles, then adjusting it's start and stop.  See video: http://www.youtube.com/watch?v=vXpnv8qpVDg
	-> No repro
[ ] Can run through walls that have 1 tile wide gaps : -> no repro, need map where this happened
	  X
	P X  -> run across 1 tile gap into and through wall
	XG
	X
[ ] P-Wings, not sure if there's a bug, but I collected a lot of other items, and I couldn't get rid of the P-Wings. 
	I kept activating the items I collected but I still had the P-Wings and didn't switch. 
	The activation worked though, as the animation played and the activated items disappeared.
[ ] This may have already been mentioned, but I was playing a timed match against three bots, 
	and one of them had star invincibility at the time the clock ran out. The other two bots dropped off the stage 
	instantly as the victory fanfare played, but this bot remained along with my character. In fact, after his star 
	ran out, I killed him -- and he respawned just as my character would. Very odd.
	-> No repro, but was it a tie game?  If so, the bot would have remained with you.
[ ] No matter what I set the Glide setting to for Chicken mode in a Single Game, the Chicken can NEVER glide. I don't know if I'm doing anything wrong or what, but... It just won't work. Oddly enough, I believe I've seen it work in Tours/Worlds. O_o I know it works SOMEWHERE in the game...just not in the Single Game.
    -> No repro
[ ] Items get overlapped by platforms, no matter what layer they are.
	-> No repro, verified powerup items appear over layer 1
[ ] for the animated tiles, i tried to set it to solid for the waterfall tiles (as in the default tile type?), yet in game it does not work.
	-> No repro, setting default tile types for animated tiles works fine -> though, it doesn't affect maps that have already been created before the change (maybe some confusion here)
[ ] Player spawn zones are ignored in SpawnZonesIgnored.map on desktop
	-> No repro, spawn zones work fine on this map
	-> It is actually a no spawn zone feature.  Spawn zones are still calculated by making sure a player has something hard to land on, not death tiles
	-> We probably need to do a little better job when no hard areas are defined to choose a good place
[ ] Falling donut blocks turn into castle pieces in SMWarld mod (on desktop) -> ask Mr Mister it is his mod
	-> The donut block from the standard tileset needs to be a "falling donut block" tile
[ ] colission doesn't always work with the brown "bounce blocks" when you have a cape or leaf; 
	when jumping and rotating you sometimes glide through them from below 
	(not sure if it also happened with leaf, but it did happen with cape)
	-> no repro
[ ] I used the power-up to take off a CPU's win so I could redo the level and after the CPU won again, it just stands on that level and doesn't move...



No Fix
[ ] play world mode with the random skins, the change every time you enter a level; they suddenly change on the world map.  For random skins it should unrandomize them after the world starts. -> They should switch every time you start a game.
	-> It looks a little weird, but it is acceptable and not worth the work to fix it (changing to a random skin happens deep inside the gamestart() code)
[ ] On/Off switches placed just beside a column of switch blocks: -> intended gameplay
	When you jump into the on/off switch and are some pixels into the switch block column you'll die. 
	It's really annoying since you often don't have the time to aim exactly.


Beta 1 Public Release Feature Requests
[ ] Update the Tanooki statues to be proportionally sized to players
[ ] Use smb3_extended1up.wav (on desktop) for 5up sound
[ ] Maybe the three waterfalls on the Classic tileset could animate differently
[ ] Is there any way you could re-add that functionality to the game (maybe with a toggle to turn the secrets on or off) before the final release? You don't have to bring all of them back, but I really miss the super fireball, triple hammer, kaboomerang, extended POW and MOD blocks, super shell kick, and homing Bullet Bills.
[ ] I think there should be a way to agree on a draw in world mode and thus skip a level. In one world there was a capture-the-flag level where you could only score with the flag in your own base, without touch return and a high auto-return time. We only were two players (human) and ended up carrying each others flag all the time and couldn't score. To bring back your own flag you had to drop the enemy flag, so stomping didn't help either. It's a bit unfair if one player has to intentionally lose just so the game can go on  (Unfair in the sense of world score, probably same for tournament I guess) Maybe just add an option for "draw" or "skip level" to the exit menu?
[ ] it'd be a nice idea to add some weapon options for the Bomb powerup. Maybe a bomb limit that ranges from 1 to 5, and a fuse limit ranging from 3 to 10 seconds.
[ ] Intro music attached to normal looped tracks
[ ] Also, as P1 on the Bonus Island World against 3 CPUs, I was unable to use items in the World screen.
[ ] Though the universal Overrides thing has Map-specific and World-specific overrides, it lacks Background-specific overrides.
[ ] Volume control for individual tunes in music packs. Just changing a number in the text file would be a lot less trouble than having to edit the music files themselves.
[ ] Bombs setting off other bombs (chain reaction) and bombs destroying breakable blocks
[ ] Enemy specific spawn zones (as part of the mode objects locations in level editor)
[ ] Be able to define a size of area in which the suicide timer runs (like, say, if you're in the same 3x3-tile area for long enough, you get killed). Right now, the game only checks to see if you haven't moved; this causes the bots to not get timed out unless you set the suicide timer to something really short, since they still jump occasionally.

[ ] I hate that being trapped by ! block or even flip block kills you. That really sux and I request deleting it
[ ] How difficult would it be to add another mode which would be basically identical to Greed mode, but with a couple of 
    tweaks? Basically it would act like Coin mode from Smash Bros. It could be called "Cash Clash" or something. 
	1) Players start with no coins, but still lose coins when hit. 
	2) There is a global timer like in Time Limit mode. The primary parameter of the mode is how long the game lasts, 
	like in Time Limit. At the end of the match, whoever has the most coins wins. 
	3) (Maybe) One extra game option - if you want, have it so whoever has the fewest coins at the end wins.
[ ] donut blocks that respawn.
[ ] ember eyecandy: works like bubbles but with different graphics and ash eyecandy: works like snow but with different graphics
[ ] A Button in the Map items that activates and deactivates certain Platforms. 
[ ] an option to when pressing "k" over an siwtch, not ! block that it can be used only a certain amount of times that is adjustable
[ ] i don't know if someone mentioned this already but it would be cool if you can kick items and shells upward like in SMW
[ ] Also, why not an item that makes you partially transparent for awhile? Kind of like the cloaking device thing from Super Smash. -> maybe an item that turns on a hard shield for a while
[ ] Make it easier to assign tile types in level editor by pressing keys or have a little menu pop up
[ ] Switch that turns off and on certain movements of some hazards[like Fire-Bars, Roto-disks, and Piranha-plants]


Feature Requests

[ ] Author filter type where you can choose a single author and it will only show you the maps from that author.  
	At startup, it would scan all maps and extra the author (everything before the underscore) then create a list of 
	possible authors to filter on.

[ ] Player needs bounce when killing hazards with shoe or tanooki?

[ ] Roulette wheel should be stopped by any key pressed by the player

[ ] In Yoshi's Eggs, could we have it so the options menu has two columns, one for eggs and one for Yoshis? 
	Oh, and an option to have the egg timer pause while the egg is being held would be nice.

[ ] Have it so that if a player hits 0 coins but still has coins of their color in play, make it so they don't die 
	until their coins disappear.

[ ] In Frenzy Mode it would be nice to have some presets like with the regular item switch.

[ ] I also think we should fix the explosions. The bob-omb explosion should resemble the SMB3 bob-omb explosion 
	and the bomb powerup should break bricks when exploded.

[ ] Since we have all these team-colored coins for Greed mode, could we have an option for higher-point coins in Coin mode? 
	Red could be 2, Green 3, and Blue 5.

[ ] I have a question. If the clear boxes in SMWorld is hit, does it turn into a normal block? Or does it become an empty 
	clear box? In SMW it turns into a normal block and that looked funny to me. If it does not in World, I request that it 
	should be corrected.

[ ] Bullet Bill shooters and piranha plants should not be active while a player is sitting on or next to it.

[ ] Sometime in the unknown future I would like to see more types of bricks and [?] boxes. It is a tad weird when you 
	are making a SMB1 themed map with SMB3 bricks or [?] boxes.

[ ] It'd be nice if the fortresses can get rid of a locked door like in SMB3.  Like you put down a fortress on your 
	world and with the world editor, link the fortress and the locked door together so that if you beat the fortress, 
	the locked door vanishes.

[ ] By the way, this led me to an idea of how to re-implement the superfire if anyone wants it back. We could use a fire 
	wand that shoots them, and on impact, the player is engulfed in flame and disappears in a poof of smoke.

[ ] Castle collapse tiles for world mode: instead of turning into the colored T tile, you can set an option for stages 
	clearing into an overworld foreground sprite. (The ones with the castles and such on them, since that image contains 
	collapsed castles too) You could have it under the stages section with a number denoting what sprite to change into, 
	0 for the T colored tile.

[ ] In level previews, eyecandy should show, and animated tiles and interactive blocks should animate.

[ ] Options to choose how many fireballs, hammers, boomerangs, etc can be on screen at once.

[ ] Options to choose how long clocks and stars last.

[ ] Option to choose how long the p-wing lets you fly.

[ ] The level editor should use the newer menu graphics.

[ ] Worlds with 2 draw-bridges and only one small path connected between them, and when you have the hourglass power-up in your reserves, you'll obviously be stuck if you activate it when you're in between them, so is there anyway you could make it so the hourglass is idiot-proof and doesn't allow you to use it when it figures out you'll be stuck if you activate it?

[ ] The tail/ cape attack sounds like it's in a cave on all maps

[ ] We need the ability to have Donut Blocks fall at certain times. Blue ones are fast, brown ones or normal, and grey ones are slow. Plus ability for donut blocks to come back after they are destroyed and customizable time for it to be triggered.


BUGS:
[ ] "Stage Type" sounds a bit misleading for configuring vehicles to go to stages

[ ] if a player is holding a shell while frozen, the shell is still held in place.  *edit: even when you let go of run, the shell is held there.  i still don't know if this is intended or not. should probably kill the shell though. :x 

[ ] There are several repeated tiles in the SMB2 and SMB3 tilesets, such as two pairs of bullet bill blasters in SMB3 and what looks to be two identical layers of the tree from SMB2. If those are taken out, it would give at least a little bit of room to add stuff / organize.

[ ] The "thumbnail" sized tiles for the world editor have purple parts in all the foreground objects, including the level markers.

[ ] Yet another, though it might be impossible to fix: Clear blocks do not give you the displayed powerup when you hit them in certain modes that have mode specific items like Health and Jail and the block gives you that instead.

[ ] Shells and mushrooms cannot do an about face when they hit the solid side of a moving platform, and just completely stop moving. example: continuous platform moves left, shell comes moving right, hits solid platform, doesn't move. It takes a stop and start from a player to get it moving again.
    -> test on test/00000000stuckshell.map
	-> or test/00000000movingstage.map
	-> This is caused because a shell/mushroom is touching both the left and right side of a platform at the same time
	-> Fixed part of this, but players and objects that are pushing against the side of the platform as it moves across the screen edge will lack good collision detection and cause the player to "bounce" against the side of the platform
	-> Bouncing effect happens all the time when platofrm is moving right and player left... crap!

[ ] You can't tag anyone when you're in the shoe.
    -> Can't reproduce this, please post exact steps or wait for next build
	1. Become tagged
	2. Get in a shoe
	3. Try to touch someone in the shoe while you're tagged.

[ ] I wanted to stomp a bot,which stood on a continious upmoving, solid-on top platform, which was set on the highes velocity. Instead of stomping the bot, my character kind of "froze" in the jumping frame and hovered exactly over the bot's head. After the upmoving platform transported me out of the screen, i did not fall down, but i could move left and right. after i have left the area of the upmoving platform, i finally fell down.


WAITING RESPONSE
[ ] I can't tell if this is a bug or not but the key power-up for Jail pops up in other modes. All I remember was Greed, Phanto, and Classic.   but possibly others -> adjusted parens but I don't think this was possible before from other modes without the game type being wrong
    -> no repro
[ ] After deleting a tile in the leveleditor, the tiletype of it remains on the spot the tile has been before.
	-> This is probably intended - there is a feature to not remove tile types if you customized them in that tile
	-> Post your exact repro case and I'll look to see if it is actually a bug
[ ] In frenzy mode if all card weights are set to 0, one ? card appears and that's it fo the game
    -> no repro - check your mode settings
[ ] World keys are rendered useless when two doors are adjacent to the spot in which you are standing.
    -> Not sure what you mean
[ ] Also, what does "Use Game" mean for the Item Block properties? If it means "use the game's item settings instead", shouldn't it be the default for each item block? Otherwise, all item blocks unless specified will use the same item settings regardless of what the user chooses. 
    -> if the user is changing the settings, then they should also change it to use those settings
[ ] Bots still dont like levels that is nothing but one single gigantic moving platform it seems, as they'll just constantly run left as if always in danger. The spawning algorithm is the same way, always spawning players just in middle of the screen. -> maybe if everything is a falling area, then just allow spawning everywhere
    -> The bot fall detection algorithm doesn't take into account moving platforms - it is computationally intensive to do this, the best solution for now is to make a row of solid tiles at the bottom of the map
[ ] Oh and maybe you could move the button that opens the world mode item selection (the pop up on the bottom) from the space bar to something like maybe the Stored Item Use button? Using a gamepad on a world without the ability to open that up is really annoying.
    -> For game controllers, it is mapped to the "Random" button for menus.  The world is technically a menu, not a game so it uses menu buttons.  See if that works for you.
[ ] SMW just crashed on me while playing a world map on capture the flag mode. I'm not sure how it happened, but the almost finished chime started just as I grabbed a flag. I'll need to look into it.
    -> Hope the projectile fix on the bottom row fixed this.  Keep an eye out for crashes in the next build.
[ ] That platform (or, to be more exact, the whole stage) is moving left.  My character is partially in the vertical wooden block.
    -> Please post map.
[ ] Also, what does "Use Game" mean for the Item Block properties? If it means "use the game's item settings instead", shouldn't it be the default for each item block? Otherwise, all item blocks unless specified will use the same item settings regardless of what the user chooses. 
    -> if the user is changing the settings, then they should also change it to use those settings
[ ] If you become a statue in the air, and have a feather, you can use its extra jump while a statue.
    -> I can't reproduce this, it may have been fixed with another bug fix

*/

/*
Procedure for adding a new game mode:
1) Add class to gamemodes.cpp and gamemodes.h
2) Add game mode type to GameModeType enum in gamemodes.h
3) Add new game mode to gamemodes array in main.cpp (gamemodes[X] line 1187)
4) Update MI_SelectField * miGoalField[22]; line in menu.h to match the new number of modes
5) Update UI_Menu mModeSettingsMenu[22]; line in modeoptionsmenu.h to accomodate a new settings menu
6) Update modeoptionsmenu.cpp: ModeOptionsMenu::Refresh()
7) Add game mode options to GameModeSettings in global.h
8) Set default settings for settings in main.cpp
9) Add menu fields to support these new options in menu.cpp and menu.h
10) Update ParseTourStopLine() and WriteTourStopLine() in global.cpp
11) Update SetRandomGameModeSettings() in modeoptionsmenu.cpp
12) Add new mode gfx to gfx\packs\Classic\menu\menu_mode_large.png and menu_mode_small.png
13) Update fShowSettingsButton[] array in menu.cpp
14) Remove old options.bin (new settings will now be read from it)
15) Change line 3186 in main.cpp: if(iMode == game_mode_pipe_minigame)
16) Change line 3274 in main.cpp: if(iMode == game_mode_pipe_minigame)
17) Update tours/0smw.txt to have documentation of mode and options
18) Update worldeditor.cpp: "miModeField = new MI_ImageSelectField" to add new game mode
19) Update worldeditor.cpp: "g_iNumGameModeSettings[]"


Procedure for adding a new game mode option:
1) Add game mode options to GameModeSettings in global.h
2) Add to SetupDefaultGameModeSettings() in global.cpp
3) Add menu fields to support these new options in modeoptionsmenu.cpp and modeoptionsmenu.h
4) Update ParseTourStopLine() and WriteTourStopLine() in global.cpp
5) Update SetRandomGameModeSettings() in modeoptionsmenu.cpp
6) Remove old options.bin (new settings will now be read from it)
7) Update tours/0smw.txt to have documentation of new mode option
8) Update worldeditor.cpp: miModeField = new MI_ImageSelectField to add new game mode
9) Update worldeditor.cpp: g_iNumGameModeSettings[]


Procedure for adding a new powerup:
1) Add class to object.cpp and object.h inhieriting from MO_Powerup
2) Update CPlayer::move() to handle using new stored powerup
3) Update NUM_POWERUPS in global.h
4) Update CPlayer::SetPowerup() to use this powerup
5) Add new gfx to gfx\packs\Classic\powerups\large.png and small.png
6) Update iPowerupPositionMap[] in uicontrol.cpp
7) Add to kill style array potentially for greed mode
*/

#ifdef _XBOX
	#include <xtl.h>
#endif

#include "global.h"				//all the global stuff
#include <time.h>
#include <math.h>

//now it's really time for an "engine" (aka resource manager)
#ifdef _WIN32
	#pragma comment(lib, "SDL_image.lib")

	#ifndef _XBOX
		#pragma comment(lib, "SDL.lib")
		#pragma comment(lib, "SDLmain.lib")
		#pragma comment(lib, "SDL_mixer.lib")

		#define WIN32_LEAN_AND_MEAN
		#include <windows.h>
		
	#endif
#endif


//------ system stuff ------
SDL_Surface		*screen;		//for gfx (maybe the gfx system should be improved -> resource manager)
SDL_Surface		*blitdest;		//the destination surface for all drawing (can be swapped from screen to another surface)

short			g_iCurrentDrawIndex = 0;

short			x_shake = 0;
short			y_shake = 0;

//Vars that keep track of spinning the screen
float spinangle = 0.0f;
float spinspeed = 0.0f;
short spindirection = 1;
short spintimer = 0;

//------ sprites (maybe this should be done in a resource manger) ------
gfxSprite		** spr_player[4];	//all player sprites (see global.h)
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


//------ game relevant stuff ------
CPlayer			*list_players[4];
short			list_players_cnt = 0;

CScore			*score[4];

short			score_cnt;
short			scoreoffsets[3] = {2, 36, 70};
short			scorepowerupoffsets[3][3] = {{37, 0, 0},
											{71, 89, 0},
											{105, 123, 141}};
short			respawn[4] = {0, 0, 0, 0};
short			respawnanimationtimer[4] = {0, 0, 0, 0};
short			respawnanimationframe[4] = {0, 0, 0, 0};

short			projectiles[4];

extern short controlkeys[2][2][4][NUM_KEYS];
extern int g_iVersion[];

//Locations for swirl spawn effects
short g_iSwirlSpawnLocations[4][2][25];

CMap			*g_map;
CTilesetManager *g_tilesetmanager;

CEyecandyContainer eyecandy[3];
SpotlightManager spotlightManager;

CObjectContainer noncolcontainer;
CObjectContainer objectcontainer[3];

bool g_fLoadMessages = true;

STextAward awards[PAWARD_LAST] = {
								STextAward("Double Kill", &game_font_small),
								STextAward("Triple Kill", &game_font_small),
								STextAward("Killing Spree",  &game_font_small),
								STextAward("Killing Spree x 2", &game_font_small),
								STextAward("Killing Spree x 3", &game_font_small),
								STextAward("Dominating", &game_font_large),
								STextAward("Dominating x 2", &game_font_large),
								STextAward("Dominating x 3", &game_font_large),
								STextAward("Unstoppable!", &game_font_large)
							};

sfxSound sfx_announcer[PANNOUNCER_SOUND_LAST];

bool  fResumeMusic = true;

sfxSound sfx_mip;
sfxSound sfx_deathsound;
sfxSound sfx_jump;
sfxSound sfx_skid;
sfxSound sfx_capejump;

sfxSound sfx_invinciblemusic;
sfxSound sfx_extraguysound;
sfxSound sfx_sprout;
sfxSound sfx_collectpowerup;
sfxSound sfx_collectfeather;
sfxSound sfx_storepowerup;
sfxSound sfx_tailspin;
sfxSound sfx_breakblock;
sfxSound sfx_bump;
sfxSound sfx_coin;
sfxSound sfx_fireball;
sfxSound sfx_springjump;
sfxSound sfx_timewarning;
sfxSound sfx_hit;
sfxSound sfx_chicken;
sfxSound sfx_transform;
sfxSound sfx_yoshi;
sfxSound sfx_pause;
sfxSound sfx_bobombsound;
sfxSound sfx_areatag;
sfxSound sfx_cannon;
sfxSound sfx_burnup;
sfxSound sfx_pipe;
sfxSound sfx_thunder;
sfxSound sfx_slowdownmusic;
sfxSound sfx_flyingsound;
sfxSound sfx_storedpowerupsound;
sfxSound sfx_kicksound;
sfxSound sfx_racesound;
sfxSound sfx_bulletbillsound;
sfxSound sfx_boomerang;
sfxSound sfx_spit;
sfxSound sfx_starwarning;
sfxSound sfx_powerdown;
sfxSound sfx_switchpress;
sfxSound sfx_superspring;
sfxSound sfx_stun;
sfxSound sfx_inventory;
sfxSound sfx_worldmove;
sfxSound sfx_treasurechest;
sfxSound sfx_flamecannon;
sfxSound sfx_wand;
sfxSound sfx_enterstage;
sfxSound sfx_gameover;
sfxSound sfx_pickup;

sfxMusic backgroundmusic[6];

CGameMode			*gamemodes[GAMEMODE_LAST];
CGM_Bonus			*bonushousemode = NULL;
CGM_Pipe_MiniGame	*pipegamemode = NULL;
CGM_Boss_MiniGame	*bossgamemode = NULL;
CGM_Boxes_MiniGame	*boxesgamemode = NULL;

short		currentgamemode = 0;

short g_iWinningPlayer;

extern short g_iPowerupToIcon[6];

extern void SetupScoreBoard(bool fOrderMatters);
extern void ShowScoreBoard();

extern void LoadCurrentMapBackground();

extern bool VersionIsEqual(int iVersion[], short iMajor, short iMinor, short iMicro, short iBuild);

FiltersList *filterslist;  //Filters list must be initiallized before maps list because it is used in maplist constructor
MapList *maplist;
SkinList *skinlist;
AnnouncerList *announcerlist;
MusicList *musiclist;
WorldMusicList *worldmusiclist;
GraphicsList *menugraphicspacklist;
GraphicsList *worldgraphicspacklist;
GraphicsList *gamegraphicspacklist;
SoundsList *soundpacklist;
TourList *tourlist;
WorldList *worldlist;
WorldMap g_worldmap;

std::vector<MapMusicOverride*> mapmusicoverrides;
std::vector<WorldMusicOverride*> worldmusicoverrides;

//Network stuff
int g_iNextNetworkID = 0;
int g_iNextMessageID = 0;
char szIPString[32] = "";

//NetServer netServer;
//NetClient netClient;

extern Uint8 GetScreenBackgroundFade();

extern short g_iCollisionMap[MOVINGOBJECT_LAST][MOVINGOBJECT_LAST];

extern void SetupDefaultGameModeSettings();

float CapFallingVelocity(float vel)
{
	//if(vel < -MAXVELY)
	//	return -MAXVELY;
	
	if(vel > MAXVELY)
		return MAXVELY;
	
	return vel;
}

float CapSideVelocity(float vel)
{
	if(vel < -MAXSIDEVELY)
		return -MAXSIDEVELY;
	
	if(vel > MAXSIDEVELY)
		return MAXSIDEVELY;
	
	return vel;
}

//handles a collision between a powerup and an object
void collisionhandler_o2o(IO_MovingObject * o1, IO_MovingObject * o2)
{
	o2->collide(o1);
}

short CountAliveTeams(short * lastteam)
{
	short findlastteam = 0;

	bool teamalive[4] = {false, false, false, false};
	for(short k = 0; k < list_players_cnt; k++)
	{
		if(!list_players[k]->isdead())
			teamalive[list_players[k]->teamID] = true;
	}

	short numteams = 0;
	for(short k = 0; k < 4; k++)
	{
		if(teamalive[k])
		{
			findlastteam = k;
			numteams++;
		}
	}

	if(lastteam != NULL)
	{
		if(numteams == 1)
			*lastteam = findlastteam;
		else
			*lastteam = -1;
	}

	return numteams;
}

void CleanDeadPlayers()
{
	bool fCheckForGameOver = false;

	for(short i = 0; i < list_players_cnt; i++)
	{
		if(list_players[i]->state == player_dead)
		{
			fCheckForGameOver = true;

			if(respawn[list_players[i]->globalID] <= 0)
				list_players[i]->die(0, true, false);

			//Set this to zero so we don't display a spawn egg when player is removed from game
			respawn[list_players[i]->globalID] = 0;

			if(game_values.gamemode->tagged == list_players[i])
				game_values.gamemode->tagged = NULL;

			if(game_values.gamemode->chicken == list_players[i])
				game_values.gamemode->chicken = NULL;

			delete list_players[i];
			
			for(short j = i; j < list_players_cnt - 1; j++)
			{
				list_players[j] = list_players[j + 1];
				list_players[j]->localID = j;
			}

			list_players_cnt--;
			list_players[list_players_cnt] = NULL;
		}
	}

	if(fCheckForGameOver && game_values.gamemode->gamemode != game_mode_bonus && game_values.gamemode->gamemode != game_mode_boss_minigame)
	{
		short lastteam = -1;
		if(!game_values.gamemode->gameover && CountAliveTeams(&lastteam) <= 1)
		{
			game_values.gamemode->gameover = true;
			game_values.gamemode->winningteam = lastteam;
			SetupScoreBoard(true);  //pass true because the order the players died in matters
			ShowScoreBoard();
		}
	}
}

//Must only be called after organizeteams() is called
short LookupTeamID(short id, short * teamID, short * subTeamID)
{
	for(short i = 0; i < score_cnt; i++)
	{
		for(short j = 0; j < game_values.teamcounts[i]; j++)
		{
			if(game_values.teamids[i][j] == id)
			{
				if(teamID)
					*teamID = i;

				if(subTeamID)
					*subTeamID = j;

				return i;
			}
		}
	}

	if(teamID)
		*teamID = -1;

	if(subTeamID)
		*subTeamID = -1;

	return -1;
}

short LookupTeamID(short id)
{
	return LookupTeamID(id, NULL, NULL);
}

CPlayer * GetPlayerFromGlobalID(short iGlobalID)
{
	for(short i = 0; i < list_players_cnt; i++)
	{
		if(list_players[i]->globalID == iGlobalID)
			return list_players[i];
	}

	return NULL;
}

sfxSound * g_PlayingSoundChannels[NUM_SOUND_CHANNELS];

void DECLSPEC soundfinished(int channel)
{
	if(!g_PlayingSoundChannels[channel])
		printf("Error: SoundFinished() tried to clear a channel that was already cleared!\n");
	else
	{
		g_PlayingSoundChannels[channel]->clearchannel();
		g_PlayingSoundChannels[channel] = NULL;
	}
}

void DECLSPEC musicfinished()
{
	if(!game_values.music)
		return;

	if(game_values.gamestate == GS_GAME && !game_values.gamemode->gameover)
	{
		if(game_values.playnextmusic)
		{
			musiclist->SetNextMusic(g_map->musicCategoryID, maplist->currentShortmapname(), g_map->szBackgroundFile);
			backgroundmusic[0].load(musiclist->GetCurrentMusic()); //In Game Music
		}

		backgroundmusic[0].play(game_values.playnextmusic, false);
	}
	else
	{
		if(fResumeMusic)
		{
			backgroundmusic[3].play(false, false);
		}
	}
}

short GetModeIconIndexFromMode(short iMode)
{
	if(iMode == game_mode_pipe_minigame)
		iMode = 25;
	else if(iMode == game_mode_boss_minigame)
		iMode = 26;
	else if(iMode == game_mode_boxes_minigame)
		iMode = 27;

	return iMode;
}

void RunGame();
void CleanUp();
bool LoadAndSplashScreen();
void SetGameModeSettingsFromMenu();
void LoadMapObjects(bool fPreview);
extern void LoadMapHazards(bool fPreview);
void UpdateScoreBoard();
void PlayNextMusicTrack();
bool IsExitAllowed();
bool IsPauseAllowed();

//Adds music overrides to the music lists
void UpdateMusicWithOverrides();

Menu g_Menu;
gv game_values;
//MenuContext menu_context;

//Joystick-Init
SDL_Joystick **joysticks = NULL;
short joystickcount = 0;

#ifdef _DEBUG
bool g_fAutoTest = false;
bool g_fRecordTest = false;
#endif

/*
void EnterBossMode(short type)
{
	if(game_values.gamestate == GS_GAME && game_values.gamemode->gamemode != game_mode_boss_minigame)
	{
		bossgamemode->SetBossType(type);

		game_values.screenfade = 2;
		game_values.screenfadespeed = 2;

		backgroundmusic[0].stop();
		ifsoundonstop(sfx_invinciblemusic);
		ifsoundonstop(sfx_timewarning);
		ifsoundonstop(sfx_slowdownmusic);

		game_values.gamestate = GS_START_GAME;
	}
}*/

//Move the screen in a small circle
void SpinScreen()
{
	if(spindirection == 0 || spindirection == 2)
	{
		if(++spintimer >= 300)
		{
			spindirection++;
			spintimer = 0;
		}
	}
	else if(spindirection == 1)
	{
		spinspeed += 0.0008f;

		if(spinspeed >= 0.05f)
		{
			spinspeed = 0.05f;
			spindirection++;
		}
	}
	else
	{
		spinspeed -= 0.0008f;

		if(spinspeed <= -0.05f)
		{
			spinspeed = -0.05f;
			spindirection = 0;
		}
	}

	spinangle += spinspeed;

	if(spinangle >= TWO_PI)
	{
		spinangle -= TWO_PI;
	}
	else if(spinangle < 0.0f)
	{
		spinangle += TWO_PI;
	}

	float shakey = spinspeed * 640.0f * sin(spinangle);
	if(shakey < 0.0f)
		shakey -= 1.0f;

	x_shake = (short)(spinspeed * 640.0f * cos(spinangle));
	y_shake = (short)(shakey);
}

// ------ MAIN ------
int main(int argc, char *argv[])
{
	if (argc < 2) {
		fprintf(stderr, "Please specify root data directory\n");
		return -1;
	}
	
	RootDataDirectory = argv[1];
	
	printf("Now using RootDataDirectory=%s\n", RootDataDirectory);
	
	g_map = new CMap();
	g_tilesetmanager = new CTilesetManager();
	
	filterslist = new FiltersList();
	maplist = new MapList(false);
	skinlist = new SkinList();
	musiclist = new MusicList();
	worldmusiclist = new WorldMusicList();
	soundpacklist = new SoundsList();
	announcerlist = new AnnouncerList();
	tourlist = new TourList();
	worldlist = new WorldList();
	
	menugraphicspacklist = new GraphicsList();
	worldgraphicspacklist = new GraphicsList();
	gamegraphicspacklist = new GraphicsList();
	
	printf("-------------------------------------------------------------------------------\n");
	printf(" %s %s\n", TITLESTRING, VERSIONNUMBER);
	printf("-------------------------------------------------------------------------------\n");
	printf("\n---------------- startup ----------------\n");

	gfx_init(640, 480, false);		//initialize the graphics (SDL)
	blitdest = screen;
	
	/*
	//Comment this in to performance test the preview map loading
	MI_MapField * miMapField = new MI_MapField(&spr_selectfield, 70, 165, "Map", 500, 120);

	for(int k = 0; k < 100; k++)
	{
		game_values.playerInput.outputControls[3].menu_right.fPressed = true;
		miMapField->SendInput(&game_values.playerInput);
		//printf("Map over-> %s\n", maplist->currentFilename());
	}

	exit(0);
	*/

	sfx_init();                     //init the sound system


	//Joystick-Init
	SDL_InitSubSystem(SDL_INIT_JOYSTICK);
	joystickcount = (short)SDL_NumJoysticks();
	joysticks = new SDL_Joystick*[joystickcount];

	for(short i = 0; i < joystickcount; i++)
		joysticks[i] = SDL_JoystickOpen(i);

	SDL_JoystickEventState(SDL_ENABLE);

	//currently this only sets the title, not the icon.
	//setting the icon isn't implemented in sdl ->  i'll ask on the mailing list
	char title[128];
	sprintf(title, "%s %s", TITLESTRING, VERSIONNUMBER);
	SDL_WM_SetCaption(title, "smw.ico");
	SDL_ShowCursor(SDL_DISABLE);

	printf("\n---------------- loading ----------------\n");
	
	for(short iScore = 0; iScore < 4; iScore++)
		score[iScore] = new CScore(iScore);

	//set standard game values
	game_values.playercontrol[0]	= 1;
	game_values.playercontrol[1]	= 1;
	game_values.showfps				= false;
	game_values.frameadvance		= false;
	game_values.autokill			= false;
	game_values.framelimiter		= WAITTIME;
	game_values.sound				= true;
	game_values.music				= true;
	game_values.gamestate			= GS_MENU;
#ifdef _DEBUG	
	game_values.fullscreen			= false;
#else	
	game_values.fullscreen			= false;
#endif

	game_values.screenResizeX		= 20.0f;
	game_values.screenResizeY		= 20.0f;
	game_values.screenResizeW		= -40.0f;
	game_values.screenResizeH		= -40.0f;

	game_values.flickerfilter		= 5;  //Full flicker filter by default
	game_values.hardwarefilter		= 2;  //Bilinear by default
	game_values.softfilter			= 0;  //No soft filter by default
	game_values.aspectratio10x11	= false;  //No 10x11 aspect ratio by default

#ifdef _XBOX
	SDL_XBOX_SetScreenPosition(game_values.screenResizeX, game_values.screenResizeY);
	SDL_XBOX_SetScreenStretch(game_values.screenResizeW, game_values.screenResizeH);
#endif

	game_values.pausegame			= false;
	game_values.exitinggame			= false;
	game_values.exityes				= false;
	game_values.awardstyle			= award_style_fireworks;
	game_values.spawnstyle			= 2;
	game_values.tournamentgames		= 2;
	game_values.tournamentwinner	= -1;
	game_values.selectedminigame	= 0;
	game_values.matchtype			= MATCH_TYPE_SINGLE_GAME;
	game_values.tourindex			= 0;
	game_values.tourstopcurrent		= 0;
	game_values.tourstoptotal		= 0;
	game_values.worldindex			= 0;
	game_values.slowdownon			= -1;
	game_values.slowdowncounter		= 0;
	game_values.teamcollision		= 0;
	game_values.screencrunch		= true;
	game_values.screenshaketimer	= 0;
	game_values.screenshakeplayerid = -1;
	game_values.screenshaketeamid	= -1;
	game_values.toplayer			= true;
	game_values.loadedannouncer		= -1;
	game_values.loadedmusic			= -1;
	game_values.scoreboardstyle     = 0;
	game_values.teamcolors          = true;
	game_values.cputurn				= -1;
	game_values.shieldtime			= 62;
	game_values.shieldstyle			= 2;
	game_values.musicvolume			= 128;
	game_values.soundvolume			= 128;
	game_values.respawn				= 2;
	game_values.itemrespawntime		= 1860;  //default item respawn is 30 seconds (30 * 62 fps)
	game_values.hiddenblockrespawn	= 1860;  //default item respawn is 30 seconds
	game_values.outofboundstime		= 5;
	game_values.warplockstyle		= 1;	// Lock Warp Exit Only
	game_values.warplocktime		= 186;  // 3 seconds
#ifdef _DEBUG
	game_values.suicidetime			= 0;	// Turn off suicide kills for debug
#else
	game_values.suicidetime			= 310;	// 5 seconds
#endif
	game_values.cpudifficulty		= 2;
	game_values.fireballttl			= 310;  // 5 seconds
	game_values.shellttl			= 496;  // 8 seconds
	game_values.blueblockttl		= 310;  // 5 seconds
	game_values.redblockttl			= 310;  // 5 seconds
	game_values.grayblockttl		= 310;  // 5 seconds
	game_values.hammerdelay			= 25;	// 0.4 second
	game_values.hammerttl			= 49;	// 0.8 second
	game_values.hammerpower			= true; //hammers die on first hit
	game_values.fireballlimit		= 0;	//Unlimited
	game_values.hammerlimit			= 0;	//Unlimited
	game_values.boomerangstyle		= 1;	//SMB3 style
	game_values.boomeranglife		= 248;	// 4 seconds of zelda boomerang
	game_values.boomeranglimit		= 0;	//Unlimited
	game_values.featherjumps		= 1;	//Allow one extra cape jump
	game_values.featherlimit		= 0;	//Unlimited
	game_values.leaflimit			= 0;	//Unlimited
	game_values.pwingslimit			= 0;	//Unlimited
	game_values.tanookilimit		= 0;	//Unlimited
	game_values.bombslimit			= 0;	//Unlimited
	game_values.wandfreezetime		= 310;  //5 seconds of freeze time
	game_values.wandlimit			= 0;	//Unlimited
	game_values.storedpowerupdelay	= 4;
	game_values.bonuswheel			= 1;
	game_values.keeppowerup			= false;
	game_values.showwinningcrown	= false;
	game_values.startgamecountdown	= true;
	game_values.startmodedisplay	= true;
	game_values.deadteamnotice		= true;
	game_values.playnextmusic		= false;
	game_values.pointspeed			= 20;
	game_values.swapstyle			= 1;	//Blink then swap
	game_values.worldpointsbonus	= -1; //no world multiplier until player uses item to boost it
	game_values.singleplayermode	= -1;
	game_values.worldskipscoreboard = false;
	game_values.overridepowerupsettings = 0;
	game_values.minigameunlocked	= false;
	game_values.poweruppreset		= 0;
	game_values.tournamentcontrolstyle = 0;

	game_values.pfFilters			= new bool[NUM_AUTO_FILTERS + filterslist->GetCount()];
	game_values.piFilterIcons		= new short[NUM_AUTO_FILTERS + filterslist->GetCount()];
	game_values.fNeedWriteFilters	= false;
	
	for(short iFilter = 0; iFilter < NUM_AUTO_FILTERS + filterslist->GetCount(); iFilter++)
	{
		game_values.pfFilters[iFilter] = false;
		game_values.piFilterIcons[iFilter] = 0;
	}

	//game_values.networktype		= 0;
	//game_values.networkhost		= false;
	//game_values.gamehost			= false;
	
	//Set the default powerup weights for bonus wheel and [?] boxes
	for(short iPreset = 0; iPreset < NUM_POWERUP_PRESETS; iPreset++)
	{
		for(short iPowerup = 0; iPowerup < NUM_POWERUPS; iPowerup++)
		{
			g_iCurrentPowerupPresets[iPreset][iPowerup] = g_iDefaultPowerupPresets[iPreset][iPowerup];
		}
	}

	UpdateMusicWithOverrides();

	announcerlist->SetCurrent(0);
	musiclist->SetCurrent(0);
	worldmusiclist->SetCurrent(0);
	menugraphicspacklist->SetCurrent(0);
	worldgraphicspacklist->SetCurrent(0);
	gamegraphicspacklist->SetCurrent(0);
	soundpacklist->SetCurrent(0);

	for(short iPlayer = 0; iPlayer < 4; iPlayer++)
	{
		game_values.storedpowerups[iPlayer] = -1;
		game_values.gamepowerups[iPlayer] = -1;
		game_values.teamids[iPlayer][0] = iPlayer;
		game_values.teamcounts[iPlayer] = 1;
		game_values.skinids[iPlayer] = 0;
		game_values.colorids[iPlayer] = iPlayer;
		game_values.randomskin[iPlayer] = false;

		projectiles[iPlayer] = 0;
		respawn[iPlayer] = 0;

		//Setup the default key/button input configurations
		for(short iInputType = 0; iInputType < 2; iInputType++)  //for keyboard/joystick
		{
			game_values.inputConfiguration[iPlayer][iInputType].iDevice = iInputType - 1;

			for(short iInputState = 0; iInputState < 2; iInputState++)  //for game/menu
			{
				for(short iKey = 0; iKey < NUM_KEYS; iKey++)
				{
					game_values.inputConfiguration[iPlayer][iInputType].inputGameControls[iInputState].keys[iKey] = controlkeys[iInputType][iInputState][iPlayer][iKey];
				}
			}
		}

		//Set the players input to the default configuration (will be overwritten by options.bin settings)
#ifdef _XBOX
		game_values.inputConfiguration[iPlayer][1].iDevice = iPlayer;
		game_values.playerInput.inputControls[iPlayer] = &game_values.inputConfiguration[iPlayer][1];
#else
		game_values.playerInput.inputControls[iPlayer] = &game_values.inputConfiguration[iPlayer][0];
#endif
	}
	
	//set game modes
	gamemodes[0] = new CGM_Classic();
	gamemodes[1] = new CGM_Frag();
	gamemodes[2] = new CGM_TimeLimit();
	gamemodes[3] = new CGM_Jail();
	gamemodes[4] = new CGM_Coins();
	gamemodes[5] = new CGM_Stomp();
	gamemodes[6] = new CGM_Eggs();
	gamemodes[7] = new CGM_CaptureTheFlag();
	gamemodes[8] = new CGM_Chicken();
	gamemodes[9] = new CGM_Tag();
	gamemodes[10] = new CGM_Star();
	gamemodes[11] = new CGM_Domination();
	gamemodes[12] = new CGM_KingOfTheHill();
	gamemodes[13] = new CGM_Race();
	gamemodes[14] = new CGM_Owned();
	gamemodes[15] = new CGM_Frenzy();
	gamemodes[16] = new CGM_Survival();
	gamemodes[17] = new CGM_Greed();
	gamemodes[18] = new CGM_Health();
	gamemodes[19] = new CGM_Collection();
	gamemodes[20] = new CGM_Chase();
	gamemodes[21] = new CGM_ShyGuyTag();

	currentgamemode = 0;
	game_values.gamemode = gamemodes[currentgamemode];

	//Special modes
	bonushousemode = new CGM_Bonus();
	pipegamemode = new CGM_Pipe_MiniGame();
	bossgamemode = new CGM_Boss_MiniGame();
	boxesgamemode = new CGM_Boxes_MiniGame();

	SetupDefaultGameModeSettings();
	
	//Read saved settings from disk
	FILE * fp = OpenFile("options.bin", "rb");

	if(fp)
	{
		int version[4];
		fread(version, sizeof(int), 4, fp);

		if(VersionIsEqual(g_iVersion, version[0], version[1], version[2], version[3]))
		{
			#ifdef _XBOX
				fread(&game_values.flickerfilter, sizeof(short), 1, fp);
				fread(&game_values.hardwarefilter, sizeof(short), 1, fp);
				fread(&game_values.softfilter, sizeof(short), 1, fp);
				fread(&game_values.aspectratio10x11, sizeof(bool), 1, fp);

				fread(&game_values.screenResizeX, sizeof(float), 1, fp);
				fread(&game_values.screenResizeY, sizeof(float), 1, fp);
				fread(&game_values.screenResizeW, sizeof(float), 1, fp);
				fread(&game_values.screenResizeH, sizeof(float), 1, fp);

				SDL_XBOX_SetScreenPosition(game_values.screenResizeX, game_values.screenResizeY);
				SDL_XBOX_SetScreenStretch(game_values.screenResizeW, game_values.screenResizeH);
			#endif

			unsigned char abyte[35];
			fread(abyte, sizeof(unsigned char), 35, fp);
			game_values.spawnstyle = (short) abyte[0];
			game_values.awardstyle = (short) abyte[1];
			game_values.teamcollision = (short)abyte[3];
			game_values.screencrunch = ((short)abyte[4] > 0 ? true : false);
			game_values.toplayer = ((short)abyte[5] > 0 ? true : false);
			game_values.scoreboardstyle = (short)abyte[6];
			game_values.teamcolors = ((short)abyte[7] > 0 ? true : false);
			game_values.sound = ((short)abyte[8] > 0 ? true : false);
			game_values.music = ((short)abyte[9] > 0 ? true : false);
			game_values.musicvolume = (short)abyte[10];
			game_values.soundvolume = (short)abyte[11];
			game_values.respawn = (short)abyte[12];
			game_values.outofboundstime = (short)abyte[15];
			game_values.cpudifficulty = (short)abyte[16];
			game_values.framelimiter = (short)abyte[19];
			game_values.bonuswheel = (short)abyte[20];
			game_values.keeppowerup = ((short)abyte[21] > 0 ? true : false);
			game_values.showwinningcrown = ((short)abyte[22] > 0 ? true : false);
			game_values.playnextmusic = ((short)abyte[23] > 0 ? true : false);
			game_values.pointspeed = (short)abyte[24];
			game_values.swapstyle = (short)abyte[25];
			game_values.overridepowerupsettings = (short)abyte[28];
			game_values.minigameunlocked = ((short)abyte[29] > 0 ? true : false);
			game_values.startgamecountdown = ((short)abyte[30] > 0 ? true : false);
			game_values.deadteamnotice = ((short)abyte[31] > 0 ? true : false);
			game_values.tournamentcontrolstyle = (short)abyte[33];
			game_values.startmodedisplay = ((short)abyte[34] > 0 ? true : false);

			fread(&game_values.shieldtime, sizeof(short), 1, fp);
			fread(&game_values.shieldstyle, sizeof(short), 1, fp);
			fread(&game_values.itemrespawntime, sizeof(short), 1, fp);
			fread(&game_values.hiddenblockrespawn, sizeof(short), 1, fp);
			fread(&game_values.fireballttl, sizeof(short), 1, fp);
			fread(&game_values.fireballlimit, sizeof(short), 1, fp);
			fread(&game_values.hammerdelay, sizeof(short), 1, fp);
			fread(&game_values.hammerttl, sizeof(short), 1, fp);
			fread(&game_values.hammerpower, sizeof(bool), 1, fp);
			fread(&game_values.hammerlimit, sizeof(short), 1, fp);
			fread(&game_values.boomerangstyle, sizeof(short), 1, fp);
			fread(&game_values.boomeranglife, sizeof(short), 1, fp);
			fread(&game_values.boomeranglimit, sizeof(short), 1, fp);
			fread(&game_values.featherjumps, sizeof(short), 1, fp);
			fread(&game_values.featherlimit, sizeof(short), 1, fp);
			fread(&game_values.leaflimit, sizeof(short), 1, fp);
			fread(&game_values.pwingslimit, sizeof(short), 1, fp);
			fread(&game_values.tanookilimit, sizeof(short), 1, fp);
			fread(&game_values.bombslimit, sizeof(short), 1, fp);
			fread(&game_values.wandfreezetime, sizeof(short), 1, fp);
			fread(&game_values.wandlimit, sizeof(short), 1, fp);
			fread(&game_values.shellttl, sizeof(short), 1, fp);
			fread(&game_values.blueblockttl, sizeof(short), 1, fp);
			fread(&game_values.redblockttl, sizeof(short), 1, fp);
			fread(&game_values.grayblockttl, sizeof(short), 1, fp);
			fread(&game_values.storedpowerupdelay, sizeof(short), 1, fp);
			fread(&game_values.warplockstyle, sizeof(short), 1, fp);
			fread(&game_values.warplocktime, sizeof(short), 1, fp);
			fread(&game_values.suicidetime, sizeof(short), 1, fp);

			fread(&game_values.poweruppreset, sizeof(short), 1, fp);
			fread(&g_iCurrentPowerupPresets, sizeof(short), NUM_POWERUP_PRESETS * NUM_POWERUPS, fp);

			//TODO: Need to test what happens when you unplug some controllers from the xbox
			//and then start up (device index will probably point to a gamepad that isn't in the list)
			//and this will cause a crash
			fread(game_values.inputConfiguration, sizeof(CInputPlayerControl), 8, fp);

			//setup player input controls for game
			for(short iPlayer = 0; iPlayer < 4; iPlayer++)
			{
				short iDevice;
				fread(&iDevice, sizeof(short), 1, fp);

#ifdef _XBOX
				game_values.playerInput.inputControls[iPlayer] = &game_values.inputConfiguration[iPlayer][1]; //Always use gamepads as input devices on xbox
#else
				if(iDevice >= joystickcount)
					iDevice = DEVICE_KEYBOARD;

				game_values.playerInput.inputControls[iPlayer] = &game_values.inputConfiguration[iPlayer][iDevice == DEVICE_KEYBOARD ? 0 : 1];
#endif
			}

			#ifndef _XBOX
				fread(&game_values.fullscreen, sizeof(bool), 1, fp);
			#endif
			
			for(short iGameMode = 0; iGameMode < GAMEMODE_LAST; iGameMode++)
				fread(&(gamemodes[iGameMode]->goal), sizeof(short), 1, fp);

			fread(&game_values.gamemodemenusettings, sizeof(GameModeSettings), 1, fp);

			fread(&game_values.teamcounts, sizeof(short), 4, fp);
			fread(&game_values.teamids, sizeof(short), 12, fp);
			fread(&game_values.skinids, sizeof(short), 4, fp);
			fread(&game_values.randomskin, sizeof(bool), 4, fp);
			fread(&game_values.playercontrol, sizeof(short), 4, fp);

			//Load skin/team settings
			for(short iPlayer = 0; iPlayer < 4; iPlayer++)
			{
				if(game_values.skinids[iPlayer] >= skinlist->GetCount() || game_values.skinids[iPlayer] < 0)
					game_values.skinids[iPlayer] = 0;
			}

			announcerlist->SetCurrent((short) abyte[2]);
			musiclist->SetCurrent((short) abyte[13]);
			worldmusiclist->SetCurrent((short) abyte[14]);
			menugraphicspacklist->SetCurrent((short) abyte[17]);
			worldgraphicspacklist->SetCurrent((short) abyte[32]);
			gamegraphicspacklist->SetCurrent((short) abyte[26]);
			soundpacklist->	SetCurrent((short) abyte[18]);

			sfx_setmusicvolume(game_values.musicvolume);
			sfx_setsoundvolume(game_values.soundvolume);
		}
		else
		{
			printf("Old options.bin detected.  Skipped reading it.\n");
		}

		fclose(fp);
	}
	
	//Assign the powerup weights to the selected preset
	for(short iPowerup = 0; iPowerup < NUM_POWERUPS; iPowerup++)
	{
		game_values.powerupweights[iPowerup] = g_iCurrentPowerupPresets[game_values.poweruppreset][iPowerup];
	}

#ifdef _XBOX
	gfx_setresolution(640, 480, false); //Sets flicker filter
	SDL_SetHardwareFilter(game_values.hardwarefilter);
	blitdest = screen;
#else
	if(game_values.fullscreen)
	{
		gfx_setresolution(640, 480, true);
		blitdest = screen;
	}
#endif

	//Calculate the swirl spawn effect locations
	float spawnradius = 100.0f;
	float spawnangle = 0.0f;
	
	for(short i = 0; i < 25; i++)
	{
		g_iSwirlSpawnLocations[0][0][i] = (short)(spawnradius * cos(spawnangle));
		g_iSwirlSpawnLocations[0][1][i] = (short)(spawnradius * sin(spawnangle));
			
		float angle = spawnangle + HALF_PI;
		g_iSwirlSpawnLocations[1][0][i] = (short)(spawnradius * cos(angle));
		g_iSwirlSpawnLocations[1][1][i] = (short)(spawnradius * sin(angle));

		angle = spawnangle + PI;
		g_iSwirlSpawnLocations[2][0][i] = (short)(spawnradius * cos(angle));
		g_iSwirlSpawnLocations[2][1][i] = (short)(spawnradius * sin(angle));

		angle = spawnangle + THREE_HALF_PI;
		g_iSwirlSpawnLocations[3][0][i] = (short)(spawnradius * cos(angle));
		g_iSwirlSpawnLocations[3][1][i] = (short)(spawnradius * sin(angle));

		spawnradius -= 4.0f;
		spawnangle += 0.1f;
	}

	//Load the gfx color palette
	gfx_loadpalette();

	//Call to setup input optimization
	game_values.playerInput.CheckIfMouseUsed();

	srand((unsigned int)time(NULL));

	bool fLoadOK = LoadAndSplashScreen();

	if(!fLoadOK)
	{
		printf("\n---------------- EXIT DURING LOADING ----------------\n\n");
        sfx_close();
		gfx_close();
		//net_close();
		return 0;
	}

	printf("\n---------------- ready, steady, go! ----------------\n");

	g_Menu.CreateMenu();
	g_Menu.RunMenu();

	while(game_values.gamestate != GS_QUIT)
	{
		switch(game_values.gamestate)
		{
			case GS_START_GAME:
			case GS_GAME:
				RunGame();
			break;

			case GS_MENU:
				g_Menu.RunMenu();
			break;

            case GS_START_WORLD:
            case GS_END_GAME:
            case GS_QUIT: // added because of warning on not handling all of enum
            break;
		}
	}	

	printf("\n---------------- shutdown ----------------\n");
	
	for(short i = 0; i < GAMEMODE_LAST; i++)
		delete gamemodes[i];

#ifdef _XBOX
	for(i = 0; i < joystickcount; i++)
		SDL_JoystickClose(joysticks[i]);

	delete[] joysticks;
#endif
	
    sfx_close();
	gfx_close();
	//net_close();

	//netServer.cleanup();
	//netClient.cleanup();

	//Delete player skins
	for(short k = 0; k < 4; k++)
	{
		for(short j = 0; j < PGFX_LAST; j++)
		{
			delete spr_player[k][j];
			delete spr_shyguy[k][j];
			delete spr_chocobo[k][j];
			delete spr_bobomb[k][j];
		}

		delete [] spr_player[k];
		delete [] spr_shyguy[k];
		delete [] spr_chocobo[k];
		delete [] spr_bobomb[k];
		
		delete score[k];
	}

	delete [] game_values.pfFilters;
	delete [] game_values.piFilterIcons;

//Return to dash on xbox
#ifdef _XBOX
	LD_LAUNCH_DASHBOARD LaunchData = { XLD_LAUNCH_DASHBOARD_MAIN_MENU };
	XLaunchNewImage( NULL, (LAUNCH_DATA*)&LaunchData );
#endif

	return 0;
}


/*
#ifdef _XBOX

void reconnectjoysticks()
{
	for(int i = 0; i < joystickcount; i++)
		SDL_JoystickClose(joysticks[i]);

	delete[] joysticks;

	SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
	SDL_InitSubSystem(SDL_INIT_JOYSTICK);

	joystickcount = SDL_NumJoysticks();
	joysticks = new SDL_Joystick*[joystickcount];

	for(i = 0; i < joystickcount; i++)
		joysticks[i] = SDL_JoystickOpen(i);

	SDL_JoystickEventState(SDL_ENABLE);
}

#endif
*/





//-----------------------------------------------------------------------------
// THE GAME LOOP
//-----------------------------------------------------------------------------

bool coldec_player2player(CPlayer * o1, CPlayer * o2);
bool coldec_player2obj(CPlayer * o1, CObject * o2);
bool coldec_obj2obj(CObject * o1, CObject * o2);

extern SDL_Rect iCountDownNumbers[4][4][2];
extern short iCountDownTimes[28];
extern short iCountDownRectSize[28];
extern short iCountDownRectGroup[28];
extern short iCountDownAnnounce[28];

#ifdef _DEBUG
short iSledgeBrotherSetting = 0;
short iSledgeBrotherAttribute = 0;
#endif

void RunGame()
{
	unsigned int	framestart, ticks;
	SDL_Event		event;
	short			i, j;
	float			realfps = 0, flipfps = 0;

	short iCountDownState = 0;
	short iCountDownTimer = 0;

	if(game_values.startgamecountdown && game_values.singleplayermode == -1)
	{
		iCountDownState = 28;
		iCountDownTimer = iCountDownTimes[0];
	}

	//Reset the screen spin variables
	spinangle = 0.0f;
	spinspeed = 0.0f;
	spindirection = 1;
	spintimer = 0;

	//Reset Secret Counters
	for(short iPlayer = 0; iPlayer < 4; iPlayer++)
		game_values.unlocksecret1part1[iPlayer] = false;

	game_values.unlocksecret1part2 = 0;
	game_values.unlocksecret2part1 = false;
	game_values.unlocksecret2part2 = 0;
	game_values.unlocksecret3part1[0] = 0;
	game_values.unlocksecret3part1[1] = 0;
	game_values.unlocksecret3part1[2] = 0;
	game_values.unlocksecret3part1[3] = 0;
	game_values.unlocksecret3part2[0] = 0;
	game_values.unlocksecret3part2[1] = 0;
	game_values.unlocksecret3part2[2] = 0;
	game_values.unlocksecret3part2[3] = 0;
	game_values.unlocksecretunlocked[0] = false;
	game_values.unlocksecretunlocked[1] = false;
	game_values.unlocksecretunlocked[2] = false;
	game_values.unlocksecretunlocked[3] = false;


	//Reset the keys each time we switch from menu to game and back
	game_values.playerInput.ResetKeys();

#ifdef _DEBUG
	static int exitgametimer = 0;
	exitgametimer = 0;
#endif
	
	y_shake = 0;
	x_shake = 0;

	//Create players for this game
	for(short iPlayer = 0; iPlayer < 4; iPlayer++)
	{
		projectiles[iPlayer] = 0;
		respawn[iPlayer] = 0;

		if(game_values.singleplayermode == -1 || game_values.singleplayermode == iPlayer)
		{
			if(game_values.playercontrol[iPlayer] > 0)
			{
				short teamid, subteamid;
				LookupTeamID(iPlayer, &teamid, &subteamid);

				CPlayerAI * ai = NULL;
				if(game_values.playercontrol[iPlayer] == 2)
					ai = new CPlayerAI();

				list_players[list_players_cnt] = new CPlayer(iPlayer, list_players_cnt, teamid, subteamid, game_values.colorids[iPlayer], spr_player[iPlayer], score[teamid], &(respawn[iPlayer]), ai);
				list_players_cnt++;
			}
			else if(!game_values.keeppowerup)
			{
				//Reset off player's stored powerups if they are not playing
				game_values.storedpowerups[iPlayer] = -1;
			}
		}

		//If the gamemode allows stored powerups, then assign the game stored slot to the powerup this player has
		if(game_values.gamemode->HasStoredPowerups())
			game_values.gamepowerups[iPlayer] = game_values.storedpowerups[iPlayer];
		else
		{
			game_values.gamepowerups[iPlayer] = -1;
		}

		game_values.bulletbilltimer[iPlayer] = 0;
		game_values.bulletbillspawntimer[iPlayer] = 0;
	}

	game_values.pausegame = false;
	game_values.exitinggame = false;
	game_values.exityes = false;
	game_values.teamdeadcounter = 0;
	game_values.screenshaketimer = 0;
	game_values.slowdownon = -1;
	game_values.slowdowncounter	= 0;
	game_values.showscoreboard = false;
	game_values.scorepercentmove = 0.0f;
	game_values.playskidsound = false;
	game_values.playinvinciblesound = false;
	game_values.playflyingsound = false;
	game_values.swapplayers = false;
	game_values.swapplayersposition = 0.0f;
	game_values.swapplayersblink = false;
	game_values.swapplayersblinkcount = 0;
	game_values.screenfade = 255;
	game_values.screenfadespeed = -8;
	game_values.noexit = false;
	game_values.noexittimer = 0;
	game_values.forceexittimer = 0;
	game_values.gamewindx = 0.0f;
	game_values.gamewindy = 0.0f;
	
	game_values.windaffectsplayers = false;
	game_values.spinscreen = false;
	game_values.reversewalk = false;
	game_values.spotlights = false;

	//Initialize game mode
	game_values.gamemode->init();

	short totalspace = 0;
	for(i = 0; i < score_cnt; i++)
	{
		totalspace += 56 + game_values.teamcounts[i] * 34;
	}
	totalspace += 20 * (score_cnt - 1);

	for(i = 0; i < score_cnt; i++)
	{
		if(game_values.scoreboardstyle == 0 || game_values.scoreboardstyle == 1)
		{
			score[i]->x = ((640 - totalspace) >> 1);

			for(short k = 0; k < i; k++)
				score[i]->x += 76 + game_values.teamcounts[k] * 34;

			short iScoreOffsetY = 0;
			if(game_values.gamemode->gamemode == game_mode_health || game_values.gamemode->gamemode == game_mode_collection)
				iScoreOffsetY = 18;

			score[i]->y = 5 + (game_values.scoreboardstyle == 1 ? 429 - iScoreOffsetY : 0);
		}
		else
		{
			short iScoreOffsetY = 0;
			if(game_values.gamemode->gamemode == game_mode_health || game_values.gamemode->gamemode == game_mode_collection)
				iScoreOffsetY = 18;

			score[i]->x = 5 + (574 - (34 * game_values.teamcounts[i])) * (i % 2);
			score[i]->y = 5 + (429 - iScoreOffsetY) * (i > 1 ? 1 : 0);
		}

		score[i]->fromx = score[i]->x;
		score[i]->fromy = score[i]->y;
		score[i]->place = i;
		score[i]->order = -1;
	}

	for(short iEyeCandyLayer = 0; iEyeCandyLayer < 3; iEyeCandyLayer++)
	{
		//Clouds
		if(g_map->eyecandy[iEyeCandyLayer] & 1)
		{
			for(i = 0; i < 4; i++)
			{
				float velx;			//speed of cloud, small clouds are slower than big ones
				short srcy, w, h;

				if(rand() % 2)
				{
					velx = (short)(rand() % 51 - 25) / 10.0f;	//big clouds: -3 - +3 pixel/frame
					srcy = 0;
					w = 60;
					h = 28;
				}
				else
				{
					velx = (short)(rand() % 41 - 20) / 10.0f;	//small clouds: -2 - +2 pixel/frame
					srcy = 28;
					w = 28;
					h = 12;
				}
				
				velx = velx < 0.5f && velx > -0.5f ? 1 : velx;	//no static clouds please

				//add cloud to eyecandy array
				eyecandy[iEyeCandyLayer].add(new EC_Cloud(&spr_clouds, (float)(rand()%640), (float)(rand()%100), velx, 0, srcy, w, h));
			}
		}
		
		//Ghosts
		if(g_map->eyecandy[iEyeCandyLayer] & 2)
		{
			for(i = 0; i < 8; i++)
			{
				short iGhostSrcY = (short)(rand() % 3) << 5;	//ghost type
				float velx = (short)(rand() % 51 - 25) / 10.0f;	//big clouds: -3 - +3 pixel/frame
				
				velx = velx < 0.5f && velx > -0.5f ? (rand() % 1 ? 1.0f : -1.0f) : velx;	//no static clouds please

				//add cloud to eyecandy array
				eyecandy[iEyeCandyLayer].add(new EC_Ghost(&spr_ghosts, (float)(rand() % 640), (float)(rand() % 100), velx, 8, 2, velx < 0.0f ? 64 : 0, iGhostSrcY, 32, 32));
			}
		}

		//Leaves
		if(g_map->eyecandy[iEyeCandyLayer] & 4)
		{
			for(i = 0; i < 15; i++)
				eyecandy[iEyeCandyLayer].add(new EC_Leaf(&spr_leaves, (float)(rand() % 640), (float)(rand() % 480)));
		}

		//Snow
		if(g_map->eyecandy[iEyeCandyLayer] & 8)
		{
			for(i = 0; i < 15; i++)
				eyecandy[iEyeCandyLayer].add(new EC_Snow(&spr_snow, (float)(rand() % 640), (float)(rand() % 480), 0));
		}

		//Fish
		short iFishWeights[] = {20, 20, 15, 10, 10, 5, 10, 10};
		short iFishSettings[][4] = { {0, 0, 64, 44}, {0, 44, 64, 44}, {0, 44, 48, 44}, {32, 32, 16, 12}, {32, 44, 16, 12}, {32, 16, 16, 28}, {32, 0, 32, 28}, {32, 44, 32, 28}}; 
		if(g_map->eyecandy[iEyeCandyLayer] & 16)
		{
			for(i = 0; i < 8; i++)
			{
				float velx = (short)(rand() % 41 - 20) / 10.0f;
				velx = velx < 0.5f && velx > -0.5f ? 1.0f : velx; //Keep fish from moving too slowly

				short srcx = iFishSettings[0][0], srcy = iFishSettings[0][1], w = iFishSettings[0][2], h = iFishSettings[0][3];

				short iRandomFish = rand() % 100;
				
				short iFishWeightCount = 0;
				for(short iFish = 0; iFish < 8; iFish++)
				{
					iFishWeightCount += iFishWeights[iFish];

					if(iRandomFish < iFishWeightCount)
					{
						srcx = iFishSettings[iFish][0];
						srcy = iFishSettings[iFish][1];
						w = iFishSettings[iFish][2];
						h = iFishSettings[iFish][3];
						break;
					}
				}

				//add cloud to eyecandy array
				short iPossibleY = (480 - h) / 10;
				float dDestY = (float)(rand() % iPossibleY + iPossibleY * i);
				eyecandy[iEyeCandyLayer].add(new EC_Cloud(&spr_fish, (float)(rand()%640), dDestY, velx, srcx + (velx > 0.0f ? 64 : 0), srcy, w, h));
			}
		}
		
		//Rain
		if(g_map->eyecandy[iEyeCandyLayer] & 32)
		{
			for(i = 0; i < 20; i++)
				eyecandy[iEyeCandyLayer].add(new EC_Rain(&spr_rain, (float)(rand() % 640), (float)(rand() % 480)));
		}

		//Bubbles
		if(g_map->eyecandy[iEyeCandyLayer] & 64)
		{
			for(i = 0; i < 10; i++)
				eyecandy[iEyeCandyLayer].add(new EC_Bubble(&spr_rain, (float)(rand() % 640), (float)(rand() % 480)));
		}
	}
	
	short iScoreTextOffset[4];
	for(short iTeam = 0; iTeam < score_cnt; iTeam++)
	{
		iScoreTextOffset[iTeam] = 34 * game_values.teamcounts[iTeam] + 1;
	}


	short iWindTimer = 0;
	float dNextWind = (float)((rand() % 41) - 20) / 4.0f;
	game_values.gamewindx = (float)((rand() % 41) - 20) / 4.0f;

	//Initialize players after game init has finished
	for(short iPlayer = 0; iPlayer < list_players_cnt; iPlayer++)
		list_players[iPlayer]->Init();
	
	
	//This is the main game loop
	while (true)
	{
		framestart = SDL_GetTicks();

		if(iWindTimer <= 0)
		{
			//Then trigger next wind event
			if(game_values.gamewindx < dNextWind)
			{
				game_values.gamewindx += 0.02f;

				if(game_values.gamewindx >= dNextWind)
					iWindTimer = (rand() % 60) + 30;
			}
			else if(game_values.gamewindx >= dNextWind)
			{
				game_values.gamewindx -= 0.02f;

				if(game_values.gamewindx <= dNextWind)
					iWindTimer = (rand() % 60) + 30;
			}
		}
		else
		{
			if(--iWindTimer <= 0)
			{
				dNextWind = (float)((rand() % 41) - 20) / 4.0f;
			}
		}

/*
#ifdef _XBOX
		if(joystickcount != SDL_NumJoysticks())
			reconnectjoysticks();
#endif
*/
#ifdef _DEBUG

		static short endgametimer = (short)(rand() % 200);
		if(g_fAutoTest && !game_values.swapplayers)
		{
			for(short k = 0; k < list_players_cnt; k++)
			{
				if(list_players[k]->isready())
				{
					//Detect player is in center of tile only
					short x = (list_players[k]->ix + HALFPW) / TILESIZE;
					
					if(list_players[k]->ix + HALFPW >= 640)
						x = (list_players[k]->ix + HALFPW - 640) / TILESIZE;

					short y = (list_players[k]->iy + HALFPH) / TILESIZE;

					int tile = tile_flag_nonsolid;
					IO_Block * block = NULL;
					short blocktype = -1;

					if(list_players[k]->iy + HALFPH >= 0 && list_players[k]->iy + HALFPH < 480)
					{
						tile = g_map->map(x, y);
						block = g_map->block(x, y);
						blocktype = g_map->blockat(x, y)->iType;
					}

					if((tile & tile_flag_solid) || 
						(block && blocktype != 3 && blocktype < 11))
					{
						game_values.pausegame = true;
						game_values.frameadvance = true;
						g_fAutoTest = false;
						break;
					}

					//Detect if any corner of player is in a tile
					short actualvalues[2][2];
					actualvalues[0][0] = list_players[k]->ix;

					if(actualvalues[0][0] < 0)
						actualvalues[0][0] += 640;

					actualvalues[0][1] = list_players[k]->ix + PW;

					if(actualvalues[0][1] >= 640)
						actualvalues[0][1] -= 640;

					actualvalues[1][0] = list_players[k]->iy;
					actualvalues[1][1] = list_players[k]->iy + PH;

					short corners[2][2];
					corners[0][0] = list_players[k]->ix / TILESIZE; 

					if(list_players[k]->ix < 0)
						corners[0][0] = (list_players[k]->ix + 640) / TILESIZE;

					corners[0][1] = (list_players[k]->ix + PW) / TILESIZE; 

					if(list_players[k]->ix + PW >= 640)
						corners[0][1] = (list_players[k]->ix + PW - 640) / TILESIZE;

					corners[1][0] = list_players[k]->iy / TILESIZE;
					corners[1][1] = (list_players[k]->iy + PH) / TILESIZE;

					for(short i = 0; i < 2; i++)
					{
						for(short j = 0; j < 2; j++)
						{
							int tile = tile_flag_nonsolid;
							IO_Block * block = NULL;
							short blocktype = -1;

							if(actualvalues[0][j] >= 0 && actualvalues[0][j] < 640 && actualvalues[1][i] > 0 && actualvalues[1][i] < 480)
							{
								tile = g_map->map(corners[0][j], corners[1][i]);
								block = g_map->block(corners[0][j], corners[1][i]);
								blocktype = g_map->blockat(corners[0][j], corners[1][i])->iType;
							}

							if( (tile & tile_flag_solid) || 
								(block && blocktype != 3 && blocktype < 11))
							{
								game_values.pausegame = true;
								game_values.frameadvance = true;
								g_fAutoTest = false;
								break;
							}
						}

						if(game_values.pausegame)
							break;
					}
				}
				
				if(game_values.pausegame)
					break;
			}
			
			/*
			//Kill off players to test spawning
			static short chooseplayer = 0;
			static short killtimer = 0;
			if(++killtimer > 20)
			{
				killtimer = 0;

				if(++chooseplayer >= list_players_cnt)
					chooseplayer = 0;

				list_players[chooseplayer]->DeathAwards();

				if( !game_values.gamemode->playerkilledself(*(list_players[chooseplayer])) )
				{
					list_players[chooseplayer]->die(0, false);
				}
				
			}
			*/
			
			//Automatically run menus
			if(game_values.showscoreboard)
			{
				if(--endgametimer < 0)
				{
					endgametimer = (short)(rand() % 200);

					if(game_values.matchtype != MATCH_TYPE_SINGLE_GAME && game_values.matchtype != MATCH_TYPE_QUICK_GAME && game_values.matchtype != MATCH_TYPE_MINIGAME)
						UpdateScoreBoard();

					CleanUp();
					game_values.gamestate = GS_MENU;

					return;
				}
			}
			else
			{
				if(++exitgametimer >= 8000)
				{
					CleanUp();
					game_values.exitinggame = false;
					game_values.exityes = false;
					game_values.gamestate = GS_MENU;

					return;
				}
			}
		}
		
#endif

		game_values.playerInput.ClearPressedKeys(game_values.exitinggame ? 1 : 0);
		//handle messages

		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
#ifndef _XBOX
				case SDL_QUIT:
				{
					CleanUp();
					game_values.gamestate = GS_QUIT;
					return;
				}
				break;
#endif
				case SDL_KEYDOWN:
				{
#ifndef _XBOX
					if(event.key.keysym.mod & (KMOD_LALT | KMOD_RALT))
					{
						if(event.key.keysym.sym == SDLK_F4)
						{
							CleanUp();
							game_values.gamestate = GS_QUIT;
							return;
						}
						else if(event.key.keysym.sym == SDLK_RETURN)
						{
							game_values.fullscreen = !game_values.fullscreen;
							gfx_setresolution(640, 480, game_values.fullscreen);
							blitdest = screen;

							g_Menu.miFullscreenField->SetKey(game_values.fullscreen ? 1 : 0);
							
							//Continue with input -> don't feed this event to the input
							//otherwise it will pause the game when switching to full/windowed screen
							continue;
						}
					}
#endif
					if(event.key.keysym.sym == SDLK_F1)
					{
						game_values.showfps = !game_values.showfps;
					}
					else if(event.key.keysym.sym == SDLK_ESCAPE)
					{
						game_values.playerInput.outputControls[0].game_cancel.fPressed = true;
					}
					else if(event.key.keysym.sym == SDLK_TAB)
					{
						PlayNextMusicTrack();
					}
#ifdef _DEBUG
					else if(event.key.keysym.sym == SDLK_LEFTBRACKET)
					{
						game_values.framelimiter++;
					}
					else if(event.key.keysym.sym == SDLK_RIGHTBRACKET)
					{
						if(game_values.framelimiter > 0)
							game_values.framelimiter--;
					}
					else if(event.key.keysym.sym == SDLK_F2)
					{
						game_values.frameadvance = !game_values.frameadvance;
					}
					else if(event.key.keysym.sym == SDLK_F5)
					{
						game_values.autokill = !game_values.autokill;
					}
					else if(event.key.keysym.sym == SDLK_z)
					{
						for(short k = 0; k < list_players_cnt; k++)
						{
							list_players[k]->DeathAwards();

							if(game_values.gamemode->playerkilledself(*(list_players[k]), kill_style_environment) == player_kill_normal)
								list_players[k]->die(0, false, false);
						}
					}
					else if(event.key.keysym.sym == SDLK_x)
					{
						short iplayer = rand() % list_players_cnt;
						list_players[iplayer]->makefrozen(300);
					}
					else if(event.key.keysym.sym == SDLK_c)
					{
						short iplayer = rand() % list_players_cnt;
						list_players[iplayer]->shield = rand() % 3 + 1;
						list_players[iplayer]->shieldtimer = 620;
					}
					else if(event.key.keysym.sym == SDLK_1)
					{
						if(event.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL))
							objectcontainer[0].add(new PU_IceWandPowerup(&spr_icewandpowerup, list_players[0]->ix + 32, list_players[0]->iy, 1, 0, 30, 30, 1, 1));
						else if(event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT))
							objectcontainer[0].add(new PU_BobombPowerup(&spr_bobombpowerup, list_players[0]->ix + 32, list_players[0]->iy - 1, 1, true, 0, 30, 30, 1, 1));	
						else
							objectcontainer[0].add(new PU_StarPowerup(&spr_starpowerup, list_players[0]->ix + 32, list_players[0]->iy, 4, true, 2, 30, 30, 1, 1));
					}
					else if(event.key.keysym.sym == SDLK_2)
					{
						if(event.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL))
							objectcontainer[0].add(new PU_BombPowerup(&spr_bombpowerup, list_players[0]->ix + 32, list_players[0]->iy, 1, 0, 30, 30, 1, 1));
						else if(event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT))
							objectcontainer[0].add(new PU_PowPowerup(&spr_powpowerup, list_players[0]->ix + 32, list_players[0]->iy - 1, 8, true, 8, 30, 30, 1, 1));
						else
							objectcontainer[0].add(new PU_ExtraGuyPowerup(&spr_1uppowerup, list_players[0]->ix + 32, list_players[0]->iy, 1, true, 0, 30, 30, 1, 1, 1));
					}
					else if(event.key.keysym.sym == SDLK_3)
					{
						if(event.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL))
							objectcontainer[0].add(new PU_PodoboPowerup(&spr_podobopowerup, list_players[0]->ix + 32, list_players[0]->iy, 1, 0, 30, 30, 1, 1));
						else if(event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT))
							objectcontainer[0].add(new PU_BulletBillPowerup(&spr_bulletbillpowerup, list_players[0]->ix + 32, list_players[0]->iy - 1, 1, true, 0, 30, 30, 1, 1));
						else
							objectcontainer[0].add(new PU_ExtraGuyPowerup(&spr_2uppowerup, list_players[0]->ix + 32, list_players[0]->iy, 1, true, 0, 30, 30, 1, 1, 2));
					}
					else if(event.key.keysym.sym == SDLK_4)
					{
						if(event.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL))
							objectcontainer[0].add(new PU_Tanooki(list_players[0]->ix + 32, list_players[0]->iy));
						else if(event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT))
							objectcontainer[1].add(new CO_Shell(0, list_players[0]->ix + 32, list_players[0]->iy, true, true, true, false));
						else
							objectcontainer[0].add(new PU_ExtraGuyPowerup(&spr_3uppowerup, list_players[0]->ix + 32, list_players[0]->iy, 1, true, 0, 30, 30, 1, 1, 3));
					}
					else if(event.key.keysym.sym == SDLK_5)
					{
						if(event.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL))
							objectcontainer[0].add(new PU_PWingsPowerup(&spr_pwingspowerup, list_players[0]->ix + 32, list_players[0]->iy));
						else if(event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT))
							objectcontainer[1].add(new CO_Shell(1, list_players[0]->ix + 32, list_players[0]->iy, false, true, true, false));
						else
							objectcontainer[0].add(new PU_ExtraGuyPowerup(&spr_5uppowerup, list_players[0]->ix + 32, list_players[0]->iy, 1, true, 0, 30, 30, 1, 1, 5));
					}
					else if(event.key.keysym.sym == SDLK_6)
					{
						if(event.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL))
							objectcontainer[1].add(new CO_Spring(&spr_spring, list_players[0]->ix + 32, list_players[0]->iy, true));
						else if(event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT))
							objectcontainer[1].add(new CO_Shell(2, list_players[0]->ix + 32, list_players[0]->iy, false, false, true, true));
						else
							objectcontainer[0].add(new PU_FirePowerup(&spr_firepowerup, list_players[0]->ix + 32, list_players[0]->iy, 1, true, 0, 30, 30, 1, 1));
					}
					else if(event.key.keysym.sym == SDLK_7)
					{
						if(event.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL))
							objectcontainer[1].add(new CO_ThrowBox(&spr_throwbox, list_players[0]->ix + 32, list_players[0]->iy, (rand() % NUM_POWERUPS + 3) - 3));
						else if(event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT))
							objectcontainer[1].add(new CO_Shell(3, list_players[0]->ix + 32, list_players[0]->iy, false, true, false, false));
						else
							objectcontainer[0].add(new PU_HammerPowerup(&spr_hammerpowerup, list_players[0]->ix + 32, list_players[0]->iy, 1, true, 0, 30, 30, 1, 1));
					}
					else if(event.key.keysym.sym == SDLK_8)
					{
						if(event.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL))
							objectcontainer[1].add(new CO_Spike(&spr_spike, list_players[0]->ix + 32, list_players[0]->iy));
						else if(event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT))
							objectcontainer[0].add(new PU_ModPowerup(&spr_modpowerup, list_players[0]->ix + 32, list_players[0]->iy, 8, true, 8, 30, 30, 1, 1));
						else
							objectcontainer[0].add(new PU_PoisonPowerup(&spr_poisonpowerup, list_players[0]->ix + 32, list_players[0]->iy, 1, true, 0, 30, 30, 1, 1));
					}
					else if(event.key.keysym.sym == SDLK_9)
					{
						if(event.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL))
							objectcontainer[1].add(new CO_KuriboShoe(&spr_kuriboshoe, list_players[0]->ix + 32, list_players[0]->iy, true));
						else if(event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT))
							objectcontainer[0].add(new PU_FeatherPowerup(&spr_featherpowerup, list_players[0]->ix + 32, list_players[0]->iy - 1, 1, 0, 30, 30, 1, 1));
						else
							objectcontainer[0].add(new PU_ClockPowerup(&spr_clockpowerup, list_players[0]->ix + 32, list_players[0]->iy - 1, 1, true, 0, 30, 30, 1, 1));
					}
					else if(event.key.keysym.sym == SDLK_0)
					{
						if(event.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL))
							objectcontainer[0].add(new PU_LeafPowerup(&spr_leafpowerup, list_players[0]->ix + 32, list_players[0]->iy - 1, 1, 0, 30, 30, 1, 1));
						else if(event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT))
							objectcontainer[0].add(new PU_BoomerangPowerup(&spr_boomerangpowerup, list_players[0]->ix + 32, list_players[0]->iy - 1, 1, true, 0, 30, 30, 1, 1));
						else
							objectcontainer[0].add(new PU_MysteryMushroomPowerup(&spr_mysterymushroompowerup, list_players[0]->ix + 32, list_players[0]->iy - 1, 1, true, 0, 30, 30, 1, 1));
					}
					else if(event.key.keysym.sym == SDLK_INSERT)
					{
						g_fAutoTest = !g_fAutoTest;
					}
#endif
					break;
				}	

#ifdef _XBOX
				case SDL_JOYBUTTONDOWN:
				{
					if(event.jbutton.state == SDL_PRESSED && event.jbutton.button == 5)
					{
						PlayNextMusicTrack();
					}

					break;
				}
#endif

				default:
					break;
			}

			//Feed the player control structures with input data
			//Use menu controls when exit game dialog is up
			game_values.playerInput.Update(event, (game_values.exitinggame ? 1 : 0));
		}

		if(game_values.screenfade == 0 && iCountDownState <= COUNTDOWN_START_INDEX)
		{
			//If the cancel button is pressed
			if(game_values.forceexittimer > 0)
			{
				if(--game_values.forceexittimer <= 0)
				{
					game_values.gamestate = GS_END_GAME;
					game_values.screenfade = 8;
					game_values.screenfadespeed = 8;
				}
			}

			for(int iPlayer = 0; iPlayer < 4; iPlayer++)
			{
				COutputControl * playerKeys = &game_values.playerInput.outputControls[iPlayer];

				//If the start key is pressed (pause key)
				if(playerKeys->game_start.fPressed && IsPauseAllowed())
				{
					if(!game_values.showscoreboard && !game_values.exitinggame)
					{
						game_values.pausegame = !game_values.pausegame;

						if(game_values.pausegame)
						{
							menu_shade.setalpha(GetScreenBackgroundFade());
							menu_shade.draw(0, 0);

							//Stop the pwings sound if it is on
							if(sfx_flyingsound.isplaying())
								ifsoundonstop(sfx_flyingsound);
						}

						//ifsoundonpause(sfx_invinciblemusic);
						//ifsoundonpause(sfx_slowdownmusic);
						ifsoundonplay(sfx_pause);
					}
				}

				//Only player 1 is allowed to exit a game
				//if(iPlayer != 0)
				//	continue;

				if((playerKeys->game_cancel.fPressed || (playerKeys->game_start.fPressed && game_values.gamemode->gameover)) && IsExitAllowed())
				{
					if(game_values.gamemode->gameover)
					{
						if(game_values.matchtype == MATCH_TYPE_TOUR || game_values.matchtype == MATCH_TYPE_TOURNAMENT)
							UpdateScoreBoard();

						CleanUp();
						game_values.gamestate = GS_MENU;

						return;
					}
					else
					{
						if(!game_values.pausegame && !game_values.exitinggame)
						{
							menu_shade.setalpha(GetScreenBackgroundFade());
							menu_shade.draw(0, 0);
							game_values.exitinggame = true;
							//ifsoundonpause(sfx_invinciblemusic);
							//ifsoundonpause(sfx_slowdownmusic);

							//Reset the keys each time we switch from menu to game and back
							game_values.playerInput.ResetKeys();
						}
					}
				}

				//Deal with input to game exit dialog box
				if(game_values.exitinggame)
				{
					if(playerKeys->menu_left.fPressed)
						game_values.exityes = true;
					else if(playerKeys->menu_right.fPressed)
						game_values.exityes = false;

					if(playerKeys->menu_select.fPressed)
					{
						if(game_values.exityes)
						{
							CleanUp();
							game_values.exitinggame = false;
							game_values.exityes = false;
							game_values.gamestate = GS_MENU;
							return;
						}
						else
						{
							game_values.exitinggame = false;
							//ifsoundonpause(sfx_invinciblemusic);
							//ifsoundonpause(sfx_slowdownmusic);

							//Reset the keys each time we switch from menu to game and back
							game_values.playerInput.ResetKeys();
						}
					}
				}
			}
		}

		if(!game_values.pausegame && !game_values.exitinggame)
		{
			if(!game_values.swapplayers && game_values.screenfade == 0)
			{
				//Count down start timer before each game
				if(iCountDownState > 0 && --iCountDownTimer <= 0)
				{
					//There is one extra count here so we hit all the numbers.  When we
					//reach 0 that means we are done counting (index 28 would be out of
					//bounds on this array)
					if(--iCountDownState != 0)
					{
						//28 is the magic number as there are 28 frames of animation
						//spread over a few seconds for the countdown numbers
						iCountDownTimer = iCountDownTimes[28 - iCountDownState];

						short countDownAnnounce = iCountDownAnnounce[28 - iCountDownState];
						if(countDownAnnounce >= 0)
							ifsoundonandreadyplay(sfx_announcer[countDownAnnounce]);
					}
				}

				//Make updates to background stuff (animate map while countdown is counting)
				if(iCountDownState > COUNTDOWN_START_INDEX)
				{
					//Move platforms
					g_map->updatePlatforms();

					//Keep updating map hazards
					objectcontainer[0].cleandeadobjects();
					objectcontainer[1].cleandeadobjects();
					objectcontainer[2].cleandeadobjects();
					noncolcontainer.cleandeadobjects();
					
					noncolcontainer.update();
					objectcontainer[0].update();
					objectcontainer[1].update();
					objectcontainer[2].update();

					eyecandy[0].cleandeadobjects();
					eyecandy[1].cleandeadobjects();
					eyecandy[2].cleandeadobjects();

					eyecandy[0].update();
					eyecandy[1].update();
					eyecandy[2].update();
					
					g_map->update();
				}
				else
				{
					//Shake screen

					if(game_values.screenshaketimer > 0)
					{
						game_values.screenshaketimer--;

						static bool shakeleft = false;
						if(shakeleft)
						{
							x_shake -= 2;
							if(x_shake <= -2)
							{
								shakeleft = false;
							}
						}
						else
						{
							x_shake += 2;
							if(x_shake >= 2)
							{
								shakeleft = true;
							}
						}

						//Kill players touching the ground (or in air for MOd blocks)
						short iNumKillPlayers = 0;
						CPlayer * pKillPlayers[4];

						CPlayer * killer = GetPlayerFromGlobalID(game_values.screenshakeplayerid);

						for(short k = 0; k < list_players_cnt; k++)
						{
							CPlayer * player = list_players[k];

							//Don't kill the player that triggered the POW/MOd
							if(player->globalID == game_values.screenshakeplayerid)
								continue;

							//Don't kill players on his team either (if friendly fire is off)
							if(game_values.teamcollision != 2 && game_values.screenshaketeamid == player->teamID)
								continue;
							
							//Kill other players
							if(!player->invincible && player->shield == 0 && player->iKuriboShoe == 0 && player->isready())
							{
								if(game_values.screenshakekillinair == player->inair)
								{
									pKillPlayers[iNumKillPlayers++] = player;
									
									if(killer)
									{
										game_values.screenshakekillscount++;
										
										if(killer->inair)
											killer->killsinrowinair--;  //Don't want to give both shake and in air award
									}
								}
							}
						}
						
						//Randomize the order in which the players are killed (so that game modes where order matters is fair)
						if(iNumKillPlayers > 0)
						{
							short iRandPlayer = rand() % iNumKillPlayers;
							for(short iPlayer = 0; iPlayer < iNumKillPlayers; iPlayer++)
							{
								PlayerKilledPlayer(game_values.screenshakeplayerid, pKillPlayers[iRandPlayer], death_style_jump, kill_style_pow, false, false);

								if(++iRandPlayer >= iNumKillPlayers)
									iRandPlayer = 0;
							}
						}

						//Kill goombas and koopas
						for(short k = 0; k < objectcontainer[0].list_end; k++)
						{
							CObject * object = objectcontainer[0].list[k];
							if(object->getObjectType() == object_moving)
							{
								IO_MovingObject * movingobject = (IO_MovingObject *)object;
								MovingObjectType type = movingobject->getMovingObjectType();
								
								if((type == movingobject_goomba || type == movingobject_koopa || type == movingobject_buzzybeetle || type == movingobject_spiny)
									&& game_values.screenshakekillinair == movingobject->inair)
								{
									CPlayer * killer = GetPlayerFromGlobalID(game_values.screenshakeplayerid);

									if(killer)
									{
										if(!game_values.gamemode->gameover)
											killer->score->AdjustScore(1);

										ifsoundonplay(sfx_kicksound);
										((MO_WalkingEnemy*)movingobject)->DieAndDropShell(true, true);

										game_values.screenshakekillscount++;
										
										if(killer->inair)
											killer->killsinrowinair--;  //Don't want to give both shake and in air award
									}
								}
							}
						}

						//Destroy throw blocks and flip shells over
						for(short k = 0; k < objectcontainer[1].list_end; k++)
						{
							CObject * object = objectcontainer[1].list[k];
							if(object->getObjectType() == object_moving)
							{
								IO_MovingObject * movingobject = (IO_MovingObject *)object;
								
								if(game_values.screenshakekillinair == movingobject->inair)
								{
									if(movingobject->getMovingObjectType() == movingobject_shell)
									{
										CO_Shell * shell = (CO_Shell*)movingobject;
										if(shell->frozen || !shell->owner || shell->owner->inair == game_values.screenshakekillinair)
											shell->Flip();  //also breaks shells if frozen
									}
									else if(movingobject->getMovingObjectType() == movingobject_throwblock)
									{
										CO_ThrowBlock * throwblock = (CO_ThrowBlock*)movingobject;
										if(throwblock->frozen || !throwblock->owner || throwblock->owner->inair == game_values.screenshakekillinair)
											throwblock->Die();
									}
									else if(movingobject->getMovingObjectType() == movingobject_throwbox)
									{
										CO_ThrowBox * throwbox = (CO_ThrowBox*)movingobject;
										if(throwbox->frozen)
											throwbox->Die();
									}
									else if(movingobject->getMovingObjectType() == movingobject_pirhanaplant)
									{
										MO_PirhanaPlant * plant = (MO_PirhanaPlant*)movingobject;
										plant->KillPlant();
									}
									else if(movingobject->getMovingObjectType() == movingobject_bulletbill)
									{
										MO_BulletBill * bulletbill = (MO_BulletBill*)movingobject;
										bulletbill->Die();
									}
								}
							}
						}

						//Add kills in row for kills from pow and mod
						if(game_values.screenshakekillscount > 1 && game_values.awardstyle != award_style_none)
						{
							game_values.screenshakekillscount = 0;

							CPlayer * killer = GetPlayerFromGlobalID(game_values.screenshakeplayerid);

							if(killer)
								killer->AddKillsInRowInAirAward();
						}
					}
					else
					{
						//Make sure we zero out the shake value after it is done
						x_shake = 0;
					}

					if(game_values.spinscreen)
					{
						SpinScreen();
					}
					
					for(short iPlayer = 0; iPlayer < 4; iPlayer++)
					{
						if(game_values.bulletbilltimer[iPlayer] > 0)
						{
							game_values.bulletbilltimer[iPlayer]--;
							
							if(--game_values.bulletbillspawntimer[iPlayer] <= 0)
							{
								game_values.bulletbillspawntimer[iPlayer] = (short)(rand() % 20 + 25);
								float speed = ((float)(rand() % 21 + 20)) / 10.0f;
								objectcontainer[2].add(new MO_BulletBill(&spr_bulletbill, &spr_bulletbilldead, 0, (short)(rand() % 448), (rand() % 2 ? speed : -speed), iPlayer, false));
								ifsoundonplay(sfx_bulletbillsound);
							}
						}
					}

					if(game_values.matchtype == MATCH_TYPE_WORLD && game_values.gamemode->gameover && game_values.forceexittimer <= 0)
					{
						if(--game_values.noexittimer <= 0)
							game_values.noexit = false;
					}

					//------------- update objects -----------------------

	#ifdef _DEBUG
					if(game_values.autokill)
					{
						for(short k = 0; k < list_players_cnt; k++)
						{
							list_players[k]->DeathAwards();

							if(!game_values.gamemode->playerkilledself(*(list_players[k]), kill_style_environment))
								list_players[k]->die(0, false, false);
						}
					}
	#endif

					//Advance the cpu's turn (AI only calculates player's actions 1 out of 4 frames)
					if(++game_values.cputurn > 3)
						game_values.cputurn = 0;

					//Player to player collisions
					for(i = 0; i < list_players_cnt; i++)
					{
						CPlayer * player1 = list_players[i];
						if(player1->state > player_dead)
						{
							for(j = i + 1; j < list_players_cnt; j++)
							{
								CPlayer * player2 = list_players[j];
								if(player2->state > player_dead)
								{
									if(coldec_player2player(player1, player2))
									{
										collisionhandler_p2p(player1, player2);

										//if player was killed by another player, continue with next player for collision detection
										if(player1->state <= player_dead)
											break;
									}
								}
							}
						}
					}
					
					//Move platforms
					g_map->updatePlatforms();

					game_values.playskidsound = false;
					game_values.playinvinciblesound = false;
					game_values.playflyingsound = false;

					for(i = 0; i < list_players_cnt; i++)
						list_players[i]->move();	//move all objects before doing object-object collision detection in
													//->think(), so we test against the new position after object-map collision detection

					//Play sound for skidding players
					if(game_values.playskidsound)
					{
						if(!sfx_skid.isplaying())
							ifsoundonplay(sfx_skid);
					}
					else
					{
						if(sfx_skid.isplaying())
							ifsoundonstop(sfx_skid);
					}

					//Play sound for players using PWings
					if(game_values.playflyingsound)
					{
						if(!sfx_flyingsound.isplaying())
							ifsoundonplay(sfx_flyingsound);
					}
					else
					{
						if(sfx_flyingsound.isplaying())
							ifsoundonstop(sfx_flyingsound);
					}

					noncolcontainer.update();
					objectcontainer[0].update();
					objectcontainer[1].update();
					objectcontainer[2].update();

					//Collide player with objects
					for(short iPlayer = 0; iPlayer < list_players_cnt; iPlayer++)
					{
						CPlayer * player = list_players[iPlayer];
						if(player->state != player_ready)
							continue;

						//Collide with objects
						for(short iLayer = 0; iLayer < 3; iLayer++)
						{
							for(short iObject = 0; iObject < objectcontainer[iLayer].list_end; iObject++)
							{
								CObject * object = objectcontainer[iLayer].list[iObject];

								if(!object->GetDead())
								{
									if(coldec_player2obj(player, object))
									{
										if(collisionhandler_p2o(player, object))
											break;
									}
								}
							}

							//if the object killed the player, then continue with the other players
							if(player->state != player_ready)
								break;

							//If player collided with a swap mushroom, the break from colliding with everything else
							if(game_values.swapplayers)
								goto SWAPBREAK;
						}
					}

					for(short iLayer1 = 0; iLayer1 < 3; iLayer1++)
					{
						short iContainerEnd1 = objectcontainer[iLayer1].list_end;
						for(short iObject1 = 0; iObject1 < iContainerEnd1; iObject1++)
						{
							CObject * object1 = objectcontainer[iLayer1].list[iObject1];

							if(object1->getObjectType() != object_moving)
								continue;

							IO_MovingObject * movingobject1 = (IO_MovingObject*)object1;

							for(short iLayer2 = iLayer1; iLayer2 < 3; iLayer2++)
							{
								short iContainerEnd2 = objectcontainer[iLayer2].list_end;
								for(short iObject2 = (iLayer1 == iLayer2 ? iObject1 + 1 : 0); iObject2 < iContainerEnd2; iObject2++)
								{
									CObject * object2 = objectcontainer[iLayer2].list[iObject2];

									if(object2->getObjectType() != object_moving)
										continue;

									IO_MovingObject * movingobject2 = (IO_MovingObject*)object2;

									//if(g_iCollisionMap[movingobject1->getMovingObjectType()][movingobject2->getMovingObjectType()])
									//	continue;

									//if(iLayer1 == iLayer2 && iObject1 == iObject2)
									//	continue;

									if(object2->GetDead())
										continue;

									MovingObjectType iType1 = movingobject1->getMovingObjectType();
									MovingObjectType iType2 = movingobject2->getMovingObjectType();
									if(g_iCollisionMap[iType1][iType2])
									{
										if(coldec_obj2obj(movingobject1, movingobject2))
										{
											collisionhandler_o2o(movingobject1, movingobject2);
										}
									}
									else if(g_iCollisionMap[iType2][iType1])
									{
										if(coldec_obj2obj(movingobject2, movingobject1))
										{
											collisionhandler_o2o(movingobject2, movingobject1);
										}
									}

									if(object1->GetDead())
										goto CONTINUEOBJECT1;
								}
							}

							//Labeled break
							CONTINUEOBJECT1:
							continue;
						}
					}

					//Commit all player actions at this point (after we have collided with any objects
					//that the player might have picked up)
					for(i = 0; i < list_players_cnt; i++)
						list_players[i]->CommitAction();

					eyecandy[0].cleandeadobjects();
					eyecandy[1].cleandeadobjects();
					eyecandy[2].cleandeadobjects();

					objectcontainer[2].cleandeadobjects();
					objectcontainer[1].cleandeadobjects();
					objectcontainer[0].cleandeadobjects();
					noncolcontainer.cleandeadobjects();
					
					CleanDeadPlayers();

					eyecandy[0].update();
					eyecandy[1].update();
					eyecandy[2].update();

					game_values.gamemode->think();

					if(game_values.slowdownon != -1 && ++game_values.slowdowncounter > 580)
					{
						game_values.slowdownon = -1;
						game_values.slowdowncounter = 0;
					}

					g_map->update();

					if(y_shake > 0 && !game_values.spinscreen)
					{
						y_shake -= CRUNCHVELOCITY;

						if(y_shake < 0)
							y_shake = 0;
					}
				
					if(game_values.showscoreboard)
					{
						if(game_values.scorepercentmove < 1.0f)
						{
							game_values.scorepercentmove += 0.01f;
							
							if(game_values.scorepercentmove >= 1.0f)
								game_values.scorepercentmove = 1.0f;
						}
						else
						{
							game_values.scorepercentmove = 1.0f;
						}

						for(i = 0; i < score_cnt; i++)
						{
							score[i]->x = (short)((float)(score[i]->destx - score[i]->fromx) * game_values.scorepercentmove) + score[i]->fromx;
							score[i]->y = (short)((float)(score[i]->desty - score[i]->fromy) * game_values.scorepercentmove) + score[i]->fromy;
						}
					}
				}
			}
			//Go to this label if a player collects a swap mushroom and we need to break out of two loops
			SWAPBREAK:

			if(game_values.swapplayers)
			{
				for(i = 0; i < list_players_cnt; i++)
				{
					list_players[i]->updateswap();
				}
			}

			if(game_values.screenfade == 255)
			{
				if(game_values.gamestate == GS_START_GAME)
				{
					CleanUp();
					SetGameModeSettingsFromMenu();
					game_values.gamestate = GS_GAME;
					
					/*
					//This code will help stop a game midway and load a new map and mode
					//It was used for the secret boss mode in 1.7 AFE
					game_values.gamemode = bossgamemode;  //boss type has already been set at this point
					bossgamemode->SetBossType(0);

					if(bossgamemode->GetBossType() == 0)
						g_map->loadMap(convertPath("maps/special/dungeon.map"), read_type_full);
					else if(bossgamemode->GetBossType() == 1)
						g_map->loadMap(convertPath("maps/special/hills.map"), read_type_full);
					else if(bossgamemode->GetBossType() == 2)
						g_map->loadMap(convertPath("maps/special/volcano.map"), read_type_full);

					LoadCurrentMapBackground();

					g_map->predrawbackground(spr_background, spr_backmap[0]);
					g_map->predrawforeground(spr_frontmap[0]);
					
					g_map->predrawbackground(spr_background, spr_backmap[1]);
					g_map->predrawforeground(spr_frontmap[1]);

					g_map->SetupAnimatedTiles();
					LoadMapObjects(false);
					///////////////////
					*/

					if(game_values.music)
					{
						musiclist->SetRandomMusic(g_map->musicCategoryID, "", "");
						backgroundmusic[0].load(musiclist->GetCurrentMusic());
						backgroundmusic[0].play(game_values.playnextmusic, false);
					}

					return;
				}
				else if(game_values.gamestate == GS_END_GAME)
				{
					CleanUp();
					game_values.gamestate = GS_MENU;
					game_values.screenfadespeed = -8;

					return;
				}
			}

			//--------------- draw everything ----------------------
			spr_backmap[g_iCurrentDrawIndex].draw(0, 0);

			//draw back eyecandy behind players
			g_map->drawPlatforms(0);

			eyecandy[0].draw();
			noncolcontainer.draw();
			
			game_values.gamemode->draw_background();

			objectcontainer[0].draw();

			g_map->drawPlatforms(1);

			if(!game_values.swapplayers)
			{
				for(i = 0; i < list_players_cnt; i++)
					list_players[i]->draw();
			}

			eyecandy[1].draw();

			objectcontainer[1].draw();

			g_map->drawPlatforms(2);

#ifdef _XBOX
			g_map->drawfrontlayer();
#else
			if(game_values.toplayer)
				g_map->drawfrontlayer();
#endif
			g_map->drawWarpLocks();

			g_map->drawPlatforms(3);

			objectcontainer[2].draw();
			eyecandy[2].draw();
			game_values.gamemode->draw_foreground();
		
			g_map->drawPlatforms(4);

			if(game_values.spotlights)
				spotlightManager.DrawSpotlights();

			g_iWinningPlayer = -1;
			
			//Draw scoreboards for all games (except special cases where we have a single player walking the map)
			if(game_values.singleplayermode == -1)
			{
				short highestScore = 0;

				bool fReverseScoring = game_values.gamemode->GetReverseScoring();
				if(fReverseScoring)
					highestScore = 32000;
				
				for(i = 0; i < score_cnt; i++)
				{
					int scoreValue = score[i]->score;
					if(game_values.gamemode->gamemode == game_mode_boxes_minigame)
						scoreValue = score[i]->subscore[0];
					
					if((scoreValue > highestScore && !fReverseScoring) || (scoreValue < highestScore && fReverseScoring))
					{
						highestScore = scoreValue;
						g_iWinningPlayer = i;
					}
					else if(scoreValue == highestScore)
					{
						g_iWinningPlayer = -1;
					}
				}
				
				//big end game scoreboard (sorted)
				if(game_values.showscoreboard)
				{
					char gameovertext[128] = "";
					if(game_values.gamemode->winningteam > -1)
					{
						if(game_values.teamcounts[game_values.gamemode->winningteam] == 1)
							sprintf(gameovertext, "Player %d Wins!", game_values.teamids[game_values.gamemode->winningteam][0] + 1);
						else
							sprintf(gameovertext, "Team %d Wins!", game_values.gamemode->winningteam + 1);
					}
					else
					{
						sprintf(gameovertext, "Tie Game");
					}
						
					game_font_large.drawCentered(320, 90, gameovertext);

				}

				//in game scoreboards
				for(i = 0; i < score_cnt; i++)
				{
					if(game_values.gamemode->gamemode == game_mode_health || game_values.gamemode->gamemode == game_mode_collection || game_values.gamemode->gamemode == game_mode_boxes_minigame)
						spr_shade[game_values.teamcounts[i] - 1].draw(score[i]->x, score[i]->y);
					else
						spr_shade[game_values.teamcounts[i] - 1].draw(score[i]->x, score[i]->y, 0, 0, 256, 41);

					for(short k = 0; k < game_values.teamcounts[i]; k++)
					{
						short globalID = game_values.teamids[i][k];

						//If player is respawning, draw an animated egg counter
						if(respawn[globalID] > 0 && !game_values.gamemode->gameover)
						{
							if(++respawnanimationtimer[globalID] > 8)
							{
								respawnanimationtimer[globalID] = 0;
								respawnanimationframe[globalID] += 32;

								if(respawnanimationframe[globalID] > 32)
									respawnanimationframe[globalID] = 0;
							}

							short scorex = score[i]->x + scoreoffsets[k];
							short scorey = score[i]->y + 2;
							spr_egg.draw(scorex, scorey, respawnanimationframe[globalID], game_values.colorids[globalID] << 5, 32, 32);
							spr_eggnumbers.draw(scorex, scorey, ((respawn[globalID] - 1) >> 1) << 5, game_values.colorids[globalID] << 5, 32, 32);
						}
						else  //otherwise draw the player's skin in the scoreboard
						{
							short iScoreboardSprite;
							if(game_values.gamemode->gameover)
							{
								if(g_iWinningPlayer != i)
									iScoreboardSprite = PGFX_DEADFLYING;
								else
									iScoreboardSprite = PGFX_JUMPING_R;
							}
							else
							{
								iScoreboardSprite = PGFX_STANDING_R;
							}
							
							//Search for player state to display
							CPlayer * player = GetPlayerFromGlobalID(globalID);
							
							if(player && !game_values.gamemode->gameover)
							{
								short iScoreOffsetX = score[i]->x + scoreoffsets[k];
								short iScoreOffsetY = score[i]->y + 2;

								if(player->ownerPlayerID > -1)
									spr_ownedtags.draw(iScoreOffsetX - 8, iScoreOffsetY - 8, player->ownerColorOffsetX, 0, 48, 48);
		
								player->GetScoreboardSprite()[iScoreboardSprite]->draw(iScoreOffsetX, iScoreOffsetY, player->iSrcOffsetX, 0, 32, 32);

								//Display jail if player is jailed
								if(player->jailtimer > 0)
									spr_jail.draw(iScoreOffsetX - 6, iScoreOffsetY - 6, (player->jailcolor + 1) * 44, 0, 44, 44);

								//Display current powerup if player is using one
								if(player->powerup > 0)
									spr_storedpowerupsmall.draw(iScoreOffsetX, iScoreOffsetY + 16, g_iPowerupToIcon[player->powerup - 1], 0, 16, 16);

								//Display tanooki powerup if player has it
								if(player->tanooki)
									spr_storedpowerupsmall.draw(iScoreOffsetX + 16, iScoreOffsetY + 16, 320, 0, 16, 16);
							}
							else
							{
								spr_player[globalID][iScoreboardSprite]->draw(score[i]->x + scoreoffsets[k], score[i]->y + 2, 0, 0, 32, 32);
							}
						
							//give crown to player(s) with most kills
							if(g_iWinningPlayer == i)
								spr_crown.draw(score[i]->x + scoreoffsets[k] + 12, score[i]->y - 4);
						}

						short storedpowerupid = game_values.gamepowerups[globalID];

						//Draw stored powerup
						if(storedpowerupid != -1)
						{
							if(!game_values.swapplayers)
							{
								spr_storedpowerupsmall.draw(score[i]->x + scorepowerupoffsets[game_values.teamcounts[i] - 1][k], score[i]->y + 25, storedpowerupid * 16, 0, 16, 16);
							}
						}
					}

					//Draw hearts for health mode
					if(game_values.gamemode->gamemode == game_mode_health)
					{
						short iLife = score[i]->subscore[0];
						short iMax = score[i]->subscore[1];
						short iHeartX = score[i]->x + scorepowerupoffsets[game_values.teamcounts[i] - 1][0] - 32;

						for(short iHeart = 0; iHeart < iLife; iHeart++)
						{
							if(iHeart == iMax - 1 && iHeart % 2 == 0)
								spr_scorehearts.draw(iHeartX + iHeart * 8, score[i]->y + 43, 32, 0, 8, 16);
							else
								spr_scorehearts.draw(iHeartX + iHeart * 8, score[i]->y + 43, iHeart % 2 ? 8 : 0, 0, 8, 16);
						}

						for(short iHeart = iLife; iHeart < iMax; iHeart++)
						{
							if(iHeart == iMax - 1 && iHeart % 2 == 0)
								spr_scorehearts.draw(iHeartX + iHeart * 8, score[i]->y + 43, 40, 0, 8, 16);
							else
								spr_scorehearts.draw(iHeartX + iHeart * 8, score[i]->y + 43, iHeart % 2 ? 24 : 16, 0, 8, 16);
						}
					}
					else if(game_values.gamemode->gamemode == game_mode_collection) //Draw cards for collection mode
					{
						//Flash collected cards if 3 have been collected
						if(score[i]->subscore[0] < 3 || score[i]->subscore[2] % 20 < 10)
						{
							short iNumCards = score[i]->subscore[0];
							short iCardValues = score[i]->subscore[1];
							short iCardX = score[i]->x + scorepowerupoffsets[game_values.teamcounts[i] - 1][0] - 20;

							for(short iCard = 0; iCard < iNumCards; iCard++)
							{
								spr_scorecards.draw(iCardX + iCard * 20, score[i]->y + 43, (iCardValues & 3) << 4, 0, 16, 16);
								iCardValues >>= 2;
							}
						}
					}
					else if(game_values.gamemode->gamemode == game_mode_boxes_minigame) //Draw coins for boxes minigame
					{
						//Flash collected cards if 3 have been collected
						short iNumCoins = score[i]->subscore[0];
						short iCoinX = score[i]->x + scorepowerupoffsets[game_values.teamcounts[i] - 1][0] - 32;
						
						short iCoin = 0;
						for(; iCoin < iNumCoins; iCoin++)
						{
							spr_scorecoins.draw(iCoinX + iCoin * 16, score[i]->y + 43, 0, 0, 16, 16);
						}

						for(short iEmptyCoin = iCoin; iEmptyCoin < 5; iEmptyCoin++)
						{
							spr_scorecoins.draw(iCoinX + iEmptyCoin * 16, score[i]->y + 43, 16, 0, 16, 16);
						}
					}
					
					short iScoreX = score[i]->x + iScoreTextOffset[i];
					short iScoreY = score[i]->y + 4;

					spr_scoretext.draw(iScoreX, iScoreY, score[i]->iDigitLeft, (score[i]->iDigitLeft == 0 ? 16 : 0), 16, 16);
					spr_scoretext.draw(iScoreX + 18, iScoreY, score[i]->iDigitMiddle, (score[i]->iDigitLeft == 0 && score[i]->iDigitMiddle == 0 ? 16 : 0), 16, 16);
					spr_scoretext.draw(iScoreX + 36, iScoreY, score[i]->iDigitRight, 0, 16, 16);
				}
			}

			if(game_values.windaffectsplayers)
			{
				short iDisplayWindMeterY = game_values.scoreboardstyle == 1 ? 8 : 440;
				spr_windmeter.draw(210, iDisplayWindMeterY, 0, 0, 220, 32);
				spr_windmeter.draw((short)(game_values.gamewindx * 20.0f) + 320, iDisplayWindMeterY + 6, 220, 0, 12, 20);
			}

			//draw arrows for being above the top of the screen
			for(i = 0; i < list_players_cnt; i++)
				list_players[i]->drawarrows();

			//Draw countdown start timer
			if(iCountDownState > 0 && game_values.screenfade == 0)
			{
				SDL_Rect * rects = iCountDownNumbers[iCountDownRectGroup[28 - iCountDownState]][iCountDownRectSize[28 - iCountDownState]];
				spr_countdown_numbers.draw(rects[1].x, rects[1].y, rects[0].x, rects[0].y, rects[0].w, rects[0].h);
			}

			if(game_values.screenfadespeed != 0)
			{
				g_map->update();
				game_values.screenfade += game_values.screenfadespeed;

				if(game_values.screenfade <= 0)
				{
					game_values.screenfadespeed = 0;
					game_values.screenfade = 0;

					//display the mode and goal at the start of the game
					//if(game_values.matchtype == MATCH_TYPE_QUICK_GAME)
					if(game_values.startmodedisplay && game_values.singleplayermode == -1)
					{
						char szMode[128];
						if(game_values.gamemode->goal < 0)
							sprintf(szMode, "%s  %s: X", game_values.gamemode->GetModeName(), game_values.gamemode->GetGoalName());
						else
							sprintf(szMode, "%s  %s: %d", game_values.gamemode->GetModeName(), game_values.gamemode->GetGoalName(), game_values.gamemode->goal);

						short iMode = GetModeIconIndexFromMode(game_values.gamemode->gamemode);

						eyecandy[2].add(new EC_Announcement(&game_font_large, &menu_mode_large, szMode, iMode, 130, 90));
					}
				}
				else if(game_values.screenfade >= 255)
				{
					game_values.screenfadespeed = 0;
					game_values.screenfade = 255;
				}
			}

			if(game_values.screenfade > 0)
			{
				menu_shade.setalpha((Uint8)game_values.screenfade);
				menu_shade.draw(0, 0);
			}

			if(game_values.swapplayers)
			{
				for(i = 0; i < list_players_cnt; i++)
				{
					list_players[i]->drawswap();

					short storedpowerupid = game_values.gamepowerups[list_players[i]->getGlobalID()];

					if(storedpowerupid != -1)
					{
						short iPowerupX, iPowerupY;

						if(game_values.swapstyle == 1)
						{
							iPowerupX = game_values.swapplayersblink ? list_players[i]->iOldPowerupX : list_players[i]->iNewPowerupX;
							iPowerupY = game_values.swapplayersblink ? list_players[i]->iOldPowerupY : list_players[i]->iNewPowerupY;
						}
						else
						{
							iPowerupX = (short)((float)(list_players[i]->iNewPowerupX - list_players[i]->iOldPowerupX) * game_values.swapplayersposition) + list_players[i]->iOldPowerupX;
							iPowerupY = (short)((float)(list_players[i]->iNewPowerupY - list_players[i]->iOldPowerupY) * game_values.swapplayersposition) + list_players[i]->iOldPowerupY;
						}

						spr_storedpowerupsmall.draw(iPowerupX, iPowerupY, storedpowerupid * 16, 0, 16, 16);
					}
				}

				if(game_values.swapstyle == 0)
				{
					if(!sfx_skid.isplaying())
						ifsoundonplay(sfx_skid);
				}

				if(++game_values.swapplayersblinkcount > 10)
				{
					game_values.swapplayersblinkcount = 0;
					game_values.swapplayersblink = !game_values.swapplayersblink;
				}

				game_values.swapplayersposition += 0.02f;
				if(game_values.swapplayersposition >= 1.0f)
				{
					game_values.swapplayersposition = 0.0f;
					game_values.swapplayers = false;
					game_values.screenfade = 0;
					
					if(game_values.swapstyle == 0)
						ifsoundonstop(sfx_skid);

					ifsoundonplay(sfx_transform);

					if(game_values.swapstyle == 1)
					{
						for(i = 0; i < list_players_cnt; i++)
							eyecandy[2].add(new EC_SingleAnimation(&spr_fireballexplosion, list_players[i]->ix + (HALFPW) - 16, list_players[i]->iy + (HALFPH) - 16, 3, 8));
					}
				}
			}

			//Draw black "behind" the game if we are shaking/moving the screen
			if(y_shake > 0)
			{
				SDL_Rect rect = {0, 0, 640, y_shake};
				SDL_FillRect(screen, &rect, 0x0);		//fill empty area with black
			}
			else if(y_shake < 0)
			{
				SDL_Rect rect = {0, 480 + y_shake, 640, 480};
				SDL_FillRect(screen, &rect, 0x0);		//fill empty area with black
			}

			if(x_shake > 0)
			{
				SDL_Rect rect = {0, 0, x_shake, 480};
				SDL_FillRect(screen, &rect, 0x0);		//fill empty area with black
			}
			else if(x_shake < 0)
			{
				SDL_Rect rect = {640 + x_shake, 0, 640, 480};
				SDL_FillRect(screen, &rect, 0x0);		//fill empty area with black
			}
		}
		else
		{
			if(game_values.pausegame)
			{
				spr_dialog.draw(224, 176);
				menu_font_large.drawCentered(320, 194, "Pause");

				//menu_font_large.drawCentered(320, 240, game_values.gamemode->GetModeName());
				short iMode = GetModeIconIndexFromMode(game_values.gamemode->gamemode);

				menu_mode_large.draw(304, 224, iMode << 5, 0, 32, 32);

				char szGoal[256];
				strcpy(szGoal, game_values.gamemode->GetGoalName());
				strcat(szGoal, ": ");

				if(game_values.gamemode->goal == -1)
					sprintf(szGoal + strlen(szGoal), "Unlimited");
				else
					sprintf(szGoal + strlen(szGoal), "%d", game_values.gamemode->goal);
				
				menu_font_large.drawCentered(320, 264, szGoal);
			}

			if(game_values.exitinggame)
			{
				spr_dialog.draw(224, 176);
				menu_font_large.drawCentered(320, 220 - (menu_font_large.getHeight() >> 1), "Exit Game");

				spr_dialogbutton.draw(235, 250, 0, (game_values.exityes ? 34 : 0), 80, 34);
				spr_dialogbutton.draw(325, 250, 0, (game_values.exityes ? 0 : 34), 80, 34);
			
				menu_font_large.draw(275 - (menu_font_large.getWidth("Yes") >> 1),  269 - (menu_font_large.getHeight() >> 1), "Yes");
				menu_font_large.draw(365 - (menu_font_large.getWidth("No") >> 1),  269 - (menu_font_large.getHeight() >> 1), "No");
			}
		}

		//Make sure music and sound effects keep playing
		if(game_values.slowdownon != -1)
		{
			if(!sfx_slowdownmusic.isplaying())
				ifsoundonplay(sfx_slowdownmusic);
		}
		else
		{
			if(sfx_slowdownmusic.isplaying())
				ifsoundonstop(sfx_slowdownmusic);
		}

		if(game_values.playinvinciblesound)
		{
			if(!sfx_invinciblemusic.isplaying() && !sfx_timewarning.isplaying() && !backgroundmusic[0].isplaying())
				ifsoundonplay(sfx_invinciblemusic);
		}
		else
		{
			if(sfx_invinciblemusic.isplaying())
				ifsoundonstop(sfx_invinciblemusic);
		}

		//If no background music is playing, then play some
		if(!backgroundmusic[0].isplaying() && !sfx_invinciblemusic.isplaying() && !sfx_timewarning.isplaying() && !game_values.gamemode->gameover)
		{
			if(game_values.playnextmusic)
			{
				musiclist->SetNextMusic(g_map->musicCategoryID, maplist->currentShortmapname(), g_map->szBackgroundFile);
				backgroundmusic[0].load(musiclist->GetCurrentMusic());
			}

			backgroundmusic[0].play(game_values.playnextmusic, false);
		}

		ticks = SDL_GetTicks() - framestart;
		if(ticks == 0)
			ticks = 1;

		if(game_values.showfps)
		{
			float potentialFps = 1000.0f / (float)(game_values.framelimiter == 0 ? 1 : game_values.framelimiter);
			menu_font_large.drawf(0, 480-menu_font_large.getHeight(), "Actual:%.1f/%.1f, Flip:%.1f, Potential:%.1f", realfps, potentialFps, flipfps, 1000.0f / (float)ticks);
		}

#ifdef _DEBUG
	if(g_fAutoTest)
		menu_font_small.drawRightJustified(635, 5, "Auto");
#endif
 
		//double buffering -> flip buffers
		SDL_Flip(screen);

		flipfps = 1000.0f / (float)ticks;

		
		//Sleep for time just under what we need
		short delay = (short)(game_values.framelimiter - SDL_GetTicks() + framestart - 2);

		if(delay > 0)
		{
			if(delay > game_values.framelimiter)
				delay = game_values.framelimiter;
			
			SDL_Delay(delay);
		}

		//Fine tune wait here
		while(SDL_GetTicks() - framestart < (unsigned short)game_values.framelimiter)
			SDL_Delay(0);   //keep framerate constant at 1000/game_values.framelimiter fps

		//Debug code to slow framerate down to 1 fps to see exact movement
#ifdef _DEBUG
		if(game_values.frameadvance)
		{
			delay = (short)(1000 - SDL_GetTicks() + framestart);

			if(delay > 0)
			{
				if(delay > 1000)
					delay = 1000;
				
				SDL_Delay(delay);
			}

			while(SDL_GetTicks() - framestart < 1000)
				SDL_Delay(0);
		}
#endif

		ticks = SDL_GetTicks() - framestart;
		if(ticks == 0)
			ticks = game_values.framelimiter;

		realfps = 1000.0f / (float)ticks;

		/*
		static float avgFPS = 0.0f;
		static short outputtimer = 0;

		avgFPS += realfps;

		if(++outputtimer == 1)
		{
			FILE * out = fopen("fps.txt", "a+");

			fprintf(out, "%.2f\n", avgFPS / (float)outputtimer);

			fclose(out);

			avgFPS = 0.0f;
			outputtimer = 0;
		}*/
	}

	//we never get here

	return;
}

void CleanUp()
{
	short i;
	//delete object list
	for(i = 0; i < list_players_cnt; i++)
	{
		delete list_players[i];
	}
	list_players_cnt = 0;

	eyecandy[0].clean();
	eyecandy[1].clean();
	eyecandy[2].clean();
	spotlightManager.ClearSpotlights();

	noncolcontainer.clean();

	objectcontainer[0].clean();
	objectcontainer[1].clean();
	objectcontainer[2].clean();
		
	LoadMapObjects(true);
	g_map->clearWarpLocks();
	g_map->resetPlatforms();
	
	//Stop all game sounds
	sfx_stopallsounds();
	sfx_invinciblemusic.resetpause();
	sfx_slowdownmusic.resetpause();

	x_shake = 0;
	y_shake = 0;
}

void UpdateScoreBoard()
{
	if(game_values.matchtype == MATCH_TYPE_WORLD)
	{
		//If no one won, then nothing on the world map has changed
		if(game_values.gamemode->winningteam < 0)
			return;

		//If this was the last stage, signal that the world is over
		if(game_values.tourstops[game_values.tourstopcurrent]->fEndStage)
		{
			game_values.tournamentwinner = 1;
			backgroundmusic[4].play(true, true);
		}

		//Add up all the winnings so far and determine overall place in the standings
		for(short iScore = 0; iScore < score_cnt; iScore++)
			game_values.tournament_scores[iScore].wins = 0;

		//Assign the order that players will show up on the scoreboard (ordered by score)
		for(short iMyScore = 0; iMyScore < score_cnt; iMyScore++)
		{
			for(short iTheirScore = 0; iTheirScore < score_cnt; iTheirScore++)
			{
				if(game_values.tournament_scores[iMyScore].total > game_values.tournament_scores[iTheirScore].total)
				{
					game_values.tournament_scores[iTheirScore].wins++;
				}
			}
		}

		//Give players the item bonuses that were won
		TourStop * tourStop = game_values.tourstops[game_values.tourstopcurrent];

		for(short iScore = 0; iScore < score_cnt; iScore++)
		{
			for(short iBonus = 0; iBonus < tourStop->iNumBonuses; iBonus++)
			{
				if(tourStop->wsbBonuses[iBonus].iWinnerPlace == score[iScore]->place)
				{
					if(game_values.worldpowerupcount[iScore] < 32)
						game_values.worldpowerups[iScore][game_values.worldpowerupcount[iScore]++] = tourStop->wsbBonuses[iBonus].iBonus;
					else
						game_values.worldpowerups[iScore][31] = tourStop->wsbBonuses[iBonus].iBonus;	
				}
			}
		}

		//Add powerups to player's world item inventory that they held at the end of the game
		for(short iPlayer = 0; iPlayer < 4; iPlayer++)
		{
			if(game_values.gamepowerups[iPlayer] != -1)
			{
				short iTeamId = LookupTeamID(iPlayer);
				if(game_values.worldpowerupcount[iTeamId] < 32)
					game_values.worldpowerups[iTeamId][game_values.worldpowerupcount[iTeamId]++] = game_values.gamepowerups[iPlayer];
			}
		}
	}
	else if(game_values.matchtype == MATCH_TYPE_TOUR)
	{
		//If no one won (tied game), then there is no need to update the scores because nothing has changed
		if(game_values.gamemode->winningteam < 0)
			return;

		//For this game, set the player's place as the type of win
		for(short iScore = 0; iScore < score_cnt; iScore++)
		{
			game_values.tournament_scores[iScore].type[game_values.tourstopcurrent] = score[iScore]->place;
		}

		//Add up all the winnings so far and determine overall place in tour
		for(short iScore = 0; iScore < score_cnt; iScore++)
		{
			game_values.tournament_scores[iScore].total = 0;

			for(short iTourStop = 0; iTourStop <= game_values.tourstopcurrent; iTourStop++)
			{
				game_values.tournament_scores[iScore].total += (3 - game_values.tournament_scores[iScore].type[iTourStop]) * game_values.tourstops[iTourStop]->iPoints;
			}

			game_values.tournament_scores[iScore].wins = 0;
		}

		for(short iMyScore = 0; iMyScore < score_cnt; iMyScore++)
		{
			for(short iTheirScore = 0; iTheirScore < score_cnt; iTheirScore++)
			{
				if(game_values.tournament_scores[iMyScore].total > game_values.tournament_scores[iTheirScore].total)
				{
					game_values.tournament_scores[iTheirScore].wins++;
				}
			}
		}

		if(++game_values.tourstopcurrent >= game_values.tourstoptotal)
		{
			//Calculate Tour Winner by counting up 1st, 2nd, 3rd, and 4th place wins

			short iWinningTeam = -2;  //Set winning team to -2 to signify a tie between teams
			short iWinningScore = 0;

			for(short iScore = 0; iScore < score_cnt; iScore++)
			{
				if(game_values.tournament_scores[iScore].total > iWinningScore)  //New winner found
				{
					iWinningTeam = iScore;
					iWinningScore = game_values.tournament_scores[iScore].total;
				}
				else if(game_values.tournament_scores[iScore].total == iWinningScore)  //Winning position tied
				{
					iWinningTeam = -2;
				}
			}

			game_values.tournamentwinner = iWinningTeam;
			backgroundmusic[4].play(true, true);
		}
	}
	else if(game_values.matchtype == MATCH_TYPE_TOURNAMENT)
	{
		short maxScore = -1;  //Max score for game
		short maxTeam = -1;  //Team ID with the max score -> reset to -1 if two teams tied for win

		for(short i = 0; i < score_cnt; i++)
		{
			if(score[i]->score > maxScore)
			{
				maxScore = score[i]->score;
				maxTeam = i;
			}
			else if(score[i]->score == maxScore)
			{
				maxTeam = -1;
			}
		}

		if(maxTeam > -1)
		{
			game_values.tournament_scores[maxTeam].type[game_values.tournament_scores[maxTeam].wins] = currentgamemode;

			if(++game_values.tournament_scores[maxTeam].wins >= game_values.tournamentgames)
			{
				game_values.tournamentwinner = maxTeam;
				
				if(game_values.music)
					backgroundmusic[4].play(true, true);
			}
		}
	}
}

void PlayNextMusicTrack()
{
	if(game_values.gamemode->gameover || game_values.playinvinciblesound || sfx_timewarning.isplaying())
		return;

	backgroundmusic[0].stop();
	musiclist->SetNextMusic(g_map->musicCategoryID, maplist->currentShortmapname(), g_map->szBackgroundFile);
	backgroundmusic[0].load(musiclist->GetCurrentMusic());
	backgroundmusic[0].play(game_values.playnextmusic, false);
}

bool coldec_player2player(CPlayer * o1, CPlayer * o2)
{
	//Special cases to deal with players overlapping the right and left sides of the screen
	if(o1->ix + PW < o2->ix)
	{
		return o1->ix + 640 < o2->ix + PW && o1->ix + PW + 640 >= o2->ix && o1->iy <= o2->iy + PH && o1->iy + PH >= o2->iy;
	}
	else if(o2->ix + PW < o1->ix)
	{
		return o1->ix < o2->ix + PW + 640 && o1->ix + PW >= o2->ix + 640 && o1->iy <= o2->iy + PH && o1->iy + PH >= o2->iy;
	}
	else  //Normal case where no overlap
	{
		return o1->ix < o2->ix + PW && o1->ix + PW >= o2->ix && o1->iy <= o2->iy + PH && o1->iy + PH >= o2->iy;
	}
}

bool coldec_player2obj(CPlayer * o1, CObject * o2)
{
	//Special cases to deal with players overlapping the right and left sides of the screen
	if(o1->ix + PW < o2->ix)
	{
		return o1->ix + 640 < o2->ix + o2->collisionWidth && o1->ix + PW + 640 >= o2->ix && o1->iy < o2->iy + o2->collisionHeight && o1->iy + PH >= o2->iy;
	}
	else if(o2->ix + o2->collisionWidth < o1->ix)
	{
		return o1->ix < o2->ix + o2->collisionWidth + 640 && o1->ix + PW >= o2->ix + 640 && o1->iy < o2->iy + o2->collisionHeight && o1->iy + PH >= o2->iy; 
	}
	else //Normal case where no overlap
	{
		return o1->ix < o2->ix + o2->collisionWidth && o1->ix + PW >= o2->ix && o1->iy < o2->iy + o2->collisionHeight && o2->iy <= o1->iy + PH;
	}
}

bool coldec_obj2obj(CObject * o1, CObject * o2)
{
	//Special cases to deal with players overlapping the right and left sides of the screen
	short o1r = o1->ix + o1->collisionWidth;
	short o1b = o1->iy + o1->collisionHeight;
	short o2r = o2->ix + o2->collisionWidth;
	short o2b = o2->iy + o2->collisionHeight;

	if(o1r < o2->ix)
	{
		return o1->ix + 640 < o2r && o1r + 640 >= o2->ix && o1->iy < o2b && o1b >= o2->iy;
	}
	else if(o2r < o1->ix)
	{
		return o1->ix < o2r + 640 && o1r >= o2->ix + 640 && o1->iy < o2b && o1b >= o2->iy;
	}
	else
	{
		return o1->ix < o2r && o1r >= o2->ix && o1->iy < o2b && o1b >= o2->iy;
	}
}

void SetGameModeSettingsFromMenu()
{
	//If this is a tour stop and the tour has settings in it, use those.  Otherwise use the menu settings.
	if(game_values.tourstops[game_values.tourstopcurrent]->fUseSettings && 
		(game_values.matchtype == MATCH_TYPE_TOUR || game_values.matchtype == MATCH_TYPE_WORLD))
		memcpy(&game_values.gamemodesettings, &game_values.tourstops[game_values.tourstopcurrent]->gmsSettings, sizeof(GameModeSettings));
	else
		memcpy(&game_values.gamemodesettings, &game_values.gamemodemenusettings, sizeof(GameModeSettings));
}

void LoadMapObjects(bool fPreview)
{
	LoadMapHazards(fPreview);

	//Clear all the previous switch settings
	for(short iSwitch = 0; iSwitch < 8; iSwitch++)
		g_map->switchBlocks[iSwitch].clear();
	
	//Add blocks (breakable, note, switch, throwable, etc)
	for(short x = 0; x < MAPWIDTH; x++)
	{
		for(short y = 0; y < MAPHEIGHT; y++)
		{
			short iType = g_map->objectdata[x][y].iType;
			if(iType == 0)
			{
				g_map->blockdata[x][y] = new B_BreakableBlock(&spr_breakableblock, x << 5, y << 5, 4, 10);
				noncolcontainer.add(g_map->blockdata[x][y]);
			}
			else if(iType == 1)
			{
				g_map->blockdata[x][y] = new B_PowerupBlock(&spr_powerupblock, x << 5, y << 5, 4, 10, g_map->objectdata[x][y].fHidden, g_map->objectdata[x][y].iSettings);
				noncolcontainer.add(g_map->blockdata[x][y]);
			}
			else if(iType == 2)
			{
				g_map->blockdata[x][y] = new B_DonutBlock(&spr_donutblock, x << 5, y << 5);
				noncolcontainer.add(g_map->blockdata[x][y]);
			}
			else if(iType == 3)
			{
				g_map->blockdata[x][y] = new B_FlipBlock(&spr_flipblock, x << 5, y << 5, g_map->objectdata[x][y].fHidden);
				noncolcontainer.add(g_map->blockdata[x][y]);
			}
			else if(iType == 4)
			{
				g_map->blockdata[x][y] = new B_BounceBlock(&spr_bounceblock, x << 5, y << 5, g_map->objectdata[x][y].fHidden);
				noncolcontainer.add(g_map->blockdata[x][y]);
			}
			else if(iType == 5)
			{
				g_map->blockdata[x][y] = new B_NoteBlock(&spr_noteblock, x << 5, y << 5, 4, 10, 1, g_map->objectdata[x][y].fHidden);
				noncolcontainer.add(g_map->blockdata[x][y]);
			}
			else if(iType == 6)
			{
				g_map->blockdata[x][y] = new B_ThrowBlock(&spr_throwblock, x << 5, y << 5, 4, 10, 0);
				noncolcontainer.add(g_map->blockdata[x][y]);
			}
			else if(iType >= 7 && iType <= 10)
			{
				short iSwitchType = iType - 7;
				g_map->blockdata[x][y] = new B_OnOffSwitchBlock(&spr_switchblocks, x << 5, y << 5, iSwitchType, g_map->iSwitches[iSwitchType]);
				noncolcontainer.add(g_map->blockdata[x][y]);
				g_map->switchBlocks[iSwitchType].push_back(g_map->blockdata[x][y]);
			}
			else if(iType >= 11 && iType <= 14)
			{
				short iSwitchType = iType - 11;

				//g_map->blockdata[x][y] = new B_SwitchBlock(&spr_switchblocks, x << 5, y << 5, iSwitchType, g_map->iSwitches[iSwitchType]);
				g_map->blockdata[x][y] = new B_SwitchBlock(&spr_switchblocks, x << 5, y << 5, iSwitchType, g_map->objectdata[x][y].iSettings[0]);
				noncolcontainer.add(g_map->blockdata[x][y]);
				g_map->switchBlocks[iSwitchType + 4].push_back(g_map->blockdata[x][y]);
			}
			else if(iType == 15)
			{
				g_map->blockdata[x][y] = new B_ViewBlock(&spr_viewblock, x << 5, y << 5, g_map->objectdata[x][y].fHidden, g_map->objectdata[x][y].iSettings);
				noncolcontainer.add(g_map->blockdata[x][y]);
			}
			else if(iType == 16)
			{
				g_map->blockdata[x][y] = new B_ThrowBlock(&spr_throwblock, x << 5, y << 5, 4, 10, 2);
				noncolcontainer.add(g_map->blockdata[x][y]);
			}
			else if(iType == 17 || iType == 18)
			{
				g_map->blockdata[x][y] = new B_NoteBlock(&spr_noteblock, x << 5, y << 5, 4, 10, iType == 17 ? 2 : 0, g_map->objectdata[x][y].fHidden);
				noncolcontainer.add(g_map->blockdata[x][y]);
			}
			else if(iType == 19)
			{
				g_map->blockdata[x][y] = new B_ThrowBlock(&spr_throwblock, x << 5, y << 5, 4, 10, 1);
				noncolcontainer.add(g_map->blockdata[x][y]);
			}
			else if(iType >= 20 && iType <= 29)
			{
				g_map->blockdata[x][y] = new B_WeaponBreakableBlock(&spr_weaponbreakableblock, x << 5, y << 5, iType - 20);
				noncolcontainer.add(g_map->blockdata[x][y]);
			}
			else
			{
				g_map->blockdata[x][y] = NULL;
			}
		}
	}

	//Scan for throw box objects and add items to them if approprate
	//Add special coins to them for the boxes minigame
	short iCountWeight = 0;
	for(short iPowerup = 0; iPowerup < NUM_POWERUPS; iPowerup++)
		iCountWeight += game_values.powerupweights[iPowerup];

	short iThrowBoxCount = 0;
	bool * fBoxHasCoin = NULL;

	if(game_values.gamemode->gamemode == game_mode_boxes_minigame)
	{
		for(short iItem = 0; iItem < g_map->iNumMapItems; iItem++)
		{
			if(g_map->mapitems[iItem].itype == 5)
				iThrowBoxCount++;
		}

		fBoxHasCoin = new bool[iThrowBoxCount];
		for(short iItem = 0; iItem < iThrowBoxCount; iItem++)
			fBoxHasCoin[iItem] = false;

		//Randomly choose boxes to put 5 coins in
		for(short iItem = 0; iItem < 5 && iItem < iThrowBoxCount; iItem++)
		{
			short iBoxIndex = rand() % iThrowBoxCount;
			
			while(fBoxHasCoin[iBoxIndex])
			{
				if(++iBoxIndex >= iThrowBoxCount)
					iBoxIndex = 0;
			}

			fBoxHasCoin[iBoxIndex] = true;
		}

		//If map has less than 5 boxes, then insert coins into map in random locations
		short iExtraCoinsNeeded = 5 - iThrowBoxCount;
		for(short iExtraCoin = 0; iExtraCoin < iExtraCoinsNeeded; iExtraCoin++)
		{
			objectcontainer[1].add(new MO_Coin(&spr_coin, 0.0f, 0.0f, 0, 0, 2, -1, 2, 0, true));
		}
	}

	//Add map objects like springs, shoes and spikes
	short iAddThrowBoxIndex = 0;
	for(short i = 0; i < g_map->iNumMapItems; i++)
	{
		MapItem * mapItem = &g_map->mapitems[i];
		short iType = mapItem->itype;
		short ix = mapItem->ix << 5;
		short iy = mapItem->iy << 5;

		if(iType == 0)
			objectcontainer[1].add(new CO_Spring(&spr_spring, ix, iy, false));
		else if(iType == 1)
			objectcontainer[1].add(new CO_Spike(&spr_spike, ix, iy));
		else if(iType == 2)
			objectcontainer[1].add(new CO_KuriboShoe(&spr_kuriboshoe, ix, iy, false));
		else if(iType == 3)
			objectcontainer[1].add(new CO_Spring(&spr_spring, ix, iy, true));
		else if(iType == 4)
			objectcontainer[1].add(new CO_KuriboShoe(&spr_kuriboshoe, ix, iy, true));
		else if(iType == 5)
		{
			short iItem = NO_POWERUP;
			if(!fPreview)
			{
				if(game_values.gamemode->gamemode == game_mode_boxes_minigame)
				{
					if(fBoxHasCoin[iAddThrowBoxIndex])
						iItem = MINIGAME_COIN;
				}
				else if(game_values.gamemode->gamemode == game_mode_health && rand() % 100 < game_values.gamemodesettings.health.percentextralife)
				{
					iItem = HEALTH_POWERUP;
				}
				else if((game_values.gamemode->gamemode == game_mode_timelimit && rand() % 100 < game_values.gamemodesettings.time.percentextratime) || 
					(game_values.gamemode->gamemode == game_mode_star && rand() % 100 < game_values.gamemodesettings.star.percentextratime))
				{
					iItem = TIME_POWERUP;
				}
				else if((game_values.gamemode->gamemode == game_mode_coins && rand() % 100 < game_values.gamemodesettings.coins.percentextracoin) || 
					(game_values.gamemode->gamemode == game_mode_greed && rand() % 100 < game_values.gamemodesettings.greed.percentextracoin))
				{
					iItem = COIN_POWERUP;
				}
				else if(game_values.gamemode->gamemode == game_mode_jail && (rand() % 100) < game_values.gamemodesettings.jail.percentkey)
				{
					iItem = JAIL_KEY_POWERUP;
				}
				else if(iCountWeight > 0 && (rand() % 100) < 40)
				{
					int iRandPowerup = rand() % iCountWeight + 1;
					iItem = 0;

					int iPowerupWeightCount = game_values.powerupweights[iItem];

					while(iPowerupWeightCount < iRandPowerup)
						iPowerupWeightCount += game_values.powerupweights[++iItem];
				}
			}

			objectcontainer[1].add(new CO_ThrowBox(&spr_throwbox, ix, iy, iItem));
			iAddThrowBoxIndex++;
		}
	}

	if(fBoxHasCoin)
	{
		delete [] fBoxHasCoin;
		fBoxHasCoin = NULL;
	}

	//Set all the 1x1 gaps up so players can run across them
	g_map->UpdateAllTileGaps();
}

bool SwapPlayers(short iUsingPlayerID)
{
	//Count available players to switch with
	short iNumAvailablePlayers = 0;
	for(short iPlayer = 0; iPlayer < list_players_cnt; iPlayer++)
	{
		if(list_players[iPlayer]->isready())
		{
			iNumAvailablePlayers++;
			list_players[iPlayer]->fOldSwapX = list_players[iPlayer]->ix;
			list_players[iPlayer]->fOldSwapY = list_players[iPlayer]->iy;

			list_players[iPlayer]->iNewPowerupX = list_players[iPlayer]->score->x + scorepowerupoffsets[game_values.teamcounts[list_players[iPlayer]->teamID] - 1][list_players[iPlayer]->subTeamID];
			list_players[iPlayer]->iNewPowerupY = list_players[iPlayer]->score->y + 25;
		}
	}

	if(iNumAvailablePlayers <= 1)
		return false;

	if(game_values.swapstyle != 2)
	{
		game_values.swapplayers = true;
		game_values.swapplayersposition = 0.0f;
		
		if(game_values.swapstyle == 1)
		{
			game_values.swapplayersblink = false;
			game_values.swapplayersblinkcount = 0;
		}
		else
		{
			game_values.screenfade = GetScreenBackgroundFade();
		}
	}

	short iIncrement = rand() % (iNumAvailablePlayers - 1);

	MysteryMushroomTempPlayer spots[4];
	for(short iPlayer = 0; iPlayer < list_players_cnt; iPlayer++)
	{
		if(!list_players[iPlayer]->isready())
			continue;

		short iNewSpot = iPlayer + iIncrement;
		
		do
		{
			if(++iNewSpot >= list_players_cnt)
				iNewSpot = 0;
		}
		while(spots[iNewSpot].fUsed || !list_players[iNewSpot]->isready());

		spots[iNewSpot].fUsed = true;
		spots[iPlayer].SetPlayer(list_players[iNewSpot], game_values.gamepowerups[list_players[iNewSpot]->getGlobalID()]);

		//Give credit for deaths to the player that used the mystery mushroom
		if(iUsingPlayerID == iNewSpot)
		{
			list_players[iPlayer]->iSuicideCreditPlayerID = list_players[iNewSpot]->globalID;
			list_players[iPlayer]->iSuicideCreditTimer = 62;
		}
	}

	for(short iPlayer = 0; iPlayer < list_players_cnt; iPlayer++)
	{
		if(!list_players[iPlayer]->isready())
			continue;

		spots[iPlayer].GetPlayer(list_players[iPlayer], &game_values.gamepowerups[list_players[iPlayer]->getGlobalID()]);
		
		if(game_values.swapstyle != 1)
			eyecandy[2].add(new EC_SingleAnimation(&spr_fireballexplosion, (short)list_players[iPlayer]->fNewSwapX + (HALFPW) - 16, (short)list_players[iPlayer]->fNewSwapY + (HALFPH) - 16, 3, 8));

		if(game_values.swapstyle == 2)
		{
			list_players[iPlayer]->xf(list_players[iPlayer]->fNewSwapX);
			list_players[iPlayer]->yf(list_players[iPlayer]->fNewSwapY);

			if(list_players[iPlayer]->carriedItem)
				list_players[iPlayer]->carriedItem->MoveToOwner();
		}
	}

	//Clean the dead mystery mushroom
	objectcontainer[0].cleandeadobjects();	

	return true;
}

bool IsExitAllowed()
{
	if(!game_values.noexit || list_players_cnt == 0)
		return true;

	for(int iPlayer = 0; iPlayer < list_players_cnt; iPlayer++)
	{
		if(game_values.playercontrol[list_players[iPlayer]->getGlobalID()] == 1)
			return false;
	}

	return true;
}

bool IsPauseAllowed()
{
	return !game_values.noexit;
}

void UpdateMusicWithOverrides()
{
	FILE * file = fopen(convertPath("music/Overrides.txt").c_str(), "r");

	if(!file)
		return;

	short iAddToCategory = 0;
	char szBuffer[256];
	while(fgets(szBuffer, 1024, file))
	{
		//Ignore comment lines
		if(szBuffer[0] == '#' || szBuffer[0] == '\n' || szBuffer[0] == '\r' || szBuffer[0] == ' ' || szBuffer[0] == '\t')
			continue;

		//Chop off line ending
		int stringLength = strlen(szBuffer);
		for(short k = 0; k < stringLength; k++)
		{
			if(szBuffer[k] == '\r' || szBuffer[k] == '\n')
			{
				szBuffer[k] = '\0';
				break;
			}
		}

		//If we found a category header
		if(szBuffer[0] == '[')
		{
			if(!_stricmp(szBuffer, "[maps]"))
				iAddToCategory = 1;
			else if(!_stricmp(szBuffer, "[worlds]"))
				iAddToCategory = 2;

			continue;
		}
		
		//If we're not in a category, ignore this line
		if(iAddToCategory == 0)
			continue;

		char * pszName = strtok(szBuffer, ",\n");
				
		if(!pszName)
			continue;

		if(iAddToCategory == 1)
		{
			MapMusicOverride * override = new MapMusicOverride();

			override->mapname = pszName;
			
			char * pszMusic = strtok(NULL, ",\n");
			while(pszMusic)
			{
				std::string sPath = convertPath(pszMusic);

				if(File_Exists(sPath.c_str()))
				{
					override->songs.push_back(sPath);
				}

				pszMusic = strtok(NULL, ",\n");
			}

			//Don't add overrides that have no songs
			if(override->songs.size() == 0)
			{
				delete override;
				continue;
			}

			mapmusicoverrides.push_back(override);
		}
		else if(iAddToCategory == 2)
		{
			WorldMusicOverride * override = new WorldMusicOverride();

			override->worldname = pszName;
			
			char * pszMusic = strtok(NULL, ",\n");
			if(pszMusic)
			{
				std::string sPath = convertPath(pszMusic);

				if(File_Exists(sPath.c_str()))
				{
					override->song = sPath;
					worldmusicoverrides.push_back(override);
				}
			}		
		}
	}

	musiclist->UpdateEntriesWithOverrides();
	worldmusiclist->UpdateEntriesWithOverrides();

	fclose(file);
}



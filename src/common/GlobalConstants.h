#ifndef GLOBALCONSTANTS_H
#define GLOBALCONSTANTS_H

//------------- global definitions -------------

#define FILEBUFSIZE 1024

//----------------------------------------------------------------
// DON'T CHANGE THE ORDER xxx_R xxx_L xxx_R xxx_L !!!
//----------------------------------------------------------------
// because this is used to determine the direction the player was facing in the last frame
// if the last sprite was facing right (spr & 0x1) is 0
// because the last bit of every PGFX_[action]_R is 0
// the last bit of PGFX_[action]_L is 1
// IF YOU CHANGE THE ORDER OF THE PGFX_L/R STUFF THIS WON'T WORK!

// see CPlayer::draw() on how this is used

#define PGFX_STANDING_R 0
#define PGFX_STANDING_L 1
#define PGFX_RUNNING_R  2
#define PGFX_RUNNING_L  3
#define PGFX_JUMPING_R  4
#define PGFX_JUMPING_L  5
#define PGFX_STOPPING_R 6
#define PGFX_STOPPING_L 7
#define PGFX_DEADFLYING 8
#define PGFX_DEAD       9
//--------------------------
#define PGFX_LAST       10

#define PANNOUNCER_SOUND_LAST   20

#define PH              25      //Player height
#define PW              22      //Player width
#define HALFPH          12
#define HALFPW          11
#define PHOFFSET        4
#define PWOFFSET        5

#define VELMOVING           4.0f        //velocity (speed) for moving left, right
#define VELSLOWMOVING       2.2f        //velocity when slow down powerup is in effect
#define VELMOVING_CHICKEN   2.9f        //speed of the chicken in the gamemode capturethechicken
#define VELMOVINGADD        0.5f
#define VELMOVINGADDICE     VELMOVINGADD / 4
#define VELTURBOMOVING      5.5f
#define VELJUMP             9.0f    //velocity for jumping
#define VELSLOWJUMP         7.0f    //velocity for jumping when slow down powerup is in effect
#define VELTURBOJUMP        10.2f   //velocity for turbo jumping
#define VELSUPERJUMP        13.0f  //super jump (jumping off of orange note blocks)
#define VELPUSHBACK         5.0f
#define VELMAXBREAKBLOCK    3.0f
#define VELNOTEBLOCKBOUNCE  3.0f
#define VELNOTEBLOCKREPEL   5.0f
#define VELBLOCKBOUNCE      3.0f
#define VELBLOCKREPEL       3.0f
#define VELPOWERUPBOUNCE    8.1f
#define FIREBALLBOUNCE      5.0f
#define HAMMERTHROW         8.5f
#define VELMOVINGFRICTION   0.2f
#define VELICEFRICTION      0.06f
#define VELAIRFRICTION      0.06f
#define VELSTOPJUMP         5.0f
#define BOUNCESTRENGTH      0.5f
#define TAGGEDBOOST         1.0f
#define VELSUPERSTOMP       10.0f
#define VELTAILSHAKE        1.0f
#define VELKURIBOBOUNCE     3.0f
#define VELENEMYBOUNCE      7.0f

#define GRAVITATION 0.40f

#define MAXVELY     20.0f
#define MAXSIDEVELY 10.0f

#define CRUNCHVELOCITY  2
#define CRUNCHAMOUNT    16
#define CRUNCHMAX       32


#define MAPWIDTH        20          //width of the map
#define MAPHEIGHT       15          //height of the map
#define MAPLAYERS       4           //number of layers the map has
#define TILESIZE        32          //size of the tiles, should be dynamically read
#define PREVIEWTILESIZE 16          //size of the preview tiles
#define THUMBTILESIZE   8           //size of the thumbnail tiles

#define MAXWARPS            32
#define MAXMAPITEMS         32
#define MAXMAPHAZARDS       30
#define NUMMAPHAZARDPARAMS  5

#define NUMSPAWNAREATYPES   6
#define MAXSPAWNAREAS       128
#define MAXDRAWAREAS        128

#define MAX_BONUS_CHESTS            5
#define MAX_WORLD_BONUSES_AWARDED   3

#define TILESETSIZE     960         //30*32 Tiles by 32 pixel in a 1024*1024 bmp
#define TILESETHEIGHT   30
#define TILESETWIDTH    32

#define NUMTILETYPES    19

#define TILESETANIMATED     -1
#define TILESETNONE         -2
#define TILESETUNKNOWN      -3

#define TILEANIMATIONSIZE   38      //number of tile animations

#define MAXMUSICCATEGORY        11
#define MAXWORLDMUSICCATEGORY   9
#define WORLDMUSICBONUS         MAXWORLDMUSICCATEGORY
#define WORLDMUSICSLEEP         MAXWORLDMUSICCATEGORY + 1
#define WORLDMUSICWORLDS        MAXWORLDMUSICCATEGORY + 2
#define MAXCATEGORYTRACKS       64

#define MAXEYECANDY 192
#define MAXOBJECTS  300
#define CORPSESTAY  200

#define WAITTIME        16      //delay between frames (default:16)
#define MAXAWARDS       10
#define MINAWARDSNEEDED 3

#define MAXTOURNAMENTGAMES  10
#define MAXWORLDITEMS       32

#define MAXRACEGOALS        8

#define ABS(x) (x>0?x:-x)

#define TWO_PI              6.2831853f
#define PI                  3.1415292f
#define THREE_HALF_PI       4.7123890f
#define THREE_QUARTER_PI    2.3561945f
#define HALF_PI             1.5707963f
#define QUARTER_PI          0.7853982f

#define MENU_ITEM_NEIGHBOR_UP       0
#define MENU_ITEM_NEIGHBOR_DOWN     1
#define MENU_ITEM_NEIGHBOR_LEFT     2
#define MENU_ITEM_NEIGHBOR_RIGHT    3

#define NUM_POWERUPS            26
#define NUM_WORLD_POWERUPS      15
#define NUM_WORLD_SCORE_BONUSES 20
#define NUM_WORLD_ITEMS         NUM_POWERUPS + NUM_WORLD_POWERUPS + NUM_WORLD_SCORE_BONUSES
#define NUM_BLOCK_SETTINGS      NUM_POWERUPS
#define NUM_POWERUP_PRESETS     17

#define HEALTH_POWERUP      -1
#define TIME_POWERUP        -2
#define JAIL_KEY_POWERUP    -3
#define COIN_POWERUP        -4
#define MINIGAME_COIN       -5
#define SECRET1_POWERUP     -6
#define SECRET2_POWERUP     -7
#define SECRET3_POWERUP     -8
#define SECRET4_POWERUP     -9
#define NO_POWERUP          -10

#define NUM_AUTO_FILTERS 12

#define NUMSTOMPENEMIES     9
#define NUMSURVIVALENEMIES  3
#define NUMFRENZYCARDS      19

#define MAX_PLAYERS 4


#endif // GLOBALCONSTANTS_H

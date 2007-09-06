/*----------------------------------------------------------+
| Super Mario War Leveleditor								|
|															|
| based on the jnrdev #1/#2 level editor					|
|															|
| ...a big bunch of crappy code ;)							|
| ...with some improvements									|
|															|
|															|
| this sourcecode is released under the GPL.				|
|															|
| start:		18.12.2003									|
| last changes:	20.02.2004									|
| save/load added 03.7.2004 by Armen - thanks!				|
| blocks/maplist/2-layers/extended tile set added 15.07.2005|
|                                                  by Two52 |
|															|
|				© 2004 Florian Hufsky <fhufsky@phorus.at>	|
+----------------------------------------------------------*/

#define _SMW_EDITOR
#include "global.h"

#ifdef PNG_SAVE_FORMAT
	#include "savepng.h"
#endif

#include <string.h>
#include <ctype.h>

#ifdef _WIN32
	#include <windows.h>

	#ifndef _XBOX
		#pragma comment(linker, "/NODEFAULTLIB:libc.lib")

		#ifdef PNG_SAVE_FORMAT
			#pragma comment(lib, "libpng.lib")
			#pragma comment(lib, "zlib.lib")
		#endif
    #endif
#endif

#define MAPTITLESTRING "SMW 1.7 Leveleditor"

enum {EDITOR_EDIT, EDITOR_TILES, EDITOR_QUIT, SAVE_AS, FIND, CLEAR_MAP, EDITOR_BLOCKS, NEW_MAP, SAVE, EDITOR_WARP, EDITOR_EYECANDY, DISPLAY_HELP, EDITOR_PLATFORM, EDITOR_TILETYPE, EDITOR_BACKGROUNDS, EDITOR_MAPITEMS};

#define MAX_PLATFORMS 8
#define MAX_PLATFORM_VELOCITY 16

class MapTile
{
	public:
		int			tile[MAPLAYERS];
		int			block;
		Warp		warp;
		bool		nospawn[NUMSPAWNAREATYPES];
		TileType	tiletype;
};

class MapPlatform
{
	public:
		short tiles[MAPWIDTH][MAPHEIGHT];
		SDL_Rect rIcon[2];
		short iVelocity;
		short iStartX;
		short iStartY;
		short iEndX;
		short iEndY;
};

SDL_Surface		*screen;
SDL_Surface		*blitdest;
SDL_Event		event;

gfxFont			font;
gfxFont			fontbig;

gfxSprite		spr_background;
gfxSprite		spr_frontmap;
gfxSprite		spr_tiletypes[6];
gfxSprite		spr_transparenttiles;

gfxSprite		spr_backgroundlevel;
gfxSprite		spr_tilesetlevel;
gfxSprite		spr_selectedtile;
gfxSprite		spr_nospawntile;
gfxSprite		spr_noitemspawntile;
gfxSprite		spr_platformstarttile;
gfxSprite		spr_platformendtile;
gfxSprite		spr_dialog;
gfxSprite		menu_shade;
gfxSprite		spr_mapitems[3];

gfxSprite		spr_platformarrows[3];
gfxSprite		spr_warps[3];

TileType		set_type = tile_solid;
int				set_tile_rows = 0;
int				set_tile_cols = 0;
int				set_tile_start_x = 0;
int				set_tile_start_y = 0;
int				set_tile_end_x = 0;
int				set_tile_end_y = 0;
bool			set_tile_drag = false;

int				set_block = 0;
TileType		set_tiletype = tile_nonsolid;
int				set_mapitem = 0;

int				set_direction = 0;
int				set_connection = 0;

int				edit_mode = 1;
int				selected_layer = 0;
int				selected_tileset = 0;

int				move_mode = 0;
int				move_start_x = 0;
int				move_start_y = 0;
int				move_offset_x = 0;
int				move_offset_y = 0;
bool			move_nodrag = false;

int				move_drag_start_x = 0;
int				move_drag_start_y = 0;
int				move_drag_offset_x = 0;
int				move_drag_offset_y = 0;

CMap			g_map;
int				state;
bool			selectedtiles[MAPWIDTH][MAPHEIGHT];
bool			moveselectedtiles[MAPWIDTH][MAPHEIGHT];
MapTile			copiedtiles[MAPWIDTH][MAPHEIGHT];
int				copiedlayer;

//// Global stuff that the map editor doesn't need, but has references to
GraphicsList menugraphicspacklist;
GraphicsList gamegraphicspacklist;
FiltersList filterslist;
gfxSprite		spr_warplock;
short			x_shake = 0;
short			y_shake = 0;
gv				game_values;
void CPlayer::flipsidesifneeded() {}
void CPlayer::KillPlayerMapHazard() {}
void IO_MovingObject::flipsidesifneeded() {}
void IO_MovingObject::KillObjectMapHazard() {}
float CapFallingVelocity(float f) {return 0.0f;}
void removeifprojectile(IO_MovingObject * object, bool playsound, bool forcedead) {}
gfxSprite		spr_thumbnail_platformarrows;
gfxSprite		spr_thumbnail_warps[2];
gfxSprite		spr_thumbnail_mapitems[2];
gfxSprite		spr_awardsouls, spr_fireballexplosion;
gfxSprite		spr_maplava, spr_mapwater, spr_mapwaterfall, spr_maplamp;
CEyecandyContainer eyecandyfront;
CGameMode		*gamemodes[GAMEMODE_LAST];
CPlayer			*list_players[4];
short			list_players_cnt = 0;
///////

SDL_Surface * s_eyecandy;
SDL_Surface * s_eyecandyindicator;
SDL_Surface * s_platform;

int save_as();
int find();
int clearMap();
bool dialog(char * title, char * instructions, char * input, int inputsize);
int display_help();

#ifdef _DEBUG
	void convertAll();
#endif

void drawmap(bool fScreenshot, short iBlockSize);
void draw_platform(short iPlatform);
void SetPlatformToDefaults(short iPlatform);

MapList maplist;
void loadcurrentmap();
int savecurrentmap();
int findcurrentstring();
int newmap();
void save_map(const std::string &file);
void insert_platforms_into_map();
void CalculatePlatformDims(short iPlatform, short * ix, short * iy, short * iw, short * ih);
void LoadBackgroundPage(SDL_Surface ** sBackgrounds, short iPage);

int editor_edit();
int editor_warp();
int editor_eyecandy();
int	editor_tiles();
int editor_blocks();
int editor_mapitems();
int editor_platforms();
int editor_tiletype();
int editor_backgrounds();

void resetselectedtiles();
void copymoveselection();
void pastemoveselection(int movex, int movey);
bool copyselectedtiles();
void clearselectedmaptiles();
void pasteselectedtiles(int movex, int movey);
void getcenterselection(int * x, int * y);
void takescreenshot();

bool viewblocks = true;
bool view_only_layer = false;
bool viewwarps = true;
bool ignoreclick = false;

char findstring[FILEBUFSIZE] = "";

short g_iNumPlatforms = 0;
MapPlatform g_Platforms[MAX_PLATFORMS];

BackgroundList backgroundlist;
extern char * g_szMusicCategoryNames[8];
extern char * g_szBackgroundConversion[26];
extern short g_iMusicCategoryConversion[26];
short g_musiccategorydisplaytimer = 0;

//main main main
int main(int argc, char *argv[])
{
    /* This must occur before any data files are loaded */
    Initialize_Paths();

	bool done;

	printf("-------------------------------------------------------------------------------\n");
	printf(" %s\n", MAPTITLESTRING);
	printf("-------------------------------------------------------------------------------\n");
	printf("\n---------------- startup ----------------\n");

	gfx_init(640,480, false);
	blitdest = screen;

	SDL_WM_SetCaption(MAPTITLESTRING, "leveleditor.ico");

	printf("\n---------------- loading graphics ----------------\n");

	spr_tiletypes[0].init(convertPath("gfx/leveleditor/leveleditor_solid.png"));
	spr_tiletypes[1].init(convertPath("gfx/leveleditor/leveleditor_solid_on_top.png"));
	spr_tiletypes[2].init(convertPath("gfx/leveleditor/leveleditor_ice.png"));
	spr_tiletypes[3].init(convertPath("gfx/leveleditor/leveleditor_deadly.png"));
	spr_tiletypes[4].init(convertPath("gfx/leveleditor/leveleditor_death_on_top.png"));
	spr_tiletypes[5].init(convertPath("gfx/leveleditor/leveleditor_death_on_bottom.png"));
	spr_transparenttiles.init(convertPath("gfx/leveleditor/leveleditor_transparent_tiles.png"), 255, 0, 255, 160);
	
	spr_backgroundlevel.init(convertPath("gfx/leveleditor/leveleditor_background_levels.png"), 255, 0, 255);
	spr_tilesetlevel.init(convertPath("gfx/leveleditor/leveleditor_tileset_levels.png"), 255, 0, 255);
	
	s_eyecandy = IMG_Load(convertPathC("gfx/leveleditor/leveleditor_eyecandy.png"));
	s_eyecandyindicator = IMG_Load(convertPathC("gfx/leveleditor/leveleditor_eyecandyindicator.png"));

	s_platform = IMG_Load(convertPathC("gfx/leveleditor/leveleditor_platform.png"));

	spr_warps[0].init(convertPath("gfx/leveleditor/leveleditor_warp.png"), 255, 0, 255);
	spr_warps[1].init(convertPath("gfx/leveleditor/leveleditor_warp_preview.png"), 255, 0, 255);
	spr_warps[2].init(convertPath("gfx/leveleditor/leveleditor_warp_thumbnail.png"), 255, 0, 255);

	spr_platformarrows[0].init(convertPath("gfx/leveleditor/leveleditor_platform_arrows.png"), 255, 0, 255, 128);
	spr_platformarrows[1].init(convertPath("gfx/leveleditor/leveleditor_platform_arrows_preview.png"), 255, 0, 255, 128);
	spr_platformarrows[2].init(convertPath("gfx/leveleditor/leveleditor_platform_arrows_thumbnail.png"), 255, 0, 255, 128);

	spr_selectedtile.init(convertPath("gfx/leveleditor/leveleditor_selectedtile.png"), 0, 0, 0, 128);
	spr_nospawntile.init(convertPath("gfx/leveleditor/leveleditor_nospawntile.png"), 0, 0, 0, 128);
	spr_noitemspawntile.init(convertPath("gfx/leveleditor/leveleditor_noitemspawntile.png"), 0, 0, 0, 128);
	spr_platformstarttile.init(convertPath("gfx/leveleditor/leveleditor_platformstarttile.png"), 0, 0, 0, 64);
	spr_platformendtile.init(convertPath("gfx/leveleditor/leveleditor_selectedtile.png"), 0, 0, 0, 64);

	spr_mapitems[0].init(convertPath("gfx/leveleditor/leveleditor_mapitems.png"), 255, 0, 255);
	spr_mapitems[1].init(convertPath("gfx/leveleditor/leveleditor_mapitems_preview.png"), 255, 0, 255);
	spr_mapitems[2].init(convertPath("gfx/leveleditor/leveleditor_mapitems_thumbnail.png"), 255, 0, 255);

	spr_dialog.init(convertPath("gfx/leveleditor/leveleditor_dialog.png"), 255, 0, 255, 255);
	menu_shade.init(convertPath("gfx/leveleditor/leveleditor_shade.png"), 255, 0, 255, 128);

	if( SDL_SetColorKey(s_eyecandy, SDL_SRCCOLORKEY | SDL_RLEACCEL, SDL_MapRGB(s_eyecandy->format, 255, 0, 255)) < 0)
	{
		printf("\n ERROR: Couldn't set ColorKey + RLE: %s\n", SDL_GetError());
	}

	if( SDL_SetColorKey(s_eyecandyindicator, SDL_SRCCOLORKEY | SDL_RLEACCEL, SDL_MapRGB(s_eyecandyindicator->format, 255, 0, 255)) < 0)
	{
		printf("\n ERROR: Couldn't set ColorKey + RLE: %s\n", SDL_GetError());
	}

	if( SDL_SetColorKey(s_platform, SDL_SRCCOLORKEY | SDL_RLEACCEL, SDL_MapRGB(s_platform->format, 255, 0, 255)) < 0)
	{
		printf("\n ERROR: Couldn't set ColorKey + RLE: %s\n", SDL_GetError());
	}

	font.init(convertPath("gfx/packs/Classic/fonts/font_small.png"));
	fontbig.init(convertPath("gfx/packs/Classic/fonts/font_large.png"));

	printf("\n---------------- load map ----------------\n");

	std::string tileSetPNG[3];
	tileSetPNG[0] = convertPath("gfx/packs/Classic/tileset.png");
	tileSetPNG[1] = convertPath("gfx/packs/Classic/tileset_medium.png");
	tileSetPNG[2] = convertPath("gfx/packs/Classic/tileset_small.png");
	g_map.loadTileSet(convertPath("maps/tileset.tls"), tileSetPNG);
	
	//Setup Platforms
	for(short iPlatform = 0; iPlatform < MAX_PLATFORMS; iPlatform++)
	{
		g_Platforms[iPlatform].rIcon[0].x = (iPlatform % 6) * 32;
		g_Platforms[iPlatform].rIcon[0].y = (iPlatform / 6) * 32 + 224;
		g_Platforms[iPlatform].rIcon[0].w = 32;
		g_Platforms[iPlatform].rIcon[0].h = 32;

		g_Platforms[iPlatform].rIcon[1].x = (iPlatform % 4) * 42 + 240;
		g_Platforms[iPlatform].rIcon[1].y = (iPlatform / 4) * 42 + 174;
		g_Platforms[iPlatform].rIcon[1].w = 32;
		g_Platforms[iPlatform].rIcon[1].h = 32;

		for(short iCol = 0; iCol < MAPWIDTH; iCol++)
		{
			for(short iRow = 0; iRow < MAPHEIGHT; iRow++)
			{
				g_Platforms[iPlatform].tiles[iCol][iRow] = TILESETSIZE;
			}
		}

		g_Platforms[iPlatform].iVelocity = 4;
	}

	loadcurrentmap();
	
	printf("\n---------------- ready, steady, go! ----------------\n");

	resetselectedtiles();

	printf("entering level editor loop...\n");
	done = false;
	while(!done)
	{
		switch(state)
		{
			case EDITOR_EDIT:
				move_nodrag = false;
				state = editor_edit();
			break;

			case EDITOR_TILES:
				state = editor_tiles();
			break;

			case EDITOR_BLOCKS:
				state = editor_blocks();
			break;

			case EDITOR_MAPITEMS:
				state = editor_mapitems();
			break;

			case EDITOR_WARP:
				state = editor_warp();
			break;

			case EDITOR_EYECANDY:
				state = editor_eyecandy();
			break;

			case EDITOR_PLATFORM:
				state = editor_platforms();
			break;

			case EDITOR_TILETYPE:
				state = editor_tiletype();
			break;

			case EDITOR_BACKGROUNDS:
				state = editor_backgrounds();
			break;

			case EDITOR_QUIT:
				done = true;
			break;

			case DISPLAY_HELP:
				state = display_help();
			break;

			case SAVE_AS:
				state = save_as();
			break;

			case FIND:
				state = find();
			break;

			case CLEAR_MAP:
				state = clearMap();
			break;

			case NEW_MAP:
				state = newmap();
			break;

			case SAVE:
				state = savecurrentmap();
			break;

			default:
				printf(" PANIC: WEIRD GAMESTATE: %d\n", state);
			break;
		}
	}



	printf("\n---------------- save map ----------------\n");

	save_map(convertPath("maps/ZZleveleditor.map"));
	g_map.saveTileSet(convertPath("maps/tileset.tls"));


	printf("\n---------------- shutdown ----------------\n");
	return 0;
}


void UpdateTileType(short x, short y)
{
	TileType type = tile_nonsolid;
	for(short k = MAPLAYERS - 1; k >= 0; k--)
	{
		if(g_map.tileset[g_map.mapdata[x][y][k]] != tile_nonsolid)
		{
			type = g_map.tileset[g_map.mapdata[x][y][k]];
			break;
		}
	}

	g_map.mapdatatop[x][y] = type;
}


int editor_edit()
{
	bool done = false;
	g_musiccategorydisplaytimer = 0;

	while (!done)
	{
		int framestart = SDL_GetTicks();

		//handle messages
		while(SDL_PollEvent(&event))
		{
			Uint8 * keystate = SDL_GetKeyState(NULL);

			switch(event.type)
			{
				case SDL_QUIT:
					done = true;
				break;

				case SDL_KEYDOWN:
					if(event.key.keysym.sym == SDLK_ESCAPE)
					{
						if(g_musiccategorydisplaytimer > 0)
							g_musiccategorydisplaytimer = 0;
						else if(edit_mode != 1)
							edit_mode = 1;
						else
							done = true;
					}

					if(event.key.keysym.sym == SDLK_INSERT)
						takescreenshot();

					if(event.key.keysym.sym == SDLK_t)
						return EDITOR_TILES;

					if(event.key.keysym.sym == SDLK_i)
						return EDITOR_BLOCKS;

					if(event.key.keysym.sym == SDLK_r)
						return EDITOR_MAPITEMS;

					//if 'B' is pressed, rotate backgrounds
					if(event.key.keysym.sym == SDLK_b)
						return EDITOR_BACKGROUNDS;
					
					if(event.key.keysym.sym == SDLK_g)
					{
						backgroundlist.next();

						spr_background.init(convertPath(backgroundlist.current_name()));
						strcpy(g_map.szBackgroundFile, getFileFromPath(backgroundlist.current_name()).c_str());

						if(!keystate[SDLK_LSHIFT] && !keystate[SDLK_RSHIFT])
						{
							//Set music to background default
							for(short iCategory = 0; iCategory < MAXMUSICCATEGORY; iCategory++)
							{
								if(!strncmp(g_szMusicCategoryNames[iCategory], g_map.szBackgroundFile, strlen(g_szMusicCategoryNames[iCategory])))
								{
									g_map.musicCategoryID = iCategory;
									break;
								}
							}
						}
					}
					
					if(event.key.keysym.sym == SDLK_r)
					{
						if(g_musiccategorydisplaytimer > 0 && ++g_map.musicCategoryID >= MAXMUSICCATEGORY)
							g_map.musicCategoryID = 0;
						
						g_musiccategorydisplaytimer = 90;
					}
					
					if(event.key.keysym.sym == SDLK_s )
					{
						if(keystate[SDLK_LSHIFT] || keystate[SDLK_RSHIFT])
							return SAVE_AS;

						return SAVE;
					}

					if(event.key.keysym.sym == SDLK_f )
					{
						if(keystate[SDLK_LSHIFT] || keystate[SDLK_RSHIFT] || findstring[0] == '\0')
							return FIND;

						findcurrentstring();
					}

					if(event.key.keysym.sym == SDLK_DELETE && (keystate[SDLK_LCTRL] || keystate[SDLK_RCTRL]))
					{
						return CLEAR_MAP;
					}

					if(event.key.keysym.sym == SDLK_DELETE || event.key.keysym.sym == SDLK_BACKSPACE)
					{
						clearselectedmaptiles();
						resetselectedtiles();
					}

					if(event.key.keysym.sym == SDLK_n)
						return NEW_MAP;

					if(event.key.keysym.sym == SDLK_v)
						viewblocks = !viewblocks;
					
					if(event.key.keysym.sym == SDLK_e)
						return EDITOR_EYECANDY;
					
					if(event.key.keysym.sym == SDLK_w)
						return EDITOR_WARP;

					if(event.key.keysym.sym == SDLK_l)
						return EDITOR_TILETYPE;

					if(event.key.keysym.sym == SDLK_h || event.key.keysym.sym == SDLK_F1)
						return DISPLAY_HELP;

					if(event.key.keysym.sym == SDLK_PAGEUP)
					{
						do
						{
							maplist.prev(false);
						}
						while(!maplist.GetValid());

						loadcurrentmap();
					}

					if(event.key.keysym.sym == SDLK_PAGEDOWN)
					{
						do
						{
							maplist.next(false);
						}
						while(!maplist.GetValid());

						loadcurrentmap();
					}

					if(event.key.keysym.sym == SDLK_y)
					{
						if(++selected_layer >= MAPLAYERS)
							selected_layer = 0;
					}

					if(event.key.keysym.sym == SDLK_u)
					{
						view_only_layer = !view_only_layer;
					}

#ifdef _DEBUG
					if(event.key.keysym.sym == SDLK_HOME)
						convertAll();

					//Move map out of maps dir
					if(event.key.keysym.sym == SDLK_F12)
					{
						//printf("Path: %s\n", maplist.currentFilename());
						
						const char * szCurrentPath = maplist.currentFilename();
						char szNewPath[PATH_MAX];
						strcpy(szNewPath, "maps/moved/");
						
						const char * psz = strrchr(szCurrentPath, '/');

						if(!psz)
							psz = szCurrentPath;
						else
							psz++;

						strcat(szNewPath, psz);

						//printf("NewPath: %s\n", szNewPath);

						if(CopyFile(szCurrentPath, szNewPath, false))
						{
							unlink(szCurrentPath);
							maplist.SetValid(false);
						}
					}
#endif

					if(event.key.keysym.sym == SDLK_o)
						g_map.optimize();

					if(event.key.keysym.sym == SDLK_m)
						edit_mode = 3;

					if(event.key.keysym.sym == SDLK_x)
						edit_mode = 4;

					if(event.key.keysym.sym == SDLK_z)
						edit_mode = 5;

					if(event.key.keysym.sym == SDLK_LCTRL)
						move_nodrag = true;

					if(event.key.keysym.sym == SDLK_p)
						return EDITOR_PLATFORM;

					if(event.key.keysym.sym == SDLK_c)
					{
						if(edit_mode == 3)
						{
							if(copyselectedtiles())
							{
								move_mode = 3;
								getcenterselection(&move_start_x, &move_start_y);
							}
						}
					}


				break;

				case SDL_KEYUP:
					if(event.key.keysym.sym == SDLK_LCTRL)
						move_nodrag = false;
					break;

				case SDL_MOUSEBUTTONDOWN:
					if(event.button.button == SDL_BUTTON_LEFT && !ignoreclick)
					{
						if(edit_mode == 0) //selected blocks
						{
							g_map.objectdata[event.button.x / TILESIZE][event.button.y / TILESIZE] = set_block;
						}
						else if(edit_mode == 1) //selected tile(s)
						{
							short ix = event.button.x / TILESIZE;
							short iy = event.button.y / TILESIZE;

							for(short i = 0; i < set_tile_cols; i++)
							{
								short iLocalX = ix + i;

								for(short j = 0; j < set_tile_rows; j++)
								{
									short iLocalY = iy + j;

									if(iLocalX >= 0 && iLocalX < MAPWIDTH && iLocalY >= 0 && iLocalY < MAPHEIGHT)
									{
										g_map.mapdata[iLocalX][iLocalY][selected_layer] = (set_tile_start_y + j) * TILESETWIDTH + set_tile_start_x + i;
										UpdateTileType(iLocalX, iLocalY);
									}
								}
							}
						}
						else if(edit_mode == 2)
						{
							g_map.warpdata[event.button.x / TILESIZE][event.button.y / TILESIZE].direction = set_direction;
							g_map.warpdata[event.button.x / TILESIZE][event.button.y / TILESIZE].connection = set_connection;
						}
						else if(edit_mode == 3)
						{
							if(move_mode == 3)
							{
								move_mode = 0;

								pasteselectedtiles(move_offset_x, move_offset_y);

								copymoveselection();
								resetselectedtiles();
								pastemoveselection(move_offset_x, move_offset_y);

								move_offset_x = 0;
								move_offset_y = 0;
							}
							else
							{
								if(selectedtiles[event.button.x / TILESIZE][event.button.y / TILESIZE])
								{	
									if(copyselectedtiles())
									{
										move_mode = 1;
										clearselectedmaptiles();
									}
								}
								else
								{
									if(!keystate[SDLK_LSHIFT] && !keystate[SDLK_RSHIFT] && !keystate[SDLK_LCTRL])
										resetselectedtiles();

									if(move_nodrag)
									{
										selectedtiles[event.button.x / TILESIZE][event.button.y / TILESIZE] = true;
									}
									else
									{
										move_mode = 2;
										move_drag_start_x = event.button.x / TILESIZE;
										move_drag_start_y = event.button.y / TILESIZE;
									}
								}

								move_start_x = event.button.x / TILESIZE;
								move_start_y = event.button.y / TILESIZE;
							}
						}
						else if(edit_mode == 4)
						{
							g_map.nospawn[0][event.button.x / TILESIZE][event.button.y / TILESIZE] = true;
						}
						else if(edit_mode == 5)
						{
							g_map.nospawn[1][event.button.x / TILESIZE][event.button.y / TILESIZE] = true;
						}
						else if(edit_mode == 6)
						{
							g_map.mapdatatop[event.button.x / TILESIZE][event.button.y / TILESIZE] = set_tiletype;
						}
						else if(edit_mode == 7)
						{
							//FIXME:: Don't place map item on solid tile or block
							//and remove map item if block or solid tile is placed on it
							if(g_map.iNumMapItems < MAXMAPITEMS)
							{
								short clickx = event.button.x / TILESIZE;
								short clicky = event.button.y / TILESIZE;

								bool fItemAlreadyThere = false;
								for(short j = 0; j < g_map.iNumMapItems; j++)
								{
									if(g_map.mapitems[j].ix == clickx && g_map.mapitems[j].iy == clicky)
									{
										fItemAlreadyThere = true;
										break;
									}
								}

								if(!fItemAlreadyThere)
								{
									MapItem * mapitem = &g_map.mapitems[g_map.iNumMapItems];
									mapitem->itype = set_mapitem;
									mapitem->ix = event.button.x / TILESIZE;
									mapitem->iy = event.button.y / TILESIZE;

									g_map.iNumMapItems++;
								}
							}
						}
					}
					else if(event.button.button == SDL_BUTTON_RIGHT)
					{
						if(edit_mode == 0)
							g_map.objectdata[event.button.x / TILESIZE][event.button.y / TILESIZE] = BLOCKSETSIZE;
						else if(edit_mode == 1)
						{
							short x = event.button.x / TILESIZE;
							short y = event.button.y / TILESIZE;
                            g_map.mapdata[x][y][selected_layer] = TILESETSIZE;
							UpdateTileType(x, y);
						}
						else if(edit_mode == 2)
						{
							g_map.warpdata[event.button.x / TILESIZE][event.button.y / TILESIZE].direction = -1;
							g_map.warpdata[event.button.x / TILESIZE][event.button.y / TILESIZE].connection = -1;
						}
						else if(edit_mode == 3)
						{
							if(move_mode == 3)
							{
								move_mode = 0;
								resetselectedtiles();
								move_offset_x = 0;
								move_offset_y = 0;
							}
							else
							{
								if(selectedtiles[event.button.x / TILESIZE][event.button.y / TILESIZE])
								{
									selectedtiles[event.button.x / TILESIZE][event.button.y / TILESIZE] = false;
								}
								else
								{
									resetselectedtiles();
								}
							}
						}
						else if(edit_mode == 4)
						{
							g_map.nospawn[0][event.button.x / TILESIZE][event.button.y / TILESIZE] = false;
						}
						else if(edit_mode == 5)
						{
							g_map.nospawn[1][event.button.x / TILESIZE][event.button.y / TILESIZE] = false;
						}
						else if(edit_mode == 6)
						{
							g_map.mapdatatop[event.button.x / TILESIZE][event.button.y / TILESIZE] = tile_nonsolid;
						}
						else if(edit_mode == 7)
						{
							short clickx = event.button.x / TILESIZE;
							short clicky = event.button.y / TILESIZE;

							for(short j = 0; j < g_map.iNumMapItems; j++)
							{
								if(g_map.mapitems[j].ix == clickx && g_map.mapitems[j].iy == clicky)
								{
									g_map.iNumMapItems--;

									for(short i = j; i < g_map.iNumMapItems; i++)
									{
										g_map.mapitems[i].itype = g_map.mapitems[i + 1].itype;
										g_map.mapitems[i].ix = g_map.mapitems[i + 1].ix;
										g_map.mapitems[i].iy = g_map.mapitems[i + 1].iy;
									}

									break;
								}
							}
						}
					}
				break;

				case SDL_MOUSEMOTION:
					if(event.motion.state == SDL_BUTTON(SDL_BUTTON_LEFT) && !ignoreclick)
					{
						if(edit_mode == 0)
						{
							g_map.objectdata[event.button.x / TILESIZE][event.button.y / TILESIZE] = set_block;
						}
						else if(edit_mode == 1)
						{
							short ix = event.button.x / TILESIZE;
							short iy = event.button.y / TILESIZE;

							for(short i = 0; i < set_tile_cols; i++)
							{
								short iLocalX = ix + i;

								for(short j = 0; j < set_tile_rows; j++)
								{
									short iLocalY = iy + j;

									if(iLocalX >= 0 && iLocalX < MAPWIDTH && iLocalY >= 0 && iLocalY < MAPHEIGHT)
									{
										g_map.mapdata[iLocalX][iLocalY][selected_layer] = (set_tile_start_y + j) * TILESETWIDTH + set_tile_start_x + i;
										UpdateTileType(iLocalX, iLocalY);
									}
								}
							}
						}
						else if(edit_mode == 2)
						{
							g_map.warpdata[event.button.x / TILESIZE][event.button.y / TILESIZE].direction = set_direction;
							g_map.warpdata[event.button.x / TILESIZE][event.button.y / TILESIZE].connection = set_connection;
						}
						else if(edit_mode == 3)
						{
							if(move_mode == 0)
							{
								selectedtiles[event.button.x / TILESIZE][event.button.y / TILESIZE] = true;
							}
						}
						else if(edit_mode == 4)
						{
							g_map.nospawn[0][event.button.x / TILESIZE][event.button.y / TILESIZE] = true;
						}
						else if(edit_mode == 5)
						{
							g_map.nospawn[1][event.button.x / TILESIZE][event.button.y / TILESIZE] = true;
						}
						else if(edit_mode == 6)
						{
							g_map.mapdatatop[event.button.x / TILESIZE][event.button.y / TILESIZE] = set_tiletype;
						}
					}
					else if(event.motion.state == SDL_BUTTON(SDL_BUTTON_RIGHT))
					{
						if(edit_mode == 0)
							g_map.objectdata[event.button.x / TILESIZE][event.button.y / TILESIZE] = BLOCKSETSIZE;
						else if(edit_mode == 1)
						{
							short x = event.button.x / TILESIZE;
							short y = event.button.y / TILESIZE;
							g_map.mapdata[x][y][selected_layer] = TILESETSIZE;
							UpdateTileType(x, y);
						}
						else if(edit_mode == 2)
						{
							g_map.warpdata[event.button.x / TILESIZE][event.button.y / TILESIZE].direction = -1;
							g_map.warpdata[event.button.x / TILESIZE][event.button.y / TILESIZE].connection = -1;
						}
						else if(edit_mode == 3)
						{
							if(move_mode == 0)
								selectedtiles[event.button.x / TILESIZE][event.button.y / TILESIZE] = false;
						}
						else if(edit_mode == 4)
						{
							g_map.nospawn[0][event.button.x / TILESIZE][event.button.y / TILESIZE] = false;
						}
						else if(edit_mode == 5)
						{
							g_map.nospawn[1][event.button.x / TILESIZE][event.button.y / TILESIZE] = false;
						}
						else if(edit_mode == 6)
						{
							g_map.mapdatatop[event.button.x / TILESIZE][event.button.y / TILESIZE] = tile_nonsolid;
						}
					}
				break;
				
				case SDL_MOUSEBUTTONUP:
					if(event.button.button == SDL_BUTTON_LEFT)
					{
						ignoreclick = false;

						if(move_mode == 1)
						{
							pasteselectedtiles(move_offset_x, move_offset_y);

							copymoveselection();
							resetselectedtiles();
							pastemoveselection(move_offset_x, move_offset_y);

							move_offset_x = 0;
							move_offset_y = 0;
						}
						else if(move_mode == 2)
						{
							int currentx = event.button.x / TILESIZE;
							int currenty = event.button.y / TILESIZE;

							int left = move_drag_start_x < currentx ? move_drag_start_x : currentx;
							int top = move_drag_start_y < currenty ? move_drag_start_y : currenty;
							int right = move_drag_start_x < currentx ? currentx : move_drag_start_x;
							int bottom = move_drag_start_y < currenty ? currenty : move_drag_start_y;

							for(int k = top; k <= bottom; k++)
							{
								for(int j = left; j <= right; j++)
								{
									selectedtiles[j][k] = true;
								}
							}
						}

						move_mode = 0;
					}
				break;

				default:
					break;
			}
		}

		if(move_mode == 1 || move_mode == 3)
		{
			int mousex, mousey;
			SDL_GetMouseState(&mousex, &mousey);
			move_offset_x = (mousex / TILESIZE) - move_start_x;
			move_offset_y = (mousey / TILESIZE) - move_start_y;
		}

		if(move_mode == 2)
		{
			int mousex, mousey;
			SDL_GetMouseState(&mousex, &mousey);
			move_drag_offset_x = (mousex / TILESIZE);
			move_drag_offset_y = (mousey / TILESIZE);
		}

		if(maplist.GetValid())
		{
			drawmap(false, TILESIZE);
		}
		else
		{
			SDL_FillRect(screen, NULL, 0x0);
			fontbig.drawCentered(320, 200, "Map has been deleted.");
		}

		if(edit_mode == 0)
		{
			font.draw(0,0, "Block Mode");
		}
		else if(edit_mode == 1)
		{
			char modestring[128] = "Tile Mode - ";
			
			if(selected_layer == 0)
				strcat(modestring, "Bottom Background");
			else if(selected_layer == 1)
				strcat(modestring, "Top Background");
			else if(selected_layer == 2)
				strcat(modestring, "Bottom Foreground");
			else if(selected_layer == 3)
				strcat(modestring, "Top Foreground");

			if(view_only_layer)
				strcat(modestring, " Only");

			font.draw(0,0, modestring);

			if(view_only_layer)
				spr_backgroundlevel.draw(2, 18 + (3 - selected_layer) * 18, selected_layer * 16, (3 - selected_layer) * 18, 16, 16);
			else
				spr_backgroundlevel.draw(2, 18, selected_layer * 16, 0, 16, 70);
		}
		else if(edit_mode == 2)
		{
			font.draw(0,0, "Warp Mode");
		}

		if(edit_mode == 3)
		{
			for(int k = 0; k < MAPHEIGHT; k++)
			{
				for(int j = 0; j < MAPWIDTH; j++)
				{
					if(selectedtiles[j][k])
						spr_selectedtile.draw((j + move_offset_x) * TILESIZE, (k + move_offset_y) * TILESIZE);
				}
			}

			//Draw dragging selection
			if(move_mode == 2)
			{
				int left = move_drag_start_x < move_drag_offset_x ? move_drag_start_x : move_drag_offset_x;
				int top = move_drag_start_y < move_drag_offset_y ? move_drag_start_y : move_drag_offset_y;
				int right = move_drag_start_x < move_drag_offset_x ? move_drag_offset_x : move_drag_start_x;
				int bottom = move_drag_start_y < move_drag_offset_y ? move_drag_offset_y : move_drag_start_y;

				for(int k = top; k <= bottom; k++)
				{
					for(int j = left; j <= right; j++)
					{
						if(!selectedtiles[j][k])
							spr_selectedtile.draw(j * TILESIZE, k * TILESIZE);
					}
				}
			}

			font.draw(0,0, "Move Mode");

			if(view_only_layer)
				spr_backgroundlevel.draw(2, 18 + (3 - selected_layer) * 18, selected_layer * 16, (3 - selected_layer) * 18, 16, 16);
			else
				spr_backgroundlevel.draw(2, 18, selected_layer * 16, 0, 16, 70);
		}
		else if(edit_mode == 4)
		{
			for(int k = 0; k < MAPHEIGHT; k++)
			{
				for(int j = 0; j < MAPWIDTH; j++)
				{
					if(g_map.nospawn[0][j][k])
						spr_nospawntile.draw(j * TILESIZE, k * TILESIZE);
				}
			}

			font.draw(0, 0, "No Player Spawn Mode");
		}
		else if(edit_mode == 5)
		{
			for(int k = 0; k < MAPHEIGHT; k++)
			{
				for(int j = 0; j < MAPWIDTH; j++)
				{
					if(g_map.nospawn[1][j][k])
						spr_noitemspawntile.draw(j * TILESIZE, k  * TILESIZE);
				}
			}

			font.draw(0, 0, "No Item Spawn Mode");
		}
		else if(edit_mode == 6)
		{
			for(int k = 0; k < MAPHEIGHT; k++)
			{
				for(int j = 0; j < MAPWIDTH; j++)
				{
					if(g_map.mapdatatop[j][k] > 0)
						spr_transparenttiles.draw(j * TILESIZE, k * TILESIZE, (g_map.mapdatatop[j][k] - 1) * TILESIZE, 0, TILESIZE, TILESIZE);
				}
			}

			font.draw(0, 0, "Tile Type Mode");
		}

		font.drawRightJustified(640, 0, maplist.currentFilename());

		if(--g_musiccategorydisplaytimer > 0)
		{
			spr_dialog.draw(224, 176);
			font.drawCentered(320, 195, "Music Category");
			fontbig.drawCentered(320, 220, g_szMusicCategoryNames[g_map.musicCategoryID]);

			font.drawCentered(320, 255, "Press 'R' Again");
			font.drawCentered(320, 270, "To Change");
		}
		
		SDL_Flip(screen);

		int delay = WAITTIME - (SDL_GetTicks() - framestart);
		if(delay < 0)
			delay = 0;
		else if(delay > WAITTIME)
			delay = WAITTIME;
		
		SDL_Delay(delay);
	}

	return EDITOR_QUIT;
}

void drawlayer(int layer, bool fUseCopied, short iBlockSize)
{
	int i, j, ts;
	SDL_Rect bltrect, tilebltrect;
	
	
	tilebltrect.w = iBlockSize;
	tilebltrect.h = iBlockSize;
	bltrect.w = iBlockSize;
	bltrect.h = iBlockSize;
	
	//draw left to right full vertical
	bltrect.x = 0;
	for(i = 0; i < MAPWIDTH; i++)
	{
		bltrect.y = -iBlockSize;	//this is okay, see

		for(j = 0; j < MAPHEIGHT; j++)
		{
			bltrect.y += iBlockSize;	// here

			if((move_mode == 1 || move_mode == 3) && i - move_offset_x >= 0 && i - move_offset_x < MAPWIDTH &&
				j - move_offset_y >= 0 && j - move_offset_y < MAPHEIGHT && 
				selectedtiles[i - move_offset_x][j - move_offset_y])
			{
				if(fUseCopied)
					ts = copiedtiles[i - move_offset_x][j - move_offset_y].tile[layer];
				else
					ts = TILESETSIZE;
			}
			else
			{
				if(fUseCopied)
					ts = TILESETSIZE;
				else
					ts = g_map.mapdata[i][j][layer];
			}

			if(ts == TILESETSIZE)
				continue;

			tilebltrect.x = (ts % TILESETWIDTH) * iBlockSize;
			tilebltrect.y = (ts / TILESETWIDTH) * iBlockSize;

			SDL_BlitSurface(g_map.tilesetsurface[iBlockSize == TILESIZE ? 0 : iBlockSize == PREVIEWTILESIZE ? 1 : 2], &tilebltrect, screen, &bltrect);
		}

		bltrect.x += iBlockSize;
	}
}

void drawmap(bool fScreenshot, short iBlockSize)
{
	if(iBlockSize != TILESIZE)
	{
		SDL_Rect srcrect;
		srcrect.x = 0;
		srcrect.y = 0;
		srcrect.w = 640;
		srcrect.h = 480;

		SDL_Rect dstrect;
		dstrect.x = 0;
		dstrect.y = 0;
		dstrect.w = iBlockSize * 20;
		dstrect.h = iBlockSize * 15;

		if(SDL_SoftStretch(spr_background.getSurface(), &srcrect, blitdest, &dstrect) < 0)
		{
			fprintf(stderr, "SDL_SoftStretch error: %s\n", SDL_GetError());
			return;
		}
	}
	else
	{
		spr_background.draw(0,0);
	}

	if((view_only_layer && selected_layer == 0) || !view_only_layer)
		drawlayer(0, false, iBlockSize);

	if(!fScreenshot)
	{
		if((view_only_layer && copiedlayer == 0) || !view_only_layer)
			drawlayer(0, true, iBlockSize);
	}
	
	if((view_only_layer && selected_layer == 1) || !view_only_layer)
		drawlayer(1, false, iBlockSize);

	if(!fScreenshot)
	{
		if((view_only_layer && copiedlayer == 1) || !view_only_layer)
			drawlayer(1, true, iBlockSize);
	}

	if((viewblocks && !view_only_layer) || fScreenshot)
	{
		SDL_Rect rSrc = {0, 0, iBlockSize, iBlockSize};
		SDL_Rect rDst = {0, 0, iBlockSize, iBlockSize};

		for(int j = 0; j < MAPHEIGHT; j++)
		{
			for(int i = 0; i < MAPWIDTH; i++)
			{
				int displayblock = BLOCKSETSIZE;
				if((move_mode == 1 || move_mode == 3) && i - move_offset_x >= 0 && i - move_offset_x < MAPWIDTH &&
					j - move_offset_y >= 0 && j - move_offset_y < MAPHEIGHT && 
					selectedtiles[i - move_offset_x][j - move_offset_y])
				{
					displayblock = copiedtiles[i - move_offset_x][j - move_offset_y].block;
				}
				else
				{
					displayblock = g_map.objectdata[i][j];
				}

				if(displayblock != BLOCKSETSIZE)
				{
					rSrc.x = displayblock * iBlockSize;
					rSrc.y = iBlockSize * 30;

					rDst.x = i * iBlockSize;
					rDst.y = j * iBlockSize;

					if(displayblock >= 7 && displayblock <= 14)
						rSrc.y = iBlockSize * (g_map.iSwitches[(displayblock - 7) % 4] + 30);

					if(displayblock >= 15 && displayblock <= 18)
					{
						rSrc.x = (displayblock - 15) * iBlockSize;
						rSrc.y = iBlockSize * 31;
					}
					
					SDL_BlitSurface(g_map.tilesetsurface[iBlockSize == TILESIZE ? 0 : iBlockSize == PREVIEWTILESIZE ? 1 : 2], &rSrc, screen, &rDst);
				}
			}
		}
	}
	
	for(short j = 0; j < g_map.iNumMapItems; j++)
	{
		spr_mapitems[iBlockSize == TILESIZE ? 0 : iBlockSize == PREVIEWTILESIZE ? 1 : 2].draw(g_map.mapitems[j].ix * iBlockSize, g_map.mapitems[j].iy * iBlockSize, g_map.mapitems[j].itype * iBlockSize, 0, iBlockSize, iBlockSize);
	}

	if((view_only_layer && selected_layer == 2) || !view_only_layer)
		drawlayer(2, false, iBlockSize);

	if(!fScreenshot)
	{
		if((view_only_layer && copiedlayer == 2) || !view_only_layer)
			drawlayer(2, true, iBlockSize);
	}
	
	if((view_only_layer && selected_layer == 3) || !view_only_layer)
		drawlayer(3, false, iBlockSize);

	if(!fScreenshot)
	{
		if((view_only_layer && copiedlayer == 3) || !view_only_layer)
			drawlayer(3, true, iBlockSize);
	}

	if((viewwarps && !view_only_layer) || fScreenshot)
	{
		for(int j = 0; j < MAPHEIGHT; j++)
		{
			for(int i = 0; i < MAPWIDTH; i++)
			{
				Warp * warp = NULL;
				if((move_mode == 1 || move_mode == 3) && i - move_offset_x >= 0 && i - move_offset_x < MAPWIDTH &&
					j - move_offset_y >= 0 && j - move_offset_y < MAPHEIGHT && 
					selectedtiles[i - move_offset_x][j - move_offset_y])
				{
					warp = &copiedtiles[i - move_offset_x][j - move_offset_y].warp;
				}
				else
				{
					warp = &g_map.warpdata[i][j];
				}
				
				if(warp->connection != -1)
				{
					SDL_Rect rSrc = {warp->connection * iBlockSize, warp->direction * iBlockSize, iBlockSize, iBlockSize};
					SDL_Rect rDst = {i * iBlockSize, j * iBlockSize, iBlockSize, iBlockSize};

					SDL_BlitSurface(spr_warps[iBlockSize == TILESIZE ? 0 : iBlockSize == PREVIEWTILESIZE ? 1 : 2].getSurface(), &rSrc, screen, &rDst);
				}
			}
		}
	}

	/*
	for(int k = 0; k < g_map.numwarpexits; k++)
	{
		SDL_Rect rSrc = {g_map.warpexits[k].connection * TILESIZE, g_map.warpexits[k].direction * TILESIZE, TILESIZE, TILESIZE};
		SDL_Rect rDst = {g_map.warpexits[k].x, g_map.warpexits[k].y, TILESIZE, TILESIZE};

		SDL_BlitSurface(spr_warps[0].getSurface(), &rSrc, screen, &rDst);
	}
	*/
}

int editor_warp()
{
	bool done = false;
	
	SDL_Rect r;
	r.x = 0;
	r.y = 0;
	r.w = 640;
	r.h = 480;

	while (!done)
	{
		int framestart = SDL_GetTicks();

		//handle messages
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_QUIT:
					done = true;
				break;

				case SDL_KEYDOWN:
					edit_mode = 2;  //change to edit mode using warps
					return EDITOR_EDIT;
				break;

				case SDL_MOUSEBUTTONDOWN:
					if(event.button.button == SDL_BUTTON_LEFT)
					{
						if(event.button.x / TILESIZE < 10 && event.button.y / TILESIZE < 4)
						{
							set_direction = event.button.y / TILESIZE;
							set_connection = event.button.x / TILESIZE;
						}

						edit_mode = 2;  //change to edit mode using warps
						
						//The user must release the mouse button before trying to add a tile
						ignoreclick = true;
						
						return EDITOR_EDIT;
					}
				break;

				default:
					break;
			}
		}

		
		drawmap(false, TILESIZE);
		menu_shade.draw(0, 0);

		SDL_BlitSurface(spr_warps[0].getSurface(), NULL, screen, &r);
		font.drawRightJustified(640, 0, maplist.currentFilename());

		SDL_Flip(screen);

		int delay = WAITTIME - (SDL_GetTicks() - framestart);
		if(delay < 0)
			delay = 0;
		else if(delay > WAITTIME)
			delay = WAITTIME;
		
		SDL_Delay(delay);
	}

	return EDITOR_QUIT;
}

int editor_eyecandy()
{
	bool done = false;
	
	SDL_Rect r;
	r.x = 224;
	r.y = 128;
	r.w = 192;
	r.h = 224;

	SDL_Rect ri;
	ri.x = r.x + 20;
	ri.y = r.y + 50 + g_map.eyecandyID * 55;
	ri.w = 152;
	ri.h = 24;

	while (!done)
	{
		int framestart = SDL_GetTicks();

		//handle messages
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_QUIT:
					done = true;
				break;

				case SDL_KEYDOWN:
						return EDITOR_EDIT;
				break;

				case SDL_MOUSEBUTTONDOWN:
					if(event.button.button == SDL_BUTTON_LEFT)
					{
						for(int k = 0; k < 3; k++)
						{
							if(event.button.x >= r.x && event.button.x < r.x + r.w &&
								event.button.y >= r.y + 30 + k * 60 && event.button.y < r.y + 30 + (k + 1) * 60)
							{
								g_map.eyecandyID = k;
								ri.y = r.y + 50 + g_map.eyecandyID * 55;
								
								//The user must release the mouse button before trying to add a tile
								ignoreclick = true;
								return EDITOR_EDIT;
							}
						}
					}
				break;

				default:
					break;
			}
		}

		
		drawmap(false, TILESIZE);
		menu_shade.draw(0, 0);

		SDL_BlitSurface(s_eyecandy, NULL, screen, &r);
		SDL_BlitSurface(s_eyecandyindicator, NULL, screen, &ri);

		font.draw(0,480-font.getHeight(), "eyecandy mode: [e] edit mode, [LMB] choose eyecandy");
		font.drawRightJustified(640, 0, maplist.currentFilename());

		SDL_Flip(screen);

		int delay = WAITTIME - (SDL_GetTicks() - framestart);
		if(delay < 0)
			delay = 0;
		else if(delay > WAITTIME)
			delay = WAITTIME;
		
		SDL_Delay(delay);
	}

	return EDITOR_QUIT;
}

int editor_platforms()
{
	bool done = false;
	
	SDL_Rect r;
	r.x = 224;
	r.y = 128;
	r.w = 192;
	r.h = 224;

	SDL_Rect ri;
	ri.x = r.x + 20;
	ri.y = r.y + 50 + g_map.eyecandyID * 55;
	ri.w = 152;
	ri.h = 24;

	SDL_Rect rNewButton[2];
	rNewButton[0].x = 0;
	rNewButton[0].y = 288;
	rNewButton[0].w = 76;
	rNewButton[0].h = 32;

	rNewButton[1].x = r.x + (r.w >> 1) - (rNewButton[0].w >> 1);
	rNewButton[1].y = r.y + r.h - 64;
	rNewButton[1].w = 76;
	rNewButton[1].h = 32;

	SDL_Rect rVelocity[4];
	rVelocity[0].x = 12;
	rVelocity[0].y = 320;
	rVelocity[0].w = 172;
	rVelocity[0].h = 13;

	rVelocity[1].x = 418;
	rVelocity[1].y = 10;
	rVelocity[1].w = 172;
	rVelocity[1].h = 13;

	rVelocity[2].x = 184;
	rVelocity[2].y = 320;
	rVelocity[2].w = 8;
	rVelocity[2].h = 16;

	rVelocity[3].x = 404;
	rVelocity[3].y = 8;
	rVelocity[3].w = 8;
	rVelocity[3].h = 16;

	SDL_Rect rPath[6];
	rPath[0].x = 0;
	rPath[0].y = 336;
	rPath[0].w = 32;
	rPath[0].h = 32;

	rPath[1].x = 0;
	rPath[1].y = 0;
	rPath[1].w = 32;
	rPath[1].h = 32;

	rPath[2].x = 32;
	rPath[2].y = 336;
	rPath[2].w = 32;
	rPath[2].h = 32;

	rPath[3].x = 0;
	rPath[3].y = 0;
	rPath[3].w = 32;
	rPath[3].h = 32;

	rPath[4].x = 64;
	rPath[4].y = 336;
	rPath[4].w = 32;
	rPath[4].h = 32;

	rPath[5].x = 0;
	rPath[5].y = 0;
	rPath[5].w = 32;
	rPath[5].h = 32;

	enum {PLATFORM_EDIT_STATE_SELECT, PLATFORM_EDIT_STATE_EDIT, PLATFORM_EDIT_STATE_PATH, PLATFORM_EDIT_STATE_TEST};

	short iPlatformEditState = PLATFORM_EDIT_STATE_SELECT;
	short iEditPlatform = 0;
	short iPlatformTop, iPlatformLeft, iPlatformWidth, iPlatformHeight;
								
	while (!done)
	{
		int framestart = SDL_GetTicks();

		//handle messages
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_QUIT:
					done = true;
				break;

				case SDL_KEYDOWN:
					if(event.key.keysym.sym == SDLK_t)
					{
						if(PLATFORM_EDIT_STATE_EDIT == iPlatformEditState)
							editor_tiles();
					}
					else if(event.key.keysym.sym == SDLK_c)
					{
						if(PLATFORM_EDIT_STATE_SELECT == iPlatformEditState)
						{
							iPlatformEditState = PLATFORM_EDIT_STATE_TEST;
							insert_platforms_into_map();
							g_map.resetPlatforms();
						}
					}
					else if(event.key.keysym.sym == SDLK_DELETE)
					{
						if(PLATFORM_EDIT_STATE_EDIT == iPlatformEditState)
						{
							//Copy platforms into empty spot
							for(short iPlatform = iEditPlatform; iPlatform < g_iNumPlatforms - 1; iPlatform++)
							{
								for(short iCol = 0; iCol < MAPWIDTH; iCol++)
								{
									for(short iRow = 0; iRow < MAPHEIGHT; iRow++)
									{
										g_Platforms[iPlatform].tiles[iCol][iRow] = g_Platforms[iPlatform + 1].tiles[iCol][iRow];
									}
								}

								g_Platforms[iPlatform].iVelocity = g_Platforms[iPlatform + 1].iVelocity;
								g_Platforms[iPlatform].iStartX = g_Platforms[iPlatform + 1].iStartX;
								g_Platforms[iPlatform].iStartY = g_Platforms[iPlatform + 1].iStartY;
								g_Platforms[iPlatform].iEndX = g_Platforms[iPlatform + 1].iEndX;
								g_Platforms[iPlatform].iEndY = g_Platforms[iPlatform + 1].iEndY;
							}
							
							g_iNumPlatforms--;
							iPlatformEditState = PLATFORM_EDIT_STATE_SELECT;
						}
					}
					else if(event.key.keysym.sym == SDLK_ESCAPE)
					{
						if(PLATFORM_EDIT_STATE_SELECT == iPlatformEditState)
						{
							return EDITOR_EDIT;
						}
						else if(PLATFORM_EDIT_STATE_EDIT == iPlatformEditState)
						{
							iPlatformEditState = PLATFORM_EDIT_STATE_SELECT;
						}
						else if(PLATFORM_EDIT_STATE_PATH == iPlatformEditState)
						{
							iPlatformEditState = PLATFORM_EDIT_STATE_EDIT;
						}
						else if(PLATFORM_EDIT_STATE_TEST == iPlatformEditState)
						{
							iPlatformEditState = PLATFORM_EDIT_STATE_SELECT;
						}
					}
					else if(event.key.keysym.sym == SDLK_KP_MINUS || event.key.keysym.sym == SDLK_MINUS)
					{
						if(PLATFORM_EDIT_STATE_EDIT == iPlatformEditState)
						{
							if(g_Platforms[iEditPlatform].iVelocity > 2)
								g_Platforms[iEditPlatform].iVelocity--;
						}
					}
					else if(event.key.keysym.sym == SDLK_KP_PLUS || event.key.keysym.sym == SDLK_EQUALS)
					{
						if(PLATFORM_EDIT_STATE_EDIT == iPlatformEditState)
						{
							if(g_Platforms[iEditPlatform].iVelocity < MAX_PLATFORM_VELOCITY)
								g_Platforms[iEditPlatform].iVelocity++;
						}
					}
					else if(event.key.keysym.sym == SDLK_p)
					{
						if(PLATFORM_EDIT_STATE_EDIT == iPlatformEditState)
						{
							iPlatformEditState = PLATFORM_EDIT_STATE_PATH;
							CalculatePlatformDims(iEditPlatform, &iPlatformLeft, &iPlatformTop, &iPlatformWidth, &iPlatformHeight);
						}
					}

				break;

				case SDL_MOUSEBUTTONDOWN:
					if(event.button.button == SDL_BUTTON_LEFT)
					{
						if(PLATFORM_EDIT_STATE_SELECT == iPlatformEditState)
						{
							//check clicks on existing platforms
							for(int iPlatform = 0; iPlatform < g_iNumPlatforms; iPlatform++)
							{
								if(event.button.x >= g_Platforms[iPlatform].rIcon[1].x && event.button.x < g_Platforms[iPlatform].rIcon[1].x + g_Platforms[iPlatform].rIcon[1].w &&
								   event.button.y >= g_Platforms[iPlatform].rIcon[1].y && event.button.y < g_Platforms[iPlatform].rIcon[1].y + g_Platforms[iPlatform].rIcon[1].h)
								{
									iEditPlatform = iPlatform;
									iPlatformEditState = PLATFORM_EDIT_STATE_EDIT;
									ignoreclick = true;
								}
							}

							if(g_iNumPlatforms < MAX_PLATFORMS && event.button.x >= rNewButton[1].x && event.button.x < rNewButton[1].x + rNewButton[1].w &&
							   event.button.y >= rNewButton[1].y && event.button.y < rNewButton[1].y + rNewButton[1].h)
							{
								//Create a new platform then edit it
								
								iEditPlatform = g_iNumPlatforms;
								g_iNumPlatforms++;
								iPlatformEditState = PLATFORM_EDIT_STATE_EDIT;
								ignoreclick = true;

								SetPlatformToDefaults(iEditPlatform);
							}
						}
						else if(PLATFORM_EDIT_STATE_EDIT == iPlatformEditState)
						{
							if(!ignoreclick)
							{
								short ix = event.button.x / TILESIZE;
								short iy = event.button.y / TILESIZE;

								for(short i = 0; i < set_tile_cols; i++)
								{
									for(short j = 0; j < set_tile_rows; j++)
									{
										if(ix + i >= 0 && ix + i < MAPWIDTH && iy + j >= 0 && iy + j < MAPHEIGHT)
											g_Platforms[iEditPlatform].tiles[ix + i][iy + j] = (set_tile_start_y + j) * TILESETWIDTH + set_tile_start_x + i;
									}
								}
							}
						}
						else if(PLATFORM_EDIT_STATE_PATH == iPlatformEditState)
						{
							if(!ignoreclick)
							{
								short iTempStartX = event.button.x / TILESIZE;
								short iTempStartY = event.button.y / TILESIZE;
								short iTempEndX = g_Platforms[iEditPlatform].iEndX;
								short iTempEndY = g_Platforms[iEditPlatform].iEndY;

								if(iTempStartX >= 0 && iTempStartX <= MAPWIDTH - iPlatformWidth &&
									iTempStartY >= 0 && iTempStartY <= MAPHEIGHT - iPlatformHeight)
								{
									//If the start isn't in the same row or column, then move the end point so that it is
									if(iTempStartX != iTempEndX && iTempStartY != iTempEndY)
									{
										short iXDiff = abs(iTempStartX - iTempEndX);
										short iYDiff = abs(iTempStartY - iTempEndY);

										//If the x difference is smaller, move it so it lines up
										if(iYDiff > iXDiff)
											iTempEndX = iTempStartX;
										else
											iTempEndY = iTempStartY;
									}

									//If the start and end are the same point, then don't allow this change
									if(iTempStartX != iTempEndX ||
										iTempStartY != iTempEndY)
									{
										g_Platforms[iEditPlatform].iStartX = iTempStartX;
										g_Platforms[iEditPlatform].iStartY = iTempStartY;
										g_Platforms[iEditPlatform].iEndX = iTempEndX;
										g_Platforms[iEditPlatform].iEndY = iTempEndY;
									}
								}
							}
						}
						break;
					}
					else if(event.button.button == SDL_BUTTON_RIGHT)
					{
						if(PLATFORM_EDIT_STATE_EDIT == iPlatformEditState)
						{
							g_Platforms[iEditPlatform].tiles[event.button.x / TILESIZE][event.button.y / TILESIZE] = TILESETSIZE;
						}
						else if(PLATFORM_EDIT_STATE_PATH == iPlatformEditState)
						{
							if(!ignoreclick)
							{
								short iTempStartX = g_Platforms[iEditPlatform].iStartX;
								short iTempStartY = g_Platforms[iEditPlatform].iStartY;
								short iTempEndX = event.button.x / TILESIZE;
								short iTempEndY = event.button.y / TILESIZE;
							
								if(iTempEndX >= 0 && iTempEndX <= MAPWIDTH - iPlatformWidth &&
									iTempEndY >= 0 && iTempEndY <= MAPHEIGHT - iPlatformHeight)
								{

									//If the start isn't in the same row or column, then move the end point so that it is
									if(iTempStartX != iTempEndX &&
										iTempStartY != iTempEndY)
									{
										short iXDiff = abs(iTempStartX - iTempEndX);
										short iYDiff = abs(iTempStartY - iTempEndY);

										//If the x difference is smaller, move it so it lines up
										if(iYDiff > iXDiff)
											iTempStartX = iTempEndX;
										else
											iTempStartY = iTempEndY;
									}

									//If the start and end are the same point, then don't allow this change
									if(iTempStartX != iTempEndX ||
										iTempStartY != iTempEndY)
									{
										g_Platforms[iEditPlatform].iStartX = iTempStartX;
										g_Platforms[iEditPlatform].iStartY = iTempStartY;
										g_Platforms[iEditPlatform].iEndX = iTempEndX;
										g_Platforms[iEditPlatform].iEndY = iTempEndY;
									}
								}
							}
						}
					}
				
				case SDL_MOUSEMOTION:
					if(PLATFORM_EDIT_STATE_EDIT == iPlatformEditState)
					{
						if(event.motion.state == SDL_BUTTON(SDL_BUTTON_LEFT) && !ignoreclick)
						{
							short ix = event.button.x / TILESIZE;
							short iy = event.button.y / TILESIZE;

							for(short i = 0; i < set_tile_cols; i++)
							{
								for(short j = 0; j < set_tile_rows; j++)
								{
									if(ix + i >= 0 && ix + i < MAPWIDTH && iy + j >= 0 && iy + j < MAPHEIGHT)
										g_Platforms[iEditPlatform].tiles[ix + i][iy + j] = (set_tile_start_y + j) * TILESETWIDTH + set_tile_start_x + i;
								}
							}
						}
						else if(event.motion.state == SDL_BUTTON(SDL_BUTTON_RIGHT))
						{
							g_Platforms[iEditPlatform].tiles[event.button.x / TILESIZE][event.button.y / TILESIZE] = TILESETSIZE;
						}
					}

					break;

				case SDL_MOUSEBUTTONUP:
					if(event.button.button == SDL_BUTTON_LEFT)
					{
						ignoreclick = false;
					}
					break;

				default:
					break;
			}
		}

		//Draw platform editing
		
		drawmap(false, TILESIZE);

		if(PLATFORM_EDIT_STATE_TEST != iPlatformEditState)
			menu_shade.draw(0, 0);

		if(PLATFORM_EDIT_STATE_SELECT == iPlatformEditState)
		{
			SDL_Rect rp;
			rp.x = 0;
			rp.y = 0;
			rp.w = 192;
			rp.h = 224;

			SDL_BlitSurface(s_platform, &rp, screen, &r);

			font.draw(0, 480 - font.getHeight(), "Platform Mode: [esc] Exit  [c] Check Paths");
			font.drawRightJustified(640, 0, maplist.currentFilename());

			for(int iPlatform = 0; iPlatform < g_iNumPlatforms; iPlatform++)
				SDL_BlitSurface(s_platform, &g_Platforms[iPlatform].rIcon[0], screen, &g_Platforms[iPlatform].rIcon[1]);

			if(g_iNumPlatforms < MAX_PLATFORMS)
				SDL_BlitSurface(s_platform, &rNewButton[0], screen, &rNewButton[1]);

		}
		else if(PLATFORM_EDIT_STATE_EDIT == iPlatformEditState)
		{
			font.draw(0, 480 - font.getHeight(), "Edit Platform: [esc] Exit  [t] Tiles  [del] Delete  [p] Path  [+/-] Velocity");
			draw_platform(iEditPlatform);

			rVelocity[3].x = 404 + (g_Platforms[iEditPlatform].iVelocity - 1) * 12;

			SDL_BlitSurface(s_platform, &rVelocity[0], screen, &rVelocity[1]);
			SDL_BlitSurface(s_platform, &rVelocity[2], screen, &rVelocity[3]);

			font.drawRightJustified(413, 10, "Slow");
			font.draw(594, 10, "Fast");
		}
		else if(PLATFORM_EDIT_STATE_PATH == iPlatformEditState)
		{
			for(short iCol = g_Platforms[iEditPlatform].iStartX; iCol < g_Platforms[iEditPlatform].iStartX + iPlatformWidth; iCol++)
			{
				for(short iRow = g_Platforms[iEditPlatform].iStartY; iRow < g_Platforms[iEditPlatform].iStartY + iPlatformHeight; iRow++)
				{
					spr_platformstarttile.draw(iCol * TILESIZE, iRow * TILESIZE);
				}
			}

			for(short iCol = g_Platforms[iEditPlatform].iEndX; iCol < g_Platforms[iEditPlatform].iEndX + iPlatformWidth; iCol++)
			{
				for(short iRow = g_Platforms[iEditPlatform].iEndY; iRow < g_Platforms[iEditPlatform].iEndY + iPlatformHeight; iRow++)
				{
					spr_platformendtile.draw(iCol * TILESIZE, iRow * TILESIZE);
				}
			}

			font.draw(0, 480 - font.getHeight(), "Edit Path: [esc] Exit  [LMB] Set Start Point  [RMB] Set End Point");

			rPath[1].x = g_Platforms[iEditPlatform].iStartX * 32;
			rPath[1].y = g_Platforms[iEditPlatform].iStartY * 32;

			rPath[3].x = g_Platforms[iEditPlatform].iEndX * 32;
			rPath[3].y = g_Platforms[iEditPlatform].iEndY * 32;

			SDL_BlitSurface(s_platform, &rPath[0], screen, &rPath[1]);
			SDL_BlitSurface(s_platform, &rPath[2], screen, &rPath[3]);

			if(g_Platforms[iEditPlatform].iStartX != g_Platforms[iEditPlatform].iEndX)
			{
				short iSpotLeft = (g_Platforms[iEditPlatform].iStartX < g_Platforms[iEditPlatform].iEndX ? g_Platforms[iEditPlatform].iStartX + 1 : g_Platforms[iEditPlatform].iEndX + 1);
				short iSpotRight = (g_Platforms[iEditPlatform].iStartX < g_Platforms[iEditPlatform].iEndX ? g_Platforms[iEditPlatform].iEndX - 1 : g_Platforms[iEditPlatform].iStartX - 1);

				for(short iSpot = iSpotLeft; iSpot <= iSpotRight; iSpot++)
				{
					rPath[5].x = iSpot * TILESIZE;
					rPath[5].y = g_Platforms[iEditPlatform].iStartY * TILESIZE;
					SDL_BlitSurface(s_platform, &rPath[4], screen, &rPath[5]);
				}
			}
			else
			{
				short iSpotTop = (g_Platforms[iEditPlatform].iStartY < g_Platforms[iEditPlatform].iEndY ? g_Platforms[iEditPlatform].iStartY + 1 : g_Platforms[iEditPlatform].iEndY + 1);
				short iSpotBottom = (g_Platforms[iEditPlatform].iStartY < g_Platforms[iEditPlatform].iEndY ? g_Platforms[iEditPlatform].iEndY - 1 : g_Platforms[iEditPlatform].iStartY - 1);

				for(short iSpot = iSpotTop; iSpot <= iSpotBottom; iSpot++)
				{
					rPath[5].x = g_Platforms[iEditPlatform].iStartX * TILESIZE;
					rPath[5].y = iSpot * TILESIZE;
					SDL_BlitSurface(s_platform, &rPath[4], screen, &rPath[5]);
				}
			}

			
			/*
			SDL_Rect rFillRect[2];
			rFillRect[0].x = 640 - (iPlatformWidth - 1) * TILESIZE;
			rFillRect[0].y = 0;
			rFillRect[0].w = (iPlatformWidth - 1) * TILESIZE;
			rFillRect[0].h = 480;

			rFillRect[1].x = 0;
			rFillRect[1].y = 480 - (iPlatformHeight - 1) * TILESIZE;
			rFillRect[1].w = 640;
			rFillRect[1].h = (iPlatformHeight - 1) * TILESIZE;

			SDL_FillRect(screen, &rFillRect[0], 0x0);
			SDL_FillRect(screen, &rFillRect[1], 0x0);
			*/
		}
		else if(PLATFORM_EDIT_STATE_TEST == iPlatformEditState)
		{
			font.draw(0, 480 - font.getHeight(), "Check Paths: [esc] Exit");
			
			g_map.updatePlatforms();
			g_map.drawPlatforms();
		}

		SDL_Flip(screen);

		int delay = WAITTIME - (SDL_GetTicks() - framestart);
		if(delay < 0)
			delay = 0;
		else if(delay > WAITTIME)
			delay = WAITTIME;
		
		SDL_Delay(delay);
	}

	return EDITOR_QUIT;
}

void draw_platform(short iPlatform)
{
	SDL_Rect bltrect, tilebltrect;
	
	tilebltrect.w = TILESIZE;
	tilebltrect.h = TILESIZE;
	
	bltrect.x = 0;
	bltrect.y = 0;
	bltrect.w = TILESIZE;
	bltrect.h = TILESIZE;
	
	for(short iCol = 0; iCol < MAPWIDTH; iCol++)
	{
		for(short iRow = 0; iRow < MAPHEIGHT; iRow++)
		{
			short ts = g_Platforms[iPlatform].tiles[iCol][iRow];
			
			if(ts == TILESETSIZE)
			{
				bltrect.y += TILESIZE;
				continue;
			}

			tilebltrect.x = (ts % TILESETWIDTH) * TILESIZE;
			tilebltrect.y = (ts / TILESETWIDTH) * TILESIZE;
		
			SDL_BlitSurface(g_map.tilesetsurface[0], &tilebltrect, screen, &bltrect);

			bltrect.y += TILESIZE;
		}

		bltrect.x += TILESIZE;
		bltrect.y = 0;
	}
}

int editor_tiles()
{
	int i, j, t;
	bool done = false;
	
	SDL_Rect r;
	r.x = 0;
	r.y = 0;
	r.w = 640;
	r.h = 480;

	/*
	short iCurrentTile = 0;
	short iConvertedTile[300];

	for(short iTile = 0; iTile < 300; iTile++)
		iConvertedTile[iTile] = g_iTileConversion[iTile];
	*/

	set_tile_drag = false;

	while (!done)
	{
		int framestart = SDL_GetTicks();

		//handle messages
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_QUIT:
					done = true;
				break;

				case SDL_KEYDOWN:
				{	
					if(!set_tile_drag)
					{
						if(event.key.keysym.sym >= SDLK_1 && event.key.keysym.sym <= SDLK_4)
							selected_tileset = event.key.keysym.sym - SDLK_1;
						else if(event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_PAGEUP)
						{
							if(selected_tileset > 0)
								selected_tileset--;
						}
						else if(event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_PAGEDOWN)
						{
							if(selected_tileset < 3)
								selected_tileset++;
						}
						else
						{
							edit_mode = 1;  //change to edit mode using tiles
							set_tile_drag = false;
							return EDITOR_EDIT;
						}
					}
				
					break;
				}

				case SDL_MOUSEBUTTONDOWN:
				{
					if(event.button.button == SDL_BUTTON_LEFT)
					{
						if(event.button.x < 512)
						{
							set_tile_start_x = event.button.x / TILESIZE;
							set_tile_start_y = event.button.y / TILESIZE;
							set_tile_end_x = set_tile_start_x;
							set_tile_end_y = set_tile_start_y;

							set_tile_drag = true;
						}
						else
						{
							for(short i = 0; i < 4; i++)
							{
								if(event.button.x >= 565 + i * 18 && event.button.x < 565 + i * 18 + 16 &&
									event.button.y >= 459 && event.button.y < 475)
								{
									selected_tileset = i;
								}
							}
						}
					}
					else if(event.button.button == SDL_BUTTON_RIGHT)
					{
						t = (event.button.x / TILESIZE) + (event.button.y / TILESIZE) * TILESETWIDTH;
						
						if(selected_tileset == 1 || selected_tileset == 3)
							t += 16;

						if(selected_tileset == 2 || selected_tileset == 3)
							t += 480;

						if(t >= TILESETSIZE)
							t = 0; 

						set_type = g_map.tileset[t];

						if(set_type == tile_nonsolid)
							set_type = tile_solid;
						else if(set_type == tile_solid)
							set_type = tile_solid_on_top;
						else if(set_type == tile_solid_on_top)
							set_type = tile_ice;
						else if(set_type == tile_ice)
							set_type = tile_death;
						else if(set_type == tile_death)
							set_type = tile_death_on_top;
						else if(set_type == tile_death_on_top)
							set_type = tile_death_on_bottom;
						else if(set_type == tile_death_on_bottom)
							set_type = tile_nonsolid;

						g_map.tileset[t] = set_type;	
					}
					
					break;
				}

				case SDL_MOUSEBUTTONUP:
				{
					if(event.button.button == SDL_BUTTON_LEFT)
					{
						if(event.button.x < 512)
						{
							if(selected_tileset == 1 || selected_tileset == 3)
							{
								set_tile_start_x += 16;
								set_tile_end_x += 16;
							}
						
							if(selected_tileset == 2 || selected_tileset == 3)
							{
								set_tile_start_y += 15;
								set_tile_end_y += 15;
							}

							set_tile_cols = set_tile_end_x - set_tile_start_x + 1;
							set_tile_rows = set_tile_end_y - set_tile_start_y + 1;

							set_tile_drag = false;
							edit_mode = 1;  //change to edit mode using tiles
							return EDITOR_EDIT;
							
						}
					}
									
					break;
				}

				case SDL_MOUSEMOTION:
				{
					if(event.button.x < 512)
					{
						if(event.motion.state == SDL_BUTTON(SDL_BUTTON_LEFT))
						{
							short x = event.button.x / TILESIZE;
							short y = event.button.y / TILESIZE;

							if(x < set_tile_start_x)
								set_tile_start_x = x;
							
							if(x > set_tile_end_x)
								set_tile_end_x = x;

							if(y < set_tile_start_y)
								set_tile_start_y = y;
							
							if(y > set_tile_end_y)
								set_tile_end_y = y;
						}
						else if(event.motion.state == SDL_BUTTON(SDL_BUTTON_RIGHT))
						{
							t = (event.button.x / TILESIZE) + (event.button.y / TILESIZE) * TILESETWIDTH;
							
							if(selected_tileset == 1 || selected_tileset == 3)
								t += 16;

							if(selected_tileset == 2 || selected_tileset == 3)
								t += 480;

							if(t >= TILESETSIZE)
								t = 0; 

							g_map.tileset[t] = set_type;
						}
					}
					
					break;
				}
				

				default:
					break;
			}
		}

		
		//drawmap(false, TILESIZE);
		//menu_shade.draw(0, 0);

		SDL_FillRect(screen, NULL, 0x0);

		SDL_Rect rectSrc;
		rectSrc.x = selected_tileset % 2 * 512;
		rectSrc.y = selected_tileset / 2 * 480;
		rectSrc.w = 512;
		rectSrc.h = 480;

		SDL_BlitSurface(g_map.tilesetsurface[0], &rectSrc, screen, &r);
		//font.drawRightJustified(640, 0, maplist.currentFilename());
		
		short iTileOffset = (selected_tileset > 1 ? 480 : 0) + (selected_tileset % 2 ? 16 : 0);

		for(i = 0; i < TILESETWIDTH >> 1; i++)
		{
			for(j = 0; j < TILESETHEIGHT >> 1; j++)
			{
				t = g_map.tileset[i + j * TILESETWIDTH + iTileOffset];
				if(t != tile_nonsolid)
					spr_tiletypes[t-1].draw(i*TILESIZE, j*TILESIZE);
			}
		}

		//Draw the selected tileset indicator
		for(i = 0; i < 4; i++)
			spr_tilesetlevel.draw(565 + i * 18, 459, i * 16, selected_tileset == i ? 0 : 16, 16, 16);

		if(set_tile_drag)
		{
			for(i = set_tile_start_x; i <= set_tile_end_x; i++)
			{
				for(j = set_tile_start_y; j <= set_tile_end_y; j++)
				{
					spr_selectedtile.draw(i * TILESIZE, j * TILESIZE);
				}
			}
		}
		/*
		
		//Test code to help convert old tilesets into new tilesets

		font.drawRightJustified(640, 0, "%d", iCurrentTile);
		spr_OldTileSet.draw(576, 224, iCurrentTile % 20 * 32, iCurrentTile / 20 * 32, 32, 32);
		

		short iTileCol = iConvertedTile[iCurrentTile] % 32;
		short iTileRow = iConvertedTile[iCurrentTile] / 32;

		if(selected_tileset == 0 && iTileCol < 16 && iTileRow < 15)
			spr_selectedtile.draw(iTileCol * 32, iTileRow * 32, 0, 0, 32, 32);
		else if(selected_tileset == 1 && iTileCol >= 16 && iTileRow < 15)
			spr_selectedtile.draw((iTileCol - 16) * 32, iTileRow * 32, 0, 0, 32, 32);
		else if(selected_tileset == 2 && iTileCol < 16 && iTileRow >= 15)
			spr_selectedtile.draw(iTileCol * 32, (iTileRow - 15) * 32, 0, 0, 32, 32);
		else if(selected_tileset == 3 && iTileCol >= 16 && iTileRow >= 15)
			spr_selectedtile.draw((iTileCol - 16) * 32, (iTileRow - 15) * 32, 0, 0, 32, 32);


		*/


		SDL_Flip(screen);

		int delay = WAITTIME - (SDL_GetTicks() - framestart);
		if(delay < 0)
			delay = 0;
		else if(delay > WAITTIME)
			delay = WAITTIME;
		
		SDL_Delay(delay);
	}

	return EDITOR_QUIT;
}

int editor_blocks()
{
	bool done = false;
	
	while (!done)
	{
		int framestart = SDL_GetTicks();

		//handle messages
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_QUIT:
					done = true;
				break;

				case SDL_KEYDOWN:
					edit_mode = 0;
					return EDITOR_EDIT;
				break;

				case SDL_MOUSEBUTTONDOWN:
					if(event.button.button == SDL_BUTTON_LEFT)
					{
						short set_block_x = event.button.x / TILESIZE;
						short set_block_y = event.button.y / TILESIZE;

						//Set the selected block to one of the interaction blocks
						if(set_block_y == 0 && set_block_x >= 0 && set_block_x <= 6)
							set_block = set_block_x;
						else if(set_block_y == 0 && set_block_x >= 7 && set_block_x <= 10)
							set_block = set_block_x + 8;
						else if(set_block_y >= 1 && set_block_y <= 2 && set_block_x >= 0 && set_block_x <= 3)
						{  //set the selected block to an on/off switch block
							
							set_block = set_block_x + 7;
							g_map.iSwitches[set_block_x] = set_block_y - 1;
						}
						else if(set_block_y == 3 && set_block_x >= 0 && set_block_x <= 3)
						{  //set the selected block to a switch block
							
							set_block = set_block_x + 11;
						}

						edit_mode = 0;

						//The user must release the mouse button before trying to add a tile
						ignoreclick = true;
						return EDITOR_EDIT;
					}
				break;

				default:
					break;
			}
		}

		drawmap(false, TILESIZE);
		menu_shade.draw(0, 0);
		
		SDL_Rect rSrc = {0, 960, 224, 32};
		SDL_Rect rDst = {0, 0, 224, 32};

		SDL_BlitSurface(g_map.tilesetsurface[0], &rSrc, screen, &rDst);

		SDL_Rect rOnOffSrc = {224, 960, 128, 64};
		SDL_Rect rOnOffDst = {0, 32, 128, 64};

		SDL_BlitSurface(g_map.tilesetsurface[0], &rOnOffSrc, screen, &rOnOffDst);

		SDL_Rect rOnOffBlockSrc = {352, 960, 128, 32};
		SDL_Rect rOnOffBlockDst = {0, 96, 128, 32};

		SDL_BlitSurface(g_map.tilesetsurface[0], &rOnOffBlockSrc, screen, &rOnOffBlockDst);

		SDL_Rect rBlocksRow2Src = {0, 992, 128, 32};
		SDL_Rect rBlocksRow2Dst = {224, 0, 128, 32};

		SDL_BlitSurface(g_map.tilesetsurface[0], &rBlocksRow2Src, screen, &rBlocksRow2Dst);

		font.drawRightJustified(640, 0, maplist.currentFilename());
				
		SDL_Flip(screen);

		int delay = WAITTIME - (SDL_GetTicks() - framestart);
		if(delay < 0)
			delay = 0;
		else if(delay > WAITTIME)
			delay = WAITTIME;
		
		SDL_Delay(delay);
	}

	return EDITOR_QUIT;
}


int editor_mapitems()
{
	bool done = false;
	
	while (!done)
	{
		int framestart = SDL_GetTicks();

		//handle messages
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_QUIT:
					done = true;
				break;

				case SDL_KEYDOWN:
					edit_mode = 0;
					return EDITOR_EDIT;
				break;

				case SDL_MOUSEBUTTONDOWN:
					if(event.button.button == SDL_BUTTON_LEFT)
					{
						short set_item_x = event.button.x / TILESIZE;
						short set_item_y = event.button.y / TILESIZE;

						//Set the selected block to one of the interaction blocks
						if(set_item_y == 0 && set_item_x >= 0 && set_item_x <= 1)
							set_mapitem = set_item_x;

						edit_mode = 7;

						//The user must release the mouse button before trying to add a tile
						ignoreclick = true;
						return EDITOR_EDIT;
					}
				break;

				default:
					break;
			}
		}

		drawmap(false, TILESIZE);
		menu_shade.draw(0, 0);
		
		SDL_Rect rSrc = {0, 960, 224, 32};
		SDL_Rect rDst = {0, 0, 224, 32};

		spr_mapitems[0].draw(0, 0, 0, 0, 64, 32);

		font.drawRightJustified(640, 0, maplist.currentFilename());
				
		SDL_Flip(screen);

		int delay = WAITTIME - (SDL_GetTicks() - framestart);
		if(delay < 0)
			delay = 0;
		else if(delay > WAITTIME)
			delay = WAITTIME;
		
		SDL_Delay(delay);
	}

	return EDITOR_QUIT;
}


int editor_tiletype()
{
	bool done = false;
	
	while (!done)
	{
		int framestart = SDL_GetTicks();

		//handle messages
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_QUIT:
					done = true;
				break;

				case SDL_KEYDOWN:
				{	
					edit_mode = 6;
					return EDITOR_EDIT;
				
					break;
				}

				case SDL_MOUSEBUTTONDOWN:
				{
					if(event.button.button == SDL_BUTTON_LEFT)
					{
						if(event.button.x < 192 && event.button.y < 32)
						{
							set_tiletype = (TileType)((event.button.x / TILESIZE) + 1);

							edit_mode = 6;
							ignoreclick = true;
							return EDITOR_EDIT;
						}
					}
				
					break;
				}
				
				default:
					break;
			}
		}

		
		drawmap(false, TILESIZE);
		menu_shade.draw(0, 0);
		
		spr_transparenttiles.draw(0, 0);

		font.drawRightJustified(640, 0, maplist.currentFilename());
				
		SDL_Flip(screen);

		int delay = WAITTIME - (SDL_GetTicks() - framestart);
		if(delay < 0)
			delay = 0;
		else if(delay > WAITTIME)
			delay = WAITTIME;
		
		SDL_Delay(delay);
	}

	return EDITOR_QUIT;
}

int editor_backgrounds()
{
	bool done = false;
	short iPage = backgroundlist.GetCurrentIndex() / 16;

	SDL_Surface * sBackgrounds[16];
	SDL_Rect rSrc = {0, 0, 160, 120};

	SDL_Rect rDst[16];

	for(short iRectY = 0; iRectY < 4; iRectY++)
	{
		for(short iRectX = 0; iRectX < 4; iRectX++)
		{
			rDst[iRectY * 4 + iRectX].x = iRectX * 160;
			rDst[iRectY * 4 + iRectX].y = iRectY * 120;
			rDst[iRectY * 4 + iRectX].w = 160;
			rDst[iRectY * 4 + iRectX].h = 120;
		}
	}

	for(short iSurface = 0; iSurface < 16; iSurface++)
		sBackgrounds[iSurface] = SDL_CreateRGBSurface(screen->flags, 160, 120, 16, 0, 0, 0, 0);

	LoadBackgroundPage(sBackgrounds, iPage);

	while (!done)
	{
		int framestart = SDL_GetTicks();

		//handle messages
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_QUIT:
					done = true;
				break;

				case SDL_KEYDOWN:
					if(event.key.keysym.sym == SDLK_ESCAPE)
					{
						done = true;
					}
					else if(event.key.keysym.sym == SDLK_PAGEDOWN || event.key.keysym.sym == SDLK_DOWN)
					{
						if((iPage + 1) * 16 < backgroundlist.GetCount())
						{
							iPage++;
							LoadBackgroundPage(sBackgrounds, iPage);
						}
					}
					else if(event.key.keysym.sym == SDLK_PAGEUP || event.key.keysym.sym == SDLK_UP)
					{
						if((iPage - 1) * 16 >= 0)
						{
							iPage--;
							LoadBackgroundPage(sBackgrounds, iPage);
						}
					}
					
				break;

				case SDL_MOUSEBUTTONDOWN:
					if(event.button.button == SDL_BUTTON_LEFT || event.button.button == SDL_BUTTON_RIGHT)
					{
						for(short iBackground = 0; iBackground < 16; iBackground++)
						{
							if(iPage * 16 + iBackground >= backgroundlist.GetCount())
								break;

							if(event.button.x >= rDst[iBackground].x && event.button.x < rDst[iBackground].x + rDst[iBackground].w && 
								event.button.y >= rDst[iBackground].y && event.button.y < rDst[iBackground].y + rDst[iBackground].h)
							{
								done = true;
								backgroundlist.SetCurrent(iPage * 16 + iBackground);

								spr_background.init(convertPath(backgroundlist.current_name()));
								strcpy(g_map.szBackgroundFile, getFileFromPath(backgroundlist.current_name()).c_str());

								if(event.button.button == SDL_BUTTON_LEFT)
								{
									//Set music to background default
									for(short iCategory = 0; iCategory < MAXMUSICCATEGORY; iCategory++)
									{
										if(!strncmp(g_szMusicCategoryNames[iCategory], g_map.szBackgroundFile, strlen(g_szMusicCategoryNames[iCategory])))
										{
											g_map.musicCategoryID = iCategory;
											break;
										}
									}
								}
							}
						}
					}
				break;

				default:
					break;
			}
		}

		SDL_Rect rect = {0, 0, 640, 480};
		SDL_FillRect(screen, &rect, 0x0);

		for(short iBackground = 0; iBackground < 16; iBackground++)
		{
			if(iPage * 16 + iBackground >= backgroundlist.GetCount())
				break;

			SDL_BlitSurface(sBackgrounds[iBackground], &rSrc, screen, &rDst[iBackground]);
		}

		font.draw(0,480-font.getHeight() * 2, "[Page Up] next page, [Page Down] previous page");
		font.draw(0,480-font.getHeight(), "[LMB] choose background with music category, [RMB] choose just background");

		int x, y;

		SDL_GetMouseState(&x, &y);

		int iID = x / 160 + y / 120 * 4 + iPage * 16;

		if(iID < backgroundlist.GetCount())
			font.draw(0, 0, backgroundlist.GetIndex(iID));

		SDL_Flip(screen);

		int delay = WAITTIME - (SDL_GetTicks() - framestart);
		if(delay < 0)
			delay = 0;
		else if(delay > WAITTIME)
			delay = WAITTIME;
		
		SDL_Delay(delay);
	}

	for(short iSurface = 0; iSurface < 16; iSurface++)
		SDL_FreeSurface(sBackgrounds[iSurface]);

	return EDITOR_EDIT;
}

void LoadBackgroundPage(SDL_Surface ** sBackgrounds, short iPage)
{
	SDL_Rect srcRectBackground = {0, 0, 640, 480};
	SDL_Rect dstRectBackground = {0, 0, 160, 120};

	for(short iIndex = 0; iIndex < 16; iIndex++)
	{
		const char * szFileName = backgroundlist.GetIndex(iPage * 16 + iIndex);

		if(!szFileName)
			return;

		SDL_Surface * temp = IMG_Load(szFileName);

		SDL_Surface * sBackground = SDL_DisplayFormat(temp);
		if(!sBackground)
		{
			printf("ERROR: Couldn't convert thumbnail background to diplay pixel format: %s\n", SDL_GetError());
			return;
		}

		SDL_FreeSurface(temp);

		if(SDL_SoftStretch(sBackground, &srcRectBackground, sBackgrounds[iIndex], &dstRectBackground) < 0)
		{
			fprintf(stderr, "SDL_SoftStretch error: %s\n", SDL_GetError());
			return;
		}

		SDL_FreeSurface(sBackground);
	}
}

int display_help()
{
	//unsigned int currentChar = 0;

	drawmap(false, TILESIZE);
	menu_shade.draw(0, 0);
	fontbig.drawCentered(320, 15, "Help");
	
	int offsety = 55;
	int offsetx = 20;
	font.draw(offsetx, offsety, "Modes:");
	offsety += font.getHeight() + 2;
	font.draw(offsetx, offsety, "[t] - Tile Mode");
	offsety += font.getHeight() + 2;
	font.draw(offsetx, offsety, "[i] - Block Mode");
	offsety += font.getHeight() + 2;
	font.draw(offsetx, offsety, "[w] - Warp Mode");
	offsety += font.getHeight() + 2;
	font.draw(offsetx, offsety, "[m] - Move Mode");
	offsety += font.getHeight() + 2;
	font.draw(offsetx, offsety, "[l] - Tile Type Mode");
	offsety += font.getHeight() + 2;
	font.draw(offsetx, offsety, "[p] - Platform Mode");
	offsety += font.getHeight() + 2;
	font.draw(offsetx, offsety, "[x] - No Player Spawn Area");
	offsety += font.getHeight() + 2;
	font.draw(offsetx, offsety, "[z] - No Item Spawn Area");
	offsety += font.getHeight() + 20;

	font.draw(offsetx, offsety, "Layers:");
	offsety += font.getHeight() + 2;
	font.draw(offsetx, offsety, "[v] - Hide Blocks");
	offsety += font.getHeight() + 2;
	font.draw(offsetx, offsety, "[y] - Select Active Tile Layer");
	offsety += font.getHeight() + 2;
	font.draw(offsetx, offsety, "[u] - Hide Inactive Tile Layers");
	offsety += font.getHeight() + 2;
	font.draw(offsetx, offsety, "[o] - Optimize Layers");
	offsety += font.getHeight() + 20;

	font.draw(offsetx, offsety, "File:");
	offsety += font.getHeight() + 2;
	font.draw(offsetx, offsety, "[n] - New Map");
	offsety += font.getHeight() + 2;
	font.draw(offsetx, offsety, "[s] - Save Map");
	offsety += font.getHeight() + 2;
	font.draw(offsetx, offsety, "[shift] + [s] - Save As");
	offsety += font.getHeight() + 2;
	font.draw(offsetx, offsety, "[f] - Find Map");
	offsety += font.getHeight() + 2;
	font.draw(offsetx, offsety, "[shift] + [f] - New Search");
	offsety += font.getHeight() + 2;
	font.draw(offsetx, offsety, "[pageup] - Go To Previous Map");
	offsety += font.getHeight() + 2;
	font.draw(offsetx, offsety, "[pagedown] - Go To Next Map");

	offsetx = 305;
	offsety = 55;

	font.draw(offsetx, offsety, "Tile, Warp and Block Modes:");
	offsety += font.getHeight() + 2;
	font.draw(offsetx, offsety, "[Left Mouse Button] - Place Item");
	offsety += font.getHeight() + 2;
	font.draw(offsetx, offsety, "[Right Mouse Button] - Remove Item");
	offsety += font.getHeight() + 20;

	font.draw(offsetx, offsety, "Move Mode:");
	offsety += font.getHeight() + 2;
	font.draw(offsetx, offsety, "[Right Mouse Button] - Select Area");
	offsety += font.getHeight() + 2;
	font.draw(offsetx, offsety, "[Left Mouse Button] - Unselect Area");
	offsety += font.getHeight() + 2;
	font.draw(offsetx, offsety, "Select And Drag - Move Selections");
	offsety += font.getHeight() + 2;
	font.draw(offsetx, offsety, "Hold [shift] - Multiple Selections");
	offsety += font.getHeight() + 2;
	font.draw(offsetx, offsety, "Hold [ctrl] - Freehand Selections");
	offsety += font.getHeight() + 2;
	font.draw(offsetx, offsety, "[delete] - Delete Selection");
	offsety += font.getHeight() + 2;
	font.draw(offsetx, offsety, "[c] - Copy Selection");
	offsety += font.getHeight() + 20;

	font.draw(offsetx, offsety, "Platforms:");
	offsety += font.getHeight() + 2;
	font.draw(offsetx, offsety, "[p] - Path");
	offsety += font.getHeight() + 2;
	font.draw(offsetx, offsety, "[+/-] - Change Speed");
	offsety += font.getHeight() + 2;
	font.draw(offsetx, offsety, "[delete] - Delete");
	offsety += font.getHeight() + 20;

	font.draw(offsetx, offsety, "Miscellaneous:");
	offsety += font.getHeight() + 2;
	font.draw(offsetx, offsety, "[b] - Background Thumbnails");
	offsety += font.getHeight() + 2;
	font.draw(offsetx, offsety, "[g] - Change Backgrounds");
	offsety += font.getHeight() + 2;
	font.draw(offsetx, offsety, "[r] - Change Music Category");
	offsety += font.getHeight() + 2;
	font.draw(offsetx, offsety, "[e] - Change Floating Eyecandy");
	offsety += font.getHeight() + 2;
	font.draw(offsetx, offsety, "[ctrl] + [delete] - Clear All");
	offsety += font.getHeight() + 2;


	SDL_Flip(screen);

    while (true)
	{
		int framestart = SDL_GetTicks();

		//handle messages
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_QUIT:
					return 0;
				break;

				case SDL_KEYDOWN:
					return 0;
				break;

				default:
					break;
			}
		}
		
		int delay = WAITTIME - (SDL_GetTicks() - framestart);
		if(delay < 0)
			delay = 0;
		else if(delay > WAITTIME)
			delay = WAITTIME;
		
		SDL_Delay(delay);
	}

	return 0;
}

int save_as()
{	
	char fileName[FILEBUFSIZE] = "";
	char mapLocation[FILEBUFSIZE] = "maps/";

	if(dialog("Save As", "Enter name:", fileName, 64))
	{
		save_map(convertPath(strcat(strcat(mapLocation, fileName),".map")));
		maplist.add(strcat(fileName, ".map"));
		maplist.find(fileName);
		loadcurrentmap();
	}

	return 0;
}

bool dialog(char * title, char * instructions, char * input, int inputsize)
{	
	unsigned int currentChar = 0;

	drawmap(false, TILESIZE);
	menu_shade.draw(0, 0);
	spr_dialog.draw(224, 176);
	fontbig.drawCentered(320, 200, title);
	font.draw(240, 235, instructions);
	font.drawRightJustified(640, 0, maplist.currentFilename());
	SDL_Flip(screen);

    while (true)
	{
		int framestart = SDL_GetTicks();

		//handle messages
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_QUIT:
					return false;
				break;

				case SDL_KEYDOWN:

					if(event.key.keysym.sym == SDLK_KP_ENTER || event.key.keysym.sym == SDLK_RETURN)
					{
						return true;
					}
					else if(event.key.keysym.sym == SDLK_ESCAPE)
					{
						return false;
					}
					else if(event.key.keysym.sym == SDLK_BACKSPACE)
					{
						if(currentChar > 0)
						{
							input[currentChar-1] = '\0';
							
							drawmap(false, TILESIZE);
							menu_shade.draw(0, 0);
							spr_dialog.draw(224, 176);
							fontbig.drawCentered(320, 200, title);
							font.draw(240, 235, instructions);
							font.draw(240, 255, input);
							font.drawRightJustified(640, 0, maplist.currentFilename());
							SDL_Flip(screen);
							
							currentChar--;
						}
					}
					else
					{
						
						/* I realize the if statement below is long and can be substituted with 
						the function isalnum(event.key.keysym.sym) but I did it this way because
						isalnum acts funny (ie wrong) when the number pad is pressed. */
						if((isdigit(event.key.keysym.sym) || event.key.keysym.sym == 45 || event.key.keysym.sym == 32 || event.key.keysym.sym == 61 || (event.key.keysym.sym >= 95 && event.key.keysym.sym <= 122)) && currentChar < (unsigned)inputsize - 1)
						{
							//insert character into fileName and onScreenText and increment current char
							Uint8 key = event.key.keysym.sym;

							Uint8 * keystate = SDL_GetKeyState(NULL);
							if (keystate[SDLK_LSHIFT] || keystate[SDLK_RSHIFT]) 
							{
								if(event.key.keysym.sym == 45)
									key = 95;
								else if(event.key.keysym.sym >= 95 && event.key.keysym.sym <= 122)
									key-= 32;  //Capitalize
								else if(event.key.keysym.sym == 48)
									key = 41;
								else if(event.key.keysym.sym == 49)
									key = 33;
								else if(event.key.keysym.sym == 50)
									key = 64;
								else if(event.key.keysym.sym == 51)
									key = 35;
								else if(event.key.keysym.sym == 52)
									key = 36;
								else if(event.key.keysym.sym == 53)
									key = 37;
								else if(event.key.keysym.sym == 54)
									key = 94;
								else if(event.key.keysym.sym == 55)
									key = 38;
								else if(event.key.keysym.sym == 57)
									key = 40;
								else if(event.key.keysym.sym == 61)
									key = 43;
							}

							input[currentChar] = key;
							currentChar++;
							input[currentChar] = '\0';

							drawmap(false, TILESIZE);
							menu_shade.draw(0, 0);
							spr_dialog.draw(224, 176);
							fontbig.drawCentered(320, 200, title);
							font.draw(240, 235, instructions);
							font.draw(240, 255, input);
							font.drawRightJustified(640, 0, maplist.currentFilename());
							SDL_Flip(screen);
						}
					}	
				break;

				default:
					break;
			}
		}
		
		int delay = WAITTIME - (SDL_GetTicks() - framestart);
		if(delay < 0)
			delay = 0;
		else if(delay > WAITTIME)
			delay = WAITTIME;
		
		SDL_Delay(delay);
	}

	return false;
}

int find()
{
	char fileName[FILEBUFSIZE] = "";
	//char mapLocation[FILEBUFSIZE] = "maps/";

	if(dialog("Find Map", "Enter name:", fileName, 64))
	{
		strcpy(findstring, fileName);

		if(maplist.find(findstring))
		{
			loadcurrentmap();
		}
	}

	return 0;
}

int clearMap()
{ 
	g_map.clearMap();
	g_iNumPlatforms = 0;

	printf("Map Cleared\n");
	return 0;
}

#ifdef _DEBUG
void convertAll()
{
	for(int k = 0; k < maplist.GetCount(); k++)
	{
		//g_map.convertMap();
		g_map.saveMap(maplist.currentFilename());
		maplist.next(false);
		g_map.loadMap(maplist.currentFilename(), read_type_full);
	}
}
#endif


void loadcurrentmap()
{
	g_map.loadMap(maplist.currentFilename(), read_type_full);

	char filename[128];
	sprintf(filename, "gfx/packs/Classic/backgrounds/%s", g_map.szBackgroundFile);
	std::string path = convertPath(filename);
	backgroundlist.SetCurrentName(filename);
	
	if(!File_Exists(path))
	{
		path = convertPath("gfx/packs/Classic/backgrounds/Land_Classic.png");
		backgroundlist.SetCurrentName("gfx/packs/Classic/backgrounds/Land_Classic.png");
	}
	
	spr_background.init(path);

	g_iNumPlatforms = g_map.iNumPlatforms;

	for(short iPlatform = 0; iPlatform < g_iNumPlatforms; iPlatform++)
	{
		for(short iCol = 0; iCol < MAPWIDTH; iCol++)
		{
			for(short iRow = 0; iRow < MAPHEIGHT; iRow++)
			{
				if(iCol < g_map.platforms[iPlatform]->iTileWidth && iRow < g_map.platforms[iPlatform]->iTileHeight)
				{
					g_Platforms[iPlatform].tiles[iCol][iRow] = g_map.platforms[iPlatform]->iTileData[iCol][iRow];
				}
				else
				{
					g_Platforms[iPlatform].tiles[iCol][iRow] = TILESETSIZE;
				}
			}
		}

		g_Platforms[iPlatform].iVelocity = (int)(g_map.platforms[iPlatform]->pPath->fVelocity * 4.0f);
		g_Platforms[iPlatform].iStartX = (int)(g_map.platforms[iPlatform]->pPath->fStartX - g_map.platforms[iPlatform]->iHalfWidth + 1) / TILESIZE;
		g_Platforms[iPlatform].iStartY = (int)(g_map.platforms[iPlatform]->pPath->fStartY - g_map.platforms[iPlatform]->iHalfHeight + 1) / TILESIZE;
		g_Platforms[iPlatform].iEndX = (int)(g_map.platforms[iPlatform]->pPath->fEndX - g_map.platforms[iPlatform]->iHalfWidth + 1) / TILESIZE;
		g_Platforms[iPlatform].iEndY = (int)(g_map.platforms[iPlatform]->pPath->fEndY - g_map.platforms[iPlatform]->iHalfHeight + 1) / TILESIZE;
	}
}

void SetPlatformToDefaults(short iPlatform)
{
	for(short iCol = 0; iCol < MAPWIDTH; iCol++)
	{
		for(short iRow = 0; iRow < MAPHEIGHT; iRow++)
		{
			g_Platforms[iPlatform].tiles[iCol][iRow] = TILESETSIZE;
		}
	}

	g_Platforms[iPlatform].iVelocity = 4;
	g_Platforms[iPlatform].iStartX = 0;
	g_Platforms[iPlatform].iStartY = 0;
	g_Platforms[iPlatform].iEndX = 1;
	g_Platforms[iPlatform].iEndY = 0;
}

int savecurrentmap()
{
	save_map(maplist.currentFilename());
	return 0;
}

void insert_platforms_into_map()
{
	//First take the created platforms and move them into the actual map
	g_map.clearPlatforms();

	g_map.iNumPlatforms = g_iNumPlatforms;
	g_map.platforms = new MovingPlatform*[g_iNumPlatforms];

	for(short iPlatform = 0; iPlatform < g_iNumPlatforms; iPlatform++)
	{
		short iTop, iLeft, iWidth, iHeight;
		CalculatePlatformDims(iPlatform, &iLeft, &iTop, &iWidth, &iHeight);

		short ** tiles = new short*[iWidth];

		for(short iCol = 0; iCol < iWidth; iCol++)
		{
			tiles[iCol] = new short[iHeight];

			for(short iRow = 0; iRow < iHeight; iRow++)
			{
				tiles[iCol][iRow] = g_Platforms[iPlatform].tiles[iCol + iLeft][iRow + iTop];
			}
		}
	
		float fVelocity = (float)g_Platforms[iPlatform].iVelocity * 0.26f;

		float fStartX = (float)(g_Platforms[iPlatform].iStartX * TILESIZE) + (float)(iWidth * TILESIZE) / 2.0f;
		float fStartY = (float)(g_Platforms[iPlatform].iStartY * TILESIZE) + (float)(iHeight * TILESIZE) / 2.0f;
		float fEndX = (float)(g_Platforms[iPlatform].iEndX * TILESIZE) + (float)(iWidth * TILESIZE) / 2.0f;
		float fEndY = (float)(g_Platforms[iPlatform].iEndY * TILESIZE) + (float)(iHeight * TILESIZE) / 2.0f;
				
		MovingPlatformPath * path = new MovingPlatformPath(fVelocity, fStartX, fStartY, fEndX, fEndY, false);

		g_map.platforms[iPlatform] = new MovingPlatform(tiles, iWidth, iHeight, path, true, 0, false);
	}
}

void save_map(const std::string &file)
{
	insert_platforms_into_map();

	//Then save the rest of the map
	g_map.saveMap(file);
}

void CalculatePlatformDims(short iPlatform, short * ix, short * iy, short * iw, short * ih)
{
	short iTop = MAPHEIGHT, iRight = -1, iBottom = -1, iLeft = MAPWIDTH;
	//Calculate the height and width of the platform
	for(short iCol = 0; iCol < MAPWIDTH; iCol++)
	{
		for(short iRow = 0; iRow < MAPHEIGHT; iRow++)
		{
			if(g_Platforms[iPlatform].tiles[iCol][iRow] != TILESETSIZE)
			{
				if(iTop > iRow)
					iTop = iRow;

				if(iLeft > iCol)
					iLeft = iCol;

				if(iBottom < iRow)
					iBottom = iRow;

				if(iRight < iCol)
					iRight = iCol;
			}
		}
	}

	short iWidth = iRight - iLeft + 1;
	short iHeight = iBottom - iTop + 1;

	if(iRight == -1 || iBottom == -1)
	{
		iWidth = 1;
		iHeight = 1;
		iLeft = 0;
		iTop = 0;
	}

	*ix = iLeft;
	*iy = iTop;

	*iw = iWidth;
	*ih = iHeight;

	//Also adjust path if it is outside the allowed area
	short iTempStartX = g_Platforms[iPlatform].iStartX;
	short iTempStartY = g_Platforms[iPlatform].iStartY;
	short iTempEndX = g_Platforms[iPlatform].iEndX;
	short iTempEndY = g_Platforms[iPlatform].iEndY;

	if(iTempStartX > MAPWIDTH - iWidth)
		iTempStartX = MAPWIDTH - iWidth;

	if(iTempStartY > MAPHEIGHT - iHeight)
		iTempStartY = MAPHEIGHT - iHeight;

	if(iTempEndX > MAPWIDTH - iWidth)
		iTempEndX = MAPWIDTH - iWidth;

	if(iTempEndY > MAPHEIGHT - iHeight)
		iTempEndY = MAPHEIGHT - iHeight;

	if(iTempStartX == iTempEndX && iTempStartY == iTempEndY)
	{
		iTempStartX = 0;
		iTempStartY = 0;

		if(MAPWIDTH - iWidth > MAPHEIGHT - iHeight)
		{
			iTempEndX = MAPWIDTH - iWidth;
			iTempEndY = 0;
		}
		else
		{
			iTempEndX = 0;
			iTempEndY = MAPHEIGHT - iHeight;
		}
	}

	g_Platforms[iPlatform].iStartX = iTempStartX;
	g_Platforms[iPlatform].iStartY = iTempStartY;
	g_Platforms[iPlatform].iEndX = iTempEndX;
	g_Platforms[iPlatform].iEndY = iTempEndY;
}

int findcurrentstring()
{
	if(findstring[0] != '\0')
	{
		if(maplist.find(findstring))
		{
			loadcurrentmap();
		}
	}

	return 0;
}

int newmap()
{
	char fileName[FILEBUFSIZE] = "";
	char mapLocation[FILEBUFSIZE] = "maps/";

	if(dialog("New Map", "Enter name:", fileName, 64))
	{
		g_map.clearMap();
		g_map.clearPlatforms();
		g_map.eyecandyID = 0;
		g_map.iNumMapItems = 0;
		g_map.saveMap(convertPath(strcat(strcat(mapLocation, fileName),".map")));
		maplist.add(strcat(fileName, ".map"));
		maplist.find(fileName);
		loadcurrentmap();
	}

	return 0;
}

void resetselectedtiles()
{
	for(int k = 0; k < MAPHEIGHT; k++)
	{
		for(int j = 0; j < MAPWIDTH; j++)
		{
			selectedtiles[j][k] = false;
		}
	}
}

void copymoveselection()
{
	for(int k = 0; k < MAPHEIGHT; k++)
	{
		for(int j = 0; j < MAPWIDTH; j++)
		{
			moveselectedtiles[j][k] = selectedtiles[j][k];
		}
	}
	
	copiedlayer = selected_layer;
}

void pastemoveselection(int movex, int movey)
{
	for(int k = 0; k < MAPHEIGHT; k++)
	{
		for(int j = 0; j < MAPWIDTH; j++)
		{
			if(moveselectedtiles[j][k])
			{
				if(j + movex >= 0 && j + movex < MAPWIDTH && 
					k + movey >= 0 && k + movey < MAPHEIGHT)
				{
					selectedtiles[j + movex][k + movey] = moveselectedtiles[j][k];
				}
			}
		}
	}
}

bool copyselectedtiles()
{
	//Copy the selected tiles and remove tiles from map
	bool ret = false;
	for(int k = 0; k < MAPHEIGHT; k++)
	{
		for(int j = 0; j < MAPWIDTH; j++)
		{
			if(selectedtiles[j][k])
			{
				ret = true;
				copiedtiles[j][k].tile[0] = g_map.mapdata[j][k][0];
				copiedtiles[j][k].tile[1] = g_map.mapdata[j][k][1];
				copiedtiles[j][k].tile[2] = g_map.mapdata[j][k][2];
				copiedtiles[j][k].tile[3] = g_map.mapdata[j][k][3];

				copiedtiles[j][k].block = g_map.objectdata[j][k];

				copiedtiles[j][k].warp.connection = g_map.warpdata[j][k].connection;
				copiedtiles[j][k].warp.direction = g_map.warpdata[j][k].direction;
				copiedtiles[j][k].warp.id = g_map.warpdata[j][k].id;

				copiedtiles[j][k].tiletype = g_map.mapdatatop[j][k];
				
				for(short iType = 0; iType < NUMSPAWNAREATYPES; iType++)
					copiedtiles[j][k].nospawn[iType] = g_map.nospawn[iType][j][k];
			}
		}
	}

	copiedlayer = selected_layer;

	return ret;
}

void clearselectedmaptiles()
{
	for(int k = 0; k < MAPHEIGHT; k++)
	{
		for(int j = 0; j < MAPWIDTH; j++)
		{
			if(selectedtiles[j][k])
			{
				if(view_only_layer)
				{
					g_map.mapdata[j][k][selected_layer] = TILESETSIZE;
				}
				else
				{
					g_map.mapdata[j][k][0] = TILESETSIZE;
					g_map.mapdata[j][k][1] = TILESETSIZE;
					g_map.mapdata[j][k][2] = TILESETSIZE;
					g_map.mapdata[j][k][3] = TILESETSIZE;
					
					g_map.objectdata[j][k] = BLOCKSETSIZE;

					g_map.warpdata[j][k].connection = -1;
					g_map.warpdata[j][k].direction = -1;
					g_map.warpdata[j][k].id = -1;

					for(short iType = 0; iType < NUMSPAWNAREATYPES; iType++)
						g_map.nospawn[iType][j][k] = false;
				}

				UpdateTileType(j, k);
			}
		}
	}
}

void pasteselectedtiles(int movex, int movey)
{
	//Paste the tiles into their new location
	//Removing tiles that hang over the edges of the map
	for(int k = 0; k < MAPHEIGHT; k++)
	{
		for(int j = 0; j < MAPWIDTH; j++)
		{
			if(selectedtiles[j][k])
			{
				if(j + movex >= 0 && j + movex < MAPWIDTH && 
					k + movey >= 0 && k + movey < MAPHEIGHT)
				{
					if(view_only_layer)
					{
						g_map.mapdata[j + movex][k + movey][selected_layer] = copiedtiles[j][k].tile[copiedlayer];
					}
					else
					{
						g_map.mapdata[j + movex][k + movey][0] = copiedtiles[j][k].tile[0];
						g_map.mapdata[j + movex][k + movey][1] = copiedtiles[j][k].tile[1];
						g_map.mapdata[j + movex][k + movey][2] = copiedtiles[j][k].tile[2];
						g_map.mapdata[j + movex][k + movey][3] = copiedtiles[j][k].tile[3];

						g_map.objectdata[j + movex][k + movey] = copiedtiles[j][k].block;

						g_map.warpdata[j + movex][k + movey].connection = copiedtiles[j][k].warp.connection;
						g_map.warpdata[j + movex][k + movey].direction = copiedtiles[j][k].warp.direction;
						g_map.warpdata[j + movex][k + movey].id = copiedtiles[j][k].warp.id;

						g_map.mapdatatop[j + movex][k + movey] = copiedtiles[j][k].tiletype;
						
						for(short iType = 0; iType < NUMSPAWNAREATYPES; iType++)
							g_map.nospawn[iType][j + movex][k + movey] = copiedtiles[j][k].nospawn[iType];
					}
				}
			}
		}
	}
}

void getcenterselection(int * x, int * y)
{
	int left = -1;
	int top = -1;
	int right = -1;
	int bottom = -1;

	for(int k = 0; k < MAPHEIGHT; k++)
	{
		for(int j = 0; j < MAPWIDTH; j++)
		{
			if(selectedtiles[j][k])
			{
				if(left == -1)
					left = j;

				if(top == -1)
					top = k;

				right = j;
				bottom = k;
			}
		}
	}

	if(right == -1)
	{
		*x = 0;
		*y = 0;
		return;
	}

	*x = ((right - left) >> 1) + left;
	*y = ((bottom - top) >> 1) + top;
}

//take screenshots in full and thumbnail sizes
void takescreenshot()
{
	short iTileSizes[3] = {32, 16, 8};
	SDL_Surface * old_screen = screen;

	for(short iScreenshotSize = 0; iScreenshotSize < 3; iScreenshotSize++)
	{
		short iTileSize = iTileSizes[iScreenshotSize];

		SDL_Surface * screenshot = SDL_CreateRGBSurface(old_screen->flags, iTileSize * 20, iTileSize * 15, old_screen->format->BitsPerPixel, 0, 0, 0, 0);
		blitdest = screenshot;
		screen = screenshot;
		drawmap(true, iTileSize);

		//Draw platforms to screenshot
		SDL_Rect rSrc = {0, 0, iTileSize, iTileSize};
		SDL_Rect rDst = {0, 0, iTileSize, iTileSize};

		for(short iPlatform = 0; iPlatform < g_iNumPlatforms; iPlatform++)
		{
			for(short iPlatformX = 0; iPlatformX < g_map.platforms[iPlatform]->iTileWidth; iPlatformX++)
			{
				for(short iPlatformY = 0; iPlatformY < g_map.platforms[iPlatform]->iTileHeight; iPlatformY++)
				{
					short iTile = g_Platforms[iPlatform].tiles[iPlatformX][iPlatformY];

					if(iTile != TILESETSIZE)
					{
						rSrc.x = iTile % TILESETWIDTH * iTileSize;
						rSrc.y = iTile / TILESETWIDTH * iTileSize;

						rDst.x = (g_Platforms[iPlatform].iStartX + iPlatformX) * iTileSize;
						rDst.y = (g_Platforms[iPlatform].iStartY + iPlatformY) * iTileSize;

						SDL_BlitSurface(g_map.tilesetsurface[iScreenshotSize], &rSrc, blitdest, &rDst);
					}
				}
			}
		}
	
		//And add platform paths
		for(short iPlatform = 0; iPlatform < g_iNumPlatforms; iPlatform++)
		{
			if(g_Platforms[iPlatform].iStartX != g_Platforms[iPlatform].iEndX)
			{
				short iCenterOffsetY = (g_map.platforms[iPlatform]->iHeight >> 1) - 16;
				iCenterOffsetY >>= iScreenshotSize; //Resize for preview and thumbnails

				bool fMoveToRight = g_Platforms[iPlatform].iStartX < g_Platforms[iPlatform].iEndX;

				short iSpotLeft = (fMoveToRight ? g_Platforms[iPlatform].iStartX : g_Platforms[iPlatform].iEndX) + 1;
				short iSpotRight = (fMoveToRight ? g_Platforms[iPlatform].iEndX : g_Platforms[iPlatform].iStartX) - 2 + g_map.platforms[iPlatform]->iTileWidth;

				for(short iSpot = iSpotLeft; iSpot <= iSpotRight; iSpot++)
					spr_platformarrows[iScreenshotSize].draw(iSpot * iTileSize, g_Platforms[iPlatform].iStartY * iTileSize + iCenterOffsetY, iTileSize * 5, 0, iTileSize, iTileSize);

				spr_platformarrows[iScreenshotSize].draw((iSpotLeft - 1) * iTileSize, g_Platforms[iPlatform].iStartY * iTileSize + iCenterOffsetY, iTileSize * 2, 0, iTileSize, iTileSize);
				spr_platformarrows[iScreenshotSize].draw((iSpotRight + 1) * iTileSize, g_Platforms[iPlatform].iStartY * iTileSize + iCenterOffsetY, iTileSize * 3, 0, iTileSize, iTileSize);
			}
			else
			{
				short iCenterOffsetX = (g_map.platforms[iPlatform]->iWidth >> 1) - 16;
				iCenterOffsetX >>= iScreenshotSize; //Resize for preview and thumbnails

				bool fMoveUp = g_Platforms[iPlatform].iStartY < g_Platforms[iPlatform].iEndY;

				short iSpotTop = (fMoveUp ? g_Platforms[iPlatform].iStartY : g_Platforms[iPlatform].iEndY) + 1;
				short iSpotBottom = (fMoveUp ? g_Platforms[iPlatform].iEndY : g_Platforms[iPlatform].iStartY) - 2 + g_map.platforms[iPlatform]->iTileHeight;

				for(short iSpot = iSpotTop; iSpot <= iSpotBottom; iSpot++)
					spr_platformarrows[iScreenshotSize].draw(g_Platforms[iPlatform].iStartX * iTileSize + iCenterOffsetX, iSpot * iTileSize, iTileSize * 4, 0, iTileSize, iTileSize);

				spr_platformarrows[iScreenshotSize].draw(g_Platforms[iPlatform].iStartX * iTileSize + iCenterOffsetX, (iSpotTop - 1) * iTileSize, 0, 0, iTileSize, iTileSize);
				spr_platformarrows[iScreenshotSize].draw(g_Platforms[iPlatform].iStartX * iTileSize + iCenterOffsetX, (iSpotBottom + 1) * iTileSize, iTileSize, 0, iTileSize, iTileSize);
			}
		}

		//Save the screenshot with the same name as the map file
		char szSaveFile[256];
		strcpy(szSaveFile, "maps/screenshots/");
		char * pszSaveFile = szSaveFile + strlen(szSaveFile);
		GetNameFromFileName(pszSaveFile, maplist.currentFilename());
		
		if(iTileSize == PREVIEWTILESIZE)
			strcat(szSaveFile, "_preview");
		else if(iTileSize == THUMBTILESIZE)
			strcat(szSaveFile, "_thumb");

#ifdef PNG_SAVE_FORMAT
		strcat(szSaveFile, ".png");
		IMG_SavePNG(screenshot, szSaveFile);
#else
		strcat(szSaveFile, ".bmp");
		SDL_SaveBMP(screenshot, szSaveFile);
#endif

		SDL_FreeSurface(screenshot);
	}

	screen = old_screen;
	blitdest = screen;
}

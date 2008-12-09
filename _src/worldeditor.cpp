/*----------------------------------------------------------+
| Super Mario War World Editor								|
|															|
|															|
| this sourcecode is released under the GPLv2.				|
|															|
| start:		3.8.2007									|
| last changes:	3.8.2007									|
|															|
|	© 2003-2007 Florian Hufsky <florian.hufsky@gmail.com>	|
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

#define MAPTITLESTRING "SMW 1.8 World Editor"

enum {EDITOR_EDIT, EDITOR_WATER, EDITOR_BACKGROUND, EDITOR_FOREGROUND, EDITOR_PATHSPRITE, EDITOR_VEHICLES, EDITOR_QUIT, SAVE_AS, FIND, CLEAR_WORLD, NEW_WORLD, RESIZE_WORLD, SAVE, EDITOR_WARP, DISPLAY_HELP, EDITOR_PATH, EDITOR_TYPE, EDITOR_BOUNDARY, EDITOR_START_ITEMS, EDITOR_STAGE};

const char * szEditModes[10] = {"Background Mode", "Foreground Mode", "Path Sprite Mode", "Stage Mode", "Path Mode", "Vehicle Mode", "Warp Mode", "Start/Door Mode", "Boundary Mode", "Stage Mode"};

SDL_Surface		*screen;
SDL_Surface		*blitdest;
SDL_Surface		*sMapSurface;

SDL_Rect		rectSrcSurface = {0, 0, 768, 608};
SDL_Rect		rectDstSurface = {0, 0, 640, 480};bool			fNeedBlackBackground = false;
short			iWorldWidth, iWorldHeight;

SDL_Event		event;

gfxFont			menu_font_small;
gfxFont			menu_font_large;

gfxSprite		spr_tilesetworld;
gfxSprite		spr_selectedtile;
gfxSprite		spr_worldstarttile;
gfxSprite		spr_dialog;
gfxSprite		menu_shade;

gfxSprite		spr_warps[3];
gfxSprite		spr_path;

gfxSprite		spr_worldbackground[3];
gfxSprite		spr_worldforeground[3];
gfxSprite		spr_worldforegroundspecial[3];
gfxSprite		spr_worldpaths[3];
gfxSprite		spr_worldvehicle[3];

gfxSprite		spr_worlditems;
gfxSprite		spr_worldpopup;

//Mode Options Menu Gfx
gfxSprite		menu_egg;
gfxSprite		menu_plain_field;
gfxSprite		menu_slider_bar;
gfxSprite		menu_stomp;
gfxSprite		menu_survival;
gfxSprite		spr_phanto;
gfxSprite		spr_selectfield;
gfxSprite		menu_verticalarrows;
gfxSprite		spr_storedpoweruplarge;
gfxSprite		menu_mode_small;
gfxSprite		menu_mode_large;

int				set_tile = 0;
bool			fAutoPaint = true;

int				edit_mode = 0;

int				draw_offset_col = 0;  //col and row offset for drawing map to surface
int				draw_offset_row = 0;
int				draw_offset_x = 0;  //x and y offset for drawing maps smaller than screensize
int				draw_offset_y = 0;

int				state;
bool			selectedtiles[MAPWIDTH][MAPHEIGHT];
bool			moveselectedtiles[MAPWIDTH][MAPHEIGHT];
WorldMapTile	copiedtiles[MAPWIDTH][MAPHEIGHT];

//// Global stuff that the map editor doesn't need, but has references to
GraphicsList menugraphicspacklist;
GraphicsList gamegraphicspacklist;
FiltersList filterslist;
gfxSprite		spr_warplock;
short			x_shake = 0;
short			y_shake = 0;
gv				game_values;
void CPlayer::flipsidesifneeded() {}
short CPlayer::KillPlayerMapHazard(bool fForce, killstyle style, bool fKillCarriedItem) {return 0;}
void IO_MovingObject::flipsidesifneeded() {}
void IO_MovingObject::KillObjectMapHazard(short playerID) {}
float CapFallingVelocity(float f) {return 0.0f;}
void removeifprojectile(IO_MovingObject * object, bool playsound, bool forcedead) {}
bool LoadMenuSkin(short playerID, short skinID, short colorID, bool fLoadBothDirections){return false;}

gfxSprite		spr_platformstarttile;
gfxSprite		spr_platformendtile;
gfxSprite		spr_platformpath;

gfxSprite		spr_thumbnail_warps[2];
gfxSprite		spr_thumbnail_mapitems[2];
gfxSprite		spr_awardsouls, spr_fireballexplosion;
gfxSprite		spr_tileanimation[3];
gfxSprite		spr_blocks[3];
gfxSprite		spr_unknowntile[3];
gfxSprite		spr_backmap[2];
gfxSprite		spr_background;
sfxSound		sfx_boomerang;
SkinList		skinlist;
gfxSprite		**spr_player[4];
CGameMode		*gamemodes[GAMEMODE_LAST];
bool			fResumeMusic;
MapList			maplist;
CMap			g_map;
CTilesetManager g_tilesetmanager;

gfxSprite		spr_hazard_fireball[3];
gfxSprite		spr_hazard_rotodisc[3];
gfxSprite		spr_hazard_bulletbill[3];
gfxSprite		spr_hazard_flame[3];
gfxSprite		spr_hazard_pirhanaplant[3];

void DECLSPEC soundfinished(int channel){}
void DECLSPEC musicfinished(){}

sfxSound * g_PlayingSoundChannels[NUM_SOUND_CHANNELS];
CEyecandyContainer eyecandy[3];
gfxSprite		spr_frontmap[2];

CPlayer			*list_players[4];
short			list_players_cnt = 0;
bool			g_fLoadMessages = true;
short			g_iCurrentDrawIndex = 0;
gfxSprite		menu_dialog;

std::vector<MapMusicOverride*> mapmusicoverrides;
std::vector<WorldMusicOverride*> worldmusicoverrides;

short LookupTeamID(short id) {return 0;}
gfxSprite		spr_scoretext;
///////

int save_as();
int find();
int clear_world();
bool dialog(const char * title, const char * instructions, char * input, int inputsize);
int display_help();

void drawmap(bool fScreenshot, short iBlockSize);

bool UpdateForeground(short iCol, short iRow);
void UpdatePath(short iCol, short iRow);
void AutoSetPath(short iCol, short iRow);
bool UpdateCoastline(short iCol, short iRow);
bool AutoSetTile(short iCol, short iRow);
void UpdatePathSprite(short iCol, short iRow);
void AutoSetPathSprite(short iCol, short iRow);
short AdjustForeground(short iSprite, short iCol, short iRow);

void GetForegroundTileValues(short iCol, short iRow, short iOldTiles[9]);
bool ForegroundTileValuesChanged(short iCol, short iRow, short iOldTiles[9]);

void ReadVehiclesIntoEditor();
void WriteVehiclesIntoWorld();
void AddVehicleToTile(short iCol, short iRow, short iType);
void RemoveVehicleFromTile(short iCol, short iRow);

void ReadWarpsIntoEditor();
void WriteWarpsIntoWorld();
void AddWarpToTile(short iCol, short iRow, short iType);
void RemoveWarpFromTile(short iCol, short iRow);

WorldMap g_worldmap;
WorldList worldlist;
void loadcurrentworld();
int savecurrentworld();
int findcurrentstring();
int new_world();
int resize_world();

int editor_edit();
int editor_warp();
int editor_boundary();
int editor_water();
int	editor_background();
int editor_foreground();
int editor_vehicles();
int	editor_path();
int editor_pathsprite();
int editor_type();
int editor_stage();
int editor_start_items();

void updateworldsurface();
void takescreenshot();

bool ignoreclick = false;

char findstring[FILEBUFSIZE] = "";

extern const char * g_szWorldMusicCategoryNames[MAXWORLDMUSICCATEGORY];
short g_musiccategorydisplaytimer = 0;

short g_messagedisplaytimer = 0;
std::string g_szMessageTitle = "";
std::string g_szMessageLine[3];
void DrawMessage();

//Vehicle stuff
std::vector<WorldVehicle*> vehiclelist;

//Warp stuff
std::vector<WorldWarp*> warplist;

bool g_fFullScreen = false;

//Stage Mode Menu
UI_Menu mStageSettingsMenu;
MI_ImageSelectField * miModeField = NULL;

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

	FILE * fp = OpenFile("worldeditor.bin", "rt");

	int saved_col = 0, saved_row = 0;
	if(fp)
	{
		fread(&saved_col, sizeof(int), 1, fp);
		fread(&saved_row, sizeof(int), 1, fp);
		fread(&g_fFullScreen, sizeof(bool), 1, fp);
		fgets(findstring, FILEBUFSIZE, fp);
		fclose(fp);
	}

	gfx_init(640,480, g_fFullScreen);
	blitdest = screen;
	g_tilesetmanager.Init(convertPath("gfx/Classic/tilesets").c_str());

	SDL_WM_SetCaption(MAPTITLESTRING, "worldeditor.ico");

	printf("\n---------------- loading graphics ----------------\n");
		
	spr_warps[0].init(convertPath("gfx/leveleditor/leveleditor_warp.png"), 255, 0, 255);
	spr_warps[1].init(convertPath("gfx/leveleditor/leveleditor_warp_preview.png"), 255, 0, 255);
	spr_warps[2].init(convertPath("gfx/leveleditor/leveleditor_warp_thumbnail.png"), 255, 0, 255);

	spr_path.init(convertPath("gfx/leveleditor/leveleditor_world_path.png"), 255, 0, 255);

	spr_selectedtile.init(convertPath("gfx/leveleditor/leveleditor_selectedtile.png"), 0, 0, 0, 128, true);
	spr_worldstarttile.init(convertPath("gfx/leveleditor/leveleditor_platformstarttile.png"), 0, 0, 0, 64, true);

	spr_dialog.init(convertPath("gfx/leveleditor/leveleditor_dialog.png"), 255, 0, 255, 255, true);
	menu_shade.init(convertPath("gfx/leveleditor/leveleditor_shade.png"), 255, 0, 255, 128, true);

	menu_font_small.init(convertPath("gfx/packs/Classic/fonts/font_small.png"));
	menu_font_large.init(convertPath("gfx/packs/Classic/fonts/font_large.png"));

	printf("\n---------------- load world ----------------\n");

	spr_worldbackground[0].init(convertPath("gfx/packs/Classic/world/world_background.png"), 255, 0, 255, false);
	spr_worldbackground[1].init(convertPath("gfx/packs/Classic/world/preview/world_background.png"), 255, 0, 255);
	spr_worldbackground[2].init(convertPath("gfx/packs/Classic/world/thumbnail/world_background.png"), 255, 0, 255);

	spr_worldforeground[0].init(convertPath("gfx/packs/Classic/world/world_foreground.png"), 255, 0, 255, false);
	spr_worldforeground[1].init(convertPath("gfx/packs/Classic/world/preview/world_foreground.png"), 255, 0, 255);
	spr_worldforeground[2].init(convertPath("gfx/packs/Classic/world/thumbnail/world_foreground.png"), 255, 0, 255);

	spr_worldforegroundspecial[0].init(convertPath("gfx/packs/Classic/world/world_foreground_special.png"), 255, 0, 255, false);
	spr_worldforegroundspecial[1].init(convertPath("gfx/packs/Classic/world/preview/world_foreground_special.png"), 255, 0, 255);
	spr_worldforegroundspecial[2].init(convertPath("gfx/packs/Classic/world/thumbnail/world_foreground_special.png"), 255, 0, 255);

	spr_worldpaths[0].init(convertPath("gfx/packs/Classic/world/world_paths.png"), 255, 0, 255, false);
	spr_worldpaths[1].init(convertPath("gfx/packs/Classic/world/preview/world_paths.png"), 255, 0, 255);
	spr_worldpaths[2].init(convertPath("gfx/packs/Classic/world/thumbnail/world_paths.png"), 255, 0, 255);

	spr_worldvehicle[0].init(convertPath("gfx/packs/Classic/world/world_vehicles.png"), 255, 0, 255);
	spr_worldvehicle[1].init(convertPath("gfx/packs/Classic/world/preview/world_vehicles.png"), 255, 0, 255);
	spr_worldvehicle[2].init(convertPath("gfx/packs/Classic/world/thumbnail/world_vehicles.png"), 255, 0, 255);

	spr_worlditems.init(convertPath("gfx/packs/Classic/world/world_powerups.png"), 255, 0, 255);
	spr_worldpopup.init(convertPath("gfx/packs/Classic/world/world_item_popup.png"), 255, 0, 255);

	//Mode Options Menu Gfx
	menu_egg.init(convertPath("gfx/packs/Classic/modeobjects/menu_egg.png"), 255, 0, 255);
	menu_stomp.init(convertPath("gfx/packs/Classic/modeobjects/menu_stomp.png"), 255, 0, 255);
	menu_survival.init(convertPath("gfx/packs/Classic/modeobjects/menu_survival.png"), 255, 0, 255);
	spr_phanto.init(convertPath("gfx/packs/Classic/modeobjects/phanto.png"), 255, 0, 255);
	menu_plain_field.init(convertPath("gfx/packs/Classic/menu/menu_plain_field.png"), 255, 0, 255);
	menu_slider_bar.init(convertPath("gfx/packs/Classic/menu/menu_slider_bar.png"), 255, 0, 255);
	spr_selectfield.init(convertPath("gfx/packs/Classic/menu/menu_selectfield.png"), 255, 0, 255);
	menu_verticalarrows.init(convertPath("gfx/packs/Classic/menu/menu_vertical_arrows.png"), 255, 0, 255);
	spr_storedpoweruplarge.init(convertPath("gfx/packs/Classic/powerups/large.png"), 255, 0, 255);

	menu_mode_large.init(convertPath("gfx/packs/Classic/menu/menu_mode_small.png"), 255, 0, 255);
	menu_mode_large.init(convertPath("gfx/packs/Classic/menu/menu_mode_large.png"), 255, 0, 255);

	sMapSurface = SDL_CreateRGBSurface(screen->flags, 768, 608, screen->format->BitsPerPixel, 0, 0, 0, 0);

	worldlist.find(findstring);
	game_values.worldindex = worldlist.GetCurrentIndex();
	loadcurrentworld();
	
	if(saved_row >= 0 && saved_row <= iWorldHeight - 15 && saved_col >= 0 && saved_col <= iWorldWidth - 20)
	{
		draw_offset_row = saved_row;
		draw_offset_col = saved_col;
		updateworldsurface();
	}

	//Setup The Mode Menu
	miModeField = new MI_ImageSelectField(&spr_selectfield, &menu_mode_small, 70, 85, "Mode", 500, 120, 16, 16);

	miModeField->Add("Classic", 0, "", false, false);
	miModeField->Add("Frag", 1, "", false, false);
	miModeField->Add("Time Limit", 2, "", false, false);
	miModeField->Add("Jail", 3, "", false, false);
	miModeField->Add("Coin Collection", 4, "", false, false);
	miModeField->Add("Stomp", 5, "", false, false);
	miModeField->Add("Yoshi's Eggs", 6, "", false, false);
	miModeField->Add("Capture The Flag", 7, "", false, false);
	miModeField->Add("Chicken", 8, "", false, false);
	miModeField->Add("Tag", 9, "", false, false);
	miModeField->Add("Star", 10, "", false, false);
	miModeField->Add("Domination", 11, "", false, false);
	miModeField->Add("King of the Hill", 12, "", false, false);
	miModeField->Add("Race", 13, "", false, false);
	miModeField->Add("Owned", 14, "", false, false);
	miModeField->Add("Frenzy", 15, "", false, false);
	miModeField->Add("Survival", 16, "", false, false);
	miModeField->Add("Greed", 17, "", false, false);
	miModeField->Add("Health", 18, "", false, false);
	miModeField->Add("Card Collection", 19, "", false, false);
	miModeField->Add("Phanto Chase", 20, "", false, false);
	miModeField->Add("Shy Guy Tag", 21, "", false, false);
	
	//miModeField->SetData(game_values.tourstops[0]->iMode, NULL, NULL);
	//miModeField->SetKey(0);
	miModeField->SetItemChangedCode(MENU_CODE_MODE_CHANGED);

	mStageSettingsMenu.AddControl(miModeField, NULL, NULL, NULL, NULL);

	printf("\n---------------- ready, steady, go! ----------------\n");

	printf("entering world editor loop...\n");
	done = false;
	while(!done)
	{
		switch(state)
		{
			case EDITOR_EDIT:
				state = editor_edit();
			break;

			case EDITOR_WATER:
				state = editor_water();
			break;

			case EDITOR_BACKGROUND:
				state = editor_background();
			break;

			case EDITOR_FOREGROUND:
				state = editor_foreground();
			break;

			case EDITOR_VEHICLES:
				state = editor_vehicles();
			break;
			
			case EDITOR_PATH:
				state = editor_path();
			break;

			case EDITOR_PATHSPRITE:
				state = editor_pathsprite();
			break;

			case EDITOR_WARP:
				state = editor_warp();
			break;

			case EDITOR_START_ITEMS:
				state = editor_start_items();
			break;

			case EDITOR_BOUNDARY:
				state = editor_boundary();
			break;

			case EDITOR_TYPE:
				state = editor_type();
			break;

			case EDITOR_STAGE:
				state = editor_stage();
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

			case CLEAR_WORLD:
				state = clear_world();
			break;

			case NEW_WORLD:
				state = new_world();
			break;

			case RESIZE_WORLD:
				state = resize_world();
			break;

			case SAVE:
				state = savecurrentworld();
			break;

			default:
				printf(" PANIC: WEIRD GAMESTATE: %d\n", state);
			break;
		}
	}

	SDL_FreeSurface(sMapSurface);

	printf("\n---------------- save world ----------------\n");

	WriteVehiclesIntoWorld();
	WriteWarpsIntoWorld();
	g_worldmap.Save(convertPath("worlds/ZZworldeditor.txt").c_str());

	fp = OpenFile("worldeditor.bin", "wt");

	if(fp)
	{
		fwrite(&draw_offset_col, sizeof(int), 1, fp);
		fwrite(&draw_offset_row, sizeof(int), 1, fp);
		fwrite(&g_fFullScreen, sizeof(bool), 1, fp);
		fprintf(fp, worldlist.current_name());
		fclose(fp);
	}

	printf("\n---------------- shutdown ----------------\n");
	return 0;
}


int editor_edit()
{
	bool done = false;
	g_musiccategorydisplaytimer = 0;

	short view_repeat_direction = -1;
	short view_repeat_timer = 0;

	bool fExiting = false;
	bool fSelectedYes = false;

	while (!done)
	{
		int framestart = SDL_GetTicks();

		if(fExiting)
		{
			//handle messages
			while(SDL_PollEvent(&event))
			{
				Uint8 * keystate = SDL_GetKeyState(NULL);

				switch(event.type)
				{
					case SDL_KEYDOWN:
					{
						SDLKey key = event.key.keysym.sym;

						if(key == SDLK_LEFT)
						{
							fSelectedYes = true;
						}
						else if(key == SDLK_RIGHT)
						{
							fSelectedYes = false;
						}
						else if(event.key.keysym.sym == SDLK_KP_ENTER || event.key.keysym.sym == SDLK_RETURN)
						{
							if(fSelectedYes)
								return EDITOR_QUIT;

							fExiting = false;
						}
#ifdef _DEBUG
						else if(event.key.keysym.sym == SDLK_ESCAPE)
						{
							return EDITOR_QUIT;
						}
#endif
					}
				}
			}
		}
		else
		{
			//handle messages
			while(SDL_PollEvent(&event))
			{
				Uint8 * keystate = SDL_GetKeyState(NULL);

				switch(event.type)
				{
					case SDL_QUIT:
					{
						done = true;
						break;
					}

					case SDL_KEYDOWN:
					{
						if(event.key.keysym.sym == SDLK_ESCAPE)
						{
							if(g_musiccategorydisplaytimer > 0)
								g_musiccategorydisplaytimer = 0;
							else if(edit_mode != 0)
							{
								edit_mode = 0;
								set_tile = 0;
							}
							else
							{
								fSelectedYes = false;
								fExiting = true;
							}
						}

						if(event.key.keysym.mod & (KMOD_LALT | KMOD_RALT))
						{
							if(event.key.keysym.sym == SDLK_RETURN)
							{
								g_fFullScreen = !g_fFullScreen;
								gfx_setresolution(640, 480, g_fFullScreen);
								blitdest = screen;
							}
						}

						if(event.key.keysym.sym == SDLK_INSERT)
							takescreenshot();

						if(event.key.keysym.sym == SDLK_1)
							return EDITOR_WATER;

						if(event.key.keysym.sym == SDLK_2)
							return EDITOR_BACKGROUND;

						if(event.key.keysym.sym == SDLK_3)
							return EDITOR_FOREGROUND;

						if(event.key.keysym.sym == SDLK_4)
							return EDITOR_PATHSPRITE;

						if(event.key.keysym.sym == SDLK_t)
							return EDITOR_TYPE;

						if(event.key.keysym.sym == SDLK_e)
							return EDITOR_STAGE;

						if(event.key.keysym.sym == SDLK_p)
							return EDITOR_PATH;

						if(event.key.keysym.sym == SDLK_v)
							return EDITOR_VEHICLES;

						if(event.key.keysym.sym == SDLK_w)
							return EDITOR_WARP;

						if(event.key.keysym.sym == SDLK_i)
							return EDITOR_START_ITEMS;

						if(event.key.keysym.sym == SDLK_b)
							return EDITOR_BOUNDARY;

						if(event.key.keysym.sym == SDLK_a)
							fAutoPaint = !fAutoPaint;
						
						if(event.key.keysym.sym == SDLK_r)
						{
							if(g_musiccategorydisplaytimer > 0 && ++g_worldmap.iMusicCategory >= MAXWORLDMUSICCATEGORY)
								g_worldmap.iMusicCategory = 0;
							
							g_musiccategorydisplaytimer = 90;
						}
						
						if(event.key.keysym.sym == SDLK_s)
						{
							if(keystate[SDLK_LSHIFT] || keystate[SDLK_RSHIFT])
								return SAVE_AS;

							return SAVE;
						}

						if(event.key.keysym.sym == SDLK_f)
						{
							if(keystate[SDLK_LSHIFT] || keystate[SDLK_RSHIFT] || findstring[0] == '\0')
								return FIND;

							findcurrentstring();
						}

						if(event.key.keysym.sym == SDLK_DELETE && (keystate[SDLK_LCTRL] || keystate[SDLK_RCTRL]))
						{
							return CLEAR_WORLD;
						}

						if(event.key.keysym.sym == SDLK_n)
							return NEW_WORLD;

						if(event.key.keysym.sym == SDLK_k)
							return RESIZE_WORLD;
											
						if(event.key.keysym.sym == SDLK_h || event.key.keysym.sym == SDLK_F1)
							return DISPLAY_HELP;

						if(event.key.keysym.sym == SDLK_UP)
						{
							if(draw_offset_row > 0)
							{
								draw_offset_row--;
								updateworldsurface();

								view_repeat_direction = 0;
								view_repeat_timer = 30;
							}
						}
						else if(event.key.keysym.sym == SDLK_DOWN)
						{
							if(draw_offset_row < iWorldHeight - 15)
							{
								draw_offset_row++;
								updateworldsurface();

								view_repeat_direction = 1;
								view_repeat_timer = 30;
							}
						}
						else if(event.key.keysym.sym == SDLK_LEFT)
						{
							if(draw_offset_col > 0)
							{
								draw_offset_col--;
								updateworldsurface();

								view_repeat_direction = 2;
								view_repeat_timer = 30;
							}
						}
						else if(event.key.keysym.sym == SDLK_RIGHT)
						{
							if(draw_offset_col < iWorldWidth - 20)
							{
								draw_offset_col++;
								updateworldsurface();

								view_repeat_direction = 3;
								view_repeat_timer = 30;
							}
						}

						if(event.key.keysym.sym == SDLK_PAGEUP)
						{
							if(--game_values.worldindex < 0)
								game_values.worldindex = worldlist.GetCount() - 1;

							worldlist.prev();

							loadcurrentworld();
						}

						if(event.key.keysym.sym == SDLK_PAGEDOWN)
						{
							if(++game_values.worldindex >= worldlist.GetCount())
								game_values.worldindex = 0;

							worldlist.next();

							loadcurrentworld();
						}

						break;
					}

					case SDL_KEYUP:
					{
						if(event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_DOWN ||
							event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_RIGHT)
						{
							view_repeat_direction = -1;
							view_repeat_timer = 0;
						}
						
						break;
					}

					case SDL_MOUSEBUTTONDOWN:
					{
						short iButtonX = event.button.x - draw_offset_x;
						short iButtonY = event.button.y - draw_offset_y;
						short iCol = iButtonX / TILESIZE + draw_offset_col;
						short iRow = iButtonY / TILESIZE + draw_offset_row;

						if(iButtonX >= 0 && iButtonY >= 0 && iButtonX < iWorldWidth * TILESIZE && iButtonY < iWorldHeight * TILESIZE)
						{
							if(event.button.button == SDL_BUTTON_LEFT && !ignoreclick)
							{
								if(edit_mode == 0) //selected background
								{
									if(g_worldmap.tiles[iCol][iRow].iBackgroundSprite != set_tile || fAutoPaint)
									{
										bool fNeedUpdate = false;
										if(g_worldmap.tiles[iCol][iRow].iBackgroundSprite != set_tile)
										{
											g_worldmap.tiles[iCol][iRow].iBackgroundSprite = set_tile;
											fNeedUpdate = true;
										}

										fNeedUpdate |= UpdateForeground(iCol, iRow);

										if((set_tile % WORLD_BACKGROUND_SPRITE_SET_SIZE) < 2 && fAutoPaint)
											fNeedUpdate |= UpdateCoastline(iCol, iRow);								

										if(fNeedUpdate)
											updateworldsurface();
									}
								}
								else if(edit_mode == 1) //selected foreground
								{
									if(g_worldmap.tiles[iCol][iRow].iForegroundSprite != set_tile)
									{
										g_worldmap.tiles[iCol][iRow].iForegroundSprite = set_tile;
										updateworldsurface();

										if(set_tile >= WORLD_BRIDGE_SPRITE_OFFSET && set_tile <= WORLD_BRIDGE_SPRITE_OFFSET + 3)
											g_worldmap.tiles[iCol][iRow].iConnectionType = set_tile - WORLD_BRIDGE_SPRITE_OFFSET + 12;
									}
								}
								else if(edit_mode == 2) //selected connection
								{
									g_worldmap.tiles[iCol][iRow].iConnectionType = set_tile;

									if(fAutoPaint)
										UpdatePath(iCol, iRow);
								}
								else if(edit_mode == 3) //selected type
								{
									//start tiles
									if(set_tile <= 1)
									{
										if(g_worldmap.tiles[iCol][iRow].iForegroundSprite != set_tile + WORLD_START_SPRITE_OFFSET)
										{
											g_worldmap.tiles[iCol][iRow].iType = 1;
											g_worldmap.tiles[iCol][iRow].iForegroundSprite = set_tile + WORLD_START_SPRITE_OFFSET;
											updateworldsurface();
										}
									}
									else if(set_tile <= 5) //doors
									{
										if(g_worldmap.tiles[iCol][iRow].iType != set_tile)
										{
											//if the door was placed on a start tile
											if(g_worldmap.tiles[iCol][iRow].iType == 1)
												g_worldmap.tiles[iCol][iRow].iForegroundSprite = 0;

											g_worldmap.tiles[iCol][iRow].iType = set_tile;
											updateworldsurface();
										}
									}
								}
								else if(edit_mode == 4) //selected path sprite
								{
									short iAdjustedTile = AdjustForeground(set_tile, iCol, iRow);
									bool fNeedUpdate = false;
		
									if(!fAutoPaint && g_worldmap.tiles[iCol][iRow].iForegroundSprite != iAdjustedTile)
									{
										g_worldmap.tiles[iCol][iRow].iForegroundSprite = iAdjustedTile;
										fNeedUpdate = true;
									}

									//Detect if there was a change so we can repaint the screen
									if(fAutoPaint)
									{
										short iOldTiles[9];
										GetForegroundTileValues(iCol, iRow, iOldTiles);
										g_worldmap.tiles[iCol][iRow].iForegroundSprite = iAdjustedTile;
										UpdatePathSprite(iCol, iRow);

										if(ForegroundTileValuesChanged(iCol, iRow, iOldTiles))
											fNeedUpdate = true;
									}

									if(fNeedUpdate)
										updateworldsurface();
								}
								else if(edit_mode == 5) //selected vehicle
								{
									AddVehicleToTile(iCol, iRow, set_tile);
								}
								else if(edit_mode == 6) //selected warp
								{
									AddWarpToTile(iCol, iRow, set_tile);
								}
								else if(edit_mode == 7) //water
								{
									if(g_worldmap.tiles[iCol][iRow].iBackgroundWater != set_tile)
									{
										g_worldmap.tiles[iCol][iRow].iBackgroundWater = set_tile;							
										updateworldsurface();
									}
								}
								else if(edit_mode == 8) //boundary
								{
									g_worldmap.tiles[iCol][iRow].iVehicleBoundary = set_tile;
								}
								else if(edit_mode == 9)
								{
									//if the stage was placed on a start tile
									if(g_worldmap.tiles[iCol][iRow].iType == 1)
									{
										g_worldmap.tiles[iCol][iRow].iForegroundSprite = 0;
										updateworldsurface();
									}

									g_worldmap.tiles[iCol][iRow].iType = set_tile;
								}
							}
							else if(event.button.button == SDL_BUTTON_RIGHT)
							{
								if(edit_mode == 0)
								{
									if(g_worldmap.tiles[iCol][iRow].iBackgroundSprite != 0 || fAutoPaint)
									{
										bool fNeedUpdate = false;

										if(g_worldmap.tiles[iCol][iRow].iBackgroundSprite != 0)
										{
											g_worldmap.tiles[iCol][iRow].iBackgroundSprite = 0;
											fNeedUpdate = true;
										}

										fNeedUpdate |= UpdateForeground(iCol, iRow);

										if(fAutoPaint)
											fNeedUpdate |= UpdateCoastline(iCol, iRow);
										
										if(fNeedUpdate)
											updateworldsurface();
									}
								}
								else if(edit_mode == 1)
								{
									if(g_worldmap.tiles[iCol][iRow].iForegroundSprite != 0)
									{
										g_worldmap.tiles[iCol][iRow].iForegroundSprite = 0;
										updateworldsurface();
									}
								}
								else if(edit_mode == 2)
								{
									g_worldmap.tiles[iCol][iRow].iConnectionType = 0;

									if(fAutoPaint)
										UpdatePath(iCol, iRow);
								}
								else if(edit_mode == 3) //selected start/door
								{
									if(g_worldmap.tiles[iCol][iRow].iType == 1)
									{
										g_worldmap.tiles[iCol][iRow].iForegroundSprite = 0;
										updateworldsurface();
									}
									else if(g_worldmap.tiles[iCol][iRow].iType <= 5)
									{
										g_worldmap.tiles[iCol][iRow].iType = 0;
										updateworldsurface();
									}
									
									g_worldmap.tiles[iCol][iRow].iType = 0;
								}
								else if(edit_mode == 4)
								{
									bool fNeedUpdate = false;
		
									if(!fAutoPaint && g_worldmap.tiles[iCol][iRow].iForegroundSprite != 0)
									{
										g_worldmap.tiles[iCol][iRow].iForegroundSprite = 0;
										fNeedUpdate = true;
									}

									//Detect if there was a change so we can repaint the screen
									if(fAutoPaint)
									{
										short iOldTiles[9];
										GetForegroundTileValues(iCol, iRow, iOldTiles);
										g_worldmap.tiles[iCol][iRow].iForegroundSprite = 0;
										UpdatePathSprite(iCol, iRow);

										if(ForegroundTileValuesChanged(iCol, iRow, iOldTiles))
											fNeedUpdate = true;
									}

									if(fNeedUpdate)
										updateworldsurface();
								}
								else if(edit_mode == 5)
								{
									RemoveVehicleFromTile(iCol, iRow);
								}
								else if(edit_mode == 6)
								{
									RemoveWarpFromTile(iCol, iRow);
								}
								else if(edit_mode == 7) //water
								{
									if(g_worldmap.tiles[iCol][iRow].iBackgroundWater != 0)
									{
										g_worldmap.tiles[iCol][iRow].iBackgroundWater = 0;
										updateworldsurface();
									}
								}
								else if(edit_mode == 8) //boundary
								{
									g_worldmap.tiles[iCol][iRow].iVehicleBoundary = 0;
								}
								else if(edit_mode == 9) //stage
								{
									g_worldmap.tiles[iCol][iRow].iType = 0;
								}
							}
						}
						
						break;
					}
					//Painting tiles with mouse movement
					case SDL_MOUSEMOTION:
					{
						short iButtonX = event.button.x - draw_offset_x;
						short iButtonY = event.button.y - draw_offset_y;
						short iCol = iButtonX / TILESIZE + draw_offset_col;
						short iRow = iButtonY / TILESIZE + draw_offset_row;

						if(iButtonX >= 0 && iButtonY >= 0 && iButtonX < iWorldWidth * TILESIZE && iButtonY < iWorldHeight * TILESIZE)
						{
							if(event.motion.state == SDL_BUTTON(SDL_BUTTON_LEFT) && !ignoreclick)
							{
								if(edit_mode == 0) //selected background
								{
									if(g_worldmap.tiles[iCol][iRow].iBackgroundSprite != set_tile || fAutoPaint)
									{
										bool fNeedUpdate = false;
										if(g_worldmap.tiles[iCol][iRow].iBackgroundSprite != set_tile)
										{
											g_worldmap.tiles[iCol][iRow].iBackgroundSprite = set_tile;
											fNeedUpdate = true;
										}

										fNeedUpdate |= UpdateForeground(iCol, iRow);

										if((set_tile % WORLD_BACKGROUND_SPRITE_SET_SIZE) < 2 && fAutoPaint)
											fNeedUpdate |= UpdateCoastline(iCol, iRow);								

										if(fNeedUpdate)
											updateworldsurface();
									}
								}
								else if(edit_mode == 1)
								{
									if(g_worldmap.tiles[iCol][iRow].iForegroundSprite != set_tile)
									{
										g_worldmap.tiles[iCol][iRow].iForegroundSprite = set_tile;
										updateworldsurface();
									}
								}
								else if(edit_mode == 2)
								{
									g_worldmap.tiles[iCol][iRow].iConnectionType = set_tile;

									if(fAutoPaint)
										UpdatePath(iCol, iRow);
								}
								else if(edit_mode == 3) //selected stage/door
								{
									if(set_tile <= 1)
									{
										if(g_worldmap.tiles[iCol][iRow].iForegroundSprite != set_tile + WORLD_START_SPRITE_OFFSET)
										{
											g_worldmap.tiles[iCol][iRow].iType = 1;
											g_worldmap.tiles[iCol][iRow].iForegroundSprite = set_tile + WORLD_START_SPRITE_OFFSET;
											updateworldsurface();
										}
									}
									else if(set_tile <= 5)
									{
										if(g_worldmap.tiles[iCol][iRow].iType != set_tile)
										{
											//if the door was placed on a start tile
											if(g_worldmap.tiles[iCol][iRow].iType == 1)
												g_worldmap.tiles[iCol][iRow].iForegroundSprite = 0;

											g_worldmap.tiles[iCol][iRow].iType = set_tile;
											updateworldsurface();
										}
									}
								}
								else if(edit_mode == 4)
								{
									short iAdjustedTile = AdjustForeground(set_tile, iCol, iRow);
									bool fNeedUpdate = false;
		
									if(!fAutoPaint && g_worldmap.tiles[iCol][iRow].iForegroundSprite != iAdjustedTile)
									{
										g_worldmap.tiles[iCol][iRow].iForegroundSprite = iAdjustedTile;
										fNeedUpdate = true;
									}

									//Detect if there was a change so we can repaint the screen
									if(fAutoPaint)
									{
										short iOldTiles[9];
										GetForegroundTileValues(iCol, iRow, iOldTiles);
										g_worldmap.tiles[iCol][iRow].iForegroundSprite = iAdjustedTile;
										UpdatePathSprite(iCol, iRow);

										if(ForegroundTileValuesChanged(iCol, iRow, iOldTiles))
											fNeedUpdate = true;
									}

									if(fNeedUpdate)
										updateworldsurface();
								}
								else if(edit_mode == 5)
								{
									AddVehicleToTile(iCol, iRow, set_tile);
								}
								else if(edit_mode == 6)
								{
									AddWarpToTile(iCol, iRow, set_tile);
								}
								else if(edit_mode == 7) //water
								{
									if(g_worldmap.tiles[iCol][iRow].iBackgroundWater != set_tile)
									{
										g_worldmap.tiles[iCol][iRow].iBackgroundWater = set_tile;							
										updateworldsurface();
									}
								}
								else if(edit_mode == 9) //stage
								{
									//if the stage was placed on a start tile
									if(g_worldmap.tiles[iCol][iRow].iType == 1)
									{
										g_worldmap.tiles[iCol][iRow].iForegroundSprite = 0;
										updateworldsurface();
									}

									g_worldmap.tiles[iCol][iRow].iType = set_tile;
								}
							}
							else if(event.motion.state == SDL_BUTTON(SDL_BUTTON_RIGHT))
							{
								if(edit_mode == 0) //selected background
								{
									if(g_worldmap.tiles[iCol][iRow].iBackgroundSprite != 0 || fAutoPaint)
									{
										bool fNeedUpdate = false;
										if(g_worldmap.tiles[iCol][iRow].iBackgroundSprite != 0)
										{
											g_worldmap.tiles[iCol][iRow].iBackgroundSprite = 0;
											fNeedUpdate = true;
										}

										fNeedUpdate |= UpdateForeground(iCol, iRow);

										if(fAutoPaint)
											fNeedUpdate |= UpdateCoastline(iCol, iRow);

										if(fNeedUpdate)
											updateworldsurface();
									}
								}
								else if(edit_mode == 1)
								{
									if(g_worldmap.tiles[iCol][iRow].iForegroundSprite != 0)
									{
										g_worldmap.tiles[iCol][iRow].iForegroundSprite = 0;
										updateworldsurface();
									}
								}
								else if(edit_mode == 2)
								{
									g_worldmap.tiles[iCol][iRow].iConnectionType = 0;

									if(fAutoPaint)
										UpdatePath(iCol, iRow);
								}
								else if(edit_mode == 3)
								{
									if(g_worldmap.tiles[iCol][iRow].iType == 1)
									{
										g_worldmap.tiles[iCol][iRow].iForegroundSprite = 0;
										updateworldsurface();
									}
									else if(g_worldmap.tiles[iCol][iRow].iType <= 5)
									{
										g_worldmap.tiles[iCol][iRow].iType = 0;
										updateworldsurface();
									}

									g_worldmap.tiles[iCol][iRow].iType = 0;
								}
								else if(edit_mode == 4)
								{
									bool fNeedUpdate = false;
		
									if(!fAutoPaint && g_worldmap.tiles[iCol][iRow].iForegroundSprite != 0)
									{
										g_worldmap.tiles[iCol][iRow].iForegroundSprite = 0;
										fNeedUpdate = true;
									}

									//Detect if there was a change so we can repaint the screen
									if(fAutoPaint)
									{
										short iOldTiles[9];
										GetForegroundTileValues(iCol, iRow, iOldTiles);
										g_worldmap.tiles[iCol][iRow].iForegroundSprite = 0;
										UpdatePathSprite(iCol, iRow);

										if(ForegroundTileValuesChanged(iCol, iRow, iOldTiles))
											fNeedUpdate = true;
									}

									if(fNeedUpdate)
										updateworldsurface();
								}
								else if(edit_mode == 5) //vehicles
								{
									RemoveVehicleFromTile(iCol, iRow);
								}
								else if(edit_mode == 6) //Warps
								{
									RemoveWarpFromTile(iCol, iRow);
								}
								else if(edit_mode == 7) //water
								{
									if(g_worldmap.tiles[iCol][iRow].iBackgroundWater != 0)
									{
										g_worldmap.tiles[iCol][iRow].iBackgroundWater = 0;							
										updateworldsurface();
									}
								}
								else if(edit_mode == 9) //stage
								{
									g_worldmap.tiles[iCol][iRow].iType = 0;
								}
							}
						}
					
						break;
					}

					case SDL_MOUSEBUTTONUP:
					{
						if(event.button.button == SDL_BUTTON_LEFT)
						{
							ignoreclick = false;
						}
					
						break;
					}

					default:
						break;
				}
			}
		
			//Allow auto-scrolling of world when the arrow keys are held down
			if(view_repeat_direction >= 0 && view_repeat_timer > 0)
			{
				if(--view_repeat_timer <= 0)
				{
					view_repeat_timer = 5;

					if(view_repeat_direction == 0 && draw_offset_row > 0)
					{
						draw_offset_row--;
						updateworldsurface();
					}
					else if(view_repeat_direction == 1 && draw_offset_row < iWorldHeight - 15)
					{
						draw_offset_row++;
						updateworldsurface();
					}
					else if(view_repeat_direction == 2 && draw_offset_col > 0)
					{
						draw_offset_col--;
						updateworldsurface();
					}
					else if(view_repeat_direction == 3 && draw_offset_col < iWorldWidth - 20)
					{
						draw_offset_col++;
						updateworldsurface();
					}
				}
			}
		}

		drawmap(false, TILESIZE);
		
		//Ask if you are sure you want to exit
		if(fExiting)
		{
			spr_dialog.draw(224, 176, 0, 0, 192, 128);
			menu_font_large.drawCentered(320, 195, "Exit");
			menu_font_large.drawCentered(320, 220, "Are You Sure?");
			menu_font_large.drawCentered(282, 254, "Yes");
			menu_font_large.drawCentered(356, 254, "No");

			spr_dialog.draw(fSelectedYes ? 250 : 326, 250, 192, 0, 64, 32);
		}
		else
		{
			//Draw Paths
			if(edit_mode == 2)
			{
				for(short iRow = draw_offset_row; iRow < draw_offset_row + 15 && iRow < iWorldHeight; iRow++)
				{
					for(short iCol = draw_offset_col; iCol <= draw_offset_col + 20 && iCol < iWorldWidth; iCol++)
					{
						short iConnection = g_worldmap.tiles[iCol][iRow].iConnectionType;

						if(iConnection > 0)
							spr_path.draw((iCol - draw_offset_col) * TILESIZE + draw_offset_x, (iRow - draw_offset_row) * TILESIZE + draw_offset_y, (iConnection - 1) << 5, 0, TILESIZE, TILESIZE);
					}
				}
			}
			else if(edit_mode == 6) //draw warps
			{
				std::vector<WorldWarp*>::iterator itr = warplist.begin(), lim = warplist.end();
				while(itr != lim)
				{
					WorldWarp * warp = *itr;

					short ix, iy;

					if(warp->iCol1 >= 0)
					{
						ix = (warp->iCol1 - draw_offset_col) * TILESIZE + draw_offset_x;
						iy = (warp->iRow1 - draw_offset_row) * TILESIZE + draw_offset_y;

						spr_warps[0].draw(ix, iy, warp->iID << 5, 0, 32, 32);
					}

					if(warp->iCol2 >= 0)
					{
						ix = (warp->iCol2 - draw_offset_col) * TILESIZE + draw_offset_x;
						iy = (warp->iRow2 - draw_offset_row) * TILESIZE + draw_offset_y;

						spr_warps[0].draw(ix, iy, warp->iID << 5, 0, 32, 32);
					}

					itr++;
				}
			}
			else if(edit_mode == 8) //draw boundaries
			{
				int color = SDL_MapRGB(blitdest->format, 255, 0, 255);
				for(short iRow = draw_offset_row; iRow < draw_offset_row + 15 && iRow < iWorldHeight; iRow++)
				{
					for(short iCol = draw_offset_col; iCol <= draw_offset_col + 20 && iCol < iWorldWidth; iCol++)
					{
						short iBoundary = g_worldmap.tiles[iCol][iRow].iVehicleBoundary - 1;

						if(iBoundary >= 0)
						{
							short ix = (iCol - draw_offset_col) * TILESIZE + draw_offset_x;
							short iy = (iRow - draw_offset_row) * TILESIZE + draw_offset_y;
							SDL_Rect r = {ix, iy, 32, 32};
							SDL_FillRect(blitdest, &r, color);

							spr_worldforegroundspecial[0].draw(ix, iy, (iBoundary % 10) << 5, (iBoundary / 10) << 5, 32, 32);
						}
					}
				}
			}
			else if(edit_mode == 9) //draw stages
			{
				int color = SDL_MapRGB(blitdest->format, 0, 0, 255);
				for(short iRow = draw_offset_row; iRow < draw_offset_row + 15 && iRow < iWorldHeight; iRow++)
				{
					for(short iCol = draw_offset_col; iCol <= draw_offset_col + 20 && iCol < iWorldWidth; iCol++)
					{
						short iType = g_worldmap.tiles[iCol][iRow].iType - 6;

						if(iType >= 0)
						{
							short ix = (iCol - draw_offset_col) * TILESIZE + draw_offset_x;
							short iy = (iRow - draw_offset_row) * TILESIZE + draw_offset_y;
							SDL_Rect r = {ix, iy, 32, 32};
							SDL_FillRect(blitdest, &r, color);

							spr_worldforegroundspecial[0].draw(ix, iy, (iType % 10) << 5, (iType / 10) << 5, 32, 32);
						}
					}
				}
			}

			if(edit_mode == 5 || edit_mode == 8) //draw vehicles
			{
				std::vector<WorldVehicle*>::iterator itr = vehiclelist.begin(), lim = vehiclelist.end();
				int color = SDL_MapRGB(blitdest->format, 0, 0, 128);
				while(itr != lim)
				{
					WorldVehicle * vehicle = *itr;

					short ix = (vehicle->iCurrentTileX  - draw_offset_col) * TILESIZE + draw_offset_x;
					short iy = (vehicle->iCurrentTileY - draw_offset_row) * TILESIZE + draw_offset_y;

					if(edit_mode == 8)
					{
						SDL_Rect r = {ix, iy, 32, 32};
						SDL_FillRect(blitdest, &r, color);
					}
					
					spr_worldvehicle[0].draw(ix, iy, vehicle->iDrawDirection << 5, vehicle->iDrawSprite << 5, 32, 32);

					itr++;

					if(edit_mode == 8)
					{
						short iBoundary = vehicle->iBoundary - 1;
						if(iBoundary == -1)
							spr_worldforegroundspecial[0].draw(ix, iy, 288, 288, 32, 32);
						else
							spr_worldforegroundspecial[0].draw(ix, iy, (iBoundary % 10) << 5, (iBoundary / 10) << 5, 32, 32);
					}
				}
			}

			menu_font_small.draw(0, 0, szEditModes[edit_mode]);
			
			if(fAutoPaint)
				menu_font_small.draw(0, 16, "Auto Paint");

			menu_font_small.drawRightJustified(640, 0, worldlist.current_name());
			

			if(--g_musiccategorydisplaytimer > 0)
			{
				spr_dialog.draw(224, 176, 0, 0, 192, 128);
				menu_font_small.drawCentered(320, 195, "Music Category");
				menu_font_large.drawCentered(320, 220, g_szWorldMusicCategoryNames[g_worldmap.iMusicCategory]);

				menu_font_small.drawCentered(320, 255, "Press 'R' Again");
				menu_font_small.drawCentered(320, 270, "To Change");
			}
			
			DrawMessage();
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

void DrawMessage()
{
	if(g_messagedisplaytimer > 0)
	{
		--g_messagedisplaytimer;

		spr_dialog.draw(224, 176, 0, 0, 192, 128);
		menu_font_large.drawCentered(320, 195, g_szMessageTitle.c_str());
		menu_font_large.drawCentered(320, 220, g_szMessageLine[0].c_str());
		menu_font_large.drawCentered(320, 240, g_szMessageLine[1].c_str());
		menu_font_large.drawCentered(320, 260, g_szMessageLine[2].c_str());
	}
}

void GetForegroundTileValues(short iCol, short iRow, short iOldTiles[9])
{
	short iIndex = 0;
	for(short iAutoRow = iRow - 1; iAutoRow <= iRow + 1; iAutoRow++)
	{
		for(short iAutoCol = iCol - 1; iAutoCol <= iCol + 1; iAutoCol++)
		{
			if(iAutoRow >= 0 && iAutoRow < iWorldHeight && iAutoCol >= 0 && iAutoCol < iWorldWidth)
			{
				iOldTiles[iIndex++] = g_worldmap.tiles[iAutoCol][iAutoRow].iForegroundSprite;
			}
		}
	}

	for(short i = iIndex; i < 9; i++)
		iOldTiles[i] = 0;
}

bool ForegroundTileValuesChanged(short iCol, short iRow, short iOldTiles[9])
{
	short iIndex = 0;
	for(short iAutoRow = iRow - 1; iAutoRow <= iRow + 1; iAutoRow++)
	{
		for(short iAutoCol = iCol - 1; iAutoCol <= iCol + 1; iAutoCol++)
		{
			if(iAutoRow >= 0 && iAutoRow < iWorldHeight && iAutoCol >= 0 && iAutoCol < iWorldWidth)
			{
				if(g_worldmap.tiles[iAutoCol][iAutoRow].iForegroundSprite != iOldTiles[iIndex++])
					return true;
			}
		}
	}

	return false;
}

void ReadVehiclesIntoEditor()
{
	std::vector<WorldVehicle*>::iterator itr = vehiclelist.begin(), lim = vehiclelist.end();
	while(itr != lim)
	{
		delete (*itr);
		itr++;
	}

	vehiclelist.clear();

	for(short iVehicle = 0; iVehicle < g_worldmap.iNumVehicles; iVehicle++)
	{
		WorldVehicle * vehicle = &g_worldmap.vehicles[iVehicle];
		WorldVehicle * vehiclecopy = new WorldVehicle();
		
		vehiclecopy->iDrawSprite = vehicle->iDrawSprite;
		vehiclecopy->iActionId = vehicle->iActionId;
		vehiclecopy->iCurrentTileX = vehicle->iCurrentTileX;
		vehiclecopy->iCurrentTileY = vehicle->iCurrentTileY;
		vehiclecopy->iMinMoves = vehicle->iMinMoves;
		vehiclecopy->iMaxMoves = vehicle->iMaxMoves;
		vehiclecopy->fSpritePaces = vehicle->fSpritePaces;
		vehiclecopy->iDrawDirection = vehicle->iDrawDirection;
		vehiclecopy->iBoundary = vehicle->iBoundary;

		vehiclelist.push_back(vehiclecopy);
	}
}

void WriteVehiclesIntoWorld()
{
	//Cleanup old vehicles
	delete [] g_worldmap.vehicles;

	//Insert new vehicles
	g_worldmap.iNumVehicles = vehiclelist.size();
	g_worldmap.vehicles = new WorldVehicle[g_worldmap.iNumVehicles];
	
	for(short iVehicle = 0; iVehicle < g_worldmap.iNumVehicles; iVehicle++)
	{
		WorldVehicle * vehicle = vehiclelist[iVehicle];
		WorldVehicle * vehiclecopy = &g_worldmap.vehicles[iVehicle];

		vehiclecopy->iDrawSprite = vehicle->iDrawSprite;
		vehiclecopy->iActionId = vehicle->iActionId;
		vehiclecopy->iCurrentTileX = vehicle->iCurrentTileX;
		vehiclecopy->iCurrentTileY = vehicle->iCurrentTileY;
		vehiclecopy->iMinMoves = vehicle->iMinMoves;
		vehiclecopy->iMaxMoves = vehicle->iMaxMoves;
		vehiclecopy->fSpritePaces = vehicle->fSpritePaces;
		vehiclecopy->iDrawDirection = vehicle->iDrawDirection;
		vehiclecopy->iBoundary = vehicle->iBoundary;
	}
}

void AddVehicleToTile(short iCol, short iRow, short iType)
{
	std::vector<WorldVehicle*>::iterator itr = vehiclelist.begin(), lim = vehiclelist.end();
	WorldVehicle * newvehicle = NULL;
	while(itr != lim)
	{
		WorldVehicle * vehicle = *itr;
		if(vehicle->iCurrentTileX == iCol && vehicle->iCurrentTileY == iRow)
		{
			newvehicle = vehicle;
			break;
		}

		itr++;
	}

	if(!newvehicle)
	{
		newvehicle = new WorldVehicle();
		newvehicle->iCurrentTileX = iCol;
		newvehicle->iCurrentTileY = iRow;
		vehiclelist.push_back(newvehicle);
	}

	newvehicle->iDrawSprite = iType;
	newvehicle->iActionId = 0;
	newvehicle->iMinMoves = 5;
	newvehicle->iMaxMoves = 8;
	newvehicle->fSpritePaces = true;
	newvehicle->iDrawDirection = 0;
	newvehicle->iBoundary = 0;
}

void RemoveVehicleFromTile(short iCol, short iRow)
{
	std::vector<WorldVehicle*>::iterator itr = vehiclelist.begin(), lim = vehiclelist.end();
	while(itr != lim)
	{
		WorldVehicle * vehicle = *itr;
		if(vehicle->iCurrentTileX == iCol && vehicle->iCurrentTileY == iRow)
		{
			delete (*itr);
			
			itr = vehiclelist.erase(itr);
			lim = vehiclelist.end();
		
			return;
		}

		itr++;
	}
}

void ReadWarpsIntoEditor()
{
	std::vector<WorldWarp*>::iterator itr = warplist.begin(), lim = warplist.end();
	while(itr != lim)
	{
		delete (*itr);
		itr++;
	}

	warplist.clear();

	for(short iWarp = 0; iWarp < g_worldmap.iNumWarps; iWarp++)
	{
		WorldWarp * warp = &g_worldmap.warps[iWarp];
		WorldWarp * warpcopy = new WorldWarp();
		
		warpcopy->iID = warp->iID;
		warpcopy->iCol1 = warp->iCol1;
		warpcopy->iRow1 = warp->iRow1;
		warpcopy->iCol2 = warp->iCol2;
		warpcopy->iRow2 = warp->iRow2;

		warplist.push_back(warpcopy);
	}
}

void WriteWarpsIntoWorld()
{
	//Cleanup old vehicles
	delete [] g_worldmap.warps;

	//Insert new vehicles
	g_worldmap.iNumWarps = warplist.size();
	g_worldmap.warps = new WorldWarp[g_worldmap.iNumWarps];
	
	for(short iWarp = 0; iWarp < g_worldmap.iNumWarps; iWarp++)
	{
		WorldWarp * warp = warplist[iWarp];
		WorldWarp * warpcopy = &g_worldmap.warps[iWarp];

		warpcopy->iID = warp->iID;
		warpcopy->iCol1 = warp->iCol1;
		warpcopy->iRow1 = warp->iRow1;
		warpcopy->iCol2 = warp->iCol2;
		warpcopy->iRow2 = warp->iRow2;
	}
}

void AddWarpToTile(short iCol, short iRow, short iType)
{
	std::vector<WorldWarp*>::iterator itr = warplist.begin(), lim = warplist.end();
	WorldWarp * newwarp = NULL;
	while(itr != lim)
	{
		WorldWarp * warp = *itr;
		if(warp->iID == iType)
		{
			newwarp = warp;
			break;
		}

		itr++;
	}

	if(!newwarp)
	{
		newwarp = new WorldWarp();

		newwarp->iID = iType;
		newwarp->iCol1 = iCol;
		newwarp->iRow1 = iRow;
		newwarp->iCol2 = -1;
		newwarp->iRow2 = -1;
		
		warplist.push_back(newwarp);
	}
	else
	{
		if(newwarp->iCol1 == -1)
		{
			newwarp->iCol1 = iCol;
			newwarp->iRow1 = iRow;
		}
		else if(newwarp->iCol1 != iCol || newwarp->iRow1 != iRow)
		{
			newwarp->iCol2 = iCol;
			newwarp->iRow2 = iRow;
		}
	}
}

void RemoveWarpFromTile(short iCol, short iRow)
{
	std::vector<WorldWarp*>::iterator itr = warplist.begin(), lim = warplist.end();
	while(itr != lim)
	{
		WorldWarp * warp = *itr;
		if(warp->iCol1 == iCol && warp->iRow1 == iRow)
		{
			if(warp->iCol2 == -1 && warp->iRow2 == -1)
			{
				delete (*itr);
				
				itr = warplist.erase(itr);
				lim = warplist.end();
			
				return;
			}
			else
			{
				warp->iCol1 = -1;
				warp->iRow1 = -1;
			}
		}
		else if(warp->iCol2 == iCol && warp->iRow2 == iRow)
		{
			if(warp->iCol1 == -1 && warp->iRow1 == -1)
			{
				delete (*itr);
				
				itr = warplist.erase(itr);
				lim = warplist.end();
			
				return;
			}
			else
			{
				warp->iCol2 = -1;
				warp->iRow2 = -1;
			}
		}

		itr++;
	}
}

void UpdatePathSprite(short iCol, short iRow)
{
	for(short iAutoRow = iRow - 1; iAutoRow <= iRow + 1; iAutoRow++)
	{
		for(short iAutoCol = iCol - 1; iAutoCol <= iCol + 1; iAutoCol++)
		{
			if(iAutoRow >= 0 && iAutoRow < iWorldHeight && iAutoCol >= 0 && iAutoCol < iWorldWidth)
			{
				AutoSetPathSprite(iAutoCol, iAutoRow);
			}
		}
	}
}

void AutoSetPathSprite(short iCol, short iRow)
{
	short iPathTypes[16] = {6, 4, 3, 5, 6, 4, 1, 5, 6, 2, 3, 5, 6, 4, 3, 5};

	short iPath = 0;
	short iNeighborIndex = 0;

	short iForegroundSprite = g_worldmap.tiles[iCol][iRow].iForegroundSprite;
	short iForegroundStyle = iForegroundSprite / WORLD_PATH_SPRITE_SET_SIZE;

	if(iForegroundSprite == 0 || iForegroundSprite >= WORLD_FOREGROUND_STAGE_OFFSET)
		return;

	for(short iAutoRow = iRow - 1; iAutoRow <= iRow + 1; iAutoRow++)
	{
		for(short iAutoCol = iCol - 1; iAutoCol <= iCol + 1; iAutoCol++)
		{
			if(iAutoCol == iCol && iAutoRow == iRow)
				continue;

			if((iAutoCol == iCol && iAutoRow != iRow) || (iAutoCol != iCol && iAutoRow == iRow))
			{
				if(iAutoRow >= 0 && iAutoRow < iWorldHeight && iAutoCol >= 0 && iAutoCol < iWorldWidth)
				{
					iForegroundSprite = g_worldmap.tiles[iAutoCol][iAutoRow].iForegroundSprite;
					
					if((iForegroundSprite >= WORLD_BRIDGE_SPRITE_OFFSET && iForegroundSprite <= WORLD_BRIDGE_SPRITE_OFFSET + 3) || 
						(iForegroundSprite >= WORLD_FOREGROUND_STAGE_OFFSET && iForegroundSprite <= WORLD_FOREGROUND_STAGE_OFFSET + 399) ||
						/*(iForegroundSprite >= WORLD_FOREGROUND_SPRITE_OFFSET && iForegroundSprite <= WORLD_FOREGROUND_SPRITE_OFFSET + 179) ||*/
						/*(iForegroundSprite >= WORLD_FOREGROUND_SPRITE_ANIMATED_OFFSET && iForegroundSprite <= WORLD_FOREGROUND_SPRITE_ANIMATED_OFFSET + 29) ||*/
						(iForegroundSprite >= WORLD_START_SPRITE_OFFSET && iForegroundSprite <= WORLD_START_SPRITE_OFFSET + 1))
					{
						iPath += 1 << iNeighborIndex;
					}
					else if(iForegroundSprite >= 0 && iForegroundSprite < WORLD_FOREGROUND_STAGE_OFFSET)
					{
						short iPathSprite = iForegroundSprite % WORLD_PATH_SPRITE_SET_SIZE;
							
						if(iPathSprite >= 1 && iPathSprite <= 18)
							iPath += 1 << iNeighborIndex;
					}
				}
				
				iNeighborIndex++;
			}
		}
	}

	//#1 == -  2 == |  3 == -o  4 == !  5 == -`  6 == o
	g_worldmap.tiles[iCol][iRow].iForegroundSprite = AdjustForeground(iPathTypes[iPath] + iForegroundStyle * WORLD_PATH_SPRITE_SET_SIZE, iCol, iRow);
}

//Convert foreground sprite to match the background sprite
short AdjustForeground(short iSprite, short iCol, short iRow)
{
	if(iSprite >= WORLD_FOREGROUND_STAGE_OFFSET)
		return iSprite;

	short iBackgroundSprite = g_worldmap.tiles[iCol][iRow].iBackgroundSprite % WORLD_BACKGROUND_SPRITE_SET_SIZE;

	short iPathStyle = iSprite / WORLD_PATH_SPRITE_SET_SIZE;
	iSprite %= WORLD_PATH_SPRITE_SET_SIZE;

	//Convert already adjusted sprites back to their "base" sprite 
	if(iSprite >= 11 && iSprite <= 18)
		iSprite = 2 - (iSprite % 2);
	else if(iSprite >= 7 && iSprite <= 10)
		iSprite -= 4;
	
	if(iBackgroundSprite == 1)
		return iSprite + iPathStyle * WORLD_PATH_SPRITE_SET_SIZE;

	if(iSprite == 2 && (iBackgroundSprite == 12 || iBackgroundSprite == 20 || iBackgroundSprite == 23 || 
		iBackgroundSprite == 24 || iBackgroundSprite == 27 || iBackgroundSprite == 32 || iBackgroundSprite == 36 ||
		iBackgroundSprite == 37 || iBackgroundSprite == 40))
	{
		iSprite = 14;
	}
	else if(iSprite == 2 && (iBackgroundSprite == 13 || iBackgroundSprite == 21 || iBackgroundSprite == 22 || 
		iBackgroundSprite == 25 || iBackgroundSprite == 26 || iBackgroundSprite == 33 || iBackgroundSprite == 34 ||
		iBackgroundSprite == 35 || iBackgroundSprite == 41))
	{
		iSprite = 16;
	}
	else if(iSprite == 1 && (iBackgroundSprite == 14 || iBackgroundSprite == 18 || iBackgroundSprite == 19 || 
		iBackgroundSprite == 26 || iBackgroundSprite == 27 || iBackgroundSprite == 31 || iBackgroundSprite == 34 ||
		iBackgroundSprite == 37 || iBackgroundSprite == 39))
	{
		iSprite = 13;
	}
	else if(iSprite == 1 && (iBackgroundSprite == 15 || iBackgroundSprite == 16 || iBackgroundSprite == 17 || 
		iBackgroundSprite == 24 || iBackgroundSprite == 25 || iBackgroundSprite == 30 || iBackgroundSprite == 35 ||
		iBackgroundSprite == 36 || iBackgroundSprite == 38))
	{
		iSprite = 15;
	}
	else if(iSprite == 2 && (iBackgroundSprite == 28 || iBackgroundSprite == 38 || iBackgroundSprite == 39 ||
		iBackgroundSprite == 43))
	{
		iSprite = 12;
	}
	else if(iSprite == 1 && (iBackgroundSprite == 28 || iBackgroundSprite == 40 || iBackgroundSprite == 41 ||
		 iBackgroundSprite == 42))
	{
		iSprite = 11;	
	}
	else
	{
		if(iSprite == 1 || iSprite == 2)
			iSprite += 16;
		else if(iSprite >= 3 && iSprite <= 6)
			iSprite += 4;
	}

	return iSprite + iPathStyle * WORLD_PATH_SPRITE_SET_SIZE;
}


void UpdatePath(short iCol, short iRow)
{
	for(short iAutoRow = iRow - 1; iAutoRow <= iRow + 1; iAutoRow++)
	{
		for(short iAutoCol = iCol - 1; iAutoCol <= iCol + 1; iAutoCol++)
		{
			if(iAutoRow >= 0 && iAutoRow < iWorldHeight && iAutoCol >= 0 && iAutoCol < iWorldWidth)
			{
				AutoSetPath(iAutoCol, iAutoRow);
			}
		}
	}
}

void AutoSetPath(short iCol, short iRow)
{
	short iPathTypes[16] = {11, 1, 2, 3, 2, 4, 2, 8, 1, 1, 6, 7, 5, 9, 10, 11};

	short iPath = 0;
	short iNeighborIndex = 0;

	if(g_worldmap.tiles[iCol][iRow].iConnectionType == 0)
		return;

	for(short iAutoRow = iRow - 1; iAutoRow <= iRow + 1; iAutoRow++)
	{
		for(short iAutoCol = iCol - 1; iAutoCol <= iCol + 1; iAutoCol++)
		{
			if(iAutoCol == iCol && iAutoRow == iRow)
				continue;

			if((iAutoCol == iCol && iAutoRow != iRow) || (iAutoCol != iCol && iAutoRow == iRow))
			{
				if(iAutoRow >= 0 && iAutoRow < iWorldHeight && iAutoCol >= 0 && iAutoCol < iWorldWidth)
				{
					if(g_worldmap.tiles[iAutoCol][iAutoRow].iConnectionType > 0)
						iPath += 1 << iNeighborIndex;
				}
				
				iNeighborIndex++;
			}
		}
	}

	//#1 == |  2 == -  3 == -!  4 == L  5 == ,-  6 == -,
	//#7 == -|  8 == -`-  9 == |-  10 == -,-  11 == +

	short iPathType = iPathTypes[iPath];
	short iForegroundSprite = g_worldmap.tiles[iCol][iRow].iForegroundSprite;

	if(iPathType == 2 && iForegroundSprite >= WORLD_BRIDGE_SPRITE_OFFSET && iForegroundSprite <= WORLD_BRIDGE_SPRITE_OFFSET + 1)
	{
		iPathType = iForegroundSprite - WORLD_BRIDGE_SPRITE_OFFSET + 12;
	}
	else if(iPathType == 1 && iForegroundSprite >= WORLD_BRIDGE_SPRITE_OFFSET + 2 && iForegroundSprite <= WORLD_BRIDGE_SPRITE_OFFSET + 3)
	{
		iPathType = iForegroundSprite - WORLD_BRIDGE_SPRITE_OFFSET + 12;
	}

	g_worldmap.tiles[iCol][iRow].iConnectionType = iPathType;
}

bool UpdateForeground(short iCol, short iRow)
{
	short iNewForeground = AdjustForeground(g_worldmap.tiles[iCol][iRow].iForegroundSprite, iCol, iRow);

	if(g_worldmap.tiles[iCol][iRow].iForegroundSprite != iNewForeground)
	{
		g_worldmap.tiles[iCol][iRow].iForegroundSprite = iNewForeground;
		return true;
	}

	return false;
}

bool UpdateCoastline(short iCol, short iRow)
{
	short iStartCol = iCol == 0 ? 0 : iCol - 1;
	short iEndCol = iCol == iWorldWidth - 1 ? iWorldWidth - 1 : iCol + 1;

	short iStartRow = iRow == 0 ? 0 : iRow - 1;
	short iEndRow = iRow == iWorldHeight - 1 ? iWorldHeight - 1 : iRow + 1;

	bool fRet = false;
	for(short iAutoRow = iStartRow; iAutoRow <= iEndRow; iAutoRow++)
	{
		for(short iAutoCol = iStartCol; iAutoCol <= iEndCol; iAutoCol++)
		{
			fRet |= AutoSetTile(iAutoCol, iAutoRow);
			fRet |= UpdateForeground(iAutoCol, iAutoRow);
		}
	}	

	return fRet;
}

bool AutoSetTile(short iCol, short iRow)
{
	//Don't need to do anything if this tile is solid
	if(g_worldmap.tiles[iCol][iRow].iBackgroundSprite % WORLD_BACKGROUND_SPRITE_SET_SIZE == 1)
		return false;

	bool iTile[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	short iNeighborIndex = 0;

	short iNeighborStyle[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	for(short iAutoRow = iRow - 1; iAutoRow <= iRow + 1; iAutoRow++)
	{
		for(short iAutoCol = iCol - 1; iAutoCol <= iCol + 1; iAutoCol++)
		{
			if(iAutoCol == iCol && iAutoRow == iRow)
				continue;

			if(iAutoRow >= 0 && iAutoRow < iWorldHeight && iAutoCol >= 0 && iAutoCol < iWorldWidth)
			{
				short iBackgroundSprite = g_worldmap.tiles[iAutoCol][iAutoRow].iBackgroundSprite;

				if(iBackgroundSprite % WORLD_BACKGROUND_SPRITE_SET_SIZE == 1)
				{
					iTile[iNeighborIndex] = true;
					iNeighborStyle[iBackgroundSprite / WORLD_BACKGROUND_SPRITE_SET_SIZE]++;
				}
			}
			
			iNeighborIndex++;
		}
	}

	short iMaxStyle = 0;
	short iTileStyleOffset = 0;
	for(short iStyle = 0; iStyle < 10; iStyle++)
	{
		if(iNeighborStyle[iStyle] > iMaxStyle)
		{
			iMaxStyle = iNeighborStyle[iStyle];
			iTileStyleOffset = iStyle * WORLD_BACKGROUND_SPRITE_SET_SIZE;
		}
	}

	short iNewTile = 0;
	if(iTile[0] && !iTile[1] && iTile[2] && !iTile[3] && !iTile[4] && iTile[5] && !iTile[6] && iTile[7])
	{
		iNewTile = iTileStyleOffset + 44;
	}
	else if(iTile[0] && !iTile[1] && !iTile[3] && iTile[4] && iTile[5] && !iTile[6])
	{
		iNewTile = iTileStyleOffset + 30;
	}
	else if(!iTile[1] && iTile[2] && iTile[3] && !iTile[4] && !iTile[6] && iTile[7])
	{
		iNewTile = iTileStyleOffset + 31;
	}
	else if(iTile[1] && !iTile[3] && !iTile[4] && iTile[5] && !iTile[6] && iTile[7])
	{
		iNewTile = iTileStyleOffset + 32;
	}
	else if(iTile[0] && !iTile[1] && iTile[2] && !iTile[3] && !iTile[4] && iTile[6])
	{
		iNewTile = iTileStyleOffset + 33;
	}
	else if(!iTile[1] && iTile[2] && iTile[3] && !iTile[4] && iTile[6])
	{
		iNewTile = iTileStyleOffset + 34;
	}
	else if(iTile[0] && !iTile[1] && !iTile[3] && iTile[4] && iTile[6])
	{
		iNewTile = iTileStyleOffset + 35;
	}
	else if(iTile[1] && !iTile[3] && iTile[4] && iTile[5] && !iTile[6])
	{
		iNewTile = iTileStyleOffset + 36;
	}
	else if(iTile[1] && iTile[3] && !iTile[4] && !iTile[6] && iTile[7])
	{
		iNewTile = iTileStyleOffset + 37;
	}
	else if(iTile[0] && !iTile[1] && iTile[2] && !iTile[4] && iTile[7])
	{
		iNewTile = iTileStyleOffset + 45;
	}
	else if(iTile[2] && !iTile[4] && iTile[5] && !iTile[6] && iTile[7])
	{
		iNewTile = iTileStyleOffset + 46;
	}
	else if(iTile[0] && !iTile[3] && iTile[5] && !iTile[6] && iTile[7])
	{
		iNewTile = iTileStyleOffset + 47;
	}
	else if(iTile[0] && !iTile[1] && iTile[2] && !iTile[3] && iTile[5])
	{
		iNewTile = iTileStyleOffset + 48;
	}
	else if(iTile[1] && iTile[3] && iTile[4] && iTile[6])
	{
		iNewTile = iTileStyleOffset + 28;
	}
	else if(iTile[1] && !iTile[3] && iTile[4] && iTile[6])
	{
		iNewTile = iTileStyleOffset + 38;
	}
	else if(iTile[1] && iTile[3] && !iTile[4] && iTile[6])
	{
		iNewTile = iTileStyleOffset + 39;
	}
	else if(iTile[1] && iTile[3] && iTile[4] && !iTile[6])
	{
		iNewTile = iTileStyleOffset + 40;
	}
	else if(!iTile[1] && iTile[3] && iTile[4] && iTile[6])
	{
		iNewTile = iTileStyleOffset + 41;
	}
	else if(!iTile[1] && iTile[3] && iTile[4] && !iTile[6])
	{
		iNewTile = iTileStyleOffset + 42;
	}
	else if(iTile[1] && !iTile[3] && !iTile[4] && iTile[6])
	{
		iNewTile = iTileStyleOffset + 43;
	}
	else if(iTile[0] && !iTile[1] && iTile[4])
	{
		iNewTile = iTileStyleOffset + 16;
	}
	else if(iTile[4] && iTile[5] && !iTile[6])
	{
		iNewTile = iTileStyleOffset + 17;
	}
	else if(iTile[3] && !iTile[6] && iTile[7])
	{
		iNewTile = iTileStyleOffset + 18;
	}
	else if(!iTile[1] && iTile[2] && iTile[3])
	{
		iNewTile = iTileStyleOffset + 19;
	}
	else if(iTile[1] && !iTile[4] && iTile[7])
	{
		iNewTile = iTileStyleOffset + 20;
	}
	else if(iTile[2] && !iTile[4] && iTile[6])
	{
		iNewTile = iTileStyleOffset + 21;
	}
	else if(iTile[0] && !iTile[3] && iTile[6])
	{
		iNewTile = iTileStyleOffset + 22;
	}
	else if(iTile[1] && !iTile[3] && iTile[5])
	{
		iNewTile = iTileStyleOffset + 23;
	}
	else if(iTile[0] && !iTile[1] && iTile[2])
	{
		iNewTile = iTileStyleOffset + 6;
	}
	else if(iTile[5] && !iTile[6] && iTile[7])
	{
		iNewTile = iTileStyleOffset + 7;
	}
	else if(iTile[0] && !iTile[3] && iTile[5])
	{
		iNewTile = iTileStyleOffset + 8;
	}
	else if(iTile[2] && !iTile[4] && iTile[7])
	{
		iNewTile = iTileStyleOffset + 9;
	}
	else if(iTile[1] && iTile[4])
	{
		iNewTile = iTileStyleOffset + 24;
	}
	else if(iTile[4] && iTile[6])
	{
		iNewTile = iTileStyleOffset + 25;
	}
	else if(iTile[3] && iTile[6])
	{
		iNewTile = iTileStyleOffset + 26;
	}
	else if(iTile[1] && iTile[3])
	{
		iNewTile = iTileStyleOffset + 27;
	}
	else if(iTile[2] && iTile[5])
	{
		iNewTile = iTileStyleOffset + 10;
	}
	else if(iTile[0] && iTile[7])
	{
		iNewTile = iTileStyleOffset + 11;
	}
	else if(iTile[1])
	{
		iNewTile = iTileStyleOffset + 12;
	}
	else if(iTile[6])
	{
		iNewTile = iTileStyleOffset + 13;
	}
	else if(iTile[3])
	{
		iNewTile = iTileStyleOffset + 14;
	}
	else if(iTile[4])
	{
		iNewTile = iTileStyleOffset + 15;
	}
	else if(iTile[2])
	{
		iNewTile = iTileStyleOffset + 2;
	}
	else if(iTile[0])
	{
		iNewTile = iTileStyleOffset + 3;
	}
	else if(iTile[5])
	{
		iNewTile = iTileStyleOffset + 4;
	}
	else if(iTile[7])
	{
		iNewTile = iTileStyleOffset + 5;
	}
	else
	{
		iNewTile = iTileStyleOffset + 0;
	}

	if(g_worldmap.tiles[iCol][iRow].iBackgroundSprite != iNewTile)
	{
		g_worldmap.tiles[iCol][iRow].iBackgroundSprite = iNewTile;
		return true;
	}

	return false;
}

void updateworldsurface()
{
	g_worldmap.DrawMapToSurface(-1, true, sMapSurface, draw_offset_col, draw_offset_row, 0);
}

void drawmap(bool fScreenshot, short iBlockSize)
{
	if(fNeedBlackBackground)
		SDL_FillRect(screen, NULL, 0x0);

	SDL_BlitSurface(sMapSurface, &rectSrcSurface, blitdest, &rectDstSurface);

	//g_worldmap.Draw(draw_offset_col, draw_offset_row);

	//Draw warp arrows
	/*
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
	*/
}

int editor_warp()
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
				{
					done = true;
					break;
				}

				case SDL_KEYDOWN:
				{
					edit_mode = 6;  //change to edit mode using warps
					return EDITOR_EDIT;
				}
			
				case SDL_MOUSEBUTTONDOWN:
				{
					short iButtonX = event.button.x / TILESIZE;
					short iButtonY = event.button.y / TILESIZE;

					if(event.button.button == SDL_BUTTON_LEFT)
					{
						if(iButtonX >= 0 && iButtonX <= 9 && iButtonY == 0)
						{
							set_tile = iButtonX;
						
							edit_mode = 6;  //change to edit mode using warps
						
							//The user must release the mouse button before trying to add a tile
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

		spr_warps[0].draw(0, 0, 0, 0, 320, 32);

		menu_font_small.drawRightJustified(640, 0, worldlist.current_name());

		DrawMessage();
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

int editor_start_items()
{
	bool done = false;
	
	SDL_Rect rItemDst[NUM_POWERUPS + NUM_WORLD_POWERUPS];

	SDL_Rect rPickedItemDst[32];

	short iColCount = 0;
	short iRowCount = 0;
	for(short iItem = 0; iItem < NUM_POWERUPS + NUM_WORLD_POWERUPS; iItem++)
	{
		rItemDst[iItem].x = 16 + iColCount * 48;
		rItemDst[iItem].y = 16 + iRowCount * 48;
		rItemDst[iItem].w = 32;
		rItemDst[iItem].h = 32;
		
		if(++iColCount > 12)
		{
			iColCount = 0;
			iRowCount++;
		}
	}

	short iPickedItem = 0;
	for(short iPickedItemY = 0; iPickedItemY < 4; iPickedItemY++)
	{
		for(short iPickedItemX = 0; iPickedItemX < 8; iPickedItemX++)
		{
			rPickedItemDst[iPickedItem].x = 122 + iPickedItemX * 52;  
			rPickedItemDst[iPickedItem].y = 240 + iPickedItemY * 64;
			rPickedItemDst[iPickedItem].w = 32;
			rPickedItemDst[iPickedItem].h = 32;

			iPickedItem++;
		}
	}

	while (!done)
	{
		int framestart = SDL_GetTicks();

		//handle messages
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_QUIT:
				{
					done = true;
					break;
				}

				case SDL_KEYDOWN:
				{
					return EDITOR_EDIT;
				}
			
				case SDL_MOUSEBUTTONDOWN:
				{
					short iButtonX = event.button.x;
					short iButtonY = event.button.y;

					if(event.button.button == SDL_BUTTON_LEFT)
					{
						if(g_worldmap.iNumInitialBonuses < 32)
						{
							for(short iItem = 0; iItem < NUM_POWERUPS + NUM_WORLD_POWERUPS; iItem++)
							{
								if(iButtonX >= rItemDst[iItem].x && iButtonX < rItemDst[iItem].w + rItemDst[iItem].x &&
									iButtonY >= rItemDst[iItem].y && iButtonY < rItemDst[iItem].h + rItemDst[iItem].y)
								{
									g_worldmap.iInitialBonuses[g_worldmap.iNumInitialBonuses++] = iItem;
									break;
								}
							}
						}

						for(short iRemoveItem = 0; iRemoveItem < g_worldmap.iNumInitialBonuses; iRemoveItem++)
						{
							if(iButtonX >= rPickedItemDst[iRemoveItem].x && iButtonX < rPickedItemDst[iRemoveItem].w + rPickedItemDst[iRemoveItem].x &&
								iButtonY >= rPickedItemDst[iRemoveItem].y && iButtonY < rPickedItemDst[iRemoveItem].h + rPickedItemDst[iRemoveItem].y)
							{
								for(short iAdjust = iRemoveItem; iAdjust < g_worldmap.iNumInitialBonuses - 1; iAdjust++)
								{
									g_worldmap.iInitialBonuses[iAdjust] = g_worldmap.iInitialBonuses[iAdjust + 1];
								}

								g_worldmap.iNumInitialBonuses--;

								break;
							}
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

		for(short iItem = 0; iItem < NUM_POWERUPS; iItem++)
		{
			spr_storedpoweruplarge.draw(rItemDst[iItem].x, rItemDst[iItem].y, iItem << 5, 0, 32, 32);
		}

		for(short iWorldItem = 0; iWorldItem < NUM_WORLD_POWERUPS; iWorldItem++)
		{
			spr_worlditems.draw(rItemDst[iWorldItem + NUM_POWERUPS].x, rItemDst[iWorldItem + NUM_POWERUPS].y, iWorldItem << 5, 0, 32, 32);
		}

		for(short iPopup = 0; iPopup < 4; iPopup++)
		{
			spr_worldpopup.draw(0, 416 - (iPopup << 6), 0, 0, 320, 64);
			spr_worldpopup.draw(320, 416 - (iPopup << 6), 192, 0, 320, 64);
		}

		for(short iPickedItem = 0; iPickedItem < g_worldmap.iNumInitialBonuses; iPickedItem++)
		{
			short iPowerup = g_worldmap.iInitialBonuses[iPickedItem];
			if(iPowerup >= NUM_POWERUPS)
				spr_worlditems.draw(rPickedItemDst[iPickedItem].x, rPickedItemDst[iPickedItem].y, (iPowerup - NUM_POWERUPS) << 5, 0, 32, 32);
			else
				spr_storedpoweruplarge.draw(rPickedItemDst[iPickedItem].x, rPickedItemDst[iPickedItem].y, iPowerup << 5, 0, 32, 32);
		}

		menu_font_small.drawRightJustified(640, 0, worldlist.current_name());

		DrawMessage();
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

int editor_boundary()
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
				{
					done = true;
					break;
				}

				case SDL_KEYDOWN:
				{
					edit_mode = 8;  //change to edit mode using warps
					return EDITOR_EDIT;
				}
			
				case SDL_MOUSEBUTTONDOWN:
				{
					short iButtonX = event.button.x / TILESIZE;
					short iButtonY = event.button.y / TILESIZE;

					if(event.button.button == SDL_BUTTON_LEFT)
					{
						if(iButtonX >= 0 && iButtonX <= 9 && iButtonY >= 0 && iButtonY <= 9)
						{
							set_tile = iButtonX + 10 * iButtonY + 1;
						
							edit_mode = 8;  //change to edit mode using warps
						
							//The user must release the mouse button before trying to add a tile
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

		int color = SDL_MapRGB(blitdest->format, 255, 0, 255);
		SDL_Rect r = {0, 0, 320, 320};
		SDL_FillRect(blitdest, &r, color);

		spr_worldforegroundspecial[0].draw(0, 0, 0, 0, 320, 320);
		
		menu_font_small.drawRightJustified(640, 0, worldlist.current_name());

		DrawMessage();
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

int editor_type()
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
				{
					done = true;
					break;
				}

				case SDL_KEYDOWN:
				{
					edit_mode = 3;  //change to edit mode using doors/start
					return EDITOR_EDIT;
				
					break;
				}

				case SDL_MOUSEBUTTONDOWN:
				{
					if(event.button.button == SDL_BUTTON_LEFT)
					{
						short iButtonX = event.button.x / TILESIZE;
						short iButtonY = event.button.y / TILESIZE;

						//Start and doors
						if(iButtonX >= 0 && iButtonX <= 5 && iButtonY == 0)
						{
							set_tile = iButtonX;
						}
			
						edit_mode = 3;  //change to edit mode using warps
						
						//The user must release the mouse button before trying to add a tile
						ignoreclick = true;
						
						return EDITOR_EDIT;
					}
			
					break;
				}

				default:
					break;
			}
		}

		
		drawmap(false, TILESIZE);
		menu_shade.draw(0, 0);

		spr_worldforegroundspecial[0].draw(0, 0, 320, 128, 64, 32);
		spr_worldforegroundspecial[0].draw(64, 0, 320, 192, 128, 32);

		spr_worldforegroundspecial[0].draw(64, 0, 448, 64, 128, 32);
		
		menu_font_small.drawRightJustified(640, 0, worldlist.current_name());

		DrawMessage();
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

int editor_water()
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
				{
					done = true;
					break;
				}

				case SDL_KEYDOWN:
				{	
					edit_mode = 7;
					return EDITOR_EDIT;
				}

				case SDL_MOUSEBUTTONDOWN:
				{
					if(event.button.button == SDL_BUTTON_LEFT)
					{
						short iButtonX = event.button.x / TILESIZE;
						short iButtonY = event.button.y / TILESIZE;

						if(iButtonY == 0)
						{
							if(iButtonX >= 0 && iButtonX <= 2)
							{
								set_tile = iButtonX + 4;
							}
						}
						
						ignoreclick = true;
						edit_mode = 7;
						return EDITOR_EDIT;
					}
					
					break;
				}

				default:
					break;
			}
		}

		SDL_FillRect(screen, NULL, 0x0);
		
		for(short iWater = 0; iWater < 3; iWater++)
			spr_worldbackground[0].draw(iWater << 5, 0, 512 + (iWater << 7), 0, 32, 32);
		
		DrawMessage();
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

int editor_background()
{
	bool done = false;
	short iPage = 0;

	while (!done)
	{
		int framestart = SDL_GetTicks();

		//handle messages
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_QUIT:
				{
					done = true;
					break;
				}

				case SDL_KEYDOWN:
				{	
					SDLKey key = event.key.keysym.sym;
					if(key >= SDLK_1 && key <= SDLK_2)
					{
						iPage = key - SDLK_1;
					}
					else
					{
						edit_mode = 0;
						return EDITOR_EDIT;
					}

					break;
				}

				case SDL_MOUSEBUTTONDOWN:
				{
					if(event.button.button == SDL_BUTTON_LEFT)
					{
						short iButtonX = event.button.x / TILESIZE;
						short iButtonY = event.button.y / TILESIZE;

						short iTileStyleOffset = ((iButtonX / 4) + (iPage * 5)) * WORLD_BACKGROUND_SPRITE_SET_SIZE;

						iButtonX %= 4;

						if(iButtonX == 0)
						{
							if(iButtonY == 0)
								set_tile = 0;
							else if(iButtonY >= 1 && iButtonY < 15)
								set_tile = iButtonY + 1;
						}
						else if(iButtonX == 1)
						{
							if(iButtonY == 0)
								set_tile = 1;
							else if(iButtonY >= 1 && iButtonY < 15)
								set_tile = iButtonY + 15;
						}
						else if(iButtonX == 2)
						{
							if(iButtonY >= 0 && iButtonY < 15)
								set_tile = iButtonY + 30;
						}
						else if(iButtonX == 3)
						{
							if(iButtonY >= 0 && iButtonY < 15)
								set_tile = iButtonY + 45;
						}
						
						set_tile += iTileStyleOffset;

						ignoreclick = true;
						edit_mode = 0;
						return EDITOR_EDIT;
					}
					
					break;
				}

				default:
					break;
			}
		}

		SDL_FillRect(screen, NULL, 0x0);

		spr_worldbackground[0].draw(0, 0, iPage * 640, 32, 640, 480);

		DrawMessage();
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

int editor_foreground()
{
	bool done = false;
	short iForegroundScreen = 0;
	
	while (!done)
	{
		int framestart = SDL_GetTicks();

		//handle messages
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_QUIT:
				{
					done = true;
					break;
				}

				case SDL_KEYDOWN:
				{	
					SDLKey key = event.key.keysym.sym;

					if(key >= SDLK_1 && key <= SDLK_6)
					{
						iForegroundScreen = key - SDLK_1;
					}
					else
					{
						edit_mode = 1;
						return EDITOR_EDIT;
					}

					break;
				}

				case SDL_MOUSEBUTTONDOWN:
				{
					if(event.button.button == SDL_BUTTON_LEFT)
					{
						short iButtonX = event.button.x / TILESIZE;
						short iButtonY = event.button.y / TILESIZE;

						if(iForegroundScreen < 4)
						{
							if(iButtonX >= 0 && iButtonX < 10)
							{
								if(iButtonY >= 0 && iButtonY < 10)
									set_tile = WORLD_FOREGROUND_STAGE_OFFSET + iButtonY * 10 + iButtonX + iForegroundScreen * 100;
							}
						}
						else if(iForegroundScreen == 4)
						{
							if(iButtonX >= 0 && iButtonX < 4)
							{
								if(iButtonY >= 0 && iButtonY < 1)
									set_tile = WORLD_BRIDGE_SPRITE_OFFSET + iButtonX;
							}
						}
						else if(iForegroundScreen == 5)
						{
							if(iButtonY >= 0 && iButtonY < 15)
							{
								if(iButtonX >= 0 && iButtonX < 12)
								{
									set_tile = WORLD_FOREGROUND_SPRITE_OFFSET + iButtonX + iButtonY * 12;
								}
								else if(iButtonX >= 12 && iButtonX < 14)
								{
									set_tile = WORLD_FOREGROUND_SPRITE_ANIMATED_OFFSET + iButtonY + (iButtonX - 12) * 15;
								}
							}
						}

						ignoreclick = true;
						edit_mode = 1;
						return EDITOR_EDIT;
					}
					
					break;
				}

				default:
					break;
			}
		}

		SDL_FillRect(screen, NULL, 0x0);

		if(iForegroundScreen < 4)
		{
			for(short iRow = 0; iRow < 10; iRow++)
			{
				for(short iCol = 0; iCol < 10; iCol++)
				{
					spr_worldforegroundspecial[0].draw(iCol << 5, iRow << 5, 384, iForegroundScreen << 5, 32, 32);
				}
			}

			spr_worldforegroundspecial[0].draw(0, 0, 0, 0, 320, 320);
		}
		else if(iForegroundScreen == 4)
		{
			spr_worldforegroundspecial[0].draw(0, 0, 320, 224, 128, 32);
		}
		else if(iForegroundScreen == 5)
		{
			spr_worldforeground[0].draw(0, 0, 0, 0, 416, 480);
			spr_worldforeground[0].draw(416, 0, 512, 0, 32, 480);
		}

		DrawMessage();
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

int editor_pathsprite()
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
				{
					done = true;
					break;
				}

				case SDL_KEYDOWN:
				{	
					edit_mode = 4;
					return EDITOR_EDIT;
				}

				case SDL_MOUSEBUTTONDOWN:
				{
					if(event.button.button == SDL_BUTTON_LEFT)
					{
						short iButtonX = event.button.x / TILESIZE;
						short iButtonY = event.button.y / TILESIZE;

						if(iButtonX >= 0 && iButtonX < 8)
						{
							if(iButtonY >= 0 && iButtonY < 6)
								set_tile = iButtonY + 1 + iButtonX * WORLD_PATH_SPRITE_SET_SIZE;
						}

						ignoreclick = true;
						edit_mode = 4;
						return EDITOR_EDIT;
					}
					
					break;
				}

				default:
					break;
			}
		}

		SDL_FillRect(screen, NULL, 0x0);

		for(short iPath = 0; iPath < 8; iPath++)
		{
			spr_worldpaths[0].draw(iPath << 5, 0, (iPath % 4) * 160, (iPath / 4) * 320, 32, 192);
		}

		DrawMessage();
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

int editor_vehicles()
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
				{
					done = true;
					break;
				}

				case SDL_KEYDOWN:
				{
					edit_mode = 5;
					return EDITOR_EDIT;
				}
				
				case SDL_MOUSEBUTTONDOWN:
				{
					if(event.button.button == SDL_BUTTON_LEFT)
					{
						short set_item_x = event.button.x / TILESIZE;
						short set_item_y = event.button.y / TILESIZE;

						//Set the selected block to one of the interaction blocks
						if(set_item_x >= 0 && set_item_x <= 8 && set_item_y == 0)
						{
							set_tile = set_item_x;

							edit_mode = 5;

							//The user must release the mouse button before trying to add a tile
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
		
		for(short iVehicle = 0; iVehicle < 9; iVehicle++)
		{
			spr_worldvehicle[0].draw(iVehicle << 5, 0, 0, iVehicle << 5, 32, 32);
		}

		menu_font_small.drawRightJustified(640, 0, worldlist.current_name());
				
		DrawMessage();
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

int editor_path()
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
				{
					done = true;
					break;
				}

				case SDL_KEYDOWN:
				{	
					edit_mode = 2;
					return EDITOR_EDIT;
				}

				case SDL_MOUSEBUTTONDOWN:
				{
					if(event.button.button == SDL_BUTTON_LEFT)
					{
						short iButtonX = event.button.x / TILESIZE;
						short iButtonY = event.button.y / TILESIZE;

						if(iButtonX >= 0 && iButtonX <= 15 && iButtonY == 0)
						{
							set_tile = iButtonX + 1;

							ignoreclick = true;
							edit_mode = 2;
							return EDITOR_EDIT;
						}
					}
					
					break;
				}

				default:
					break;
			}
		}

		SDL_FillRect(screen, NULL, 0x0);
		spr_path.draw(0, 0, 0, 0, 480, 32);

		DrawMessage();
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

void DisplayModeDetails(short iModeId, short iMouseX, short iMouseY)
{
	menu_mode_large.draw(iMouseX, iMouseY, iModeId << 5, 0, 32, 32);
}

int editor_stage()
{
	bool done = false;
	int iModeDisplay = -1;
	int iEditStage = 0;
	
	while (!done)
	{
		int framestart = SDL_GetTicks();

		//handle messages
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_QUIT:
				{
					done = true;
					break;
				}

				case SDL_KEYDOWN:
				{
					edit_mode = 9;  //change to edit mode using stages
					return EDITOR_EDIT;
				
					break;
				}

				case SDL_MOUSEBUTTONDOWN:
				{
					short iButtonX = event.button.x / TILESIZE;
					short iButtonY = event.button.y / TILESIZE;

					if(event.button.button == SDL_BUTTON_LEFT)
					{
						//Stages
						if(iButtonX >= 0 && iButtonX < g_worldmap.iNumStages - (iButtonY * 20) && iButtonY >= 0 && iButtonY <= (g_worldmap.iNumStages - 1) / 20)
						{
							set_tile = iButtonX + (iButtonY * 20) + 6;

							edit_mode = 9;  //change to edit mode using warps
						
							//The user must release the mouse button before trying to add a tile
							ignoreclick = true;
							
							return EDITOR_EDIT;
						}
						//New stage button
						else if(iButtonX >= 200 && iButtonX < 300 && iButtonY >= 20 && iButtonY < 30)
						{
							
						}
					}
					else if(event.button.button == SDL_BUTTON_RIGHT)
					{
						iEditStage = iButtonX + (iButtonY * 20);


					}
			
					break;
				}

				case SDL_MOUSEMOTION:
				{
					iModeDisplay = -1;

					short iMouseX = event.button.x / TILESIZE;
					short iMouseY = event.button.y / TILESIZE;
					
					if(iMouseX >= 0 && iMouseX < g_worldmap.iNumStages - (iMouseY * 20) && iMouseY >= 0 && iMouseY <= (g_worldmap.iNumStages - 1) / 20)
					{
						iModeDisplay = game_values.tourstops[iMouseX + (iMouseY * 20)]->iMode;
					}
				}

				default:
					break;
			}
		}

		
		drawmap(false, TILESIZE);
		menu_shade.draw(0, 0);

		int color = SDL_MapRGB(blitdest->format, 0, 0, 255);
		for(short iStage = 0; iStage < g_worldmap.iNumStages; iStage++)
		{
			short ix = (iStage % 20) << 5;
			short iy = ((iStage / 20) << 5);

			SDL_Rect r = {ix, iy, 32, 32};
			SDL_FillRect(blitdest, &r, color);

			spr_worldforegroundspecial[0].draw(ix, iy, (iStage % 10) << 5, (iStage / 10) << 5, 32, 32);
		}
		
		menu_font_small.drawRightJustified(640, 0, worldlist.current_name());

		if(iModeDisplay >= 0)
		{
			DisplayModeDetails(iModeDisplay, event.button.x, event.button.y);
		}

		DrawMessage();
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



int display_help()
{
	//unsigned int currentChar = 0;

	drawmap(false, TILESIZE);
	menu_shade.draw(0, 0);
	menu_font_large.drawCentered(320, 15, "Help");
	
	int offsety = 55;
	int offsetx = 30;
	menu_font_small.draw(offsetx, offsety, "Modes:");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "[1] - Water Mode");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "[2] - Land Mode");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "[3] - Objects Mode");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "[4] - Path Mode");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "[p] - Connection Mode");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "[w] - Warp Mode");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "[v] - Vehicle");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "[t] - Stages and Doors");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "[b] - Vehicle Boundaries");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "[i] - Initial Powerups");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "[e] - Edit Stages");
	offsety += menu_font_small.getHeight() + 20;

	menu_font_small.draw(offsetx, offsety, "File:");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "[n] - New World");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "[s] - Save World");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "[shift] + [s] - Save As");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "[f] - Find World");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "[shift] + [f] - New Search");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "[pageup] - Go To Previous World");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "[pagedown] - Go To Next World");

	offsetx = 320;
	offsety = 55;

	menu_font_small.draw(offsetx, offsety, "Place Tiles:");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "[Left Mouse Button] - Place Item");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "[Right Mouse Button] - Remove Item");
	offsety += menu_font_small.getHeight() + 20;

	/*
	menu_font_small.draw(offsetx, offsety, "Move Mode:");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "[Right Mouse Button] - Select Area");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "[Left Mouse Button] - Unselect Area");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "Select And Drag - Move Selections");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "Hold [shift] - Multiple Selections");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "Hold [ctrl] - Freehand Selections");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "[delete] - Delete Selection");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "[c] - Copy Selection");
	offsety += menu_font_small.getHeight() + 20;
	*/

	menu_font_small.draw(offsetx, offsety, "Miscellaneous:");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "[r] - Change Music Category");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "[Arrow Keys] - Navigate World");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "[a] - Automatic Path/Land");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "[ctrl] + [delete] - Clear All");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "[insert] - Screenshot");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "[alt] + [enter] - Full Screen/Window");
	

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
	char mapLocation[FILEBUFSIZE] = "worlds/";

	if(dialog("Save As", "Enter name:", fileName, 64))
	{
		worldlist.add(strcat(mapLocation, strcat(fileName, ".txt")));
		worldlist.find(fileName);
		game_values.worldindex = worldlist.GetCurrentIndex();
		savecurrentworld();
		loadcurrentworld();
	}

	return 0;
}

bool dialog(const char * title, const char * instructions, char * input, int inputsize)
{	
	unsigned int currentChar = 0;

	drawmap(false, TILESIZE);
	menu_shade.draw(0, 0);
	spr_dialog.draw(224, 176, 0, 0, 192, 128);
	menu_font_large.drawCentered(320, 200, title);
	menu_font_small.draw(240, 235, instructions);
	menu_font_small.drawRightJustified(640, 0, worldlist.current_name());
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
							spr_dialog.draw(224, 176, 0, 0, 192, 128);
							menu_font_large.drawCentered(320, 200, title);
							menu_font_small.draw(240, 235, instructions);
							menu_font_small.draw(240, 255, input);
							menu_font_small.drawRightJustified(640, 0, worldlist.current_name());
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
							spr_dialog.draw(224, 176, 0, 0, 192, 128);
							menu_font_large.drawCentered(320, 200, title);
							menu_font_small.draw(240, 235, instructions);
							menu_font_small.draw(240, 255, input);
							menu_font_small.drawRightJustified(640, 0, worldlist.current_name());
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

		if(worldlist.find(findstring))
		{
			game_values.worldindex = worldlist.GetCurrentIndex();
			loadcurrentworld();
		}
	}

	return 0;
}

int clear_world()
{ 
	g_worldmap.Clear();
	updateworldsurface();

	printf("World Cleared\n");
	return 0;
}

void loadcurrentworld()
{
	g_worldmap.Load(TILESIZE);
	ReadVehiclesIntoEditor();
	ReadWarpsIntoEditor();

	draw_offset_col = 0;
	draw_offset_row = 0;

	rectSrcSurface.x = 0;
	rectSrcSurface.y = 0;
	fNeedBlackBackground = false;

	g_worldmap.GetWorldSize(&iWorldWidth, &iWorldHeight);

	if(iWorldWidth >= 20)
	{
		rectSrcSurface.w = 640;
		draw_offset_x = 0;
	}
	else
	{
		rectSrcSurface.w = iWorldWidth * TILESIZE;
		draw_offset_x = (640 - iWorldWidth * TILESIZE) >> 1;
		fNeedBlackBackground = true;
	}

	if(iWorldHeight >= 15)
	{
		rectSrcSurface.h = 480;
		draw_offset_y = 0;
	}
	else
	{
		rectSrcSurface.h = iWorldHeight * TILESIZE;
		draw_offset_y = (480 - iWorldHeight * TILESIZE) >> 1;
		fNeedBlackBackground = true;
	}

	rectDstSurface.x = draw_offset_x;
	rectDstSurface.y = draw_offset_y;

	updateworldsurface();
}

int savecurrentworld()
{
	g_messagedisplaytimer = 60;
	g_szMessageTitle = "Saved";
	g_szMessageLine[0] = "Your world has";
	g_szMessageLine[1] = "been saved.";
	g_szMessageLine[2] = "";

	WriteVehiclesIntoWorld();
	WriteWarpsIntoWorld();
	g_worldmap.Save();
	return 0;
}

int findcurrentstring()
{
	if(findstring[0] != '\0')
	{
		if(worldlist.find(findstring))
		{
			game_values.worldindex = worldlist.GetCurrentIndex();
			loadcurrentworld();
		}
	}

	return 0;
}

int new_world()
{
	char fileName[FILEBUFSIZE] = "";
	char worldLocation[FILEBUFSIZE] = "worlds/";
	char szWidth[5], szHeight[5];

	if(dialog("New World", "Enter name:", fileName, 64) && 
		dialog("New World", "Width:", szWidth, 4) && 
		dialog("New World", "Height:", szHeight, 4))
	{
		short iWidth = atoi(szWidth);
		short iHeight = atoi(szHeight);

		if(iWidth < 1)
			iWidth = 1;

		if(iHeight < 1)
			iHeight = 1;

		std::vector<WorldVehicle*>::iterator itrVehicle = vehiclelist.begin(), limVehicle = vehiclelist.end();
		while(itrVehicle != limVehicle)
		{
			delete (*itrVehicle);
			itrVehicle++;
		}

		vehiclelist.clear();

		std::vector<WorldWarp*>::iterator itrWarp = warplist.begin(), limWarp = warplist.end();
		while(itrWarp != limWarp)
		{
			delete (*itrWarp);
			itrWarp++;
		}

		warplist.clear();

		g_worldmap.New(iWidth, iHeight);
		worldlist.add(strcat(worldLocation, strcat(fileName, ".txt")));
		worldlist.find(fileName);
		game_values.worldindex = worldlist.GetCurrentIndex();
		savecurrentworld();
		loadcurrentworld();
	}

	return 0;
}


int resize_world()
{
	char szWidth[5], szHeight[5];

	if(dialog("Resize World", "Width:", szWidth, 4) && 
		dialog("Resize World", "Height:", szHeight, 4))
	{
		short iWidth = atoi(szWidth);
		short iHeight = atoi(szHeight);

		if(iWidth < 1)
			iWidth = 1;

		if(iHeight < 1)
			iHeight = 1;

		std::vector<WorldVehicle*>::iterator itrVehicle = vehiclelist.begin(), limVehicle = vehiclelist.end();
		while(itrVehicle != limVehicle)
		{
			if((*itrVehicle)->iCurrentTileX >= iWidth || (*itrVehicle)->iCurrentTileY >= iHeight)
				RemoveVehicleFromTile((*itrVehicle)->iCurrentTileX, (*itrVehicle)->iCurrentTileY);

			itrVehicle++;
		}

		std::vector<WorldWarp*>::iterator itrWarp = warplist.begin(), limWarp = warplist.end();
		while(itrWarp != limWarp)
		{
			delete (*itrWarp);
				
			itrWarp = warplist.erase(itrWarp);
			limWarp = warplist.end();

			itrWarp++;
		}

		g_worldmap.Resize(iWidth, iHeight);

		savecurrentworld();
		loadcurrentworld();
	}

	return 0;
}

//take screenshots in full and thumbnail sizes
void takescreenshot()
{
	short iTileSizes[3] = {32, 16, 8};

	for(short iScreenshotSize = 0; iScreenshotSize < 3; iScreenshotSize++)
	{
		short iTileSize = iTileSizes[iScreenshotSize];
		g_worldmap.Load(iTileSize);

		short w, h;
		g_worldmap.GetWorldSize(&w, &h);

		//Draw most of the world to screenshot
		SDL_Surface * sScreenshot = SDL_CreateRGBSurface(screen->flags, iTileSize * w, iTileSize * h, screen->format->BitsPerPixel, 0, 0, 0, 0);
		blitdest = sScreenshot;

		g_worldmap.DrawMapToSurface(sScreenshot); 

		//Draw vehicles to screenshot
		std::vector<WorldVehicle*>::iterator itr = vehiclelist.begin(), lim = vehiclelist.end();
		while(itr != lim)
		{
			WorldVehicle * vehicle = *itr;

			short ix = vehicle->iCurrentTileX * iTileSize;
			short iy = vehicle->iCurrentTileY * iTileSize;
			
			spr_worldvehicle[iScreenshotSize].draw(ix, iy, vehicle->iDrawDirection * iTileSize, vehicle->iDrawSprite * iTileSize, iTileSize, iTileSize);

			itr++;
		}

		//Draw warps to screenshot
		std::vector<WorldWarp*>::iterator itrWarp = warplist.begin(), limWarp = warplist.end();
		while(itrWarp != limWarp)
		{
			WorldWarp * warp = *itrWarp;

			if(warp->iCol1 >= 0)
				spr_warps[iScreenshotSize].draw(warp->iCol1 * iTileSize, warp->iRow1 * iTileSize, warp->iID * iTileSize, 0, iTileSize, iTileSize);

			if(warp->iCol2 >= 0)
				spr_warps[iScreenshotSize].draw(warp->iCol2 * iTileSize, warp->iRow2 * iTileSize, warp->iID * iTileSize, 0, iTileSize, iTileSize);

			itrWarp++;
		}

		//Save the screenshot with the same name as the map file
		char szSaveFile[256];
		strcpy(szSaveFile, "worlds/screenshots/");
		char * pszSaveFile = szSaveFile + strlen(szSaveFile);
		GetNameFromFileName(pszSaveFile, worldlist.current_name());
		
		if(iTileSize == PREVIEWTILESIZE)
			strcat(szSaveFile, "_preview");
		else if(iTileSize == THUMBTILESIZE)
			strcat(szSaveFile, "_thumb");

#ifdef PNG_SAVE_FORMAT
		strcat(szSaveFile, ".png");
		IMG_SavePNG(sScreenshot, szSaveFile);
#else
		strcat(szSaveFile, ".bmp");
		SDL_SaveBMP(sScreenshot, szSaveFile);
#endif

		SDL_FreeSurface(sScreenshot);
	}

	g_worldmap.Load(iTileSizes[0]);
	blitdest = screen;
}



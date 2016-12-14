/*----------------------------------------------------------+
| Super Mario War Screenshot Maker							|
|                                                           |
| Quick and dirty way to make map screenshots via           |
| command line                                              |
|                                                           |
| this sourcecode is released under the GPL.				|
|															|
|				(C) 2007 Florian Hufsky <fhufsky@phorus.at>	|
+----------------------------------------------------------*/

// TODO: Update this project.

#define SMW_EDITOR
//#include "global.h"

#ifdef PNG_SAVE_FORMAT
	// this function was added to SDL2
	#ifndef USE_SDL2
	    #include "savepng.h"
	#endif
#endif

#include <string.h>
#include <ctype.h>

#ifdef _WIN32
	#include <windows.h>
#endif

#define MAPTITLESTRING "SMW 1.7 Screenshot Maker"
#define MAX_PLATFORMS 8

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

gfxSprite		spr_background;

gfxSprite		spr_platformarrows[3];
gfxSprite		spr_warps[3];

CMap			g_map;

//// Global stuff that the map editor doesn't need, but has references to
SDL_Surface		*screen;
SDL_Surface		*blitdest;

//GraphicsList menugraphicspacklist;
GraphicsList gamegraphicspacklist;
gfxSprite		spr_warplock;
short			x_shake = 0;
short			y_shake = 0;
gv				game_values;
void CPlayer::flipsidesifneeded() {}
void CPlayer::KillPlayerMapHazard() {}
void IO_MovingObject::flipsidesifneeded() {}
void IO_MovingObject::KillObjectMapHazard() {}
void removeifprojectile(IO_MovingObject * object, bool playsound, bool forcedead) {}
///////

void drawmap(bool fScreenshot, short iBlockSize);

void loadmap(char * szMapFile);
void takescreenshot();

short g_iNumPlatforms = 0;
MapPlatform g_Platforms[MAX_PLATFORMS];

BackgroundList backgroundlist;

char * szMapName = NULL;

//main main main
int main(int argc, char *argv[])
{
    if (argc != 2) {
		printf("Usage: screenshot mapfile.map\n");
		exit(0);
	}

    /* This must occur before any data files are loaded */
    Initialize_Paths();

	printf("-------------------------------------------------------------------------------\n");
	printf(" %s\n", MAPTITLESTRING);
	printf("-------------------------------------------------------------------------------\n");
	printf("\n---------------- startup ----------------\n");

	gfx_init(640, 480, false);
	blitdest = screen;

	spr_warps[0].init(convertPath("gfx/leveleditor/leveleditor_warp.png"), 255, 0, 255);
	spr_warps[1].init(convertPath("gfx/leveleditor/leveleditor_warp_preview.png"), 255, 0, 255);
	spr_warps[2].init(convertPath("gfx/leveleditor/leveleditor_warp_thumbnail.png"), 255, 0, 255);

	spr_platformarrows[0].init(convertPath("gfx/leveleditor/leveleditor_platform_arrows.png"), 255, 0, 255, 128);
	spr_platformarrows[1].init(convertPath("gfx/leveleditor/leveleditor_platform_arrows_preview.png"), 255, 0, 255, 128);
	spr_platformarrows[2].init(convertPath("gfx/leveleditor/leveleditor_platform_arrows_thumbnail.png"), 255, 0, 255, 128);

	printf("\n---------------- load map ----------------\n");

	std::string tileSetPNG[3];
	tileSetPNG[0] = convertPath("gfx/packs/Classic/tileset.png");
	tileSetPNG[1] = convertPath("gfx/packs/Classic/tileset_medium.png");
	tileSetPNG[2] = convertPath("gfx/packs/Classic/tileset_small.png");
	g_map->loadTileSet(convertPath("maps/tileset.tls"), tileSetPNG);

	//Setup Platforms
    for (short iPlatform = 0; iPlatform < MAX_PLATFORMS; iPlatform++) {
		g_Platforms[iPlatform].rIcon[0].x = (iPlatform % 6) * 32;
		g_Platforms[iPlatform].rIcon[0].y = (iPlatform / 6) * 32 + 224;
		g_Platforms[iPlatform].rIcon[0].w = 32;
		g_Platforms[iPlatform].rIcon[0].h = 32;

		g_Platforms[iPlatform].rIcon[1].x = (iPlatform % 4) * 42 + 240;
		g_Platforms[iPlatform].rIcon[1].y = (iPlatform / 4) * 42 + 174;
		g_Platforms[iPlatform].rIcon[1].w = 32;
		g_Platforms[iPlatform].rIcon[1].h = 32;

        for (short iCol = 0; iCol < MAPWIDTH; iCol++) {
            for (short iRow = 0; iRow < MAPHEIGHT; iRow++) {
				g_Platforms[iPlatform].tiles[iCol][iRow] = TILESETSIZE;
			}
		}

		g_Platforms[iPlatform].iVelocity = 4;
	}

	szMapName = argv[1];
	loadmap(szMapName);
	takescreenshot();

	return 0;
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
    for (i = 0; i < MAPWIDTH; i++) {
		bltrect.y = -iBlockSize;	//this is okay, see

        for (j = 0; j < MAPHEIGHT; j++) {
			bltrect.y += iBlockSize;	// here

			ts = g_map->mapdata[i][j][layer];

			if (ts == TILESETSIZE)
				continue;

			tilebltrect.x = (ts % TILESETWIDTH) * iBlockSize;
			tilebltrect.y = (ts / TILESETWIDTH) * iBlockSize;

			SDL_BlitSurface(g_map->tilesetsurface[iBlockSize == TILESIZE ? 0 : iBlockSize == PREVIEWTILESIZE ? 1 : 2], &tilebltrect, screen, &bltrect);
		}

		bltrect.x += iBlockSize;
	}
}

void drawmap(bool fScreenshot, short iBlockSize)
{
    if (iBlockSize != TILESIZE) {
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

        if (SDL_SCALEBLIT(spr_background.getSurface(), &srcrect, blitdest, &dstrect) < 0) {
			fprintf(stderr, "SDL_SCALEBLIT error: %s\n", SDL_GetError());
			return;
		}
    } else {
		spr_background.draw(0, 0);
	}

	drawlayer(0, false, iBlockSize);
	drawlayer(1, false, iBlockSize);


	SDL_Rect rSrc = {0, 0, iBlockSize, iBlockSize};
	SDL_Rect rDst = {0, 0, iBlockSize, iBlockSize};

    for (int j = 0; j < MAPHEIGHT; j++) {
        for (int i = 0; i < MAPWIDTH; i++) {
			int displayblock = displayblock = g_map->objectdata[i][j];

            if (displayblock != BLOCKSETSIZE) {
				rSrc.x = displayblock * iBlockSize;
				rSrc.y = iBlockSize * 30;

				rDst.x = i * iBlockSize;
				rDst.y = j * iBlockSize;

				if (displayblock >= 7 && displayblock <= 14)
					rSrc.y = iBlockSize * (g_map->iSwitches[(displayblock - 7) % 4] + 30);

				SDL_BlitSurface(g_map->tilesetsurface[iBlockSize == TILESIZE ? 0 : iBlockSize == PREVIEWTILESIZE ? 1 : 2], &rSrc, screen, &rDst);
			}
		}
	}

	drawlayer(2, false, iBlockSize);
	drawlayer(3, false, iBlockSize);

    for (int j = 0; j < MAPHEIGHT; j++) {
        for (int i = 0; i < MAPWIDTH; i++) {
			Warp * warp = &g_map->warpdata[i][j];

            if (warp->connection != -1) {
				SDL_Rect rSrc = {warp->connection * iBlockSize, warp->direction * iBlockSize, iBlockSize, iBlockSize};
				SDL_Rect rDst = {i * iBlockSize, j * iBlockSize, iBlockSize, iBlockSize};

				SDL_BlitSurface(spr_warps[iBlockSize == TILESIZE ? 0 : iBlockSize == PREVIEWTILESIZE ? 1 : 2].getSurface(), &rSrc, screen, &rDst);
			}
		}
	}
}


void loadmap(char * szMapFile)
{
	g_map->loadMap(szMapFile, read_type_full);

	char filename[128];
	sprintf(filename, "gfx/packs/Classic/backgrounds/%s", g_map->szBackgroundFile);
	std::string path = convertPath(filename);
	backgroundlist.SetCurrentName(filename);

    if (!File_Exists(path)) {
		path = convertPath("gfx/packs/Classic/backgrounds/Land_Classic.png");
		backgroundlist.SetCurrentName("gfx/packs/Classic/backgrounds/Land_Classic.png");
	}

	spr_background.init(path);

	g_iNumPlatforms = g_map->iNumPlatforms;

    for (short iPlatform = 0; iPlatform < g_iNumPlatforms; iPlatform++) {
        for (short iCol = 0; iCol < MAPWIDTH; iCol++) {
            for (short iRow = 0; iRow < MAPHEIGHT; iRow++) {
                if (iCol < g_map->platforms[iPlatform]->iTileWidth && iRow < g_map->platforms[iPlatform]->iTileHeight) {
					g_Platforms[iPlatform].tiles[iCol][iRow] = g_map->platforms[iPlatform]->iTileData[iCol][iRow];
                } else {
					g_Platforms[iPlatform].tiles[iCol][iRow] = TILESETSIZE;
				}
			}
		}

		g_Platforms[iPlatform].iVelocity = (int)(g_map->platforms[iPlatform]->pPath->fVelocity * 4.0f);
		g_Platforms[iPlatform].iStartX = (int)(g_map->platforms[iPlatform]->pPath->fStartX - g_map->platforms[iPlatform]->iHalfWidth + 1) / TILESIZE;
		g_Platforms[iPlatform].iStartY = (int)(g_map->platforms[iPlatform]->pPath->fStartY - g_map->platforms[iPlatform]->iHalfHeight + 1) / TILESIZE;
		g_Platforms[iPlatform].iEndX = (int)(g_map->platforms[iPlatform]->pPath->fEndX - g_map->platforms[iPlatform]->iHalfWidth + 1) / TILESIZE;
		g_Platforms[iPlatform].iEndY = (int)(g_map->platforms[iPlatform]->pPath->fEndY - g_map->platforms[iPlatform]->iHalfHeight + 1) / TILESIZE;
	}
}


//take screenshots in full and thumbnail sizes
void takescreenshot()
{
	short iTileSizes[3] = {32, 16, 8};
	SDL_Surface * old_screen = screen;

    for (short iScreenshotSize = 0; iScreenshotSize < 3; iScreenshotSize++) {
		short iTileSize = iTileSizes[iScreenshotSize];

		SDL_Surface * screenshot = SDL_CreateRGBSurface(old_screen->flags, iTileSize * 20, iTileSize * 15, old_screen->format->BitsPerPixel, 0, 0, 0, 0);
		blitdest = screenshot;
		screen = screenshot;
		drawmap(true, iTileSize);

		//Draw platforms to screenshot
		SDL_Rect rSrc = {0, 0, iTileSize, iTileSize};
		SDL_Rect rDst = {0, 0, iTileSize, iTileSize};

        for (short iPlatform = 0; iPlatform < g_iNumPlatforms; iPlatform++) {
            for (short iPlatformX = 0; iPlatformX < g_map->platforms[iPlatform]->iTileWidth; iPlatformX++) {
                for (short iPlatformY = 0; iPlatformY < g_map->platforms[iPlatform]->iTileHeight; iPlatformY++) {
					short iTile = g_Platforms[iPlatform].tiles[iPlatformX][iPlatformY];

                    if (iTile != TILESETSIZE) {
						rSrc.x = iTile % TILESETWIDTH * iTileSize;
						rSrc.y = iTile / TILESETWIDTH * iTileSize;

						rDst.x = (g_Platforms[iPlatform].iStartX + iPlatformX) * iTileSize;
						rDst.y = (g_Platforms[iPlatform].iStartY + iPlatformY) * iTileSize;

						SDL_BlitSurface(g_map->tilesetsurface[iScreenshotSize], &rSrc, blitdest, &rDst);
					}
				}
			}
		}

		//And add platform paths
        for (short iPlatform = 0; iPlatform < g_iNumPlatforms; iPlatform++) {
            if (g_Platforms[iPlatform].iStartX != g_Platforms[iPlatform].iEndX) {
				short iCenterOffsetY = (g_map->platforms[iPlatform]->iHeight >> 1) - 16;
				iCenterOffsetY >>= iScreenshotSize; //Resize for preview and thumbnails

				bool fMoveToRight = g_Platforms[iPlatform].iStartX < g_Platforms[iPlatform].iEndX;

				short iSpotLeft = (fMoveToRight ? g_Platforms[iPlatform].iStartX : g_Platforms[iPlatform].iEndX) + 1;
				short iSpotRight = (fMoveToRight ? g_Platforms[iPlatform].iEndX : g_Platforms[iPlatform].iStartX) - 2 + g_map->platforms[iPlatform]->iTileWidth;

				for (short iSpot = iSpotLeft; iSpot <= iSpotRight; iSpot++)
					spr_platformarrows[iScreenshotSize].draw(iSpot * iTileSize, g_Platforms[iPlatform].iStartY * iTileSize + iCenterOffsetY, iTileSize * 5, 0, iTileSize, iTileSize);

				spr_platformarrows[iScreenshotSize].draw((iSpotLeft - 1) * iTileSize, g_Platforms[iPlatform].iStartY * iTileSize + iCenterOffsetY, iTileSize * 2, 0, iTileSize, iTileSize);
				spr_platformarrows[iScreenshotSize].draw((iSpotRight + 1) * iTileSize, g_Platforms[iPlatform].iStartY * iTileSize + iCenterOffsetY, iTileSize * 3, 0, iTileSize, iTileSize);
            } else {
				short iCenterOffsetX = (g_map->platforms[iPlatform]->iWidth >> 1) - 16;
				iCenterOffsetX >>= iScreenshotSize; //Resize for preview and thumbnails

				bool fMoveUp = g_Platforms[iPlatform].iStartY < g_Platforms[iPlatform].iEndY;

				short iSpotTop = (fMoveUp ? g_Platforms[iPlatform].iStartY : g_Platforms[iPlatform].iEndY) + 1;
				short iSpotBottom = (fMoveUp ? g_Platforms[iPlatform].iEndY : g_Platforms[iPlatform].iStartY) - 2 + g_map->platforms[iPlatform]->iTileHeight;

				for (short iSpot = iSpotTop; iSpot <= iSpotBottom; iSpot++)
					spr_platformarrows[iScreenshotSize].draw(g_Platforms[iPlatform].iStartX * iTileSize + iCenterOffsetX, iSpot * iTileSize, iTileSize * 4, 0, iTileSize, iTileSize);

				spr_platformarrows[iScreenshotSize].draw(g_Platforms[iPlatform].iStartX * iTileSize + iCenterOffsetX, (iSpotTop - 1) * iTileSize, 0, 0, iTileSize, iTileSize);
				spr_platformarrows[iScreenshotSize].draw(g_Platforms[iPlatform].iStartX * iTileSize + iCenterOffsetX, (iSpotBottom + 1) * iTileSize, iTileSize, 0, iTileSize, iTileSize);
			}
		}

		//Save the screenshot with the same name as the map file
		char szSaveFile[256];
		strcpy(szSaveFile, "screenshots/");
		char * pszSaveFile = szSaveFile + strlen(szSaveFile);
		GetNameFromFileName(pszSaveFile, szMapName);

		if (iTileSize == PREVIEWTILESIZE)
			strcat(szSaveFile, "_preview");
		else if (iTileSize == THUMBTILESIZE)
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

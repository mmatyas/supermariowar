#include "map.h"

#include "IO_Block.h"
#include "GameValues.h"
#include "gfx.h"
#include "GlobalConstants.h"
#include "Game.h"
#include "FileIO.h"
#include "movingplatform.h"
#include "RandomNumberGenerator.h"
#include "ResourceManager.h"
#include "TilesetManager.h"

#include "SDL_image.h"
#include "sdl12wrapper.h"

#include <cmath>
#include <iostream>
using std::cout;
using std::endl;

#if defined(__MACOSX__)
#include <sys/stat.h>
#endif


#ifdef PNG_SAVE_FORMAT
    // this function was added to SDL2
    #ifndef USE_SDL2
        #include "savepng.h"
    #endif
#endif

#ifndef __EMSCRIPTEN__
    inline void smallDelay() { SDL_Delay(10); }
#else
    inline void smallDelay() {}
#endif

#ifdef _WIN32
    #ifndef _XBOX
        #pragma comment(linker, "/NODEFAULTLIB:libc.lib")

        #ifdef PNG_SAVE_FORMAT
            #pragma comment(lib, "libpng.lib")
            #pragma comment(lib, "zlib.lib")
        #endif
    #endif
#endif


extern gfxSprite spr_frontmap[2];
extern short g_iTileConversion[];
extern int g_iVersion[];
extern const char * g_szBackgroundConversion[26];
extern short g_iMusicCategoryConversion[26];

extern bool VersionIsEqual(int iVersion[], short iMajor, short iMinor, short iMicro, short iBuild);
extern bool VersionIsEqualOrBefore(int iVersion[], short iMajor, short iMinor, short iMicro, short iBuild);
extern bool VersionIsEqualOrAfter(int iVersion[], short iMajor, short iMinor, short iMicro, short iBuild);

//Converts the tile type into the flags that this tile carries (solid + ice + death, etc)
short g_iTileTypeConversion[NUMTILETYPES] = {0, 1, 2, 5, 121, 9, 17, 33, 65, 6, 21, 37, 69, 3961, 265, 529, 1057, 2113, 4096};

short g_iCurrentDrawIndex = 0;


extern short g_iDefaultPowerupPresets[NUM_POWERUP_PRESETS][NUM_POWERUPS];

extern SDL_Surface* screen;
extern SDL_Surface* blitdest;

extern CGameValues game_values;
extern CMap* g_map;
extern CTilesetManager* g_tilesetmanager;
extern CResourceManager* rm;
extern CGame* smw;

extern GraphicsList* gamegraphicspacklist;

//[Direction][Frame]
SDL_Rect g_rFlameRects[4][4] = { { {0, 0, 96, 32}, {0, 32, 96, 32}, {0, 64, 96, 32}, {0, 96, 96, 32} },
    { {96, 0, 96, 32}, {96, 32, 96, 32}, {96, 64, 96, 32}, {96, 96, 96, 32} },
    { {0, 128, 32, 96}, {32, 128, 32, 96}, {64, 128, 32, 96}, {96, 128, 32, 96} },
    { {128, 128, 32, 96}, {160, 128, 32, 96}, {192, 128, 32, 96}, {224, 128, 32, 96} }
};

//[Type][Direction][Frame]
SDL_Rect g_rPirhanaRects[4][4][4] = { { { {0, 0, 32, 48}, {32, 0, 32, 48}, {64, 0, 32, 48}, {96, 0, 32, 48} },
        { {128, 0, 32, 48}, {160, 0, 32, 48}, {192, 0, 32, 48}, {224, 0, 32, 48} },
        { {304, 0, 48, 32}, {304, 32, 48, 32}, {304, 64, 48, 32}, {304, 96, 48, 32} },
        { {304, 128, 48, 32}, {304, 160, 48, 32}, {304, 192, 48, 32}, {304, 224, 48, 32} }
    },

    {   { {0, 48, 32, 48}, {32, 48, 32, 48}, {64, 48, 32, 48}, {96, 48, 32, 48} },
        { {128, 48, 32, 48}, {160, 48, 32, 48}, {192, 48, 32, 48}, {224, 48, 32, 48} },
        { {256, 0, 48, 32}, {256, 32, 48, 32}, {256, 64, 48, 32}, {256, 96, 48, 32} },
        { {256, 128, 48, 32}, {256, 160, 48, 32}, {256, 192, 48, 32}, {256, 224, 48, 32} }
    },

    {   { {0, 96, 32, 64}, {32, 96, 32, 64}, {0, 0, 0, 0}, {0, 0, 0, 0} },
        { {64, 96, 32, 64}, {96, 96, 32, 64}, {0, 0, 0, 0}, {0, 0, 0, 0} },
        { {192, 128, 64, 32}, {192, 160, 64, 32}, {0, 0, 0, 0}, {0, 0, 0, 0} },
        { {192, 192, 64, 32}, {192, 224, 64, 32}, {0, 0, 0, 0}, {0, 0, 0, 0} }
    },

    {   { {0, 160, 32, 48}, {32, 160, 32, 48}, {0, 0, 0, 0}, {0, 0, 0, 0} },
        { {64, 160, 32, 48}, {96, 160, 32, 48}, {0, 0, 0, 0}, {0, 0, 0, 0} },
        { {144, 128, 48, 32}, {144, 160, 48, 32}, {0, 0, 0, 0}, {0, 0, 0, 0} },
        { {144, 192, 48, 32}, {144, 224, 48, 32}, {0, 0, 0, 0}, {0, 0, 0, 0} }
    }
};

short iPlatformPathDotOffset[3] = {0, 12, 18};
short iPlatformPathDotSize[3] = {12, 6, 4};

short iFireballHazardSize[3] = {18, 9, 5};

short iStandardOffset[3] = {0, 32, 48};
float dBulletBillFrequency[3] = {10.0f, 5.0f, 2.5f};

short iPirhanaPlantOffsetY[4][3] = {{0, 0, 0}, {48, 24, 12}, {96, 48, 24}, {160, 80, 40}};

void DrawMapHazard(MapHazard * hazard, short iSize, bool fDrawCenter)
{
    short iSizeShift = 5 - iSize;
    short iTileSize = 1 << iSizeShift;

    SDL_Rect rDotSrc = {iPlatformPathDotOffset[iSize] + 22, 0, iPlatformPathDotSize[iSize], iPlatformPathDotSize[iSize]}, rDotDst;
    SDL_Rect rPathSrc = {iStandardOffset[iSize], 12, iTileSize, iTileSize}, rPathDst;

    gfx_setrect(&rPathDst, hazard->ix << (iSizeShift - 1), hazard->iy << (iSizeShift - 1), iTileSize, iTileSize);

    if (fDrawCenter) {
        if (hazard->itype <= 1) {
            SDL_BlitSurface(rm->spr_platformpath.getSurface(), &rPathSrc, blitdest, &rPathDst);
        }
    }

    if (hazard->itype == 0) { //fireball string
        short iNumDots = 16;
        float dRadius = (float)((hazard->iparam[0] - 1) * 24) / (float)(1 << iSize) + (iPlatformPathDotSize[iSize] >> 1);
        float dAngle = hazard->dparam[1];
        for (short iDot = 0; iDot < iNumDots; iDot++) {
            rDotDst.x = (short)(dRadius * cos(dAngle)) + rPathDst.x + (iTileSize >> 1) - (iPlatformPathDotSize[iSize] >> 1);
            rDotDst.y = (short)(dRadius * sin(dAngle)) + rPathDst.y + (iTileSize >> 1) - (iPlatformPathDotSize[iSize] >> 1);
            rDotDst.h = rDotDst.w = iPlatformPathDotSize[iSize];

            rm->spr_platformpath.draw(rDotDst.x, rDotDst.y, rDotSrc.x, rDotSrc.y, rDotDst.w, rDotDst.h);
            dAngle += TWO_PI / iNumDots;
        }

        //Draw the fireball string
        for (short iFireball = 0; iFireball < hazard->iparam[0]; iFireball++) {
            short x = (hazard->ix << (iSizeShift - 1)) + (short)((float)(iFireball * (24 >> iSize)) * cos(hazard->dparam[1])) + (iTileSize >> 1) - (iFireballHazardSize[iSize] >> 1);
            short y = (hazard->iy << (iSizeShift - 1)) + (short)((float)(iFireball * (24 >> iSize)) * sin(hazard->dparam[1])) + (iTileSize >> 1) - (iFireballHazardSize[iSize] >> 1);

            rm->spr_hazard_fireball[iSize].draw(x, y, 0, 0, iFireballHazardSize[iSize], iFireballHazardSize[iSize]);
        }
    } else if (hazard->itype == 1) { //rotodisc
        short iNumDots = 16;
        float dRadius = (hazard->dparam[2] + (iTileSize >> 1) - (iPlatformPathDotSize[iSize] >> 1)) / (float)(1 << iSize);
        float dAngle = hazard->dparam[1];
        for (short iDot = 0; iDot < iNumDots; iDot++) {
            rDotDst.x = (short)(dRadius * cos(dAngle)) + rPathDst.x + (iTileSize >> 1) - (iPlatformPathDotSize[iSize] >> 1);
            rDotDst.y = (short)(dRadius * sin(dAngle)) + rPathDst.y + (iTileSize >> 1) - (iPlatformPathDotSize[iSize] >> 1);
            rDotDst.h = rDotDst.w = iPlatformPathDotSize[iSize];

            rm->spr_platformpath.draw(rDotDst.x, rDotDst.y, rDotSrc.x, rDotSrc.y, rDotDst.w, rDotDst.h);
            dAngle += TWO_PI / iNumDots;
        }

        //Draw the rotodiscs
        float dSector = TWO_PI / hazard->iparam[0];
        dAngle = hazard->dparam[1];
        dRadius = hazard->dparam[2] / (float)(1 << iSize);
        for (short iRotodisc = 0; iRotodisc < hazard->iparam[0]; iRotodisc++) {
            short x = rPathDst.x + (short)(dRadius * cos(dAngle));
            short y = rPathDst.y + (short)(dRadius * sin(dAngle));

            rm->spr_hazard_rotodisc[iSize].draw(x, y, 0, 0, iTileSize, iTileSize);

            dAngle += dSector;
        }
    } else if (hazard->itype == 2) { //bullet bill
        rm->spr_hazard_bulletbill[iSize].draw(rPathDst.x, rPathDst.y, 0, hazard->dparam[0] < 0.0f ? 0 : iTileSize, iTileSize, iTileSize);

        short iBulletPathX = rPathDst.x - iPlatformPathDotSize[iSize];
        if (hazard->dparam[0] > 0.0f)
            iBulletPathX = rPathDst.x + iTileSize;

        short iBulletPathSpacing = (short)(hazard->dparam[0] * dBulletBillFrequency[iSize]);
        while (iBulletPathX >= 0 && iBulletPathX < smw->GetScreenWidth(iSize)) {
            gfx_setrect(&rDotDst, iBulletPathX, rPathDst.y + ((iTileSize - iPlatformPathDotSize[iSize]) >> 1), iPlatformPathDotSize[iSize], iPlatformPathDotSize[iSize]);
            SDL_BlitSurface(rm->spr_platformpath.getSurface(), &rDotSrc, blitdest, &rDotDst);

            iBulletPathX += hazard->iparam[0] < 0.0f ? -iBulletPathSpacing : iBulletPathSpacing;
        }
    } else if (hazard->itype == 3) { //flame cannon
        SDL_Rect * rect = &g_rFlameRects[hazard->iparam[1]][2];

        short iOffsetX = 0;
        short iOffsetY = 0;

        if (hazard->iparam[1] == 1) {
            iOffsetX = -(iTileSize << 1);
        } else if (hazard->iparam[1] == 2) {
            iOffsetY = -(iTileSize << 1);
        }

        rm->spr_hazard_flame[iSize].draw(rPathDst.x + iOffsetX, rPathDst.y + iOffsetY, rect->x >> iSize, rect->y >> iSize, rect->w >> iSize, rect->h >> iSize);
    } else if (hazard->itype >= 4 && hazard->itype <= 7) { //pirhana plants
        SDL_Rect * rect = &g_rPirhanaRects[hazard->itype - 4][hazard->iparam[1]][0];
        short iOffsetX = 0;
        short iOffsetY = 0;

        if (hazard->iparam[1] == 0) {
            if (hazard->itype == 6)
                iOffsetY = -iTileSize;
            else
                iOffsetY = -(iTileSize >> 1);
        } else if (hazard->iparam[1] == 2) {
            if (hazard->itype == 6)
                iOffsetX = -iTileSize;
            else
                iOffsetX = -(iTileSize >> 1);
        }

        rm->spr_hazard_pirhanaplant[iSize].draw(rPathDst.x + iOffsetX, rPathDst.y + iOffsetY, rect->x >> iSize, rect->y >> iSize, rect->w >> iSize, rect->h >> iSize);
    }
}

void DrawPlatform(short pathtype, TilesetTile ** tiles, short startX, short startY, short endX, short endY, float angle, float radiusX, float radiusY, short iSize, short iPlatformWidth, short iPlatformHeight, bool fDrawPlatform, bool fDrawShadow)
{
    short iStartX = startX >> iSize;
    short iStartY = startY >> iSize;
    short iEndX = endX >> iSize;
    short iEndY = endY >> iSize;

    float fRadiusX = radiusX / (float)(1 << iSize);
    float fRadiusY = radiusY / (float)(1 << iSize);

    short iSizeShift = 5 - iSize;
    int iTileSize = 1 << iSizeShift;

    if (fDrawPlatform) {
        for (short iPlatformX = 0; iPlatformX < iPlatformWidth; iPlatformX++) {
            for (short iPlatformY = 0; iPlatformY < iPlatformHeight; iPlatformY++) {
                TilesetTile * tile = &tiles[iPlatformX][iPlatformY];

                int iDstX = 0;
                int iDstY = 0;

                if (pathtype == 2) {
                    iDstX = iStartX + (iPlatformX << iSizeShift) + (short)(fRadiusX * cos(angle)) - (iPlatformWidth << (iSizeShift - 1));
                    iDstY = iStartY + (iPlatformY << iSizeShift) + (short)(fRadiusY * sin(angle)) - (iPlatformHeight << (iSizeShift - 1));
                } else {
                    iDstX = iStartX + (iPlatformX << iSizeShift) - (iPlatformWidth << (iSizeShift - 1));
                    iDstY = iStartY + (iPlatformY << iSizeShift) - (iPlatformHeight << (iSizeShift - 1));
                }

                SDL_Rect bltrect = {iDstX, iDstY, iTileSize, iTileSize};
                if (tile->iID >= 0) {
                    SDL_BlitSurface(g_tilesetmanager->GetTileset(tile->iID)->GetSurface(iSize), &g_tilesetmanager->rRects[iSize][tile->iCol][tile->iRow], blitdest, &bltrect);
                } else if (tile->iID == TILESETANIMATED) {
                    SDL_BlitSurface(rm->spr_tileanimation[iSize].getSurface(), &g_tilesetmanager->rRects[iSize][tile->iCol << 2][tile->iRow], blitdest, &bltrect);
                } else if (tile->iID == TILESETUNKNOWN) {
                    //Draw unknown tile
                    SDL_BlitSurface(rm->spr_unknowntile[iSize].getSurface(), &g_tilesetmanager->rRects[iSize][0][0], blitdest, &bltrect);
                }

                bool fNeedWrap = false;
                if (iDstX + iTileSize >= smw->GetScreenWidth(iSize)) {
                    iDstX -= smw->GetScreenWidth(iSize);
                    fNeedWrap = true;
                } else if (iDstX < 0) {
                    iDstX += smw->GetScreenWidth(iSize);
                    fNeedWrap = true;
                }

                if (fNeedWrap) {
                    bltrect.x = iDstX;
                    bltrect.y = iDstY;
                    bltrect.w = iTileSize;
                    bltrect.h = iTileSize;

                    if (tile->iID >= 0)
                        SDL_BlitSurface(g_tilesetmanager->GetTileset(tile->iID)->GetSurface(iSize), &g_tilesetmanager->rRects[iSize][tile->iCol][tile->iRow], blitdest, &bltrect);
                    else if (tile->iID == TILESETANIMATED)
                        SDL_BlitSurface(rm->spr_tileanimation[iSize].getSurface(), &g_tilesetmanager->rRects[iSize][tile->iCol << 2][tile->iRow], blitdest, &bltrect);
                    else if (tile->iID == TILESETUNKNOWN)
                        SDL_BlitSurface(rm->spr_unknowntile[iSize].getSurface(), &g_tilesetmanager->rRects[iSize][0][0], blitdest, &bltrect);
                }
            }
        }
    }

    SDL_Rect rPathSrc = {iPlatformPathDotOffset[iSize], 0, iPlatformPathDotSize[iSize], iPlatformPathDotSize[iSize]}, rPathDst;

    if (pathtype == 0) { //line segment
        if (fDrawShadow) {
            for (short iCol = 0; iCol < iPlatformWidth; iCol++) {
                for (short iRow = 0; iRow < iPlatformHeight; iRow++) {
                    if (tiles[iCol][iRow].iID != -2)
                        rm->spr_platformstarttile.draw(iStartX - (iPlatformWidth << (iSizeShift - 1)) + (iCol << iSizeShift), iStartY - (iPlatformHeight << (iSizeShift - 1)) + (iRow << iSizeShift), 0, 0, iTileSize, iTileSize);
                }
            }

            for (short iCol = 0; iCol < iPlatformWidth; iCol++) {
                for (short iRow = 0; iRow < iPlatformHeight; iRow++) {
                    if (tiles[iCol][iRow].iID != -2)
                        rm->spr_platformendtile.draw(iEndX - (iPlatformWidth << (iSizeShift - 1)) + (iCol << iSizeShift), iEndY - (iPlatformHeight << (iSizeShift - 1)) + (iRow << iSizeShift), 0, 0, iTileSize, iTileSize);
                }
            }
        }

        //Draw connecting dots
        float dDiffX = (float)(iEndX - iStartX);
        float dDiffY = (float)(iEndY - iStartY);

        short iDistance = (short)sqrt(dDiffX * dDiffX + dDiffY * dDiffY);

        short iNumSpots = (iDistance >> iSizeShift);
        float dIncrementX = dDiffX / (float)iNumSpots;
        float dIncrementY = dDiffY / (float)iNumSpots;

        float dX = (float)(iStartX) - (float)(iPlatformPathDotSize[iSize] >> 1);
        float dY = (float)(iStartY) - (float)(iPlatformPathDotSize[iSize] >> 1);

        for (short iSpot = 0; iSpot < iNumSpots + 1; iSpot++) {
            gfx_setrect(&rPathDst, (short)dX, (short)dY, iPlatformPathDotSize[iSize], iPlatformPathDotSize[iSize]);
            SDL_BlitSurface(rm->spr_platformpath.getSurface(), &rPathSrc, blitdest, &rPathDst);

            dX += dIncrementX;
            dY += dIncrementY;
        }
    } else if (pathtype == 1) { //continuous straight path
        if (fDrawShadow) {
            for (short iCol = 0; iCol < iPlatformWidth; iCol++) {
                for (short iRow = 0; iRow < iPlatformHeight; iRow++) {
                    if (tiles[iCol][iRow].iID != -2)
                        rm->spr_platformstarttile.draw(iStartX - (iPlatformWidth << (iSizeShift - 1)) + (iCol << iSizeShift), iStartY - (iPlatformHeight << (iSizeShift - 1)) + (iRow << iSizeShift), 0, 0, iTileSize, iTileSize);
                }
            }
        }

        float dIncrementX = (float)iTileSize * cos(angle);
        float dIncrementY = (float)iTileSize * sin(angle);

        float dX = (float)(iStartX) - (float)(iPlatformPathDotSize[iSize] >> 1);
        float dY = (float)(iStartY) - (float)(iPlatformPathDotSize[iSize] >> 1);

        for (short iSpot = 0; iSpot < 50; iSpot++) {
            gfx_setrect(&rPathDst, (short)dX, (short)dY, iPlatformPathDotSize[iSize], iPlatformPathDotSize[iSize]);
            SDL_BlitSurface(rm->spr_platformpath.getSurface(), &rPathSrc, blitdest, &rPathDst);

            short iWrapX = (short)dX;
            short iWrapY = (short)dY;
            bool fNeedWrap = false;
            if (dX + iPlatformPathDotSize[iSize] >= smw->GetScreenWidth(iSize)) {
                iWrapX = (short)(dX - smw->GetScreenWidth(iSize));
                fNeedWrap = true;
            } else if (dX < 0.0f) {
                iWrapX = (short)(dX + smw->GetScreenWidth(iSize));
                fNeedWrap = true;
            }

            if (dY + iPlatformPathDotSize[iSize] >= smw->GetScreenHeight(iSize)) {
                iWrapY = (short)(dY - smw->GetScreenHeight(iSize));
                fNeedWrap = true;
            } else if (dY < 0.0f) {
                iWrapY = (short)(dY + smw->GetScreenHeight(iSize));
                fNeedWrap = true;
            }

            if (fNeedWrap) {
                gfx_setrect(&rPathDst, iWrapX, iWrapY, iPlatformPathDotSize[iSize], iPlatformPathDotSize[iSize]);
                SDL_BlitSurface(rm->spr_platformpath.getSurface(), &rPathSrc, blitdest, &rPathDst);
            }

            dX += dIncrementX;
            dY += dIncrementY;
        }
    } else if (pathtype == 2) { //ellipse
        //Calculate the starting position
        if (fDrawShadow) {
            short iEllipseStartX = (short)(fRadiusX * cos(angle)) - (iPlatformWidth << (iSizeShift - 1)) + iStartX;
            short iEllipseStartY = (short)(fRadiusY * sin(angle)) - (iPlatformHeight << (iSizeShift - 1)) + iStartY;

            for (short iCol = 0; iCol < iPlatformWidth; iCol++) {
                for (short iRow = 0; iRow < iPlatformHeight; iRow++) {
                    if (tiles[iCol][iRow].iID != -2)
                        rm->spr_platformstarttile.draw(iEllipseStartX + (iCol << iSizeShift), iEllipseStartY + (iRow << iSizeShift), 0, 0, iTileSize, iTileSize);
                }
            }
        }

        float fAngle = angle;
        for (short iSpot = 0; iSpot < 32; iSpot++) {
            short iX = (short)(fRadiusX * cos(fAngle)) - (iPlatformPathDotSize[iSize] >> 1) + iStartX;
            short iY = (short)(fRadiusY * sin(fAngle)) - (iPlatformPathDotSize[iSize] >> 1) + iStartY;

            gfx_setrect(&rPathDst, iX, iY, iPlatformPathDotSize[iSize], iPlatformPathDotSize[iSize]);
            SDL_BlitSurface(rm->spr_platformpath.getSurface(), &rPathSrc, blitdest, &rPathDst);

            if (iX + iPlatformPathDotSize[iSize] >= smw->GetScreenWidth(iSize)) {
                gfx_setrect(&rPathDst, iX - smw->GetScreenWidth(iSize), iY, iPlatformPathDotSize[iSize], iPlatformPathDotSize[iSize]);
                SDL_BlitSurface(rm->spr_platformpath.getSurface(), &rPathSrc, blitdest, &rPathDst);
            } else if (iX < 0) {
                gfx_setrect(&rPathDst, iX + smw->GetScreenWidth(iSize), iY, iPlatformPathDotSize[iSize], iPlatformPathDotSize[iSize]);
                SDL_BlitSurface(rm->spr_platformpath.getSurface(), &rPathSrc, blitdest, &rPathDst);
            }

            fAngle += TWO_PI / 32.0f;
        }
    }
}


CMap::CMap()
{
    platforms = NULL;
    iNumPlatforms = 0;
    iNumMapItems = 0;
    iNumMapHazards = 0;

    animatedTilesSurface = NULL;

    for (short iSwitch = 0; iSwitch < 4; iSwitch++)
        iSwitches[iSwitch] = 0;
}

CMap::~CMap()
{}


//With the new 32x30 tile set, we need to convert old maps to use the
//correct indexes into the tile set
/*
void CMap::convertMap()
{
	int i, j, k;

	bool fTilesNeedConversion = true;
	bool fBlocksNeedConversion = true;

	for (j = 0; j < MAPHEIGHT; j++)
	{
		for (i = 0; i < MAPWIDTH; i++)
		{
			for (k = 0; k < MAPLAYERS; k++)
			{
				//if we find an empty tile (value 960), then we already have a converted map
				if (mapdata[i][j][k] == TILESETSIZE)
					fTilesNeedConversion = false;
			}

			if (objectdata[i][j.iType == -1)
				fBlocksNeedConversion = false;
		}
	}

	if (!fTilesNeedConversion && !fBlocksNeedConversion)
		return;

	//Otherwise, convert this map
	for (j = 0; j < MAPHEIGHT; j++)
	{
		for (i = 0; i < MAPWIDTH; i++)
		{
			for (k = 0; k < MAPLAYERS; k++)
			{
				mapdata[i][j][k] = g_iTileConversion[mapdata[i][j][k]];
			}
		}
	}
}*/

void CMap::clearMap()
{
    int i, j, k;

    for (j = 0; j < MAPHEIGHT; j++) {
        for (i = 0; i < MAPWIDTH; i++) {
            for (k = 0; k < MAPLAYERS; k++) {
                //reset tile
                mapdata[i][j][k].iID = TILESETNONE;  //no tile selected
            }

            mapdatatop[i][j].iType = tile_nonsolid;
            mapdatatop[i][j].iFlags = tile_flag_nonsolid;

            objectdata[i][j].iType = -1;
            warpdata[i][j].direction = WARP_UNDEFINED;
            warpdata[i][j].connection = -1;

            for (short iSpawn = 0; iSpawn < NUMSPAWNAREATYPES; iSpawn++)
                nospawn[iSpawn][i][j] = false;
        }
    }

    eyecandy[0] = 0;
    eyecandy[1] = 0;
    eyecandy[2] = 0;

    iNumMapItems = 0;
    iNumMapHazards = 0;

    for (short iSwitch = 0; iSwitch < 4; iSwitch++)
        iSwitches[iSwitch] = 0;

    bltrect.w = TILESIZE;
    bltrect.h = TILESIZE;
}

void CMap::clearPlatforms()
{
    for (short iLayer = 0; iLayer < 5; iLayer++)
        platformdrawlayer[iLayer].clear();

    if (platforms) {
        for (short iPlatform = 0; iPlatform < iNumPlatforms; iPlatform++) {
            delete platforms[iPlatform];
        }

        delete [] platforms;
        platforms = NULL;
    }

    iNumPlatforms = 0;

    std::list<MovingPlatform*>::iterator iter = tempPlatforms.begin(), lim = tempPlatforms.end();

    while (iter != lim) {
        delete (*iter);
        ++iter;
    }

    tempPlatforms.clear();
}

void CMap::ClearAnimatedTiles()
{
    std::vector<AnimatedTile*>::iterator iter = animatedtiles.begin(), lim = animatedtiles.end();

    while (iter != lim) {
        delete (*iter);
        ++iter;
    }

    animatedtiles.clear();
}

void CMap::loadMap(const std::string& file, ReadType iReadType)
{
    iTileAnimationTimer = 0;
    iTileAnimationFrame = 0;

    ClearAnimatedTiles();

    eyecandy[0] = 0;
    eyecandy[1] = 0;
    eyecandy[2] = 0;

    FILE * mapfile;
    short i, j, k;

    /*
    cout << "loading map " << file;

    if (iReadType == read_type_preview)
    	cout << " (preview)";
    if (iReadType == read_type_summary)
    	cout << " (summary)";

    cout << " ...";
    */

    mapfile = fopen(file.c_str(), "rb");
    if (mapfile == NULL) {
        cout << endl << " ERROR: Couldn't open map" << endl;
        return;
    }

    //Load version number
    int version[4];
    //version[0] = ReadInt(mapfile); //Major
    //version[1] = ReadInt(mapfile); //Minor
    //version[2] = ReadInt(mapfile); //Micro
    //version[3] = ReadInt(mapfile); //Build

    ReadIntChunk(version, 4, mapfile);

    if (iReadType != read_type_summary) {
        cout << "loading map " << file;

        if (iReadType == read_type_preview)
            cout << " (preview)";

        if (VersionIsEqualOrAfter(version, 1, 6, 0, 0)) {
            cout << " [v" << version[0] << '.' << version[1] << '.'
                 << version[2] << '.' << version[3] << "]";
        }
        else
            cout << " [v1.5]";

        cout << " ...";
    }

    MapReader* reader = MapReader::getLoaderByVersion(version);
    reader->load(*this, mapfile, iReadType);
    delete reader;
    reader = NULL;

    fclose(mapfile);

    if (iReadType == read_type_summary)
        return;

    clearWarpLocks();
    cout << " done" << endl;
}

void CMap::UpdateAllTileGaps()
{
    //Detect 1 tile gaps between solid tiles
    for (short j = 0; j < MAPHEIGHT; j++) {
        for (short i = 0; i < MAPWIDTH; i++) {
            SetTileGap(i, j);
        }
    }
}

void CMap::UpdateTileGap(short i, short j)
{
    for (short iRow = j; iRow <= j + 1; iRow++) {
        for (short iCol = i - 1; iCol <= i + 1; iCol++) {
            if (iRow >= MAPHEIGHT)
                break;

            short ix = iCol;
            if (ix < 0)
                ix = MAPWIDTH - 1;
            else if (ix >= MAPWIDTH)
                ix = 0;

            SetTileGap(ix, iRow);
        }
    }
}

void CMap::SetTileGap(short i, short j)
{
    short iLeftTile = i - 1;
    if (iLeftTile < 0)
        iLeftTile = MAPWIDTH - 1;

    short iRightTile = i + 1;
    if (iRightTile >= MAPWIDTH)
        iRightTile = 0;

    int topLeftTile = 0;
    int topCenterTile = 0;
    int topRightTile = 0;

    IO_Block * topLeftBlock = NULL;
    IO_Block * topCenterBlock = NULL;
    IO_Block * topRightBlock = NULL;

    if (j > 0) {
        topLeftTile = mapdatatop[iLeftTile][j - 1].iFlags;
        topCenterTile = mapdatatop[i][j - 1].iFlags;
        topRightTile = mapdatatop[iRightTile][j - 1].iFlags;

        topLeftBlock = blockdata[iLeftTile][j - 1];
        topCenterBlock = blockdata[i][j - 1];
        topRightBlock = blockdata[iRightTile][j - 1];
    }

    int leftTile = mapdatatop[iLeftTile][j].iFlags;
    int centerTile = mapdatatop[i][j].iFlags;
    int rightTile = mapdatatop[iRightTile][j].iFlags;

    IO_Block * leftBlock = blockdata[iLeftTile][j];
    IO_Block * centerBlock = blockdata[i][j];
    IO_Block * rightBlock = blockdata[iRightTile][j];

    bool fLeftSolid = (leftTile != tile_flag_nonsolid && leftTile != tile_flag_gap) || (leftBlock && !leftBlock->isTransparent() && !leftBlock->isHidden());

    //The following line used to be:
    //bool fCenterSolid = centerTile != tile_flag_nonsolid || (centerBlock && !centerBlock->isTransparent() && !centerBlock->isHidden());
    //And I don't remember why I specifically did it that way.  It was causing a bug so I changed it back to this:
    bool fCenterSolid = (centerTile != tile_flag_nonsolid && centerTile != tile_flag_gap) || (centerBlock && !centerBlock->isTransparent() && !centerBlock->isHidden());

    bool fRightSolid = (rightTile != tile_flag_nonsolid && rightTile != tile_flag_gap) || (rightBlock && !rightBlock->isTransparent() && !rightBlock->isHidden());

    bool fTopLeftSolid = (topLeftTile & tile_flag_solid) || (topLeftBlock && !topLeftBlock->isTransparent() && !topLeftBlock->isHidden());
    bool fTopCenterSolid = (topCenterTile & tile_flag_solid) || (topCenterBlock && !topCenterBlock->isTransparent() && !topCenterBlock->isHidden());
    bool fTopRightSolid = (topRightTile & tile_flag_solid) || (topRightBlock && !topRightBlock->isTransparent() && !topRightBlock->isHidden());

    if (fLeftSolid && !fCenterSolid && fRightSolid && !fTopLeftSolid && !fTopCenterSolid && !fTopRightSolid) {
        mapdatatop[i][j].iType = tile_gap;
        mapdatatop[i][j].iFlags = tile_flag_gap;
    } else if (mapdatatop[i][j].iFlags == tile_flag_gap) {
        mapdatatop[i][j].iType = tile_nonsolid;
        mapdatatop[i][j].iFlags = tile_flag_nonsolid;
    }
}

void CMap::loadPlatforms(FILE * mapfile, bool fPreview, int version[4], short * translationid, short * tilesetwidths, short * tilesetheights, short iMaxTilesetID)
{
    clearPlatforms();

    //Load moving platforms
    iNumPlatforms = (short)ReadInt(mapfile);
    platforms = new MovingPlatform*[iNumPlatforms];

    for (short iPlatform = 0; iPlatform < iNumPlatforms; iPlatform++) {
        short iWidth = (short)ReadInt(mapfile);
        short iHeight = (short)ReadInt(mapfile);

        TilesetTile ** tiles = new TilesetTile*[iWidth];
        MapTile ** types = new MapTile*[iWidth];

        for (short iCol = 0; iCol < iWidth; iCol++) {
            tiles[iCol] = new TilesetTile[iHeight];
            types[iCol] = new MapTile[iHeight];

            for (short iRow = 0; iRow < iHeight; iRow++) {
                TilesetTile * tile = &tiles[iCol][iRow];

                if (VersionIsEqualOrAfter(version, 1, 8, 0, 0)) {
                    tile->iID = ReadByteAsShort(mapfile);
                    tile->iCol = ReadByteAsShort(mapfile);
                    tile->iRow = ReadByteAsShort(mapfile);

                    if (tile->iID >= 0) {
                        if (iMaxTilesetID != -1 && tile->iID > iMaxTilesetID)
                            tile->iID = 0;

                        //Make sure the column and row we read in is within the bounds of the tileset
                        if (tile->iCol < 0 || (tilesetwidths && tile->iCol >= tilesetwidths[tile->iID]))
                            tile->iCol = 0;

                        if (tile->iRow < 0 || (tilesetheights && tile->iRow >= tilesetheights[tile->iID]))
                            tile->iRow = 0;

                        //Convert tileset ids into the current game's tileset's ids
                        if (translationid)
                            tile->iID = translationid[tile->iID];
                    }

                    TileType iType = (TileType)ReadInt(mapfile);

                    if (iType >= 0 && iType < NUMTILETYPES) {
                        types[iCol][iRow].iType = iType;
                        types[iCol][iRow].iFlags = g_iTileTypeConversion[iType];
                    } else {
                        types[iCol][iRow].iType = tile_nonsolid;
                        types[iCol][iRow].iFlags = tile_flag_nonsolid;
                    }
                } else {
                    short iTile = ReadInt(mapfile);
                    TileType type;

                    if (iTile == TILESETSIZE) {
                        tile->iID = TILESETNONE;
                        tile->iCol = 0;
                        tile->iRow = 0;

                        type = tile_nonsolid;
                    } else {
                        tile->iID = g_tilesetmanager->GetClassicTilesetIndex();
                        tile->iCol = iTile % TILESETWIDTH;
                        tile->iRow = iTile / TILESETWIDTH;

                        type = g_tilesetmanager->GetClassicTileset()->GetTileType(tile->iCol, tile->iRow);
                    }

                    if (type >= 0 && type < NUMTILETYPES) {
                        types[iCol][iRow].iType = type;
                        types[iCol][iRow].iFlags = g_iTileTypeConversion[type];
                    } else {
                        mapdatatop[iCol][iRow].iType = tile_nonsolid;
                        mapdatatop[iCol][iRow].iFlags = tile_flag_nonsolid;
                    }
                }
            }
        }

        short iDrawLayer = 2;
        if (VersionIsEqualOrAfter(version, 1, 8, 0, 1))
            iDrawLayer = ReadInt(mapfile);

        //printf("Layer: %d\n", iDrawLayer);

        short iPathType = 0;

        if (VersionIsEqualOrAfter(version, 1, 8, 0, 0))
            iPathType = ReadInt(mapfile);

        //printf("PathType: %d\n", iPathType);

        MovingPlatformPath * path = NULL;
        if (iPathType == 0) { //segment path
            float fStartX = ReadFloat(mapfile);
            float fStartY = ReadFloat(mapfile);
            float fEndX = ReadFloat(mapfile);
            float fEndY = ReadFloat(mapfile);
            float fVelocity = ReadFloat(mapfile);

            path = new StraightPath(fVelocity, fStartX, fStartY, fEndX, fEndY, fPreview);

            //printf("Read segment path\n");
            //printf("StartX: %.2f StartY:%.2f EndX:%.2f EndY:%.2f Velocity:%.2f\n", fStartX, fStartY, fEndX, fEndY, fVelocity);
        } else if (iPathType == 1) { //continuous path
            float fStartX = ReadFloat(mapfile);
            float fStartY = ReadFloat(mapfile);
            float fAngle = ReadFloat(mapfile);
            float fVelocity = ReadFloat(mapfile);

            path = new StraightPathContinuous(fVelocity, fStartX, fStartY, fAngle, fPreview);

            //printf("Read continuous path\n");
            //printf("StartX: %.2f StartY:%.2f Angle:%.2f Velocity:%.2f\n", fStartX, fStartY, fAngle, fVelocity);
        } else if (iPathType == 2) { //elliptical path
            float fRadiusX = ReadFloat(mapfile);
            float fRadiusY = ReadFloat(mapfile);
            float fCenterX = ReadFloat(mapfile);
            float fCenterY = ReadFloat(mapfile);
            float fAngle = ReadFloat(mapfile);
            float fVelocity = ReadFloat(mapfile);

            path = new EllipsePath(fVelocity, fAngle, fRadiusX, fRadiusY, fCenterX, fCenterY, fPreview);

            //printf("Read elliptical path\n");
            //printf("CenterX: %.2f CenterY:%.2f Angle:%.2f RadiusX: %.2f RadiusY: %.2f Velocity:%.2f\n", fCenterX, fCenterY, fAngle, fRadiusX, fRadiusY, fVelocity);
        }

        MovingPlatform * platform = new MovingPlatform(tiles, types, iWidth, iHeight, iDrawLayer, path, fPreview);
        platforms[iPlatform] = platform;
        platformdrawlayer[iDrawLayer].push_back(platform);
    }
}

void CMap::saveMap(const std::string& file)
{
    FILE *mapfile;
    int i, j, k;

    cout << "saving map " << file << " ... ";

    mapfile = fopen(file.c_str(), "wb");
    if (mapfile == NULL) {
        cout << endl << " ERROR: couldn't save map" << endl;
        return;
    }

    //First write the map compatibility version number
    //(this will allow the map loader to identify if the map needs conversion)
    WriteInt(g_iVersion[0], mapfile); //Major
    WriteInt(g_iVersion[1], mapfile); //Minor
    WriteInt(g_iVersion[2], mapfile); //Micro
    WriteInt(g_iVersion[3], mapfile); //Build

    //Calculate warp zones
    bool usedtile[MAPWIDTH][MAPHEIGHT];
    for (j = 0; j < MAPHEIGHT; j++)
        for (i = 0; i < MAPWIDTH; i++)
            usedtile[i][j] = false;

    //bool fAutoMapFilters[NUM_AUTO_FILTERS];
    //for (short iFilter = 0; iFilter < NUM_AUTO_FILTERS; iFilter++)
    //fAutoMapFilters[iFilter] = false;

    int iHazardCount = 0;
    int iWarpCount = 0;
    int iIceCount = 0;
    int iPowerupBlockCount = 0;
    int iPlatformCount = 0;
    int iDensity = 0;
    int iOnOffBlockCount = 0;
    int iThrowBlockCount = 0;
    int iBreakableBlockCount = 0;
    int iItemDestroyableBlockCount = 0;
    int iHiddenBlockCount = 0;

    for (short iPlatform = 0; iPlatform < iNumPlatforms; iPlatform++) {
        for (short iCol = 0; iCol < platforms[iPlatform]->iTileWidth; iCol++) {
            for (short iRow = 0; iRow < platforms[iPlatform]->iTileHeight; iRow++) {
                //Set the tile type flags for each tile
                int iType = platforms[iPlatform]->iTileType[iCol][iRow].iType;
                if (iType >= 0 && iType < NUMTILETYPES) {
                    platforms[iPlatform]->iTileType[iCol][iRow].iFlags = g_iTileTypeConversion[iType];
                } else {
                    platforms[iPlatform]->iTileType[iCol][iRow].iType = tile_nonsolid;
                    platforms[iPlatform]->iTileType[iCol][iRow].iFlags = tile_flag_nonsolid;
                }

                TilesetTile * tile = &platforms[iPlatform]->iTileData[iCol][iRow];
                int iFlags = platforms[iPlatform]->iTileType[iCol][iRow].iFlags;

                if (tile->iID != TILESETNONE)
                    iPlatformCount++;

                if (iFlags & tile_flag_has_death)
                    iHazardCount++;

                if (iFlags & tile_flag_ice)
                    iIceCount++;
            }
        }
    }

    //Examine tiles to calculate map summary data (i.e. presence of death blocks, powerups, ice, switches, etc.)
    short numWarpExits = 0;
    for (j = 0; j < MAPHEIGHT; j++) {
        for (i = 0; i < MAPWIDTH; i++) {
            //Set the tile type flags for each tile
            int iType = mapdatatop[i][j].iType;
            if (iType >= 0 && iType < NUMTILETYPES) {
                mapdatatop[i][j].iFlags = g_iTileTypeConversion[iType];
            } else {
                mapdatatop[i][j].iType = tile_nonsolid;
                mapdatatop[i][j].iFlags = tile_flag_nonsolid;
            }

            //Calculate what warp tiles belong together (any warps that have the same connection that are
            //next to each other are merged into a single warp)
            //If there are too many warps, then remove any warp encountered that is over that limit
            if (warpdata[i][j].connection != -1 && !usedtile[i][j]) {
                int movex = 0, movey = 0;
                int currentx = i, currenty = j;

                if (warpdata[i][j].direction == 0 || warpdata[i][j].direction == 2)
                    movex = 1;
                else
                    movey = 1;

                while (currentx < MAPWIDTH && currenty < MAPHEIGHT) {
                    if (warpdata[currentx][currenty].direction != warpdata[i][j].direction ||
                            warpdata[currentx][currenty].connection != warpdata[i][j].connection) {
                        break;
                    }

                    usedtile[currentx][currenty] = true;

                    if (numWarpExits < MAXWARPS) {
                        warpdata[currentx][currenty].id = numWarpExits;
                    } else {
                        warpdata[currentx][currenty].connection = -1;
                        warpdata[currentx][currenty].direction = WARP_UNDEFINED;
                    }

                    currentx += movex;
                    currenty += movey;
                }

                numWarpExits++;
            }

            short iBlockType = objectdata[i][j].iType;
            int iFlags = mapdatatop[i][j].iFlags;

            //Calculate auto map filters
            if (iFlags & tile_flag_has_death)
                iHazardCount++;

            if (warpdata[i][j].connection != -1)
                iWarpCount++;

            if (iFlags & tile_flag_ice)
                iIceCount++;

            if (iBlockType == 1 || iBlockType == 15) //Powerup/View Block
                iPowerupBlockCount++;

            if (iBlockType == 0) //Breakable Block
                iBreakableBlockCount++;

            if (iBlockType == 6 || iBlockType == 16) //Blue/Red Throw Block
                iThrowBlockCount++;

            if (iBlockType >= 11 && iBlockType <= 14) //On/Off Block
                iOnOffBlockCount++;

            if (iFlags & tile_flag_solid)
                iDensity++;

            if ((iBlockType == 1 || iBlockType == 3 || iBlockType == 4 || iBlockType == 5 || iBlockType == 15 || iBlockType == 17 || iBlockType == 18) && objectdata[i][j].fHidden) //Hidden blocks
                iHiddenBlockCount++;

            if (iBlockType >= 20 && iBlockType <= 29) //Item Destroyable Blocks
                iItemDestroyableBlockCount++;
        }
    }

    //Save summary info for auto map filters (hazards, ice, warps, platforms, bonus blocks, density, etc)
    WriteInt(iHazardCount, mapfile);
    WriteInt(iWarpCount, mapfile);
    WriteInt(iIceCount, mapfile);
    WriteInt(iPowerupBlockCount, mapfile);
    WriteInt(iBreakableBlockCount, mapfile);
    WriteInt(iThrowBlockCount, mapfile);
    WriteInt(iOnOffBlockCount, mapfile);
    WriteInt(iPlatformCount, mapfile);
    WriteInt(iNumMapHazards, mapfile);
    WriteInt(iItemDestroyableBlockCount, mapfile);
    WriteInt(iHiddenBlockCount, mapfile);
    WriteInt(iNumMapItems, mapfile);
    WriteInt(iDensity, mapfile);

    //Write tileset names and indexes for translation at load time
    //Number of tilesets used by this map
    short iTilesetCount = g_tilesetmanager->GetCount();
    bool * fTilesetUsed = new bool[iTilesetCount];
    for (short iTileset = 0; iTileset < iTilesetCount; iTileset++)
        fTilesetUsed[iTileset] = false;

    //Scan map to see how many tilesets were used
    for (j = 0; j < MAPHEIGHT; j++) {
        for (i = 0; i < MAPWIDTH; i++) {
            for (k = 0; k < MAPLAYERS; k++) {
                if (mapdata[i][j][k].iID >= 0)
                    fTilesetUsed[mapdata[i][j][k].iID] = true;
            }
        }
    }

    //Scan platforms too for tilesets used
    for (short iPlatform = 0; iPlatform < iNumPlatforms; iPlatform++) {
        for (short iCol = 0; iCol < platforms[iPlatform]->iTileWidth; iCol++) {
            for (short iRow = 0; iRow < platforms[iPlatform]->iTileHeight; iRow++) {
                TilesetTile * tile = &platforms[iPlatform]->iTileData[iCol][iRow];

                if (tile->iID >= 0)
                    fTilesetUsed[tile->iID] = true;
            }
        }
    }

    short iUsedTilesets = 0;
    for (short iTileset = 0; iTileset < iTilesetCount; iTileset++) {
        if (fTilesetUsed[iTileset])
            iUsedTilesets++;
    }

    WriteInt(iUsedTilesets, mapfile);

    //Write each of the tileset names with the index that will be used by this mapfile to represent that tileset
    for (short iTileset = 0; iTileset < iTilesetCount; iTileset++) {
        if (fTilesetUsed[iTileset]) {
            //Tileset ID
            WriteInt(iTileset, mapfile);

            //Tileset Name
            WriteString(g_tilesetmanager->GetTileset(iTileset)->GetName(), mapfile);
        }
    }

    delete [] fTilesetUsed;

    //save map tiles and blocks
    for (j = 0; j < MAPHEIGHT; j++) {
        for (i = 0; i < MAPWIDTH; i++) {
            for (k = 0; k < MAPLAYERS; k++) {
                //Tile sprites (4 layers)
                //WriteInt(mapdata[i][j][k], mapfile);

                TilesetTile * tile = &mapdata[i][j][k];

                //Make sure the tile's col and row are within the tileset
                if (tile->iID >= 0) {
                    if (tile->iCol < 0 || tile->iCol >= g_tilesetmanager->GetTileset(tile->iID)->GetWidth())
                        tile->iCol = 0;

                    if (tile->iRow < 0 || tile->iRow >= g_tilesetmanager->GetTileset(tile->iID)->GetHeight())
                        tile->iRow = 0;
                }

                WriteByteFromShort(tile->iID, mapfile);
                WriteByteFromShort(tile->iCol, mapfile);
                WriteByteFromShort(tile->iRow, mapfile);
            }

            //Interaction blocks
            WriteByteFromShort(objectdata[i][j].iType, mapfile);
            WriteBool(objectdata[i][j].fHidden, mapfile);
        }
    }

    //Write background File
    WriteString(szBackgroundFile, mapfile);

    //Save the default on/off switch states
    for (short iSwitch = 0; iSwitch < 4; iSwitch++)
        WriteInt(iSwitches[iSwitch], mapfile);

    //Write moving platforms
    WriteInt(iNumPlatforms, mapfile);

    for (short iPlatform = 0; iPlatform < iNumPlatforms; iPlatform++) {
        WriteInt(platforms[iPlatform]->iTileWidth, mapfile);
        WriteInt(platforms[iPlatform]->iTileHeight, mapfile);

        for (short iCol = 0; iCol < platforms[iPlatform]->iTileWidth; iCol++) {
            for (short iRow = 0; iRow < platforms[iPlatform]->iTileHeight; iRow++) {
                TilesetTile * tile = &platforms[iPlatform]->iTileData[iCol][iRow];

                //Make sure the tile's col and row are within the tileset
                if (tile->iID >= 0) {
                    if (tile->iCol < 0 || tile->iCol >= g_tilesetmanager->GetTileset(tile->iID)->GetWidth())
                        tile->iCol = 0;

                    if (tile->iRow < 0 || tile->iRow >= g_tilesetmanager->GetTileset(tile->iID)->GetHeight())
                        tile->iRow = 0;
                }

                WriteByteFromShort(tile->iID, mapfile);
                WriteByteFromShort(tile->iCol, mapfile);
                WriteByteFromShort(tile->iRow, mapfile);

                WriteInt(platforms[iPlatform]->iTileType[iCol][iRow].iType, mapfile);
            }
        }

        WriteInt(platforms[iPlatform]->iDrawLayer, mapfile);

        short iPathType = platforms[iPlatform]->pPath->iType;
        WriteInt(iPathType, mapfile);

        if (iPathType == 0) {
            StraightPath * path = (StraightPath*)platforms[iPlatform]->pPath;
            WriteFloat(path->dPathPointX[0], mapfile);
            WriteFloat(path->dPathPointY[0], mapfile);
            WriteFloat(path->dPathPointX[1], mapfile);
            WriteFloat(path->dPathPointY[1], mapfile);
            WriteFloat(path->dVelocity, mapfile);
        } else if (iPathType == 1) {
            StraightPathContinuous * path = (StraightPathContinuous*)platforms[iPlatform]->pPath;
            WriteFloat(path->dPathPointX[0], mapfile);
            WriteFloat(path->dPathPointY[0], mapfile);
            WriteFloat(path->dAngle, mapfile);
            WriteFloat(path->dVelocity, mapfile);
        } else if (iPathType == 2) { //elliptical path
            EllipsePath * path = (EllipsePath*)platforms[iPlatform]->pPath;
            WriteFloat(path->dRadiusX, mapfile);
            WriteFloat(path->dRadiusY, mapfile);
            WriteFloat(path->dPathPointX[0], mapfile);
            WriteFloat(path->dPathPointY[0], mapfile);
            WriteFloat(path->dAngle[0], mapfile);
            WriteFloat(path->dVelocity, mapfile);
        }
    }

    //Write map items (carried springs, spikes, kuribo's shoe, etc)
    WriteInt(iNumMapItems, mapfile);

    for (short iMapItem = 0; iMapItem < iNumMapItems; iMapItem++) {
        WriteInt(mapitems[iMapItem].itype, mapfile);
        WriteInt(mapitems[iMapItem].ix, mapfile);  //tile aligned
        WriteInt(mapitems[iMapItem].iy, mapfile);
    }

    //Write map hazards (fireball strings, rotodiscs, pirhana plants, etc)
    WriteInt(iNumMapHazards, mapfile);

    for (short iMapHazard = 0; iMapHazard < iNumMapHazards; iMapHazard++) {
        WriteInt(maphazards[iMapHazard].itype, mapfile);
        WriteInt(maphazards[iMapHazard].ix, mapfile);
        WriteInt(maphazards[iMapHazard].iy, mapfile);

        for (short iParam = 0; iParam < NUMMAPHAZARDPARAMS; iParam++)
            WriteInt(maphazards[iMapHazard].iparam[iParam], mapfile);

        for (short iParam = 0; iParam < NUMMAPHAZARDPARAMS; iParam++)
            WriteFloat(maphazards[iMapHazard].dparam[iParam], mapfile);
    }

    //Write eyecandy for all eyecandy layers
    WriteInt(eyecandy[0], mapfile);
    WriteInt(eyecandy[1], mapfile);
    WriteInt(eyecandy[2], mapfile);

    //Write music category
    WriteInt(musicCategoryID, mapfile);

    //Write the rest of the map data later so that we can just load part of the map for the preview
    for (j = 0; j < MAPHEIGHT; j++) {
        for (i = 0; i < MAPWIDTH; i++) {
            //Write tile collision types (ice, solid, death, etc.)
            WriteInt(mapdatatop[i][j].iType, mapfile);

            //Write per tile warp data
            WriteInt(warpdata[i][j].direction, mapfile);
            WriteInt(warpdata[i][j].connection, mapfile);
            WriteInt(warpdata[i][j].id, mapfile);

            //Write per tile allowed spawn types (player, team specific (1-4), item)
            for (short iType = 0; iType < NUMSPAWNAREATYPES; iType++)
                WriteBool(nospawn[iType][i][j], mapfile);
        }
    }

    //Count blocks that have supplement data (like powerup weights for powerup blocks or state for switched blocks)
    short iBlockCount = 0;
    short iSwitchBlockCount = 0;
    for (j = 0; j < MAPHEIGHT; j++) {
        for (i = 0; i < MAPWIDTH; i++) {
            //powerup block
            if (objectdata[i][j].iType == 1 || objectdata[i][j].iType == 15)
                iBlockCount++;

            //switched blocks
            if (objectdata[i][j].iType >= 11 && objectdata[i][j].iType <= 14)
                iSwitchBlockCount++;
        }
    }

    //Write out the switch block state
    WriteInt(iSwitchBlockCount, mapfile);
    for (j = 0; j < MAPHEIGHT; j++) {
        for (i = 0; i < MAPWIDTH; i++) {
            if (objectdata[i][j].iType >= 11 && objectdata[i][j].iType <= 14) {
                WriteByteFromShort(i, mapfile);
                WriteByteFromShort(j, mapfile);
                WriteByteFromShort(objectdata[i][j].iSettings[0], mapfile);

                //TODO: REmove this and comment in the LINE ABOVE
                //WriteByteFromShort(iSwitches[objectdata[i][j].iType - 11], mapfile);
            }
        }
    }

    //Write number of warp exits
    WriteInt(numWarpExits, mapfile);

    for (j = 0; j < MAPHEIGHT; j++)
        for (i = 0; i < MAPWIDTH; i++)
            usedtile[i][j] = false;

    //scan warp data and write out warp exits
    for (j = 0; j < MAPHEIGHT; j++) {
        for (i = 0; i < MAPWIDTH; i++) {
            //if we found a warp area and it hasn't been consumed by another warp area
            //then scan for surrounding warp areas and consume them to create a warp exit
            if (warpdata[i][j].connection != -1 && !usedtile[i][j]) {
                int movex = 0, movey = 0;
                int currentx = i, currenty = j;

                if (warpdata[i][j].direction == 0 || warpdata[i][j].direction == 2)
                    movex = 1;
                else
                    movey = 1;


                int numblocks = 0;
                while (currentx < MAPWIDTH && currenty < MAPHEIGHT) {
                    if (warpdata[currentx][currenty].direction != warpdata[i][j].direction ||
                            warpdata[currentx][currenty].connection != warpdata[i][j].connection) {
                        break;
                    }

                    usedtile[currentx][currenty] = true;

                    currentx += movex;
                    currenty += movey;
                    numblocks++;
                }

                //We're on the next block that isn't a warp
                currentx -= movex;
                currenty -= movey;

                WriteInt(warpdata[i][j].direction, mapfile);
                WriteInt(warpdata[i][j].connection, mapfile);
                WriteInt(warpdata[i][j].id, mapfile);

                //Write out warp exit x,y position for player and position for lock icon to display
                if (warpdata[i][j].direction == 0) {
                    WriteInt((((currentx << 5) + TILESIZE - (i << 5)) >> 1) + (i << 5) - HALFPW, mapfile);
                    WriteInt((j << 5) - 1 + PHOFFSET, mapfile);

                    WriteInt((((currentx << 5) + TILESIZE - (i << 5)) >> 1) + (i << 5) - 16, mapfile);
                    WriteInt((j << 5), mapfile);
                } else if (warpdata[i][j].direction == 2) {
                    WriteInt((((currentx << 5) + TILESIZE - (i << 5)) >> 1) + (i << 5) - HALFPW, mapfile);
                    WriteInt((j << 5) + 1 + PHOFFSET, mapfile);

                    WriteInt((((currentx << 5) + TILESIZE - (i << 5)) >> 1) + (i << 5) - 16, mapfile);
                    WriteInt((j << 5), mapfile);
                } else if (warpdata[i][j].direction == 1) {
                    WriteInt((i << 5) + TILESIZE - PW - PWOFFSET, mapfile);
                    WriteInt((currenty << 5) + TILESIZE - PH - 1, mapfile);

                    WriteInt((i << 5), mapfile);
                    WriteInt((((currenty << 5) + TILESIZE - (j << 5)) >> 1) + (j << 5) - 16, mapfile);
                } else if (warpdata[i][j].direction == 3) {
                    WriteInt((i << 5) - 1 + PWOFFSET, mapfile);
                    WriteInt((currenty << 5) + TILESIZE - PH - 1, mapfile);

                    WriteInt((i << 5), mapfile);
                    WriteInt((((currenty << 5) + TILESIZE - (j << 5)) >> 1) + (j << 5) - 16, mapfile);
                }

                WriteInt(i, mapfile);
                WriteInt(j, mapfile);
                WriteInt(numblocks, mapfile);

            }
        }
    }

    //Calculate player/team spawn zones
    for (short iType = 0; iType <= 5; iType++) {
        calculatespawnareas(iType, false, false);

        //Ok try to find somewhere to spawn using areas that may or may not be there
        if (numspawnareas[iType] == 0)
            calculatespawnareas(iType, true, false);

        //Ok, try to find somewhere to spawn that might be over a death tile
        if (numspawnareas[iType] == 0)
            calculatespawnareas(iType, true, true);
    }

    //Write spawn areas
    for (i = 0; i < NUMSPAWNAREATYPES; i++) {
        WriteInt(numspawnareas[i], mapfile);

        for (int m = 0; m < numspawnareas[i]; m++) {
            WriteInt(spawnareas[i][m].left, mapfile);
            WriteInt(spawnareas[i][m].top, mapfile);
            WriteInt(spawnareas[i][m].width, mapfile);
            WriteInt(spawnareas[i][m].height, mapfile);
            WriteInt(spawnareas[i][m].size, mapfile);
        }
    }

    for (j = 0; j < MAPHEIGHT; j++)
        for (i = 0; i < MAPWIDTH; i++)
            usedtile[i][j] = false;

    //Figure out areas that contain tiles in layers 2 and 3
    for (j = 0; j < MAPHEIGHT; j++) {
        for (i = 0; i < MAPWIDTH; i++) {
            if (mapdata[i][j][2].iID <= TILESETNONE && mapdata[i][j][3].iID <= TILESETNONE)
                usedtile[i][j] = true;
        }
    }

    //Figure out where the overlayer draw rects are
    numdrawareas = 0;
    for (j = 0; j < MAPHEIGHT; j++) {
        for (i = 0; i < MAPWIDTH; i++) {
            if (!usedtile[i][j]) {
                bool fDownDone = false;
                bool fRightDone = false;

                int downsize = j + 1;
                int rightsize = i + 1;
                int attempt = 1;

                while (true) {
                    if (!fRightDone) {
                        for (int right = i; right < rightsize; right++) {
                            if (right >= MAPWIDTH || j + attempt >= MAPHEIGHT || usedtile[right][j + attempt])
                                fRightDone = true;
                        }

                        if (!fRightDone)
                            downsize++;
                    }

                    if (!fDownDone) {
                        for (int down = j; down < downsize; down++) {
                            if (i + attempt >= MAPWIDTH || down >= MAPHEIGHT || usedtile[i + attempt][down])
                                fDownDone = true;
                        }

                        if (!fDownDone)
                            rightsize++;
                    }

                    if (fDownDone && fRightDone) {
                        if (numdrawareas < MAXDRAWAREAS) {
                            drawareas[numdrawareas].x = (Sint16)(i << 5);
                            drawareas[numdrawareas].y = (Sint16)(j << 5);
                            drawareas[numdrawareas].w = (Uint16)((rightsize - i) << 5);
                            drawareas[numdrawareas].h = (Uint16)((downsize - j) << 5);

                            numdrawareas++;
                        }

                        for (int down = j; down < downsize; down++) {
                            for (int right = i; right < rightsize; right++) {
                                usedtile[right][down] = true;
                            }
                        }

                        break;
                    }

                    attempt++;
                }
            }
        }
    }

    //Write draw areas
    WriteInt(numdrawareas, mapfile);

    for (int m = 0; m < numdrawareas; m++) {
        WriteInt(drawareas[m].x, mapfile);
        WriteInt(drawareas[m].y, mapfile);
        WriteInt(drawareas[m].w, mapfile);
        WriteInt(drawareas[m].h, mapfile);
    }

    //Write the number of blocks we have supplement info for
    WriteInt(iBlockCount, mapfile);

    for (j = 0; j < MAPHEIGHT; j++) {
        for (i = 0; i < MAPWIDTH; i++) {
            if (objectdata[i][j].iType == 1 || objectdata[i][j].iType == 15) { //powerup or view block
                WriteByteFromShort(i, mapfile);
                WriteByteFromShort(j, mapfile);

                WriteByteFromShort(NUM_BLOCK_SETTINGS, mapfile);
                for (short iSetting = 0; iSetting < NUM_BLOCK_SETTINGS; iSetting++)
                    WriteByteFromShort(objectdata[i][j].iSettings[iSetting], mapfile);
            }
        }
    }

    //Write mode item locations like flags and race goals
    WriteInt(iNumRaceGoals, mapfile);
    for (j = 0; j < iNumRaceGoals; j++) {
        WriteInt(racegoallocations[j].x, mapfile);
        WriteInt(racegoallocations[j].y, mapfile);
    }

    WriteInt(iNumFlagBases, mapfile);
    for (j = 0; j < iNumFlagBases; j++) {
        WriteInt(flagbaselocations[j].x, mapfile);
        WriteInt(flagbaselocations[j].y, mapfile);
    }

    fclose(mapfile);

#if defined(__MACOSX__)
    chmod(file.c_str(), S_IRWXU | S_IRWXG | S_IROTH);
#endif

    cout << "done" << endl;

    /*
    	//Save thumbnail
    	char szSaveThumbnail[256];
    	strcpy(szSaveThumbnail, "maps/cache/");
    	char * pszSaveThumbnail = szSaveThumbnail + strlen(szSaveThumbnail);
    	GetNameFromFileName(pszSaveThumbnail, file.c_str());

    #ifdef PNG_SAVE_FORMAT
    	strcat(szSaveThumbnail, ".png");
    #else
    	strcat(szSaveThumbnail, ".bmp");
    #endif

    	saveThumbnail(convertPath(szSaveThumbnail), true);
    */
}

SDL_Surface * CMap::createThumbnailSurface(bool fUseClassicPack)
{
    SDL_Surface * sThumbnail = SDL_CreateRGBSurface(screen->flags, 160, 120, 16, 0, 0, 0, 0);

    char localSzBackgroundFile[128];
    std::string path;

    if (fUseClassicPack) {
        sprintf(localSzBackgroundFile, "gfx/packs/Classic/backgrounds/%s", g_map->szBackgroundFile);
        path = convertPath(localSzBackgroundFile);

        //if the background file doesn't exist, use the classic background
        if (!File_Exists(path))
            path = convertPath("gfx/packs/Classic/backgrounds/Land_Classic.png");
    } else {
        sprintf(localSzBackgroundFile, "gfx/packs/backgrounds/%s", g_map->szBackgroundFile);
        path = convertPath(localSzBackgroundFile, gamegraphicspacklist->current_name());

        //if the background file doesn't exist, use the classic background
        if (!File_Exists(path))
            path = convertPath("gfx/packs/backgrounds/Land_Classic.png", gamegraphicspacklist->current_name());
    }

    SDL_Surface * temp = IMG_Load(path.c_str());
    if (!temp) {
        printf("ERROR: Couldn't load thumbnail background: %s\n", IMG_GetError());
        return NULL;
    }

#ifdef __EMSCRIPTEN__
    SDL_Surface * sBackground = SDL_DisplayFormatAlpha(temp);
#else
    SDL_Surface * sBackground = SDL_DisplayFormat(temp);
#endif
    if (!sBackground) {
        printf("ERROR: Couldn't convert thumbnail background to diplay pixel format: %s\n", SDL_GetError());
        return NULL;
    }

    SDL_FreeSurface(temp);

	SDL_Rect srcRectBackground = {0, 0, smw->ScreenWidth, smw->ScreenHeight};
    SDL_Rect dstRectBackground = {0, 0, 160, 120};

    if (SDL_SCALEBLIT(sBackground, &srcRectBackground, sThumbnail, &dstRectBackground) < 0) {
        fprintf(stderr, "SDL_SoftStretch error: %s\n", SDL_GetError());
        return NULL;
    }

    SDL_FreeSurface(sBackground);

    preDrawPreviewBackground(sThumbnail, true);
    preDrawPreviewBlocks(sThumbnail, true);
    preDrawPreviewMapItems(sThumbnail, true);
    drawThumbnailHazards(sThumbnail);
    drawThumbnailPlatforms(sThumbnail);
    preDrawPreviewForeground(sThumbnail, true);
    preDrawPreviewWarps(sThumbnail, true);

    return sThumbnail;
}

//Save thumbnail image
void CMap::saveThumbnail(const std::string &sFile, bool fUseClassicPack)
{
    SDL_Surface * sThumbnail = createThumbnailSurface(fUseClassicPack);

    if (!sThumbnail)
        return;

    //Save the screenshot with the same name as the map file

#ifdef PNG_SAVE_FORMAT
    IMG_SavePNG(sThumbnail, sFile.c_str());
#else
    SDL_SaveBMP(sThumbnail, sFile.c_str());
#endif

    SDL_FreeSurface(sThumbnail);
}

void CMap::calculatespawnareas(short iType, bool fUseTempBlocks, bool fIgnoreDeath)
{
    bool usedtile[MAPWIDTH][MAPHEIGHT];
    short i, j;

    for (j = 0; j < MAPHEIGHT; j++)
        for (i = 0; i < MAPWIDTH; i++)
            usedtile[i][j] = false;

    //Figure out areas where spawning is allowed
    for (j = 0; j < MAPHEIGHT; j++) {
        for (i = 0; i < MAPWIDTH; i++) {
            bool fUsed = false;

            if (j >= 13 || j == 0)
                fUsed = true;

            if (!fUsed) {
                if (nospawn[iType][i][j])
                    fUsed = true;
            }

            if (!fUsed && (mapdatatop[i][j].iFlags & tile_flag_solid)) {
                fUsed = true;
            }

            //If there is a block there
            if (!fUsed) {
                if (objectdata[i][j].iType != -1)
                    fUsed = true;
            }

            //If this is a player spawn area, have extra restrictions like don't spawn over spikes etc.
            if (0 <= iType && 4 >= iType) {
                //If there is a death tile directly above
                if (!fUsed) {
                    if (j > 0) {
                        if (mapdatatop[i][j - 1].iFlags & tile_flag_death_on_bottom) {
                            fUsed = true;
                        }
                    }
                }

                //If there is a death tile anywhere below this tile
                if (!fUsed && !fIgnoreDeath) {
                    int m;
                    for (m = j; m < MAPHEIGHT; m++) {
                        TileType type = mapdatatop[i][m].iType;
                        int flags = mapdatatop[i][m].iFlags;
                        short objBlock = objectdata[i][m].iType;

                        if (m == j && (flags & tile_flag_solid_on_top))
                            continue;

                        if (type == tile_death_on_top || type == tile_death || type == tile_super_death_top || type == tile_super_death || type == tile_player_death) {
                            fUsed = true;
                            break;
                        }

                        if (fUseTempBlocks) {
                            if ((type != tile_nonsolid && type != tile_gap) || objBlock != -1) {
                                break;
                            }
                        } else {
                            //Ignore the blocks that might not be there anymore (destroyed, turned off, etc)
                            if ((type != tile_nonsolid && type != tile_gap) || (objBlock != -1 && objBlock != 0 && objBlock != 2 && objBlock != 6 && (objBlock < 11 || objBlock > 14) && objBlock != 16 && objBlock < 19)) {
                                break;
                            }
                        }
                    }

                    //If we didn't find a landing spot from here to bottom, then try to wrap around and see
                    if (m == MAPHEIGHT) {
                        for (m = 0; m < j; m++) {
                            TileType type = mapdatatop[i][m].iType;
                            short objBlock = objectdata[i][m].iType;

                            if (type == tile_death_on_top || type == tile_death || type == tile_super_death_top || type == tile_super_death || type == tile_player_death) {
                                fUsed = true;
                                break;
                            }

                            if (fUseTempBlocks) {
                                if ((type != tile_nonsolid && type != tile_gap) || objBlock != -1) {
                                    break;
                                }
                            } else {
                                if ((type != tile_nonsolid && type != tile_gap) || (objBlock != -1 && objBlock != 0 && objBlock != 2 && objBlock != 6 && (objBlock < 11 || objBlock > 14) && objBlock != 16 && objBlock < 19)) {
                                    break;
                                }
                            }
                        }

                        //if (m == j)
                        //fUsed = true;
                    }
                }
            }

            usedtile[i][j] = fUsed;
        }
    }

    //Figure out where the spawn areas are
    numspawnareas[iType] = 0;
    for (j = 0; j < MAPHEIGHT; j++) {
        for (i = 0; i < MAPWIDTH; i++) {
            if (!usedtile[i][j]) {
                bool fDownDone = false;
                bool fRightDone = false;

                short downsize = j + 1;
                short rightsize = i + 1;
                short attempt = 1;

                while (true) {
                    if (!fRightDone) {
                        for (int right = i; right < rightsize; right++) {
                            if (right >= MAPWIDTH || j + attempt >= MAPHEIGHT || usedtile[right][j + attempt])
                                fRightDone = true;
                        }

                        if (!fRightDone)
                            downsize++;
                    }

                    if (!fDownDone) {
                        for (int down = j; down < downsize; down++) {
                            if (i + attempt >= MAPWIDTH || down >= MAPHEIGHT || usedtile[i + attempt][down])
                                fDownDone = true;
                        }

                        if (!fDownDone)
                            rightsize++;
                    }

                    if (fDownDone && fRightDone) {
                        if (numspawnareas[iType] < MAXSPAWNAREAS) {
                            spawnareas[iType][numspawnareas[iType]].left = i;
                            spawnareas[iType][numspawnareas[iType]].top = j;
                            spawnareas[iType][numspawnareas[iType]].width = rightsize - 1 - i;
                            spawnareas[iType][numspawnareas[iType]].height = downsize - 1 - j;
                            spawnareas[iType][numspawnareas[iType]].size = (rightsize - i) * (downsize - j);

                            numspawnareas[iType]++;
                        }

                        for (short down = j; down < downsize; down++) {
                            for (short right = i; right < rightsize; right++) {
                                usedtile[right][down] = true;
                            }
                        }

                        break;
                    }

                    attempt++;
                }
            }
        }
    }
}

void CMap::AnimateTiles(short iFrame)
{
    //If the end index for this frame is the same as the next frame, that means there are 0
    //animated tiles to paint this frame so we should skip it
    if (iAnimatedVectorIndices[iFrame] == iAnimatedVectorIndices[iFrame + 1])
        return;

    //For each animated tile we are painting this frame, paint it to the map tiles or a platform
    for (short iTile = iAnimatedVectorIndices[iFrame]; iTile < iAnimatedVectorIndices[iFrame + 1]; iTile++) {
        AnimatedTile * tile = animatedtiles[iTile];
        SDL_Rect * rDst = &(tile->rDest);

        if (tile->fBackgroundAnimated) {
            SDL_BlitSurface(animatedTilesSurface, &(tile->rAnimationSrc[0][iTileAnimationFrame]), animatedBackmapSurface, rDst);
        }

        if (tile->fForegroundAnimated) {
            SDL_BlitSurface(animatedTilesSurface, &(tile->rAnimationSrc[1][iTileAnimationFrame]), animatedFrontmapSurface, rDst);
        }

        if (tile->pPlatform) {
            SDL_BlitSurface(animatedTilesSurface, &(tile->rAnimationSrc[0][iTileAnimationFrame]), tile->pPlatform->sSurface[g_iCurrentDrawIndex], rDst);
        }
    }
}

void CMap::draw(SDL_Surface *targetSurface, int layer)
{
    int i, j;

    //draw left to right full vertical
    bltrect.x = 0;
    for (i = 0; i < MAPWIDTH; i++) {
        bltrect.y = -TILESIZE;	//this is okay, see

        for (j = 0; j < MAPHEIGHT; j++) {
            bltrect.y += TILESIZE;	// here

            TilesetTile * tile = &mapdata[i][j][layer];

            //If there is no tile to draw, continue
            if (tile->iID == TILESETNONE)
                continue;

            //If this is an animated tile, then setup an animated tile struct for use in drawing
            if (tile->iID >= 0) {
                g_tilesetmanager->Draw(targetSurface, tile->iID, 0, tile->iCol, tile->iRow, i, j);
                //SDL_BlitSurface(rm->spr_maptiles[0].getSurface(), &g_tilesetmanager->rRects[0][tile->iCol][tile->iRow], targetSurface, &bltrect);
            } else if (tile->iID == TILESETANIMATED) {
                //See if we already have this tile
                bool fNeedNewAnimatedTile = true;

                short iNewTileId = j * MAPWIDTH + i;
                std::vector<AnimatedTile*>::iterator iter = animatedtiles.begin(), lim = animatedtiles.end();
                while (iter != lim) {
                    if (iNewTileId == (*iter)->id) {
                        fNeedNewAnimatedTile = false;
                        break;
                    }

                    ++iter;
                }

                if (fNeedNewAnimatedTile) {
                    AnimatedTile * animatedtile = new AnimatedTile();
                    animatedtile->id = iNewTileId;

                    animatedtile->fBackgroundAnimated = false;
                    animatedtile->fForegroundAnimated = false;
                    animatedtile->pPlatform = NULL;

                    for (short iLayer = 0; iLayer < 4; iLayer++) {
                        TilesetTile * layerTile = &mapdata[i][j][iLayer];
                        TilesetTile * toTile = &animatedtile->layers[iLayer];

                        toTile->iID = layerTile->iID;
                        toTile->iCol = layerTile->iCol;
                        toTile->iRow = layerTile->iRow;

                        if (layerTile->iID >= 0) { //If it is part of a tileset
                            gfx_setrect(&(animatedtile->rSrc[iLayer][0]), layerTile->iCol << 5, layerTile->iRow << 5, TILESIZE, TILESIZE);
                        } else if (layerTile->iID == TILESETANIMATED) {
                            for (short iRect = 0; iRect < 4; iRect++) {
                                gfx_setrect(&(animatedtile->rSrc[iLayer][iRect]), (iRect + (layerTile->iCol << 2)) << 5, layerTile->iRow << 5, TILESIZE, TILESIZE);
                            }

                            //Background is animated if it is a background layer or if it is a foreground layer and we are not displaying the foreground
                            if (iLayer < 2 || !game_values.toplayer)
                                animatedtile->fBackgroundAnimated = true;

                            //Foreground is animated if it is a foreground layer and we are displaying the foreground
                            if (iLayer >= 2 && game_values.toplayer)
                                animatedtile->fForegroundAnimated = true;
                        }
                    }

                    gfx_setrect(&(animatedtile->rDest), bltrect.x, bltrect.y, TILESIZE, TILESIZE);
                    animatedtiles.push_back(animatedtile);
                }
            } else if (tile->iID == TILESETUNKNOWN) { //Draw red X where tile should be
                SDL_BlitSurface(rm->spr_unknowntile[0].getSurface(), &g_tilesetmanager->rRects[0][0][0], targetSurface, &bltrect);
            }
        }

        bltrect.x += TILESIZE;
    }

    bltrect.x = 0;
    bltrect.y = 0;
	bltrect.w = smw->ScreenWidth;
    bltrect.h = smw->ScreenHeight;
}

void CMap::addPlatformAnimatedTiles()
{
    for (short iPlatform = 0; iPlatform < iNumPlatforms; iPlatform++) {
        MovingPlatform * platform = platforms[iPlatform];

        short iHeight = platform->iTileHeight;
        short iWidth = platform->iTileWidth;
        TilesetTile ** tiles = platform->iTileData;

        short iDestX = 0;
        short iDestY = 0;

        for (short iRow = 0; iRow < iHeight; iRow++) {
            for (short iCol = 0; iCol < iWidth; iCol++) {
                if (tiles[iCol][iRow].iID == TILESETANIMATED) {
                    AnimatedTile * animatedtile = new AnimatedTile();
                    animatedtile->id = -1;  //we don't want this ID to collide with an animated map tile

                    animatedtile->fBackgroundAnimated = false;
                    animatedtile->fForegroundAnimated = false;
                    animatedtile->pPlatform = platform;

                    TilesetTile * tile = &tiles[iCol][iRow];
                    TilesetTile * toTile = &animatedtile->layers[0];

                    toTile->iID = tile->iID;
                    toTile->iCol = tile->iCol;
                    toTile->iRow = tile->iRow;

                    for (short iRect = 0; iRect < 4; iRect++) {
                        gfx_setrect(&(animatedtile->rSrc[0][iRect]), (iRect + (tile->iCol << 2)) << 5, tile->iRow << 5, TILESIZE, TILESIZE);
                    }

                    gfx_setrect(&(animatedtile->rDest), iDestX, iDestY, TILESIZE, TILESIZE);
                    animatedtiles.push_back(animatedtile);

                }

                iDestX += TILESIZE;
            }

            iDestY += TILESIZE;
            iDestX = 0;
        }
    }
}

void CMap::drawThumbnailHazards(SDL_Surface * targetSurface)
{
    blitdest = targetSurface;

    for (short iHazard = 0; iHazard < iNumMapHazards; iHazard++) {
        DrawMapHazard(&maphazards[iHazard], 2, false);
    }

    blitdest = screen;
}

void CMap::drawThumbnailPlatforms(SDL_Surface * targetSurface)
{
    blitdest = targetSurface;

    for (short iPlatform = 0; iPlatform < iNumPlatforms; iPlatform++) {
        MovingPlatform * platform = platforms[iPlatform];
        MovingPlatformPath * basepath = platform->pPath;

        if (basepath->iType == 0) {
            StraightPath * path = (StraightPath*) basepath;
            DrawPlatform(path->iType, platform->iTileData, ((short)path->dPathPointX[0]) << 1, ((short)path->dPathPointY[0]) << 1, ((short)path->dPathPointX[1]) << 1, ((short)path->dPathPointY[1]) << 1, 0.0f, 0.0f, 0.0f, 2, platform->iTileWidth, platform->iTileHeight, true, true);
        } else if (basepath->iType == 1) {
            StraightPathContinuous * path = (StraightPathContinuous*) basepath;
            DrawPlatform(path->iType, platform->iTileData, ((short)path->dPathPointX[0]) << 1, ((short)path->dPathPointY[0]) << 1, 0, 0, path->dAngle, 0.0f, 0.0f, 2, platform->iTileWidth, platform->iTileHeight, true, true);
        } else if (basepath->iType == 2) {
            EllipsePath * path = (EllipsePath*) basepath;
            DrawPlatform(path->iType, platform->iTileData, ((short)path->dPathPointX[0]) << 1, ((short)path->dPathPointY[0]) << 1, 0, 0, path->dStartAngle, path->dRadiusX * 2, path->dRadiusY * 2, 2, platform->iTileWidth, platform->iTileHeight, true, true);
        }
    }

    blitdest = screen;
}

void CMap::preDrawPreviewWarps(SDL_Surface * targetSurface, bool fThumbnail)
{
    short iTileSize = 16;
    short iScreenshotSize = 0;

    if (fThumbnail) {
        iTileSize = 8;
        iScreenshotSize = 1;
    }

    for (int j = 0; j < MAPHEIGHT; j++) {
        for (int i = 0; i < MAPWIDTH; i++) {
            Warp * wWarp = &g_map->warpdata[i][j];

            if (wWarp->connection != -1) {
                SDL_Rect rSrc = {wWarp->connection * iTileSize, wWarp->direction * iTileSize, iTileSize, iTileSize};
                SDL_Rect rDst = {i * iTileSize, j * iTileSize, iTileSize, iTileSize};

                SDL_BlitSurface(rm->spr_thumbnail_warps[iScreenshotSize].getSurface(), &rSrc, targetSurface, &rDst);
            }
        }
    }
}

void CMap::preDrawPreviewMapItems(SDL_Surface * targetSurface, bool fThumbnail)
{
    short iTileSize = 16;
    short iScreenshotSize = 0;

    if (fThumbnail) {
        iTileSize = 8;
        iScreenshotSize = 1;
    }

    for (int j = 0; j < iNumMapItems; j++) {
        SDL_Rect rSrc = {mapitems[j].itype * iTileSize, 0, iTileSize, iTileSize};
        SDL_Rect rDst = {mapitems[j].ix * iTileSize, mapitems[j].iy * iTileSize, iTileSize, iTileSize};

        SDL_BlitSurface(rm->spr_thumbnail_mapitems[iScreenshotSize].getSurface(), &rSrc, targetSurface, &rDst);
    }
}

void CMap::preDrawPreviewBackground(SDL_Surface * targetSurface, bool fThumbnail)
{
    drawPreview(targetSurface, 0, fThumbnail);
    smallDelay(); //Sleeps to help the music from skipping

    drawPreview(targetSurface, 1, fThumbnail);
    smallDelay();

    if (!game_values.toplayer) {
        drawPreview(targetSurface, 2, fThumbnail);
        smallDelay();

        drawPreview(targetSurface, 3, fThumbnail);
        smallDelay();
    }

    //drawPreviewBlocks(targetSurface, fThumbnail);
}

void CMap::preDrawPreviewBackground(gfxSprite * background, SDL_Surface * targetSurface, bool fThumbnail)
{
    SDL_Rect srcrect;
    srcrect.x = 0;
    srcrect.y = 0;
	srcrect.w = smw->ScreenWidth;
    srcrect.h = smw->ScreenHeight;

    SDL_Rect dstrect;
    dstrect.x = 0;
    dstrect.y = 0;

    if (fThumbnail) {
        dstrect.w = 160;
        dstrect.h = 120;
    } else {
		dstrect.w = smw->ScreenWidth/2;
        dstrect.h = smw->ScreenHeight/2;
    }

    if (SDL_SCALEBLIT(background->getSurface(), &srcrect, targetSurface, &dstrect) < 0) {
        fprintf(stderr, "SDL_SoftStretch error: %s\n", SDL_GetError());
        return;
    }

    smallDelay();
    preDrawPreviewBackground(targetSurface, fThumbnail);
}

void CMap::preDrawPreviewBlocks(SDL_Surface * targetSurface, bool fThumbnail)
{
    if (!fThumbnail) {
        SDL_FillRect(targetSurface, NULL, SDL_MapRGB(targetSurface->format, 255, 0, 255));
        SDL_SETCOLORKEY(targetSurface, SDL_FALSE, SDL_MapRGB(targetSurface->format, 255, 0, 255));
        smallDelay();
    }

    drawPreviewBlocks(targetSurface, fThumbnail);
}


void CMap::preDrawPreviewForeground(SDL_Surface * targetSurface, bool fThumbnail)
{
    if (!fThumbnail) {
        SDL_FillRect(targetSurface, NULL, SDL_MapRGB(targetSurface->format, 255, 0, 255));
        SDL_SETCOLORKEY(targetSurface, SDL_FALSE, SDL_MapRGB(targetSurface->format, 255, 0, 255));
        smallDelay();
    }

    if (!game_values.toplayer)
        return;

    drawPreview(targetSurface, 2, fThumbnail);
    smallDelay();
    drawPreview(targetSurface, 3, fThumbnail);
}

void CMap::drawPreview(SDL_Surface * targetSurface, int layer, bool fThumbnail)
{
    int i, j;

    //draw left to right full vertical
    short iTilesetIndex = 1;

    if (fThumbnail)
        iTilesetIndex = 2;

    for (i = 0; i < MAPWIDTH; i++) {
        for (j = 0; j < MAPHEIGHT; j++) {
            TilesetTile * tile = &mapdata[i][j][layer];
            if (tile->iID == TILESETNONE)
                continue;

            //Handle drawing preview for animated tiles
            if (tile->iID >= 0) {
                g_tilesetmanager->Draw(targetSurface, tile->iID, iTilesetIndex, tile->iCol, tile->iRow, i, j);
                //SDL_BlitSurface(rm->spr_maptiles[iTilesetIndex].getSurface(), &g_tilesetmanager->rRects[iTilesetIndex][tile->iCol][tile->iRow], targetSurface, &rectDst);
            } else if (tile->iID == TILESETANIMATED) {
                SDL_BlitSurface(rm->spr_tileanimation[iTilesetIndex].getSurface(), &g_tilesetmanager->rRects[iTilesetIndex][tile->iCol << 2][tile->iRow], targetSurface, &g_tilesetmanager->rRects[iTilesetIndex][i][j]);
            } else if (tile->iID == TILESETUNKNOWN) {
                SDL_BlitSurface(rm->spr_unknowntile[iTilesetIndex].getSurface(), &g_tilesetmanager->rRects[iTilesetIndex][0][0], targetSurface, &g_tilesetmanager->rRects[iTilesetIndex][i][j]);
            }
        }
    }
}

void CMap::drawPreviewBlocks(SDL_Surface * targetSurface, bool fThumbnail)
{
    int i, j, ts;

    short iBlockSize = PREVIEWTILESIZE;

    if (fThumbnail)
        iBlockSize = THUMBTILESIZE;

    SDL_Rect rectDst;
    rectDst.w = iBlockSize;
    rectDst.h = iBlockSize;

    SDL_Rect rectSrc;
    rectSrc.w = iBlockSize;
    rectSrc.h = iBlockSize;

    rectDst.x = 0;
    for (i = 0; i < MAPWIDTH; i++) {
        rectDst.y = -iBlockSize;	//this is okay, see

        for (j = 0; j < MAPHEIGHT; j++) {
            rectDst.y += iBlockSize;	// here

            ts = objectdata[i][j].iType;
            if (ts == -1)
                continue;

            //Don't draw hidden blocks
            if (objectdata[i][j].fHidden)
                continue;

            rectSrc.x = (Sint16)(ts * iBlockSize);
            rectSrc.y = 0;

            //Draw the turned off switch blocks too
            if (ts >= 7 && ts <= 10) {
                if (iSwitches[(ts - 7) % 4] == 0)
                    rectSrc.y = iBlockSize;
            } else if (ts >= 11 && ts <= 14) {
                if (objectdata[i][j].iSettings[0] == 0)
                    rectSrc.y = iBlockSize;
            } else if (ts >= 15 && ts <= 19) {
                rectSrc.x = iBlockSize * (ts - 15);
                rectSrc.y = iBlockSize;
            } else if (ts >= 20 && ts <= 29) {
                rectSrc.x = iBlockSize * (ts - 20);
                rectSrc.y = iBlockSize << 1;
            }

            if (fThumbnail)
                SDL_BlitSurface(rm->spr_blocks[2].getSurface(), &rectSrc, targetSurface, &rectDst);
            else
                SDL_BlitSurface(rm->spr_blocks[1].getSurface(), &rectSrc, targetSurface, &rectDst);
        }

        rectDst.x += iBlockSize;
    }
}

void CMap::predrawbackground(gfxSprite &background, gfxSprite &mapspr)
{
    SDL_Rect r;
    r.x = 0;
    r.y = 0;
	r.w = smw->ScreenWidth;
	r.h = smw->ScreenHeight;

    SDL_BlitSurface(background.getSurface(), NULL, mapspr.getSurface(), &r);

    draw(mapspr.getSurface(), 0);
    draw(mapspr.getSurface(), 1);

    if (!game_values.toplayer) {
        draw(mapspr.getSurface(), 2);
        draw(mapspr.getSurface(), 3);
    }

    //Add animated tile objects for each animated tile in a platform (to be used later for drawing)
    addPlatformAnimatedTiles();

    /*
    //Draws the spawn areas
    SDL_Rect dest;
    dest.w = 32;
    dest.h = 32;
    short iType = 1; //use [5] for item spawn areas

    for (int m = 0; m < numspawnareas[iType]; m++)
    {
    	dest.x = spawnareas[iType][m].left << 5;
    	dest.y = spawnareas[iType][m].top << 5;

    	dest.w = (spawnareas[iType][m].width << 5) + TILESIZE;
    	dest.h = (spawnareas[iType][m].height << 5) + TILESIZE;

        //int color = 0x00 << 24 | RANDOM_INT(256) << 16 | RANDOM_INT(256) << 8 | RANDOM_INT(256);
    	int color = 0x128 << 24 | 0x255 << 8;

    	SDL_FillRect(mapspr.getSurface(), &dest, color);		//fill empty area with black
    }
    */
}

void CMap::predrawforeground(gfxSprite &foregroundspr)
{
    SDL_FillRect(foregroundspr.getSurface(), NULL, SDL_MapRGB(foregroundspr.getSurface()->format, 255, 0, 255));
    SDL_SETCOLORKEY(foregroundspr.getSurface(), SDL_FALSE, SDL_MapRGB(foregroundspr.getSurface()->format, 255, 0, 255));

    draw(foregroundspr.getSurface(), 2);
    draw(foregroundspr.getSurface(), 3);
}

void CMap::SetupAnimatedTiles()
{
    iAnimatedBackgroundLayers = 2;
    if (!game_values.toplayer)
        iAnimatedBackgroundLayers = 4;

    g_iCurrentDrawIndex = 0;

    iAnimatedTileCount = animatedtiles.size();

    if (animatedTilesSurface) {
        SDL_FreeSurface(animatedTilesSurface);
        animatedTilesSurface = NULL;
    }

    if (iAnimatedTileCount > 0) {
        SDL_Surface * backgroundSurface = rm->spr_background.getSurface();
        SDL_Surface * animatedTileSrcSurface = rm->spr_tileanimation[0].getSurface();

        animatedFrontmapSurface = rm->spr_frontmap[g_iCurrentDrawIndex].getSurface();
        animatedBackmapSurface = rm->spr_backmap[g_iCurrentDrawIndex].getSurface();
        animatedTilesSurface = SDL_CreateRGBSurface(screen->flags, 1024, 1024, screen->format->BitsPerPixel, 0, 0, 0, 0);

        int iTransparentColor = SDL_MapRGB(animatedTilesSurface->format, 255, 0, 255);

        std::vector<AnimatedTile*>::iterator iter = animatedtiles.begin(), lim = animatedtiles.end();

        bool fSrcSurfaceFull = false;

        SDL_Rect rDst = {0, 0, 32, 32};
        while (iter != lim && !fSrcSurfaceFull) {
            AnimatedTile * tile = *iter;
            SDL_Rect * rSrc = &(tile->rDest);

            //If the background layer has an animated tile, then create the set of 4 images that will be
            //drawn to this tile during the gameplay (gfx optimization by only drawing from animatedTilesSurface)
            if (tile->fBackgroundAnimated) {
                for (short sTileAnimationFrame = 0; sTileAnimationFrame < 4; sTileAnimationFrame++) {
                    gfx_setrect(&tile->rAnimationSrc[0][sTileAnimationFrame], &rDst);

                    SDL_BlitSurface(backgroundSurface, rSrc, animatedTilesSurface, &rDst);

                    for (short iLayer = 0; iLayer < iAnimatedBackgroundLayers; iLayer++) {
                        TilesetTile * tilesetTile = &tile->layers[iLayer];
                        if (tilesetTile->iID >= 0) {
                            SDL_BlitSurface(g_tilesetmanager->GetTileset(tilesetTile->iID)->GetSurface(0), &(tile->rSrc[iLayer][0]), animatedTilesSurface, &rDst);
                        } else if (tilesetTile->iID == TILESETANIMATED) {
                            SDL_BlitSurface(animatedTileSrcSurface, &(tile->rSrc[iLayer][sTileAnimationFrame]), animatedTilesSurface, &rDst);
                        } else if (tilesetTile->iID == TILESETUNKNOWN) {
                            SDL_BlitSurface(rm->spr_unknowntile[0].getSurface(), &g_tilesetmanager->rRects[0][0][0], animatedTilesSurface, &rDst);
                        }
                    }

                    rDst.x += 32;
                    if (rDst.x >= 1024) {
                        rDst.x = 0;
                        rDst.y += 32;
                        if (rDst.y >= 1024) {
                            tile->fForegroundAnimated = false;
                            fSrcSurfaceFull = true;
                            break;
                        }
                    }
                }
            }

            //If we have run out of room on the animatedTilesSurface surface, then stop reading animated tiles
            if (fSrcSurfaceFull) {
                iter++;
                break;
            }

            if (tile->fForegroundAnimated) {
                for (short sTileAnimationFrame = 0; sTileAnimationFrame < 4; sTileAnimationFrame++) {
                    gfx_setrect(&tile->rAnimationSrc[1][sTileAnimationFrame], &rDst);

                    SDL_FillRect(animatedTilesSurface, &rDst, iTransparentColor);

                    for (short iLayer = 2; iLayer < 4; iLayer++) {
                        TilesetTile * tilesetTile = &tile->layers[iLayer];
                        if (tilesetTile->iID >= 0) {
                            SDL_BlitSurface(g_tilesetmanager->GetTileset(tilesetTile->iID)->GetSurface(0), &(tile->rSrc[iLayer][0]), animatedTilesSurface, &rDst);
                        } else if (tilesetTile->iID == TILESETANIMATED) {
                            SDL_BlitSurface(animatedTileSrcSurface, &(tile->rSrc[iLayer][sTileAnimationFrame]), animatedTilesSurface, &rDst);
                        } else if (tilesetTile->iID == TILESETUNKNOWN) {
                            SDL_BlitSurface(rm->spr_unknowntile[0].getSurface(), &g_tilesetmanager->rRects[0][0][0], animatedTilesSurface, &rDst);
                        }
                    }

                    rDst.x += 32;
                    if (rDst.x >= 1024) {
                        rDst.x = 0;
                        rDst.y += 32;
                        if (rDst.y >= 1024) {
                            fSrcSurfaceFull = true;
                            break;
                        }
                    }
                }
            }

            //If we have run out of room on the animatedTilesSurface surface, then stop reading animated tiles
            if (fSrcSurfaceFull) {
                iter++;
                break;
            }

            if (tile->pPlatform) {
                for (short sTileAnimationFrame = 0; sTileAnimationFrame < 4; sTileAnimationFrame++) {
                    gfx_setrect(&tile->rAnimationSrc[0][sTileAnimationFrame], &rDst);

                    SDL_FillRect(animatedTilesSurface, &rDst, iTransparentColor);

                    TilesetTile * tilesetTile = &tile->layers[0];
                    if (tilesetTile->iID == TILESETANIMATED) {
                        SDL_BlitSurface(animatedTileSrcSurface, &(tile->rSrc[0][sTileAnimationFrame]), animatedTilesSurface, &rDst);
                    } else {
                        cout << endl << " ERROR: A nonanimated platform tile was added to the animated tile list" << endl;
                    }

                    rDst.x += 32;
                    if (rDst.x >= 1024) {
                        rDst.x = 0;
                        rDst.y += 32;
                        if (rDst.y >= 1024) {
                            fSrcSurfaceFull = true;
                            break;
                        }
                    }
                }
            }

            ++iter;
        }

        //Turn off animation for the tiles that couldn't fit onto our animation page (256 tiles max)
        while (iter != lim) {
            AnimatedTile * tile = *iter;

            tile->fBackgroundAnimated = false;
            tile->fForegroundAnimated = false;
            tile->pPlatform = NULL;

            ++iter;
        }

        //Figure out how many animated tiles we will be painting a frame (evenly distribute the painting as much as possible)
        for (short iAnimatedFrame = 0; iAnimatedFrame <= NUM_FRAMES_BETWEEN_TILE_ANIMATION; iAnimatedFrame++)
            iAnimatedVectorIndices[iAnimatedFrame] = (iAnimatedFrame * iAnimatedTileCount) / NUM_FRAMES_BETWEEN_TILE_ANIMATION;

        //Draw all animated tiles to the front buffer
        for (short iFrame = 0; iFrame < NUM_FRAMES_BETWEEN_TILE_ANIMATION; iFrame++)
            AnimateTiles(iFrame);

        //Setup the back buffer to draw animated tiles to each frame
        //This is flipped every NUM_FRAMES_BETWEEN_TILE_ANIMATION to be the displayed surface
        animatedFrontmapSurface = rm->spr_frontmap[1 - g_iCurrentDrawIndex].getSurface();
        animatedBackmapSurface = rm->spr_backmap[1 - g_iCurrentDrawIndex].getSurface();

        //Draw the first set of animated tiles to the back buffer
        AnimateTiles(0);
    }
}

void CMap::updatePlatforms()
{
    for (short iPlatform = 0; iPlatform < iNumPlatforms; iPlatform++) {
        platforms[iPlatform]->update();
    }

    std::list<MovingPlatform*>::iterator iter = tempPlatforms.begin(), lim = tempPlatforms.end();

    while (iter != lim) {
        if ((*iter)->fDead) {
            delete (*iter);

            iter = tempPlatforms.erase(iter);
            lim = tempPlatforms.end();
        } else {
            (*iter)->update();
            ++iter;
        }
    }
}

void CMap::drawPlatforms(short iLayer)
{
    std::list<MovingPlatform*>::iterator iterate = platformdrawlayer[iLayer].begin(), lim = platformdrawlayer[iLayer].end();

    while (iterate != lim) {
        (*iterate)->draw();
        iterate++;
    }

    if (iLayer == 2) {
        std::list<MovingPlatform*>::iterator iterateTemps = tempPlatforms.begin(), limTemps = tempPlatforms.end();
        while (iterateTemps != limTemps) {
            (*iterateTemps)->draw();
            iterateTemps++;
        }
    }
}

void CMap::drawPlatforms(short iOffsetX, short iOffsetY, short iLayer)
{
    std::list<MovingPlatform*>::iterator iterate = platformdrawlayer[iLayer].begin(), lim = platformdrawlayer[iLayer].end();

    while (iterate != lim) {
        (*iterate)->draw(iOffsetX, iOffsetY);
        iterate++;
    }
}

void CMap::movingPlatformCollision(IO_MovingObject * object)
{
    for (short iPlatform = 0; iPlatform < iNumPlatforms; iPlatform++) {
        platforms[iPlatform]->collide(object);
    }

    std::list<MovingPlatform*>::iterator iterateAll = tempPlatforms.begin(), lim = tempPlatforms.end();
    while (iterateAll != lim) {
        (*iterateAll)->collide(object);
        iterateAll++;
    }
}

bool CMap::movingPlatformCheckSides(IO_MovingObject * object)
{
    bool fRet = false;
    for (short iPlatform = 0; iPlatform < iNumPlatforms; iPlatform++) {
        fRet |= platforms[iPlatform]->collision_detection_check_sides(object);
    }

    std::list<MovingPlatform*>::iterator iterateAll = tempPlatforms.begin(), lim = tempPlatforms.end();
    while (iterateAll != lim) {
        fRet |= (*iterateAll)->collision_detection_check_sides(object);
        iterateAll++;
    }

    return fRet;
}

void CMap::resetPlatforms()
{
    for (short iPlatform = 0; iPlatform < iNumPlatforms; iPlatform++) {
        platforms[iPlatform]->ResetPath();
    }

    std::list<MovingPlatform*>::iterator iter = tempPlatforms.begin(), lim = tempPlatforms.end();

    while (iter != lim) {
        delete (*iter);
        ++iter;
    }

    tempPlatforms.clear();
}

void CMap::lockconnection(int connection)
{
    //Lock all warp connections
    if (connection == -1) {
        for (short iConnection = 0; iConnection <= maxConnection; iConnection++) {
            warplocked[iConnection] = true;
        }
    } else { //otherwise just lock the one connection
        warplocked[connection] = true;
    }
}

WarpExit * CMap::getRandomWarpExit(int connection, int currentID)
{
    int indices[MAXWARPS];
    int numIndices = 0;

    WarpExit * currentWarp = NULL;

    for (int k = 0; k < numwarpexits; k++) {
        if (warpexits[k].connection == connection) {
            if (warpexits[k].id == currentID)
                currentWarp = &warpexits[k];
            else
                indices[numIndices++] = k;
        }
    }

    if (numIndices == 0)
        return currentWarp;

    return &warpexits[indices[RANDOM_INT( numIndices)]];
}

void CMap::clearWarpLocks()
{
    for (short iConnection = 0; iConnection < 10; iConnection++) {
        warplocktimer[iConnection] = 0;
        warplocked[iConnection] = false;
    }

    for (short iWarpExit = 0; iWarpExit < numwarpexits; iWarpExit++) {
        warpexits[iWarpExit].locktimer = 0;
    }
}

void CMap::drawWarpLocks()
{
    for (int iWarpExit = 0; iWarpExit < numwarpexits; iWarpExit++) {
        if (warplocked[warpexits[iWarpExit].connection] || warpexits[iWarpExit].locktimer > 0) {
            rm->spr_warplock.draw(warpexits[iWarpExit].lockx, warpexits[iWarpExit].locky);
        }
    }
}

void CMap::update()
{
    //Unlock locked warps if the time is up
    for (short iConnection = 0; iConnection <= maxConnection; iConnection++) {
        if (warplocked[iConnection]) {
            if (++warplocktimer[iConnection] > game_values.warplocktime) {
                warplocked[iConnection] = false;
                warplocktimer[iConnection] = 0;
            }
        }
    }

    //If warp is individually locked, then reduce lock timer
    for (short iWarpExit = 0; iWarpExit < numwarpexits; iWarpExit++) {
        if (warpexits[iWarpExit].locktimer > 0)
            --warpexits[iWarpExit].locktimer;
    }

    //Animate the animated tiles
    if (++iTileAnimationTimer >= NUM_FRAMES_BETWEEN_TILE_ANIMATION) {
        iTileAnimationTimer = 0;

        //Flip front and back buffers
        animatedFrontmapSurface = rm->spr_frontmap[g_iCurrentDrawIndex].getSurface();
        animatedBackmapSurface = rm->spr_backmap[g_iCurrentDrawIndex].getSurface();

        g_iCurrentDrawIndex = 1 - g_iCurrentDrawIndex;

        if (++iTileAnimationFrame >= NUM_FRAMES_IN_TILE_ANIMATION)
            iTileAnimationFrame = 0;
    }

    //If there is at least 1 animated tile, then draw its animation
    if (iAnimatedTileCount > 0)
        AnimateTiles(iTileAnimationTimer);
}

bool CMap::findspawnpoint(short iType, short * x, short * y, short width, short height, bool tilealigned)
{
    if (totalspawnsize[iType] <= 0) {
		*x = RANDOM_INT(smw->ScreenWidth);
		*y = RANDOM_INT(smw->ScreenHeight * 0.87f);
        return true;
    }

    int spawnarea = RANDOM_INT(totalspawnsize[iType]);

    int currentsize = 0;
    for (int m = 0; m < numspawnareas[iType]; m++) {
        currentsize += spawnareas[iType][m].size;

        if (spawnarea >= currentsize)
            continue;

        short areawidth = (spawnareas[iType][m].width << 5) + TILESIZE;
        short areaheight = (spawnareas[iType][m].height << 5) + TILESIZE;

        if (width > areawidth || height > areaheight)
            continue;

        if (tilealigned) {
            short xoffset = spawnareas[iType][m].width;
            short yoffset = spawnareas[iType][m].height;

            if (xoffset > 0)
                xoffset = (short)RANDOM_INT(xoffset);

            if (yoffset > 0)
                yoffset = (short)RANDOM_INT(yoffset);

            *x = (xoffset << 5) + (spawnareas[iType][m].left << 5) + (TILESIZE >> 1) - (width >> 1);
            *y = (yoffset << 5) + (spawnareas[iType][m].top << 5) + (TILESIZE >> 1) - (height >> 1);
        } else {
            short xoffset = areawidth - width - 2;
            short yoffset = areaheight - height - 2;

            if (xoffset > 0)
                xoffset = (short)RANDOM_INT(xoffset) + 1;

            if (yoffset > 0)
                yoffset = (short)RANDOM_INT(yoffset) + 1;

            *x = xoffset + (spawnareas[iType][m].left << 5);
            *y = yoffset + (spawnareas[iType][m].top << 5);
        }

        break;
    }
    //Check to see if we are spawning into a platform
    for (short iPlatform = 0; iPlatform < iNumPlatforms; iPlatform++) {
        if (platforms[iPlatform]->IsInNoSpawnZone(*x, *y, width, height))
            return false;
    }

    //Check to see if we are spawning into a temporary (falling) platform
    std::list<MovingPlatform*>::iterator iterateAll = tempPlatforms.begin(), lim = tempPlatforms.end();
    while (iterateAll != lim) {
        if ((*iterateAll)->IsInNoSpawnZone(*x, *y, width, height))
            return false;

        iterateAll++;
    }

    return true;
}

void CMap::AddPermanentPlatform(MovingPlatform * platform)
{
    platforms[iNumPlatforms++] = platform;
    platformdrawlayer[platform->iDrawLayer].push_back(platform);
}

void CMap::AddTemporaryPlatform(MovingPlatform * platform)
{
    tempPlatforms.push_back(platform);
}

bool CMap::IsInPlatformNoSpawnZone(short x, short y, short width, short height)
{
    for (short iPlatform = 0; iPlatform < iNumPlatforms; iPlatform++) {
        if (platforms[iPlatform]->IsInNoSpawnZone(x, y, width, height))
            return true;
    }

    std::list<MovingPlatform*>::iterator iterateAll = tempPlatforms.begin(), lim = tempPlatforms.end();
    while (iterateAll != lim) {
        if ((*iterateAll)->IsInNoSpawnZone(x, y, width, height))
            return true;

        iterateAll++;
    }

    return false;
}

void CMap::drawfrontlayer()
{
    for (int k = 0; k < numdrawareas; k++)
        rm->spr_frontmap[g_iCurrentDrawIndex].draw(drawareas[k].x, drawareas[k].y, drawareas[k].x, drawareas[k].y, drawareas[k].w, drawareas[k].h);

    //Draw gaps in pink for debugging
    /*
    for (short i = 0; i < MAPHEIGHT; i++)
    {
    	for (short j = 0; j < MAPWIDTH; j++)
    	{
            if (mapdatatop[j][i].iType == tile_gap)
    		{
    			SDL_Rect r = {j << 5, i << 5, TILESIZE, TILESIZE};
    			SDL_FillRect(blitdest, &r, SDL_MapRGB(blitdest->format, 255, 0, 255));
    		}
    	}
    }*/
}

bool CMap::checkforwarp(short iData1, short iData2, short iData3, short iDirection)
{
    Warp * warp1 = NULL;
    Warp * warp2 = NULL;

    if (iDirection == 0 || iDirection == 2) {
        warp1 = &warpdata[iData1][iData3];
        warp2 = &warpdata[iData2][iData3];
    } else {
        warp1 = &warpdata[iData1][iData2];
        warp2 = &warpdata[iData1][iData3];
    }

    return warp1->direction == warp2->direction
        && warp1->id == warp2->id
        && warp1->direction == iDirection
        && !warplocked[warp1->connection]
        && warpexits[warp1->id].locktimer <= 0;
}

void CMap::optimize()
{
    for (int j = 0; j < MAPHEIGHT; j++) {
        for (int i = 0; i < MAPWIDTH; i++) {
            for (int m = 1; m < MAPLAYERS; m++) {
                TilesetTile * tile = &mapdata[i][j][m];
                TileType type = g_tilesetmanager->GetTileset(tile->iID)->GetTileType(tile->iCol, tile->iRow);
                if (type != tile_nonsolid && type != tile_gap && type != tile_solid_on_top) {
                    for (int k = m - 1; k >= 0; k--) {
                        TilesetTile * compareTile = &mapdata[i][j][k];
                        if (compareTile->iID == TILESETNONE) {
                            TilesetTile * fromTile = &mapdata[i][j][k + 1];
                            compareTile->iID = fromTile->iID;
                            compareTile->iCol = fromTile->iCol;
                            compareTile->iRow = fromTile->iRow;

                            fromTile->iID = TILESETNONE;
                            fromTile->iCol = 0;
                            fromTile->iRow = 0;
                        } else {
                            break;
                        }
                    }
                }
            }
        }
    }
}

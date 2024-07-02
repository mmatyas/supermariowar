#include "MI_WorldPreviewDisplay.h"

#include "GlobalConstants.h"
#include "world.h"

extern SDL_Surface* blitdest;
extern SDL_Surface* screen;

extern WorldMap g_worldmap;


MI_WorldPreviewDisplay::MI_WorldPreviewDisplay(short x, short y, short cols, short rows)
    : UI_Control(x, y)
    , iCols(cols)
    , iRows(rows)
{
    sMapSurface = SDL_CreateRGBSurface(screen->flags, 384, 304, screen->format->BitsPerPixel, 0, 0, 0, 0);
    Init();
}

MI_WorldPreviewDisplay::~MI_WorldPreviewDisplay()
{
    if (sMapSurface) {
        SDL_FreeSurface(sMapSurface);
        sMapSurface = NULL;
    }
}

void MI_WorldPreviewDisplay::Init()
{
    iMapOffsetX = 0;
    iMapOffsetY = 0;

    iMapGlobalOffsetX = 0;
    iMapGlobalOffsetY = 0;

    iMapDrawOffsetCol = 0;
    iMapDrawOffsetRow = 0;

    iMoveDirection = iScrollCols > 0 ? 0 : (iScrollRows > 0 ? 1 : -1);

    iAnimationTimer = 0;
    iAnimationFrame = 0;

    if (g_worldmap.iWidth > 20)
        rectSrcSurface.w = 320;
    else
        rectSrcSurface.w = g_worldmap.iWidth * PREVIEWTILESIZE;

    if (g_worldmap.iHeight > 15)
        rectSrcSurface.h = 240;
    else
        rectSrcSurface.h = g_worldmap.iHeight * PREVIEWTILESIZE;

    rectDstSurface.w = 320;
    rectDstSurface.h = 240;
    rectDstSurface.x = m_pos.x;
    rectDstSurface.y = m_pos.y;

    if (g_worldmap.iWidth < 20)
        rectDstSurface.x += (20 - g_worldmap.iWidth) * 8;

    if (g_worldmap.iHeight < 15)
        rectDstSurface.y += (15 - g_worldmap.iHeight) * 8;

    short iNumScrollTiles = (g_worldmap.iWidth > 20 ? g_worldmap.iWidth - 20 : 0) + (g_worldmap.iHeight > 15 ? g_worldmap.iHeight - 15 : 0);

    iScrollSpeed = 0;
    iScrollSpeedTimer = 0;
    if (iNumScrollTiles < 9)
        iScrollSpeed = (12 - iNumScrollTiles) >> 2;
}

void MI_WorldPreviewDisplay::Update()
{
    if (!m_visible)
        return;

    bool fNeedMapSurfaceUpdate = false;
    bool fNeedFullRefresh = false;

    if (++iAnimationTimer > 15) {
        iAnimationTimer = 0;
        iAnimationFrame += PREVIEWTILESIZE;

        if (iAnimationFrame >= 64)
            iAnimationFrame = 0;

        fNeedMapSurfaceUpdate = true;
    }

    if (++iScrollSpeedTimer > iScrollSpeed) {
        iScrollSpeedTimer = 0;

        if (iMoveDirection == 0) {
            --iMapGlobalOffsetX;
            if (++iMapOffsetX >= PREVIEWTILESIZE) {
                iMapOffsetX = 0;

                fNeedMapSurfaceUpdate = true;
                fNeedFullRefresh = true;

                if (++iMapDrawOffsetCol >= iScrollCols) {
                    if (iScrollRows > 0) {
                        iMoveDirection = 1;
                        iMapOffsetY = 0;
                    } else {
                        iMoveDirection = 2;
                        iMapOffsetX = PREVIEWTILESIZE;
                        --iMapDrawOffsetCol;
                        iMapGlobalOffsetX += PREVIEWTILESIZE;
                    }
                }
            }
        } else if (iMoveDirection == 1) {
            --iMapGlobalOffsetY;
            if (++iMapOffsetY >= PREVIEWTILESIZE) {
                iMapOffsetY = 0;

                fNeedMapSurfaceUpdate = true;
                fNeedFullRefresh = true;

                if (++iMapDrawOffsetRow >= iScrollRows) {
                    if (iScrollCols > 0) {
                        iMoveDirection = 2;
                        iMapOffsetX = PREVIEWTILESIZE;
                        --iMapDrawOffsetCol;
                        iMapGlobalOffsetX += PREVIEWTILESIZE;
                    } else {
                        iMoveDirection = 3;
                        iMapOffsetY = PREVIEWTILESIZE;
                        --iMapDrawOffsetRow;
                        iMapGlobalOffsetY += PREVIEWTILESIZE;
                    }
                }
            }
        } else if (iMoveDirection == 2) { //scroll left
            ++iMapGlobalOffsetX;
            if (--iMapOffsetX <= 0) {
                fNeedMapSurfaceUpdate = true;
                fNeedFullRefresh = true;

                if (--iMapDrawOffsetCol < 0) {
                    iMapDrawOffsetCol = 0;

                    if (iScrollRows > 0) {
                        iMoveDirection = 3;
                        iMapOffsetY = PREVIEWTILESIZE;
                        --iMapDrawOffsetRow;
                        iMapGlobalOffsetY += PREVIEWTILESIZE;
                        iMapGlobalOffsetX -= PREVIEWTILESIZE;
                    } else {
                        iMoveDirection = 0;
                        iMapOffsetX = 0;
                        iMapGlobalOffsetX -= PREVIEWTILESIZE;
                    }
                } else {
                    iMapOffsetX = PREVIEWTILESIZE;
                }
            }
        } else if (iMoveDirection == 3) { //scroll up
            ++iMapGlobalOffsetY;
            if (--iMapOffsetY <= 0) {
                fNeedMapSurfaceUpdate = true;
                fNeedFullRefresh = true;

                if (--iMapDrawOffsetRow < 0) {
                    iMapDrawOffsetRow = 0;
                    iMoveDirection = iScrollCols > 0 ? 0 : 1;

                    if (iScrollCols > 0) {
                        iMoveDirection = 0;
                        iMapOffsetX = 0;
                        iMapGlobalOffsetY -= PREVIEWTILESIZE;
                    } else {
                        iMoveDirection = 1;
                        iMapOffsetY = 0;
                        iMapGlobalOffsetY -= PREVIEWTILESIZE;
                    }
                } else {
                    iMapOffsetY = PREVIEWTILESIZE;
                }
            }
        }
    }

    if (fNeedMapSurfaceUpdate)
        UpdateMapSurface(fNeedFullRefresh);
}

void MI_WorldPreviewDisplay::Draw()
{
    if (!m_visible)
        return;

    rectSrcSurface.x = iMapOffsetX;
    rectSrcSurface.y = iMapOffsetY;

    SDL_BlitSurface(sMapSurface, &rectSrcSurface, blitdest, &rectDstSurface);
}

void MI_WorldPreviewDisplay::SetWorld()
{
    g_worldmap.Load(PREVIEWTILESIZE);

    short w, h;
    g_worldmap.GetWorldSize(&w, &h);

    iScrollCols = w > 20 ? w - 20 : 0;
    iScrollRows = h > 15 ? h - 15 : 0;

    Init();

    UpdateMapSurface(true);
}

void MI_WorldPreviewDisplay::UpdateMapSurface(bool fFullRefresh)
{
    g_worldmap.DrawMapToSurface(-1, fFullRefresh, sMapSurface, iMapDrawOffsetCol, iMapDrawOffsetRow, iAnimationFrame);

    SDL_Surface * olddest = blitdest;
    blitdest = sMapSurface;
    g_worldmap.Draw(iMapGlobalOffsetX, iMapGlobalOffsetY, false, false);
    blitdest = olddest;
}

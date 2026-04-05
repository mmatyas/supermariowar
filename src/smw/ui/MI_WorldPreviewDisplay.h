#pragma once

#include "gfx/gfxSprite.h"
#include "uicontrol.h"

#include "SDL.h"


class MI_WorldPreviewDisplay : public UI_Control {
public:
    MI_WorldPreviewDisplay(short x, short y, short cols, short rows);

    //Updates animations or other events every frame
    void Update() override;

    //Draws every frame
    void Draw() override;

    void SetWorld();

protected:
    void Init();
    void UpdateMapSurface(bool fFullRefresh);

    gfxSprite map_sprite;

    short iCols = 0;
    short iRows = 0;

    short iMapOffsetX = 0;
    short iMapOffsetY = 0;
    short iMapGlobalOffsetX = 0;
    short iMapGlobalOffsetY = 0;
    short iMapDrawOffsetCol = 0;
    short iMapDrawOffsetRow = 0;

    short iMoveDirection = 0;

    short iAnimationTimer = 0;
    short iAnimationFrame = 0;

    short iScrollCols = 0;
    short iScrollRows = 0;

    SDL_Rect rectSrcSurface;
    SDL_Rect rectDstSurface;

    short iScrollSpeed = 0;
    short iScrollSpeedTimer = 0;
};

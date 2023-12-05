#pragma once

#include "uicontrol.h"
#include "SDL.h"

#include <map>
#include <string>

class MapListNode;


class MI_MapBrowser : public UI_Control {
public:
    MI_MapBrowser();
    virtual ~MI_MapBrowser();

    void Update() override;
    void Draw() override;

    MenuCodeEnum SendInput(CPlayerInput * playerInput) override;
    MenuCodeEnum Modify(bool modify) override;

    void Reset(short type);

private:
    void LoadPage(short page, bool fUseFilters);

    short iPage = 0;
    short iSelectedCol = 0;
    short iSelectedRow = 0;
    short iSelectedIndex = 0;

    SDL_Surface* mapSurfaces[9];
    MapListNode* mapListNodes[9];
    std::string mapNames[9];
    std::map<std::string, MapListNode*>::iterator mapListItr[9];

    short iFilterTagAnimationTimer = 0;
    short iFilterTagAnimationFrame = 0;

    short iType = 0;
    short iMapCount = 0;

    SDL_Rect srcRectBackground;
    SDL_Rect dstRectBackground;
};

#pragma once

#include "uicontrol.h"

#include <string>
#include <vector>

class gfxSprite;


class MFS_ListItem {
public:
    MFS_ListItem() {}

    MFS_ListItem(std::string sname, short icon, bool fselected) {
        sName = std::move(sname);
        iIcon = icon;
        fSelected = fselected;
    }

    ~MFS_ListItem();

    std::string sName;  //Display name
    short iIcon = 0; //Icon to display with name
    bool fSelected = false; //Filter selected
};


class MI_MapFilterScroll : public UI_Control {
public:
    MI_MapFilterScroll(gfxSprite* nspr, short x, short y, short width, short numlines);

    void Update() override;
    void Draw() override;
    MenuCodeEnum SendInput(CPlayerInput * playerInput) override;
    MenuCodeEnum Modify(bool modify) override;

    void Add(std::string name, short icon);

    bool MoveNext();
    bool MovePrev();

private:

    std::vector<MFS_ListItem*> items;
    std::vector<MFS_ListItem*>::iterator current;

    gfxSprite* spr = nullptr;

    short iSelectedColumn = 0;
    short iNumLines = 0;
    short iSelectedLine = 0;
    unsigned short iIndex = 0;
    short iWidth = 0;
    short iOffset = 0;

    short iTopStop = 0;
    short iBottomStop = 0;
};

#pragma once

#include "MI_MapPreview.h"

#include <memory>
#include <string>

class MI_Image;


class MI_MapField: public MI_MapPreview {
public:
    MI_MapField(gfxSprite* nspr, short x, short y, std::string name, short width, short indent, bool showtags);
    virtual ~MI_MapField() = default;

    //Called when user selects this control to change it's value
    MenuCodeEnum Modify(bool modify) override;

    //Updates animations or other events every frame
    void Update() override;

    //Draws every frame
    void Draw() override;

    //Sends player input to control on every frame
    MenuCodeEnum SendInput(CPlayerInput * playerInput) override;
    MenuCodeEnum MouseClick(short iMouseX, short iMouseY)  override;

    MenuCodeEnum ChooseRandomMap();

    bool MovePrev(bool fScrollFast);
    bool MoveNext(bool fScrollFast);

    void SetDimensions(short iWidth, short iIndent) override;

protected:
    bool Move(bool fNext, bool fScrollFast);

    std::string szName;

    std::unique_ptr<MI_Image> miModifyImageLeft;
    std::unique_ptr<MI_Image> miModifyImageRight;

    short iSlideListOutGoal;

    std::string sSearchString;
    short iSearchStringTimer;

    bool fShowtags;
};

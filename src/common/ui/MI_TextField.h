#pragma once

#include "uicontrol.h"

#include <memory>
#include <string>


class MI_TextField : public UI_Control {
public:
    MI_TextField(gfxSprite * nspr, short x, short y, const char * name, short width, short indent);

    void SetTitle(std::string name);

    // Gets the values of the currently selected item
    char* GetValue() const {
        return szOutValue;
    }

    //Called when user selects this control to change it's value
    MenuCodeEnum Modify(bool modify);

    void Clear() {
        szOutValue[0] = 0;
        iCursorIndex = 0;
        iNumChars = 1;
    }

    //Updates animations or other events every frame
    void Update();

    //Draws every frame
    virtual void Draw();

    //Sends player input to control on every frame
    virtual MenuCodeEnum SendInput(CPlayerInput * playerInput);

    //When the item is changed, this code will be returned from SendInput()
    void SetItemChangedCode(MenuCodeEnum code) {
        mcItemChangedCode = code;
    }
    void SetControlSelectedCode(MenuCodeEnum code) {
        mcControlSelectedCode = code;
    }

    //Set where the data of this control is written to (some member of game_values probably)
    void SetData(char* data, short maxchars);

    MenuCodeEnum MouseClick(short iMouseX, short iMouseY);

    void Refresh();

    void SetDisallowedChars(const char * chars);

protected:
    void UpdateCursor();

    short iCursorIndex = 0;
    short iNumChars = 0;
    short iMaxChars = 0;

    gfxSprite* spr = nullptr;
    std::string szName;
    std::string szTempValue;
    char* szOutValue = nullptr;

    short iWidth = 0;
    short iIndent = 0;

    std::unique_ptr<MI_Image> miModifyCursor;

    MenuCodeEnum mcItemChangedCode = MENU_CODE_NONE;
    MenuCodeEnum mcControlSelectedCode = MENU_CODE_NONE;

    short iAdjustmentY = 0;
    short iStringWidth = 0;
    short iAllowedWidth = 0;

    char szDisallowedChars[32];
};

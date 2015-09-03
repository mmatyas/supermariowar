#ifndef UI_MAP_FIELD
#define UI_MAP_FIELD

#include "MI_MapPreview.h"

class MI_MapField: public MI_MapPreview
{
public:

    MI_MapField(gfxSprite * nspr, short x, short y, const char * name, short width, short indent, bool showtags);
    virtual ~MI_MapField();

    //Called when user selects this control to change it's value
    MenuCodeEnum Modify(bool modify);

    //Updates animations or other events every frame
    void Update();

    //Draws every frame
    void Draw();

    //Sends player input to control on every frame
    MenuCodeEnum SendInput(CPlayerInput * playerInput);

    MenuCodeEnum ChooseRandomMap();

    MenuCodeEnum MouseClick(short iMouseX, short iMouseY);

    bool MovePrev(bool fScrollFast);
    bool MoveNext(bool fScrollFast);

    void SetDimensions(short iWidth, short iIndent);

protected:

    bool Move(bool fNext, bool fScrollFast);

    char * szName;

    MI_Image * miModifyImageLeft;
    MI_Image * miModifyImageRight;

    short iSlideListOutGoal;

    std::string sSearchString;
    short iSearchStringTimer;

    bool fShowtags;
};

#endif // UI_MAP_FIELD

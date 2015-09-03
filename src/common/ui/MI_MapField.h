#ifndef UI_MAP_FIELD
#define UI_MAP_FIELD

#include "uicontrol.h"

class MI_MapField : public UI_Control
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

    void AdjustIndicators();

    void LoadCurrentMap();
    void LoadMap(const char * szMapPath);

    bool SetMap(const char * szMapName, bool fWorld);
    void SetSpecialMap(const char * szMapName, const char * szMapPath);

    const char * GetMapName() { return szMapName; }
    std::string GetMapFilePath();

    MenuCodeEnum ChooseRandomMap();

    MenuCodeEnum MouseClick(short iMouseX, short iMouseY);

    bool MovePrev(bool fScrollFast);
    bool MoveNext(bool fScrollFast);

    void SetDimensions(short iWidth, short iIndent);

protected:

    bool Move(bool fNext, bool fScrollFast);

    gfxSprite * spr;

    SDL_Surface * surfaceMapBackground;
    SDL_Surface * surfaceMapBlockLayer;
    SDL_Surface * surfaceMapForeground;
    SDL_Rect rectDst;

    char * szName;
    char szMapName[256];
    short iWidth, iIndent;

    MI_Image * miModifyImageLeft;
    MI_Image * miModifyImageRight;

    short iSlideListOut;
    short iSlideListOutGoal;

    std::string sSearchString;
    short iSearchStringTimer;

    bool fShowtags;
};

#endif // UI_MAP_FIELD

#ifndef UI_MAP_PREVIEW
#define UI_MAP_PREVIEW

#include "SDL.h"
#include "uicontrol.h"

#include <string>

class gfxSprite;


class MI_MapPreview : public UI_Control
{
public:

    MI_MapPreview(gfxSprite* nspr, short x, short y, short width, short indent);
    virtual ~MI_MapPreview();

    //Updates animations or other events every frame
    virtual void Update();

    //Draws every frame
    virtual void Draw();

    void LoadCurrentMap();
    void LoadMap(const std::string& szMapPath);

    bool SetMap(const char * szMapName, bool fWorld);
    void SetSpecialMap(const char * szMapName, const char * szMapPath);

    const char * GetMapName() { return szMapName; }
    std::string GetMapFilePath() const;

    virtual void SetDimensions(short iWidth, short iIndent);

protected:

    gfxSprite * spr;

    SDL_Surface * surfaceMapBackground;
    SDL_Surface * surfaceMapBlockLayer;
    SDL_Surface * surfaceMapForeground;
    SDL_Rect rectDst;

    short iWidth, iIndent;
    char szMapName[256];

    short iSlideListOut;
};

#endif // UI_MAP_PREVIEW

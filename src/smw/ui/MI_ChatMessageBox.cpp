#include "MI_ChatMessageBox.h"

#include "ResourceManager.h"

extern CResourceManager* rm;


MI_ChatMessageBox::MI_ChatMessageBox(short x, short y, short width, short numlines)
    : UI_Control(x, y)
    , iNumLines(numlines)
    , iWidth(width)
{}


void MI_ChatMessageBox::Draw()
{
    rm->menu_dialog.draw(ix, iy, 0, 0, iWidth - 16, iNumLines * 32 + 32);
    rm->menu_dialog.draw(ix + iWidth - 16, iy, 496, 0, 16, iNumLines * 32 + 32);
    rm->menu_dialog.draw(ix, iy + iNumLines * 32 + 32, 0, 464, iWidth - 16, 16);
    rm->menu_dialog.draw(ix + iWidth - 16, iy + iNumLines * 32 + 32, 496, 464, 16, 16);
}

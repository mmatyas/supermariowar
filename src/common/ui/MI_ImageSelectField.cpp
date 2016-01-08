#include "MI_ImageSelectField.h"

#include "ResourceManager.h"
extern CResourceManager* rm;

/**************************************
 * MI_ImageSelectField Class
 **************************************/

MI_ImageSelectField::MI_ImageSelectField(gfxSprite * nspr, gfxSprite * nspr_image, short x, short y, const char * name, short width, short indent, short imageHeight, short imageWidth) :
    MI_SelectField(nspr, x, y, name, width, indent)
{
    spr_image = nspr_image;

    iImageWidth = imageWidth;
    iImageHeight = imageHeight;
}

MI_ImageSelectField::MI_ImageSelectField(const MI_ImageSelectField& other)
    : MI_SelectField(other)
{
    spr_image = other.spr_image;
    iImageWidth = other.iImageWidth;
    iImageHeight = other.iImageHeight;
}

MI_ImageSelectField::~MI_ImageSelectField()
{}

void MI_ImageSelectField::Draw()
{
    if (!fShow)
        return;

    spr->draw(ix, iy, 0, (fSelected ? 32 : 0), iIndent - 16, 32);
    spr->draw(ix + iIndent - 16, iy, 0, (fSelected ? 96 : 64), 32, 32);
    spr->draw(ix + iIndent + 16, iy, 528 - iWidth + iIndent, (fSelected ? 32 : 0), iWidth - iIndent - 16, 32);

    rm->menu_font_large.drawChopRight(ix + 16, iy + 5, iIndent - 8, szName);

    if (!items.empty()) {
        rm->menu_font_large.drawChopRight(ix + iIndent + iImageWidth + 10, iy + 5, iWidth - iIndent - 24, (*current)->sName.c_str());
    }

    spr_image->draw(ix + iIndent + 8, iy + 16 - (iImageHeight >> 1), ((*current)->iIconOverride >= 0 ? (*current)->iIconOverride : (*current)->iValue) * iImageWidth, 0, iImageWidth, iImageHeight);

    miModifyImageRight->Draw();
    miModifyImageLeft->Draw();
}
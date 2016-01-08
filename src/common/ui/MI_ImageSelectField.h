#ifndef UI_IMAGE_SELECT_FIELD_H
#define UI_IMAGE_SELECT_FIELD_H

#include "ui/MI_SelectField.h"

class MI_ImageSelectField : public MI_SelectField
{
public:

    MI_ImageSelectField(gfxSprite * nspr, gfxSprite * nspr_image, short x, short y, const char * name, short width, short indent, short imageHeight, short imageWidth);
    MI_ImageSelectField(const MI_ImageSelectField&);
    virtual ~MI_ImageSelectField();

    void Draw();

private:

    gfxSprite * spr_image;
    short iImageWidth, iImageHeight;
};

#endif // UI_IMAGE_SELECT_FIELD_H

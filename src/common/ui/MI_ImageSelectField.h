#pragma once

#include "ui/MI_SelectField.h"


class MI_ImageSelectField : public MI_SelectField<short> {
public:
    MI_ImageSelectField(
        gfxSprite* nspr, gfxSprite* nspr_image,
        short x, short y,
        std::string name,
        short width, short indent,
        short imageHeight, short imageWidth);
    virtual ~MI_ImageSelectField() = default;

    void Draw() override;

private:
    gfxSprite* spr_image = nullptr;
    short iImageWidth = 0;
    short iImageHeight = 0;
};

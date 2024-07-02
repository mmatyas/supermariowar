#include "MI_ImageSelectField.h"

#include "ResourceManager.h"
#include "ui/MI_Image.h"

extern CResourceManager* rm;

/**************************************
 * MI_ImageSelectField Class
 **************************************/

MI_ImageSelectField::MI_ImageSelectField(
        gfxSprite* nspr, gfxSprite* nspr_image,
        short x, short y,
        std::string name,
        short width, short indent,
        short imageHeight, short imageWidth)
    : MI_SelectField<short>(nspr, x, y, std::move(name), width, indent)
    , spr_image(nspr_image)
    , iImageWidth(imageWidth)
    , iImageHeight(imageHeight)
{}

void MI_ImageSelectField::Draw()
{
    if (!fShow)
        return;

    m_spr->draw(m_pos.x, m_pos.y, 0, (fSelected ? 32 : 0), m_indent - 16, 32);
    m_spr->draw(m_pos.x + m_indent - 16, m_pos.y, 0, (fSelected ? 96 : 64), 32, 32);
    m_spr->draw(m_pos.x + m_indent + 16, m_pos.y, 528 - m_width + m_indent, (fSelected ? 32 : 0), m_width - m_indent - 16, 32);

    rm->menu_font_large.drawChopRight(m_pos.x + 16, m_pos.y + 5, m_indent - 8, m_name.c_str());

    if (!m_items.empty()) {
        rm->menu_font_large.drawChopRight(m_pos.x + m_indent + iImageWidth + 10, m_pos.y + 5, m_width - m_indent - 24, currentItem().name.c_str());
    }

    spr_image->draw(m_pos.x + m_indent + 8, m_pos.y + 16 - (iImageHeight >> 1), (currentItem().iconOverride >= 0 ? currentItem().iconOverride : currentItem().value) * iImageWidth, 0, iImageWidth, iImageHeight);

    miModifyImageRight->Draw();
    miModifyImageLeft->Draw();
}

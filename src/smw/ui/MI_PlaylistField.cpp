#include "MI_PlaylistField.h"

#include "FileList.h"
#include "GameValues.h"
#include "input.h"
#include "ResourceManager.h"
#include "ui/MI_Image.h"

extern MusicList *musiclist;
extern WorldMusicList *worldmusiclist;
extern CGameValues game_values;
extern CResourceManager* rm;


MI_PlaylistField::MI_PlaylistField(gfxSprite* nspr, short x, short y, std::string name, short width, short indent)
    : UI_Control(x, y)
    , spr(nspr)
    , szName(std::move(name))
    , iWidth(width)
    , iIndent(indent)
{
    miModifyImageLeft = std::make_unique<MI_Image>(nspr, m_pos.x + indent - 26, m_pos.y + 4, 32, 64, 26, 24, 4, 1, 8);
    miModifyImageLeft->setVisible(false);

    miModifyImageRight = std::make_unique<MI_Image>(nspr, m_pos.x + iWidth - 16, m_pos.y + 4, 32, 88, 26, 24, 4, 1, 8);
    miModifyImageRight->setVisible(false);
}

MenuCodeEnum MI_PlaylistField::Modify(bool modify)
{
    miModifyImageLeft->setVisible(modify);
    miModifyImageRight->setVisible(modify);

    fModifying = modify;
    return MENU_CODE_MODIFY_ACCEPTED;
}

MenuCodeEnum MI_PlaylistField::SendInput(CPlayerInput * playerInput)
{
    for (int iPlayer = 0; iPlayer < 4; iPlayer++) {
        if (playerInput->outputControls[iPlayer].menu_right.fPressed || playerInput->outputControls[iPlayer].menu_down.fPressed) {
            musiclist->next();
            rm->backgroundmusic[2].stop();
            rm->backgroundmusic[2].load(musiclist->music(1));

            if (game_values.music)
                rm->backgroundmusic[2].play(false, false);

            return MENU_CODE_NONE;
        }

        if (playerInput->outputControls[iPlayer].menu_left.fPressed || playerInput->outputControls[iPlayer].menu_up.fPressed) {
            musiclist->prev();
            rm->backgroundmusic[2].stop();
            rm->backgroundmusic[2].load(musiclist->music(1));

            if (game_values.music)
                rm->backgroundmusic[2].play(false, false);

            return MENU_CODE_NONE;
        }

        if (playerInput->outputControls[iPlayer].menu_random.fPressed) {
            musiclist->random();
            rm->backgroundmusic[2].stop();
            rm->backgroundmusic[2].load(musiclist->music(1));

            if (game_values.music)
                rm->backgroundmusic[2].play(false, false);

            return MENU_CODE_NONE;
        }

        if (playerInput->outputControls[iPlayer].menu_select.fPressed || playerInput->outputControls[iPlayer].menu_cancel.fPressed) {
            miModifyImageLeft->setVisible(false);
            miModifyImageRight->setVisible(false);

            fModifying = false;

            return MENU_CODE_UNSELECT_ITEM;
        }
    }

    return MENU_CODE_NONE;
}

void MI_PlaylistField::Update()
{
    miModifyImageRight->Update();
    miModifyImageLeft->Update();
}

void MI_PlaylistField::Draw()
{
    if (!m_visible)
        return;

    //Draw the select field background
    spr->draw(m_pos.x, m_pos.y, 0, (fSelected ? 32 : 0), iIndent - 16, 32);
    spr->draw(m_pos.x + iIndent - 16, m_pos.y, 0, (fSelected ? 96 : 64), 32, 32);
    spr->draw(m_pos.x + iIndent + 16, m_pos.y, 528 - iWidth + iIndent, (fSelected ? 32 : 0), iWidth - iIndent - 16, 32);

    rm->menu_font_large.drawChopRight(m_pos.x + 16, m_pos.y + 5, iIndent - 8, szName.c_str());
    rm->menu_font_large.drawChopRight(m_pos.x + iIndent + 8, m_pos.y + 5, iWidth - iIndent - 24, musiclist->currentName().c_str());

    miModifyImageLeft->Draw();
    miModifyImageRight->Draw();
}

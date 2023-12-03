#include "MI_PlayerSelect.h"

#include "GameValues.h"
#include "ResourceManager.h"
#include "ui/MI_Image.h"

extern CResourceManager* rm;
extern CGameValues game_values;


MI_PlayerSelect::MI_PlayerSelect(gfxSprite * nspr, short x, short y, std::string name, short width, short indent)
    : UI_Control(x, y)
    , spr(nspr)
    , szName(std::move(name))
    , iWidth(width)
    , iIndent(indent)
{
    miModifyImage = std::make_unique<MI_Image>(nspr, ix, iy - 6, 32, 128, 78, 78, 4, 1, 8);
    miModifyImage->Show(false);

    const short iSpacing = (width - indent - 136) / 5;
    for (size_t i = 0; i < iPlayerPosition.size(); i++) {
        iPlayerPosition[i] = iSpacing + indent + i * (iSpacing + 34);
    }

    SetImagePosition();
}

MenuCodeEnum MI_PlayerSelect::Modify(bool modify)
{
    miModifyImage->Show(modify);
    fModifying = modify;
    return MENU_CODE_MODIFY_ACCEPTED;
}

MenuCodeEnum MI_PlayerSelect::SendInput(CPlayerInput* playerInput)
{
    for (int iPlayer = 0; iPlayer < 4; iPlayer++) {
        if (playerInput->outputControls[iPlayer].menu_right.fPressed) {
            if (++iSelectedPlayer > 3)
                iSelectedPlayer = 0;

            SetImagePosition();
        }

        if (playerInput->outputControls[iPlayer].menu_left.fPressed) {
            if (--iSelectedPlayer < 0)
                iSelectedPlayer = 3;

            SetImagePosition();
        }

        if (playerInput->outputControls[iPlayer].menu_up.fPressed) {
            if (--game_values.playercontrol[iSelectedPlayer] < 0)
                game_values.playercontrol[iSelectedPlayer] = 2;

            if (game_values.playercontrol[iSelectedPlayer] == 0) {
                int iCountPlayers = 0;
                for (int iPlayer = 0; iPlayer < 4; iPlayer++) {
                    if (game_values.playercontrol[iPlayer] > 0)
                        iCountPlayers++;
                }

                if (iCountPlayers < 2)
                    game_values.playercontrol[iSelectedPlayer] = 2;
            }
        }

        if (playerInput->outputControls[iPlayer].menu_down.fPressed) {
            if (++game_values.playercontrol[iSelectedPlayer] > 2)
                game_values.playercontrol[iSelectedPlayer] = 0;

            if (game_values.playercontrol[iSelectedPlayer] == 0) {
                int iCountPlayers = 0;
                for (int iPlayer = 0; iPlayer < 4; iPlayer++) {
                    if (game_values.playercontrol[iPlayer] > 0)
                        iCountPlayers++;
                }

                if (iCountPlayers < 2)
                    game_values.playercontrol[iSelectedPlayer] = 1;
            }
        }

        if (playerInput->outputControls[iPlayer].menu_select.fPressed || playerInput->outputControls[iPlayer].menu_cancel.fPressed) {
            miModifyImage->Show(false);
            fModifying = false;
            return MENU_CODE_UNSELECT_ITEM;
        }
    }

    return MENU_CODE_NONE;
}

void MI_PlayerSelect::SetImagePosition()
{
    miModifyImage->SetPosition(ix + iPlayerPosition[iSelectedPlayer] - 22, iy - 7);
}

void MI_PlayerSelect::Update()
{
    miModifyImage->Update();
}

void MI_PlayerSelect::Draw()
{
    if (!fShow)
        return;

    spr->draw(ix, iy, 0, (fSelected ? 64 : 0), iIndent - 16, 64);
    spr->draw(ix + iIndent - 16, iy, 0, (fSelected ? 192 : 128), 32, 64);
    spr->draw(ix + iIndent + 16, iy, 528 - iWidth + iIndent, (fSelected ? 64 : 0), iWidth - iIndent - 16, 64);

    rm->menu_font_large.drawChopRight(ix + 16, iy + 20, iIndent - 8, szName.c_str());

    miModifyImage->Draw();

    for (short iPlayer = 0; iPlayer < 4; iPlayer++) {
        spr->draw(ix + iPlayerPosition[iPlayer], iy + 16, game_values.playercontrol[iPlayer] * 34 + 32, 206, 34, 32);
    }
}

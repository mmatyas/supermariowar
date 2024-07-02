#include "MI_NetRoomTeamSelect.h"

#include "FileList.h"
#include "GameValues.h"
#include "RandomNumberGenerator.h"
#include "ResourceManager.h"
#include "ui/MI_Image.h"

extern CGameValues game_values;
extern CResourceManager* rm;

extern SkinList *skinlist;

MI_NetRoomTeamSelect::MI_NetRoomTeamSelect(short x, short y, short player_id, std::function<void()>&& on_change_accepted)
    : UI_Control(x, y)
    , x(x)
    , y(y)
    , owner_player(player_id)
    , iAnimationTimer(0)
    , iAnimationFrame(0)
    , iRandomAnimationFrame(0)
    , iFastScroll(0)
    , iFastScrollTimer(0)
    , onChangeAccepted(on_change_accepted)
{
    miHoverImage = new MI_Image(&rm->menu_player_select, x - 6, y - 6, 32 + 18, 128 + 18, 44, 44, 1, 1, 1);
    miHoverImage->setVisible(false);
    miModifyImage = new MI_Image(&rm->menu_player_select, x - 24, y - 24, 32, 128, 78, 78, 4, 1, 8);
    miModifyImage->setVisible(false);
}

MI_NetRoomTeamSelect::~MI_NetRoomTeamSelect() {
    delete miHoverImage;
    delete miModifyImage;
}

void MI_NetRoomTeamSelect::Update() {
    if (++iAnimationTimer > 7) {
        iAnimationTimer = 0;

        iAnimationFrame += 2;
        if (iAnimationFrame > 2)
            iAnimationFrame = 0;

        iRandomAnimationFrame += 32;
        if (iRandomAnimationFrame >= 128)
            iRandomAnimationFrame = 0;
    }

    if (fSelected) {
        miHoverImage->setVisible(true);
        miModifyImage->setVisible(false);

        if (fModifying) {
            miHoverImage->setVisible(false);
            miModifyImage->setVisible(true);
            miModifyImage->Update();
        }
    }
    else {
        miHoverImage->setVisible(false);
        miModifyImage->setVisible(false);
    }
}

void MI_NetRoomTeamSelect::Draw() {
    if (!m_visible)
        return;

    miHoverImage->Draw();
    miModifyImage->Draw();

    // if selected, animate
    if (fSelected)
        rm->spr_player[owner_player][iAnimationFrame]->draw(x, y, 0, 0, 32, 32);
    else
        rm->spr_player[owner_player][0]->draw(x, y, 0, 0, 32, 32);
}

MenuCodeEnum MI_NetRoomTeamSelect::SendInput(CPlayerInput * playerInput) {
    if (playerInput->outputControls[0].menu_select.fPressed
        || playerInput->outputControls[0].menu_cancel.fPressed) {
        onChangeAccepted();
        fModifying = false;
        return MENU_CODE_UNSELECT_ITEM;
    }

    // FIXME: The following code was copy-pasted from MI_TeamSelect

    COutputControl* playerKeys = &game_values.playerInput.outputControls[0];

    //Scroll up/down through player skins
    if (!game_values.randomskin[0]) {
        if (playerKeys->menu_up.fPressed) {
            do {
                if (playerKeys->menu_down.fDown) {
                    game_values.skinids[owner_player] = RANDOM_INT(skinlist->count());
                } else {
                    if (--game_values.skinids[owner_player] < 0)
                        game_values.skinids[owner_player] = (short)skinlist->count() - 1;
                }
            } while (!rm->LoadMenuSkin(owner_player, game_values.skinids[owner_player], game_values.colorids[owner_player], false));
        } else if (playerKeys->menu_up.fDown) {
            if (iFastScroll == 0) {
                if (++iFastScrollTimer > 40) {
                    iFastScroll = 1;
                }
            } else {
                if (++iFastScrollTimer > 5) {
                    do {
                        if (--game_values.skinids[owner_player] < 0)
                            game_values.skinids[owner_player] = (short)skinlist->count() - 1;
                    } while (!rm->LoadMenuSkin(owner_player, game_values.skinids[owner_player], game_values.colorids[owner_player], false));

                    iFastScrollTimer = 0;
                }
            }
        }

        if (playerKeys->menu_down.fPressed) {
            do {
                if (playerKeys->menu_up.fDown) {
                    game_values.skinids[owner_player] = RANDOM_INT( skinlist->count());
                } else {
                    if (++game_values.skinids[owner_player] >= skinlist->count())
                        game_values.skinids[owner_player] = 0;
                }
            } while (!rm->LoadMenuSkin(owner_player, game_values.skinids[owner_player], game_values.colorids[owner_player], false));
        } else if (playerKeys->menu_down.fDown) {
            if (iFastScroll == 0) {
                if (++iFastScrollTimer > 40) {
                    iFastScroll = 1;
                }
            } else {
                if (++iFastScrollTimer > 5) {
                    do {
                        if (++game_values.skinids[owner_player] >= skinlist->count())
                            game_values.skinids[owner_player] = 0;
                    } while (!rm->LoadMenuSkin(owner_player, game_values.skinids[owner_player], game_values.colorids[owner_player], false));

                    iFastScrollTimer = 0;
                }
            }
        }

        if ((!playerKeys->menu_up.fDown && !playerKeys->menu_down.fDown) ||
                (playerKeys->menu_up.fDown && playerKeys->menu_down.fDown)) {
            iFastScroll = 0;
            iFastScrollTimer = 0;
        }
    } else {
        iFastScroll = 0;
        iFastScrollTimer = 0;
    }

    return MENU_CODE_NONE;
}

MenuCodeEnum MI_NetRoomTeamSelect::Modify(bool modify) {
    printf("modify\n");
    fModifying = modify;
    return MENU_CODE_MODIFY_ACCEPTED;
}

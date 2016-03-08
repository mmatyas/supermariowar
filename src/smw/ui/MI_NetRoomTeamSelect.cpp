#include "MI_NetRoomTeamSelect.h"

#include "GameValues.h"
#include "ResourceManager.h"
#include "FileList.h"

extern CGameValues game_values;
extern CResourceManager* rm;

extern SkinList *skinlist;

MI_NetRoomTeamSelect::MI_NetRoomTeamSelect(short x, short y, short player_id)
    : UI_Control(x, y)
    , x(x)
    , y(y)
    , owner_player(player_id)
    , player_ready(true)
    , iAnimationTimer(0)
    , iAnimationFrame(0)
    , iRandomAnimationFrame(0)
    , iFastScroll(0)
    , iFastScrollTimer(0)
{
    /*for (short iTeam = 0; iTeam < 4; iTeam++) {
        iTeamCounts[iTeam] = game_values.teamcounts[iTeam];

        for (short iSlot = 0; iSlot < 3; iSlot++)
            iTeamIDs[iTeam][iSlot] = game_values.teamids[iTeam][iSlot];

        fReady[iTeam] = false;
    }*/

    miHoverImage = new MI_Image(&rm->menu_player_select, x - 6, y - 6, 32 + 18, 128 + 18, 44, 44, 1, 1, 1);
    miHoverImage->Show(false);
    miModifyImage = new MI_Image(&rm->menu_player_select, x - 24, y - 24, 32, 128, 78, 78, 4, 1, 8);
    miModifyImage->Show(false);
}

MI_NetRoomTeamSelect::~MI_NetRoomTeamSelect() {
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
        miHoverImage->Show(true);
        miModifyImage->Show(false);

        if (fModifying) {
            miHoverImage->Show(false);
            miModifyImage->Show(true);
            miModifyImage->Update();
        }
    }
    else {
        miHoverImage->Show(false);
        miModifyImage->Show(false);
    }
}

void MI_NetRoomTeamSelect::Draw() {
    if (!fShow)
        return;

    miHoverImage->Draw();
    miModifyImage->Draw();

    // if changing or selected, animate
    if (!player_ready || fSelected)
        rm->spr_player[owner_player][iAnimationFrame]->draw(x, y, 0, 0, 32, 32);
    else
        rm->spr_player[owner_player][0]->draw(x, y, 0, 0, 32, 32);
}

MenuCodeEnum MI_NetRoomTeamSelect::SendInput(CPlayerInput * playerInput) {
    if (playerInput->outputControls[0].menu_select.fPressed || playerInput->outputControls[0].menu_cancel.fPressed) {
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
                    game_values.skinids[0] = RANDOM_INT(skinlist->GetCount());
                } else {
                    if (--game_values.skinids[0] < 0)
                        game_values.skinids[0] = (short)skinlist->GetCount() - 1;
                }
            } while (!rm->LoadMenuSkin(0, game_values.skinids[0], game_values.colorids[0], false));
        } else if (playerKeys->menu_up.fDown) {
            if (iFastScroll == 0) {
                if (++iFastScrollTimer > 40) {
                    iFastScroll = 1;
                }
            } else {
                if (++iFastScrollTimer > 5) {
                    do {
                        if (--game_values.skinids[0] < 0)
                            game_values.skinids[0] = (short)skinlist->GetCount() - 1;
                    } while (!rm->LoadMenuSkin(0, game_values.skinids[0], game_values.colorids[0], false));

                    iFastScrollTimer = 0;
                }
            }
        }

        if (playerKeys->menu_down.fPressed) {
            do {
                if (playerKeys->menu_up.fDown) {
                    game_values.skinids[0] = RANDOM_INT( skinlist->GetCount());
                } else {
                    if (++game_values.skinids[0] >= skinlist->GetCount())
                        game_values.skinids[0] = 0;
                }
            } while (!rm->LoadMenuSkin(0, game_values.skinids[0], game_values.colorids[0], false));
        } else if (playerKeys->menu_down.fDown) {
            if (iFastScroll == 0) {
                if (++iFastScrollTimer > 40) {
                    iFastScroll = 1;
                }
            } else {
                if (++iFastScrollTimer > 5) {
                    do {
                        if (++game_values.skinids[0] >= skinlist->GetCount())
                            game_values.skinids[0] = 0;
                    } while (!rm->LoadMenuSkin(0, game_values.skinids[0], game_values.colorids[0], false));

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

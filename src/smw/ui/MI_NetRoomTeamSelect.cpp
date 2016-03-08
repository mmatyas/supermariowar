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

    return MENU_CODE_NONE;
}

MenuCodeEnum MI_NetRoomTeamSelect::Modify(bool modify) {
    printf("modify\n");
    fModifying = modify;
    return MENU_CODE_MODIFY_ACCEPTED;
}

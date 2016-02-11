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
}

MI_NetRoomTeamSelect::~MI_NetRoomTeamSelect() {}

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
}

void MI_NetRoomTeamSelect::Draw() {
    if (!fShow)
        return;

    rm->spr_player[owner_player][player_ready ? 0 : iAnimationFrame]->draw(x, y, 0, 0, 32, 32);
}

MenuCodeEnum MI_NetRoomTeamSelect::SendInput(CPlayerInput * playerInput) {

}

MenuCodeEnum MI_NetRoomTeamSelect::Modify(bool modify) {
    fModifying = modify;
    return MENU_CODE_MODIFY_ACCEPTED;
}

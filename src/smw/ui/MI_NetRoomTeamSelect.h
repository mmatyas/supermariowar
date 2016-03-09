#ifndef UI_NET_ROOM_TEAM_SELECT_H
#define UI_NET_ROOM_TEAM_SELECT_H

#include "uimenu.h"
#include "uicontrol.h"

#include <functional>

class MI_NetRoomTeamSelect : public UI_Control {
public:
    MI_NetRoomTeamSelect(short x, short y, short player_id, std::function<void()>&& on_change_accepted = [](){});
    virtual ~MI_NetRoomTeamSelect();

    void Update();
    void Draw();

    MenuCodeEnum SendInput(CPlayerInput * playerInput);
    MenuCodeEnum Modify(bool modify);
private:
    short x, y;
    short owner_player;

    short iAnimationTimer;
    short iAnimationFrame;
    short iRandomAnimationFrame;

    short iFastScroll;
    short iFastScrollTimer;

    MI_Image* miModifyImage;
    MI_Image* miHoverImage;

    std::function<void()> onChangeAccepted;
};

#endif // UI_NET_ROOM_TEAM_SELECT_H

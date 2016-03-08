#ifndef UI_NET_ROOM_TEAM_SELECT_H
#define UI_NET_ROOM_TEAM_SELECT_H

#include "uimenu.h"
#include "uicontrol.h"

class MI_NetRoomTeamSelect : public UI_Control {
public:
    MI_NetRoomTeamSelect(short x, short y, short player_id);
    virtual ~MI_NetRoomTeamSelect();

    void Update();
    void Draw();

    MenuCodeEnum SendInput(CPlayerInput * playerInput);
    MenuCodeEnum Modify(bool modify);
private:
    short x, y;
    short owner_player;
    bool player_ready;
    // The list of players for each team
    // (there can be 4 teams, and a team can contain 3 players at most)
    /*short iTeamIDs[4][3];
    short iTeamCounts[4];*/

    short iAnimationTimer;
    short iAnimationFrame;
    short iRandomAnimationFrame;

    short iFastScroll;
    short iFastScrollTimer;

    MI_Image* miModifyImage;
    MI_Image* miHoverImage;
};

#endif // UI_NET_ROOM_TEAM_SELECT_H

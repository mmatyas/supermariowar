#ifndef UI_TEAM_SELECT_H
#define UI_TEAM_SELECT_H

#include "uimenu.h"
#include "uicontrol.h"

class MI_TeamSelect : public UI_Control
{
	public:
		MI_TeamSelect(gfxSprite * spr_background, short x, short y);
		virtual ~MI_TeamSelect();

		void Update();
		void Draw();

		MenuCodeEnum SendInput(CPlayerInput * playerInput);
		MenuCodeEnum Modify(bool modify);

		void Reset();
		short OrganizeTeams();
		short GetTeam(short iPlayerID);

	private:

		void FindNewTeam(short iPlayerID, short iDirection);

		MI_Image * miImage;
		gfxSprite * spr;

		short iTeamIDs[4][3];
		short iTeamCounts[4];
		short iNumTeams;

		short iAnimationTimer;
		short iAnimationFrame;
		short iRandomAnimationFrame;

		bool fReady[4];
		bool fAllReady;

		short iFastScroll[4];
		short iFastScrollTimer[4];

	friend class MenuState;
};

#endif // UI_TEAM_SELECT_H

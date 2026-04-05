#ifndef UI_TEAM_SELECT_H
#define UI_TEAM_SELECT_H

#include "uimenu.h"
#include "uicontrol.h"

class MI_Image;

class MI_TeamSelect : public UI_Control
{
	public:
		MI_TeamSelect(gfxSprite * spr_background, short x, short y);
		~MI_TeamSelect() override;

		void Update() override;
		void Draw() override;

		MenuCodeEnum SendInput(CPlayerInput * playerInput) override;
		MenuCodeEnum Modify(bool modify) override;

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

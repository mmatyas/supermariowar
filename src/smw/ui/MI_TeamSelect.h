#ifndef UI_TEAM_SELECT_H
#define UI_TEAM_SELECT_H

#include "GlobalConstants.h"
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
		/// Up to 4 players the original layout with 4 team slots is used;
		/// if any of players 5-8 is playing, there are 8 team slots.
		bool IsWideLayout() const;
		short GetTeamSlotCount() const;
		void SetPlayerColor(short iPlayerID, short iTeam);
		void DrawBackground(bool fWide) const;
		void DrawReadyPanel(short iPlayer, short x, short y, short w) const;

		gfxSprite * spr;

		short iTeamIDs[MAX_PLAYERS][MAX_TEAM_SIZE];
		short iTeamCounts[MAX_PLAYERS];
		short iNumTeams;

		short iAnimationTimer;
		short iAnimationFrame;
		short iRandomAnimationFrame;

		bool fReady[MAX_PLAYERS];
		bool fAllReady;

		short iFastScroll[MAX_PLAYERS];
		short iFastScrollTimer[MAX_PLAYERS];

	friend class MenuState;
};

#endif // UI_TEAM_SELECT_H

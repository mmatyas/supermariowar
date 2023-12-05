#ifndef UICUSTOMCONTROL_H
#define UICUSTOMCONTROL_H

#include "input.h"
#include "uicontrol.h"
#include "ui/MI_AnnouncerField.h"
#include "ui/MI_Button.h"
#include "ui/MI_MapField.h"
#include "ui/MI_PowerupSlider.h"
#include "ui/MI_ImageSelectField.h"
#include "ui/NetworkListScroll.h"
#include "uimenu.h"
#include "world.h"

#include <map>
#include <string>

#define NUMBONUSITEMSONWHEEL    10

class SimpleFileList;
class MapListNode;
class MI_ScoreText;
class MI_Text;


#ifdef _XBOX
class MI_ScreenResize : public UI_Control
{
	public:

		MI_ScreenResize();
		virtual ~MI_ScreenResize();

		void Draw();

		MenuCodeEnum SendInput(CPlayerInput * playerInput);
		MenuCodeEnum Modify(bool modify);

	private:

		MI_Text * miText[7];
		float dPreset;
};
#endif //_XBOX

class MI_MapBrowser : public UI_Control
{
	public:
		MI_MapBrowser();
		virtual ~MI_MapBrowser();

		void Update();
		void Draw();

		MenuCodeEnum SendInput(CPlayerInput * playerInput);
		MenuCodeEnum Modify(bool modify);

		void Reset(short type);

	private:

		void LoadPage(short page, bool fUseFilters);

		short iPage;
		short iSelectedCol;
		short iSelectedRow;
		short iSelectedIndex;

		SDL_Surface * mapSurfaces[9];
		MapListNode * mapListNodes[9];
		const char * mapNames[9];
		std::map<std::string, MapListNode*>::iterator mapListItr[9];

		short iFilterTagAnimationTimer;
		short iFilterTagAnimationFrame;

		short iType;
		short iMapCount;

		SDL_Rect srcRectBackground;
		SDL_Rect dstRectBackground;
};

class MI_World : public UI_Control
{
	public:

		MI_World();
		virtual ~MI_World();

		void Update();
		void Draw();

		MenuCodeEnum SendInput(CPlayerInput * playerInput);
		MenuCodeEnum Modify(bool modify);

		void Init();
		void SetControllingTeam(short iPlayerID);
		void DisplayTeamControlAnnouncement();

		void SetCurrentStageToCompleted(short iWinningTeam);
    void ClearCloud() {
        fUsingCloud = false;
    }

	private:

		MenuCodeEnum InitGame(short iStage, short iPlayer, bool fNeedAiControl);

		void RestartDrawCycleIfNeeded();
		bool UsePowerup(short iPlayer, short iTeam, short iIndex, bool fPopupIsUp);

		void SetMapOffset();
		void RepositionMapImage();

		void AdvanceTurn();
		void UpdateMapSurface(short iCycleIndex);
		void UseCloud(bool fUseCloud);

		short iState;
		short iStateTransition[4];
		short iItemPopupDrawY[4];
		short iPopupOffsets[4];
		short iPopupOffsetsCurrent[4];
		short iPopupOrder[4];
		short iNumPopups;
		short iStoredItemPopupDrawY;

		bool iPopupFlag[4];

		short iItemCol[4];
		short iItemPage[4];

		SDL_Surface * sMapSurface[2];
		SDL_Rect rectSrcSurface;
		SDL_Rect rectDstSurface;

		short iCurrentSurfaceIndex;
		short iCycleIndex;
		short iDrawFullRefresh;

		short iAnimationFrame;

		short iMapOffsetX;
		short iMapOffsetY;

		short iMapDrawOffsetCol;
		short iMapDrawOffsetRow;

		short iNextMapDrawOffsetCol;
		short iNextMapDrawOffsetRow;

		short iDrawWidth, iDrawHeight;
		short iSrcOffsetX, iSrcOffsetY;
		short iDstOffsetX, iDstOffsetY;

		short iControllingTeam;
		short iControllingPlayerId;
		short iReturnDirection;

		bool fForceStageStart;
		short iVehicleId;

		short iWarpCol, iWarpRow;

		short iScreenfade;
		short iScreenfadeRate;

		float dTeleportStarRadius;
		float dTeleportStarAngle;
		short iTeleportStarAnimationFrame;
		short iTeleportStarAnimationTimer;

		bool fNoInterestingMoves;

		short iSleepTurns;
		bool fUsingCloud;

		short iPressSelectTimer;
		COutputControl * pressSelectKeys;
};

class ChatMessage
{
    public:

        ChatMessage() {
            playerName = "";
            message = "";
        }

        ChatMessage(const std::string& name, const std::string& text) {
            playerName = name;
            message = text;
        }

        std::string playerName;
        std::string message;
        // time?
};

class MI_ChatMessageBox : public UI_Control
{
    public:

        MI_ChatMessageBox(short x, short y, short width, short numlines);
        virtual ~MI_ChatMessageBox() {}

        //void Update();
        void Draw();

    protected:

        std::vector<ChatMessage> messages;
        short iNumLines;

        short iWidth, iHeight;
};

#endif // UICUSTOMCONTROL_H

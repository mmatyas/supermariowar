#ifndef UICUSTOMCONTROL_H
#define UICUSTOMCONTROL_H

#include "input.h"
#include "uicontrol.h"
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

class MI_StoredPowerupResetButton : public MI_Button
{
	public:

		MI_StoredPowerupResetButton(gfxSprite * nspr, short x, short y, const char * name, short width, short justified);
		~MI_StoredPowerupResetButton() {}

		void Draw();
};

class MI_PlayerSelect : public UI_Control
{
	public:

		MI_PlayerSelect(gfxSprite * nspr, short x, short y, const char * name, short width, short indent);
		virtual ~MI_PlayerSelect();

		void SetImagePosition();

		void Update();
		void Draw();

		MenuCodeEnum SendInput(CPlayerInput * playerInput);
		MenuCodeEnum Modify(bool modify);

	private:

		gfxSprite * spr;

		char * szName;
		short iWidth, iIndent;

		short iSelectedPlayer;
		short iPlayerPosition[4];

		MI_Image * miModifyImage;
};

class MI_PowerupSelection : public UI_Control
{
	public:

		MI_PowerupSelection(short x, short y, short width, short numlines);
		virtual ~MI_PowerupSelection();

		MenuCodeEnum Modify(bool modify);
		MenuCodeEnum SendInput(CPlayerInput * playerInput);

		void Update();
		void Draw();

		void MoveNext();
		void MovePrev();

	private:

		void AdjustDisplayArrows();

		void SetupPowerupFields();
		void EnablePowerupFields(bool fEnable);

		short iIndex, iOffset;
		short iTopStop, iBottomStop;
		short iNumLines;
		short iWidth;

		UI_Menu * mMenu;

		MI_SelectField * miOverride;
		MI_SelectField * miPreset;

		MI_PowerupSlider * miPowerupSlider[NUM_POWERUPS];

		MI_Button * miRestoreDefaultsButton;
		MI_Button * miClearButton;

		MI_Image * miDialogImage;
		MI_Text * miDialogAreYouText;
		MI_Text * miDialogSureText;
		MI_Button * miDialogYesButton;
		MI_Button * miDialogNoButton;

		MI_Image * miUpArrow;
		MI_Image * miDownArrow;
};


class MI_WorldPreviewDisplay : public UI_Control
{
	public:

		MI_WorldPreviewDisplay(short x, short y, short cols, short rows);
		virtual ~MI_WorldPreviewDisplay();

		//Updates animations or other events every frame
		void Update();

		//Draws every frame
		void Draw();

		void SetWorld();

	protected:

		void Init();
		void UpdateMapSurface(bool fFullRefresh);

		SDL_Surface * sMapSurface;
		SDL_Rect rectDst;

		short iCols, iRows;

		short iMapOffsetX, iMapOffsetY;
		short iMapGlobalOffsetX, iMapGlobalOffsetY;
		short iMapDrawOffsetCol, iMapDrawOffsetRow;

		short iMoveDirection;

		short iAnimationTimer;
		short iAnimationFrame;

		short iScrollCols, iScrollRows;

		SDL_Rect rectSrcSurface, rectDstSurface;

		short iScrollSpeed, iScrollSpeedTimer;
};

class MI_AnnouncerField : public UI_Control
{
	public:

		MI_AnnouncerField(gfxSprite * nspr, short x, short y, const char * name, short width, short indent, SimpleFileList * pList);
		virtual ~MI_AnnouncerField();

		//Called when user selects this control to change it's value
		MenuCodeEnum Modify(bool modify);

		//Updates animations or other events every frame
		void Update();

		//Draws every frame
		void Draw();

		//Sends player input to control on every frame
		virtual MenuCodeEnum SendInput(CPlayerInput * playerInput);

	protected:

		void UpdateName();

		gfxSprite * spr;

		char * szName;
		short iWidth, iIndent;

		char szFieldName[256];

		MI_Image * miModifyImageLeft;
		MI_Image * miModifyImageRight;

		SimpleFileList * list;
};

class MI_PacksField : public MI_AnnouncerField
{
	public:

		MI_PacksField(gfxSprite * nspr, short x, short y, const char * name, short width, short indent, SimpleFileList * pList, MenuCodeEnum code);
		virtual ~MI_PacksField();

		MenuCodeEnum SendInput(CPlayerInput * playerInput);

	private:
		MenuCodeEnum itemChangedCode;
};

class MI_PlaylistField : public UI_Control
{
	public:

		MI_PlaylistField(gfxSprite * nspr, short x, short y, const char * name, short width, short indent);
		virtual ~MI_PlaylistField();

		//Called when user selects this control to change it's value
		MenuCodeEnum Modify(bool modify);

		//Updates animations or other events every frame
		void Update();

		//Draws every frame
		virtual void Draw();

		//Sends player input to control on every frame
		MenuCodeEnum SendInput(CPlayerInput * playerInput);

	protected:

		gfxSprite * spr;

		char * szName;
		short iWidth, iIndent;

		MI_Image * miModifyImageLeft;
		MI_Image * miModifyImageRight;
};

class MI_TourStop : public UI_Control
{
	public:

		MI_TourStop(short x, short y, bool fWorld);
		virtual ~MI_TourStop();

		MenuCodeEnum Modify(bool fModify);

		void Update();
		void Draw();
		void Refresh(short iTourStop);

	private:

		MI_ImageSelectField * miModeField;
		MI_SelectField * miGoalField;
		MI_SelectField * miPointsField;
		MI_MapField * miMapField;
		MI_Button * miStartButton;

		MI_SelectField * miBonusField;
		MI_Image * miEndStageImage[2];

		MI_Image * miBonusIcon[10];
		MI_Image * miBonusBackground[10];

		MI_Image * miTourStopLeftHeaderBar;
		MI_Image * miTourStopMenuRightHeaderBar;
		MI_Text * miTourStopMenuHeaderText;

		bool fIsWorld;
};

class MI_TournamentScoreboard : public UI_Control
{
	public:

		MI_TournamentScoreboard(gfxSprite * spr_background, short x, short y);
		virtual ~MI_TournamentScoreboard();

		void Update();
		void Draw();

		void CreateScoreboard(short numTeams, short numGames, gfxSprite * spr_icons);
		void RefreshWorldScores(short gameWinner);
		void RefreshTourScores();
		void RefreshTournamentScores(short gameWinner);
		void DetermineScoreboardWinners();
		void StopSwirl();

	private:

		void FreeScoreboard();
    short GetYFromPlace(short iPlace) {
        return iy + iPlace * 69 + (4 - iNumTeams) * 35;
    }
    float GetIconSpacing() {
        return (float)(372 - iNumGames * 32) / (float)(iNumGames + 1);
    }

		bool fCreated;

		MI_Image ** miTeamImages;
		MI_Image *** miIconImages;
		MI_Image *** miPlayerImages;

		short iNumTeams;
		short iNumGames;

		short iTournamentWinner;
		short iGameWinner;

		short iSwirlIconTeam;
		short iSwirlIconGame;

		short iFireworksCounter;
		short iWinnerTextCounter;
		short iExplosionCounter;

		gfxSprite * sprBackground;
		gfxSprite * sprIcons;

		short iTeamIDs[4][3];
		short iTeamCounts[4];

		MI_ScoreText * tourScores[4];
		MI_ScoreText * tourPoints[10];
		MI_Image * miTourPointBar;
		MI_Image * tourBonus[10];

		MI_Image * worldBonus[4][MAX_WORLD_BONUSES_AWARDED];
		MI_Image * worldScoreModifier;
		MI_Image * worldPlace[4];
		MI_ScoreText * worldScore;
		MI_Image * worldPointsBackground[4];
};

class MI_BonusWheel : public UI_Control
{
	public:

		MI_BonusWheel(short x, short y);
		virtual ~MI_BonusWheel();

		MenuCodeEnum Modify(bool modify);

		void Update();
		void Draw();

    bool GetPowerupSelectionDone() {
        return fPowerupSelectionDone;
    }
		void Reset(bool fTournament);

	private:

		short iState;
		short iDisplayPowerupIndex;
		short iDisplayPowerupTimer;

		MI_Image * miBonusImages[NUMBONUSITEMSONWHEEL];
		MI_Image ** miPlayerImages;

		MI_Button * miContinueButton;

		short iChosenPowerups[NUMBONUSITEMSONWHEEL];

		short iPressSelectTimer;
		bool fPressedSelect;
		bool fPowerupSelectionDone;

		short iSelectorAnimation;
		short iSelectorAnimationCounter;

		float dSelectionSpeed;
		float dSelectionAngle;
		short iSelectedPowerup;
		short iNextSelectionSoundIndex;
		float dSelectionSector[NUMBONUSITEMSONWHEEL + 1];

		float dSelectionWinddownSpeed;

		float dSelectionSpeedGoal;
		short iSelectionSpeedTimer;

		short iNumPlayers;
		short iWinningTeam;

		bool fCpuControlled;
};

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

class MFS_ListItem
{
	public:
		MFS_ListItem() {
			sName = "";
			fSelected = false;
		}

		MFS_ListItem(std::string sname, short icon, bool fselected) {
			sName = sname;
			iIcon = icon;
			fSelected = fselected;
		}

		~MFS_ListItem();

		std::string sName;  //Display name
		short iIcon;        //Icon to display with name
		bool fSelected;     //Filter selected
};

class MI_MapFilterScroll : public UI_Control
{
	public:
		MI_MapFilterScroll(gfxSprite * nspr, short x, short y, short width, short numlines);
		virtual ~MI_MapFilterScroll();

		void Update();
		void Draw();

		MenuCodeEnum SendInput(CPlayerInput * playerInput);
		MenuCodeEnum Modify(bool modify);

		void Add(std::string name, short icon);

		bool MoveNext();
		bool MovePrev();

	private:

		std::vector<MFS_ListItem*> items;
		std::vector<MFS_ListItem*>::iterator current;

		gfxSprite * spr;

		short iSelectedColumn;
		short iNumLines;
		short iSelectedLine;
		unsigned short iIndex;
		short iWidth;
		short iOffset;

		short iTopStop;
		short iBottomStop;
};

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
		bool  fItemPopup;
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

#ifndef __UICONTROL_H_
#define __UICONTROL_H_

#include "uimenu.h"
#include "input.h"

class UI_Menu;
class SimpleFileList;
class MapListNode;

class UI_Control
{
	public:
		UI_Control(short x, short y);
        virtual ~UI_Control() {}
	
		virtual void Update() {}
		virtual void Draw() {}

		virtual MenuCodeEnum SendInput(CPlayerInput *) {return MENU_CODE_NONE;}

		bool Select(bool select) 
		{
			fSelected = select;

			if(fSelected && fAutoModify)
				Modify(true);

			return fModifying;
		}

		virtual MenuCodeEnum Modify(bool modify) {fModifying = modify; return MENU_CODE_MODIFY_ACCEPTED;}

		void SetAutoModify(bool autoModify) {fAutoModify = autoModify;}
		bool IsAutoModify() {return fAutoModify;}

		void SetPosition(short x, short y) {ix = x; iy = y;}

		void SetNeighbor(short iNeighbor, UI_Control * uiControl) {neighborControls[iNeighbor] = uiControl;}
		UI_Control * GetNeighbor(short iNeighbor) {return neighborControls[iNeighbor];}

		void Show(bool show) {fShow = show;}
		bool IsVisible() {return fShow;}

		void SetMenuParent(UI_Menu * menu) {uiMenu = menu;}

	protected:

		bool fSelected;
		bool fModifying;
		bool fAutoModify;

		short ix, iy;

		UI_Control * neighborControls[4];

		bool fShow;

		UI_Menu * uiMenu;
};

/*********************************************************
 *    Non Selectable Controls (Text and Images)
 *********************************************************/
class MI_Image : public UI_Control
{
	public:
		MI_Image(gfxSprite * nspr, short x, short y, short srcx, short srcy, short w, short h, short numxframes, short numyframes, short speed);
		virtual ~MI_Image();
		
		void Update();
		void Draw();

		void SetPosition(short x, short y) {ix = x; iy = y;}
		void SetAnimationSpeed(short speed) {iSpeed = speed;}
		void SetImage(short srcx, short srcy, short w, short h) {isrcx = srcx; isrcy = srcy; iw = w; ih = h; iXFrame = srcx; iYFrame = srcy;}

		void SetPulse(bool pulse) {fPulse = pulse;}
		void SetSwirl(bool swirl, float radius, float angle, float radiusSpeed, float angleSpeed) {fSwirl = swirl; dSwirlRadius = radius; dSwirlAngle = angle; dSwirlRadiusSpeed = radiusSpeed; dSwirlAngleSpeed = angleSpeed;}
		void StopSwirl() {SetSwirl(false, 0.0f, 0.0f, 0.0f, 0.0f);}

		bool IsSwirling() {return fSwirl;}

	private:
		gfxSprite * spr;

		short iNumXFrames, iNumYFrames;
		short isrcx, isrcy;
		short iw, ih;
		short iSpeed, iTimer;
		short iXFrame, iYFrame;

		bool fPulse;
		short iPulseValue, iPulseDelay;
		bool fPulseOut;

		bool fSwirl;
		float dSwirlRadius;
		float dSwirlAngle;
		float dSwirlRadiusSpeed;
		float dSwirlAngleSpeed;
};

class MI_Text : public UI_Control
{
	public:
		MI_Text(char * text, short x, short y, short w, short size, short justified);
		virtual ~MI_Text();
		
		void SetText(char * text);
		void Draw();

	private:
		char * szText;
		short iw;
		short iJustified;
		gfxFont * font;
};

class MI_ScoreText : public UI_Control
{
	public:
		MI_ScoreText(short x, short y);
		virtual ~MI_ScoreText() {}
		
		void Draw();
		void SetScore(short iScore);

	private:
		short iScore;
		short iDigitLeftSrcX, iDigitMiddleSrcX, iDigitRightSrcX;
		short iDigitLeftDstX, iDigitMiddleDstX, iDigitRightDstX;

};
/*********************************************************
 *    Selectable Controls (Buttons, Fields)
 *********************************************************/
class MI_IPField : public UI_Control
{

	public:

		MI_IPField(gfxSprite * nspr, short x, short y);
		virtual ~MI_IPField();

		char * GetValue();
		
		void Update();
		void Draw();
		
		MenuCodeEnum SendInput(CPlayerInput * playerInput);
		MenuCodeEnum Modify(bool modify);
		
	private:

		void AssignHostAddress();
		void MoveImage();

		gfxSprite * spr;

		int iSelectedDigit;
		
		int values[12];

		char szValue[16];
		short iDigitPosition[12];

		MI_Image * miModifyImage;
};

class SF_ListItem
{
	public:
		SF_ListItem()
		{
			sName = "";
			
			iValue = 0;
			sValue = "";
			fValue = false;
			fHidden = false;
		}

		SF_ListItem(std::string sname, short ivalue, std::string svalue, bool fvalue, bool fhidden)
		{
			sName = sname;

			iValue = ivalue;
			sValue = svalue;
			fValue = fvalue;
			fHidden = fhidden;
		}

		~SF_ListItem();

		std::string sName;  //Display name

		short iValue;  //int data
		std::string sValue;  //string data
		bool fValue;  //bool data for toggle controls

		bool fHidden;
};

class MI_SelectField : public UI_Control
{
	public:

		MI_SelectField(gfxSprite * nspr, short x, short y, char * name, short width, short indent);
		virtual ~MI_SelectField();

		void SetTitle(char * name);

		//When this control is selected, advance the value one instead of modifying the control
		void SetAutoAdvance(bool advance) {fAutoAdvance = advance;}
		void SetNoWrap(bool nowrap) {fNoWrap = nowrap;}
		void Disable(bool disable) {fDisable = disable;}

		//Gets the values of the currently selected item
		short GetShortValue() {return (*current)->iValue;}
		std::string GetStringValue() {return (*current)->sValue;}
		bool GetBoolValue() {return (*current)->fValue;}

		//sets the currently selected item
		bool SetKey(short iID);
		bool SetIndex(unsigned short iPosition);

		//Gets the currently selected item
		SF_ListItem GetValue() {return **current;}

		//Called when user selects this control to change it's value
		MenuCodeEnum Modify(bool modify);

		//Adds an item to the list
		void Add(std::string name, short ivalue, std::string svalue, bool fvalue, bool fhidden);
		void Clear() {items.clear();}

		bool HideItem(short iID, bool fhide);
		void HideAllItems(bool fHide);

		//Updates animations or other events every frame
		void Update();

		//Draws every frame
		virtual void Draw();

		//Sends player input to control on every frame
		virtual MenuCodeEnum SendInput(CPlayerInput * playerInput);

		//When the item is changed, this code will be returned from SendInput()
		void SetItemChangedCode(MenuCodeEnum code) {mcItemChangedCode = code;}
		void SetControlSelectedCode(MenuCodeEnum code) {mcControlSelectedCode = code;}

		//Set where the data of this control is written to (some member of game_values probably)
		void SetData(short * ivalue, std::string * svalue, bool * fvalue) {iValue = ivalue; sValue = svalue; fValue = fvalue;}
		
	protected:

		void SetValues();
		bool MoveNext();
		bool MovePrev();
		bool MoveRandom();

		gfxSprite * spr;
		char szName[64];

		short * iValue;
		std::string * sValue;
		bool * fValue;

		std::vector<SF_ListItem*> items;
		std::vector<SF_ListItem*>::iterator current;
		short iIndex;

		short iWidth, iIndent;

		MI_Image * miModifyImageLeft;
		MI_Image * miModifyImageRight;
		
		MenuCodeEnum mcItemChangedCode;
		MenuCodeEnum mcControlSelectedCode;

		bool fAutoAdvance;
		bool fNoWrap;
		bool fDisable;

		short iAdjustmentY;
};

class MI_ImageSelectField : public MI_SelectField
{
	public:

		MI_ImageSelectField(gfxSprite * nspr, gfxSprite * nspr_image, short x, short y, char * name, short width, short indent, short imageHeight, short imageWidth);
		virtual ~MI_ImageSelectField();

		void Draw();
		
	private:

		gfxSprite * spr_image;
		short iImageWidth, iImageHeight;
};

class MI_InputControlField : public UI_Control
{
	public:
		MI_InputControlField(gfxSprite * nspr, short x, short y, char * name, short width, short indent);
        virtual ~MI_InputControlField() {};
		
		void Draw();
		MenuCodeEnum SendInput(CPlayerInput * playerInput);
		void SetKey(short * iSetKey, short key, short device);

		MenuCodeEnum Modify(bool modify);

		void SetDevice(short device) {iDevice = device;}
		void SetKey(short * key) {iKey = key;}
		void SetType(short type) {iType = type;}
		void SetKeyIndex(short keyindex) {iKeyIndex = keyindex;}
		void SetPlayerIndex(short playerindex) {iPlayerIndex = playerindex;}

	private:

		gfxSprite * spr;
		char * szName;

		short iWidth, iIndent;

		short iDevice;
		short * iKey;
		short iType;
		short iKeyIndex;
		short iPlayerIndex;
};

class MI_Button : public UI_Control
{
	public:

		MI_Button(gfxSprite * nspr, short x, short y, char * name, short width, short justified);
		~MI_Button() {}

		MenuCodeEnum Modify(bool fModify);

		void Draw();
		MenuCodeEnum SendInput(CPlayerInput * playerInput);

		void SetName(char * name);
		void SetCode(MenuCodeEnum code) {menuCode = code;}
		void SetImage(gfxSprite * nsprImage, short x, short y, short w, short h);

	protected:

		gfxSprite * spr;
		char * szName;

		short iWidth, iIndent;
		short iTextJustified;

		MenuCodeEnum menuCode;

		gfxSprite * sprImage;
		short iImageSrcX;
		short iImageSrcY;
		short iImageW;
		short iImageH;

		short iTextW;

		short iAdjustmentY;
		short iHalfWidth;

};

class MI_StoredPowerupResetButton : public MI_Button
{
	public:

		MI_StoredPowerupResetButton(gfxSprite * nspr, short x, short y, char * name, short width, short justified);
		~MI_StoredPowerupResetButton() {}

		void Draw();
};

class MI_InputControlContainer : public UI_Control
{
	public:

		MI_InputControlContainer(gfxSprite * spr_button, short x, short y, short playerID);
		virtual ~MI_InputControlContainer();

		void Update();
		void Draw();

		MenuCodeEnum SendInput(CPlayerInput * playerInput);
		MenuCodeEnum Modify(bool modify);

		void SetPlayer(short iPlayerID);

		void UpdateDeviceKeys(short iDevice);

	private:

		void SetVisibleInputFields();

		short iPlayerID;
		short iDevice;
		short iSelectedInputType;
		
		UI_Menu * mInputMenu;

		MI_Image * miImage[2];
		MI_Text * miText;
		MI_SelectField * miDeviceSelectField;
		MI_Button * miInputTypeButton;
		MI_InputControlField * miGameInputControlFields[NUM_KEYS];
		MI_InputControlField * miMenuInputControlFields[NUM_KEYS];

		MI_Button * miBackButton;
};

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

	friend class Menu;
};

class MI_PlayerSelect : public UI_Control
{
	public:

		MI_PlayerSelect(gfxSprite * nspr, short x, short y, char * name, short width, short indent);
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

class MI_SliderField : public MI_SelectField
{
	public:

		MI_SliderField(gfxSprite * nspr, gfxSprite * nsprSlider, short x, short y, char * name, short width, short indent1, short indent2);
		virtual ~MI_SliderField();

		//Draws every frame
		virtual void Draw();
		//Sends player input to control on every frame
		MenuCodeEnum SendInput(CPlayerInput * playerInput);

	protected:

		gfxSprite * sprSlider;
		short iIndent2;

};

class MI_PowerupSlider : public MI_SliderField
{
	public:

		MI_PowerupSlider(gfxSprite * nspr, gfxSprite * nsprSlider, gfxSprite * nsprPowerup, short x, short y, short width, short powerupIndex);
		virtual ~MI_PowerupSlider();

		//Draws every frame
		void Draw();

	protected:

		gfxSprite * sprPowerup;
		short iPowerupIndex;
		short iHalfWidth;
};

class MI_MapField : public UI_Control
{
	public:

		MI_MapField(gfxSprite * nspr, short x, short y, char * name, short width, short indent, bool showtags);
		virtual ~MI_MapField();

		//Called when user selects this control to change it's value
		MenuCodeEnum Modify(bool modify);

		//Updates animations or other events every frame
		void Update();

		//Draws every frame
		virtual void Draw();

		void Disable(bool disable) {fDisable = disable;}

		//Sends player input to control on every frame
		MenuCodeEnum SendInput(CPlayerInput * playerInput);

		void AdjustIndicators();

		void SetMap(const char * szMapName);
		void LoadCurrentMap();

		const char * GetMapName() {return szMapName;}

	protected:

		gfxSprite * spr;
		
		SDL_Surface * surfaceMapBackground;
		SDL_Surface * surfaceMapForeground;
		SDL_Rect rectDst;

		char * szName;
		const char * szMapName;
		short iWidth, iIndent;

		MI_Image * miModifyImageLeft;
		MI_Image * miModifyImageRight;

		short iSlideListOut;
		short iSlideListOutGoal;

		bool fDisable;
};

class MI_AnnouncerField : public UI_Control
{
	public:

		MI_AnnouncerField(gfxSprite * nspr, short x, short y, char * name, short width, short indent, SimpleFileList * pList);
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

		MI_PacksField(gfxSprite * nspr, short x, short y, char * name, short width, short indent, SimpleFileList * pList, MenuCodeEnum code);
		virtual ~MI_PacksField();

		MenuCodeEnum SendInput(CPlayerInput * playerInput);

	private:
		MenuCodeEnum itemChangedCode;
};

class MI_PlaylistField : public UI_Control
{
	public:

		MI_PlaylistField(gfxSprite * nspr, short x, short y, char * name, short width, short indent);
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

		MI_TourStop(short x, short y);
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

		MI_Image * miTourStopLeftHeaderBar;
		MI_Image * miTourStopMenuRightHeaderBar;
		MI_Text * miTourStopMenuHeaderText;
};

class MI_TournamentScoreboard : public UI_Control
{
	public:

		MI_TournamentScoreboard(gfxSprite * spr_background, short x, short y);
		virtual ~MI_TournamentScoreboard();

		void Update();
		void Draw();

		void CreateScoreboard(short numTeams, short numGames, gfxSprite * spr_icons, bool tour);
		void RefreshScores();
		void RefreshScores(short gameWinner);
		void StopSwirl();

	private:

		void FreeScoreboard();
		short GetYFromPlace(short iPlace) {return iy + iPlace * 69 + (4 - iNumTeams) * 35;}
		float GetIconSpacing() {return (float)(372 - iNumGames * 32) / (float)(iNumGames + 1);}

		bool fCreated;
		bool fTour;

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

		gfxSprite * sprBackground;
		gfxSprite * sprIcons;

		short iTeamIDs[4][3];
		short iTeamCounts[4];

		MI_ScoreText * tourScores[4];
		MI_ScoreText * tourPoints[10];
		MI_Image * miTourPointBar;
		MI_Image * tourBonus[10];
};

class MI_BonusWheel : public UI_Control
{
	public:

		MI_BonusWheel(gfxSprite * spr_background, gfxSprite * spr_icons, short x, short y);
		virtual ~MI_BonusWheel();

		MenuCodeEnum Modify(bool modify);

		void Update();
		void Draw();

		bool GetPowerupSelectionDone() {return fPowerupSelectionDone;}
		void Reset(bool fTournament);

	private:

		gfxSprite * spr;
		MI_Image ** miBonusImages;
		MI_Image ** miPlayerImages;

		MI_Button * miContinueButton;

		short iPowerupOrder[NUM_POWERUPS];

		bool fPowerupSelectionDone;

		short iSelectorAnimation;
		short iSelectorAnimationCounter;
		float dSelectionSpeed;
		float dSelectionAngle;
		short iSelectedPowerup;

		short iNumPlayers;
		short iWinningTeam;
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
		MFS_ListItem()
		{
			sName = "";
			fSelected = false;
		}

		MFS_ListItem(std::string sname, short icon, bool fselected)
		{
			sName = sname;
			iIcon = icon;
			fSelected = fselected;
		}

		~MFS_ListItem();

		std::string sName;  //Display name
		short iIcon;		//Icon to display with name
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

#endif //__UICONTROL_H_


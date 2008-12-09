#ifndef __UICONTROL_H_
#define __UICONTROL_H_

#include "uimenu.h"
#include "input.h"

class UI_Menu;

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

		bool IsModifying() {return fModifying;}

		void SetControllingTeam(short teamid) {iControllingTeam = teamid;}

	protected:

		bool fSelected;
		bool fModifying;
		bool fAutoModify;

		short ix, iy;

		UI_Control * neighborControls[4];

		bool fShow;

		UI_Menu * uiMenu;

		short iControllingTeam;
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
		void SetImageSource(gfxSprite * nspr) {spr = nspr;}

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
		MI_Text(const char * text, short x, short y, short w, short size, short justified);
		virtual ~MI_Text();
		
		void SetText(const char * text);
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

		~SF_ListItem() {}

		std::string sName;  //Display name

		short iValue;  //int data
		std::string sValue;  //string data
		bool fValue;  //bool data for toggle controls

		bool fHidden;
};

class MI_SelectField : public UI_Control
{
	public:

		MI_SelectField(gfxSprite * nspr, short x, short y, const char * name, short width, short indent);
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

		//Gets a random value, but does not set the control to that value
		short GetRandomShortValue() {return goodRandomItems[rand() % goodRandomItems.size()]->iValue;}
		bool GetRandomBoolValue() {return goodRandomItems[rand() % goodRandomItems.size()]->fValue;}

		//sets the currently selected item
		bool SetKey(short iID);
		bool SetIndex(unsigned short iPosition);

		//Gets the currently selected item
		SF_ListItem GetValue() {return **current;}

		//Called when user selects this control to change it's value
		MenuCodeEnum Modify(bool modify);

		//Adds an item to the list
		void Add(std::string name, short ivalue, std::string svalue, bool fvalue, bool fhidden, bool fGoodRandom = true);
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
		char * szName;

		short * iValue;
		std::string * sValue;
		bool * fValue;

		std::vector<SF_ListItem*> items;
		std::vector<SF_ListItem*>::iterator current;

		std::vector<SF_ListItem*> goodRandomItems;

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

		MI_ImageSelectField(gfxSprite * nspr, gfxSprite * nspr_image, short x, short y, const char * name, short width, short indent, short imageHeight, short imageWidth);
		virtual ~MI_ImageSelectField();

		void Draw();
		
	private:

		gfxSprite * spr_image;
		short iImageWidth, iImageHeight;
};

class MI_Button : public UI_Control
{
	public:

		MI_Button(gfxSprite * nspr, short x, short y, const char * name, short width, short justified);
		~MI_Button() {}

		MenuCodeEnum Modify(bool fModify);

		void Draw();
		MenuCodeEnum SendInput(CPlayerInput * playerInput);

		void SetName(const char * name);
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

class MI_SliderField : public MI_SelectField
{
	public:

		MI_SliderField(gfxSprite * nspr, gfxSprite * nsprSlider, short x, short y, const char * name, short width, short indent1, short indent2);
		virtual ~MI_SliderField();

		void SetPosition(short x, short y);

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

class MI_FrenzyModeOptions : public UI_Control
{
	public:

		MI_FrenzyModeOptions(short x, short y, short width, short numlines);
		virtual ~MI_FrenzyModeOptions();

		MenuCodeEnum Modify(bool modify);
		MenuCodeEnum SendInput(CPlayerInput * playerInput);

		void Update();
		void Draw();

		void MoveNext();
		void MovePrev();

		void SetRandomGameModeSettings();

	private:
		
		void SetupPowerupFields();
		void AdjustDisplayArrows();

		short iIndex, iOffset;
		short iTopStop, iBottomStop;
		short iNumLines;
		short iWidth;

		UI_Menu * mMenu;
	
		MI_SelectField * miQuantityField;
		MI_SelectField * miRateField;
		MI_SelectField * miStoredShellsField;
		MI_PowerupSlider * miPowerupSlider[NUMFRENZYCARDS];
		MI_Button * miBackButton;

		MI_Image * miUpArrow;
		MI_Image * miDownArrow;
};

#endif //__UICONTROL_H_


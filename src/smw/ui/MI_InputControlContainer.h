#include "uicontrol.h"

class gfxSprite;
class MI_Image;
class MI_Text;
class MI_SelectField;
class MI_Button;

class MI_InputControlField : public UI_Control
{
	public:
		MI_InputControlField(gfxSprite * nspr, short x, short y, const char * name, short width, short indent);
        virtual ~MI_InputControlField();

		void Draw();
		MenuCodeEnum SendInput(CPlayerInput * playerInput);
		void SetKey(SDL_KEYTYPE * iSetKey, SDL_KEYTYPE key, short device);

		MenuCodeEnum Modify(bool modify);

    void SetDevice(short device) {
        iDevice = device;
    }
    void SetKey(SDL_KEYTYPE * key) {
        iKey = key;
    }
    void SetType(short type) {
        iType = type;
    }
    void SetKeyIndex(short keyindex) {
        iKeyIndex = keyindex;
    }
    void SetPlayerIndex(short playerindex) {
        iPlayerIndex = playerindex;
    }

	private:

		gfxSprite * spr;
		char * szName;

		short iWidth, iIndent;

		short iDevice;
		SDL_KEYTYPE * iKey;
		short iType;
		short iKeyIndex;
		short iPlayerIndex;

		static const char * Joynames[30];
#ifndef USE_SDL2
		static const char * Keynames[340];
#endif
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

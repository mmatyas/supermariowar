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

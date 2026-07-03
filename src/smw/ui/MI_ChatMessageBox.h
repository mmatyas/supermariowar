#pragma once

#include "uicontrol.h"

#include <string>
#include <vector>


struct CMB_ChatMessage {
    std::string playerName;
    std::string message;
    // time?
};


class MI_ChatMessageBox : public UI_Control {
public:
    MI_ChatMessageBox(short x, short y, short width, short numlines);

    void Draw() override;

protected:
    std::vector<CMB_ChatMessage> messages;
    short iNumLines;

    short iWidth, iHeight;
};

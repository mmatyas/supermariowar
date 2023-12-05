#pragma once

#include "uicontrol.h"

#include <string>
#include <vector>


class CMB_ChatMessage {
public:
    CMB_ChatMessage() {}

    CMB_ChatMessage(std::string name, std::string text)
        : playerName(std::move(name))
        , message(std::move(text))
    {}

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

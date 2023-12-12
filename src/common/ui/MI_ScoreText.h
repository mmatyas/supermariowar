#pragma once

#include "uicontrol.h"


class MI_ScoreText : public UI_Control {
public:
    MI_ScoreText(short x, short y);

    void Draw() override;
    void SetScore(short iScore);

private:
    short iDigitLeftSrcX = 0;
    short iDigitMiddleSrcX = 0;
    short iDigitRightSrcX = 0;

    short iDigitLeftDstX = 0;
    short iDigitMiddleDstX = 0;
    short iDigitRightDstX = 0;
};

#include "MI_ScoreText.h"

#include "ResourceManager.h"

extern CResourceManager* rm;


MI_ScoreText::MI_ScoreText(short x, short y)
    : UI_Control(x, y)
{}

void MI_ScoreText::Draw()
{
    if (!fShow)
        return;

    rm->spr_scoretext.draw(iDigitRightDstX, m_pos.y, iDigitRightSrcX, 0, 16, 16);

    if (iDigitLeftSrcX > 0) {
        rm->spr_scoretext.draw(iDigitMiddleDstX, m_pos.y, iDigitMiddleSrcX, 0, 16, 16);
        rm->spr_scoretext.draw(iDigitLeftDstX, m_pos.y, iDigitLeftSrcX, 0, 16, 16);
    }
    else if (iDigitMiddleSrcX > 0) {
        rm->spr_scoretext.draw(iDigitMiddleDstX, m_pos.y, iDigitMiddleSrcX, 0, 16, 16);
    }
}

void MI_ScoreText::SetScore(short sScore)
{
    short iDigits = sScore;
    while (iDigits > 999)
        iDigits -= 1000;

    iDigitLeftSrcX = iDigits / 100 * 16;
    iDigitMiddleSrcX = iDigits % 100 / 10 * 16;
    iDigitRightSrcX = iDigits % 10 * 16;

    if (iDigitLeftSrcX == 0) {
        if (iDigitMiddleSrcX == 0) {
            iDigitRightDstX = m_pos.x - 8;
        } else {
            iDigitMiddleDstX = m_pos.x - 16;
            iDigitRightDstX = m_pos.x;
        }
    } else {
        iDigitLeftDstX = m_pos.x - 24;
        iDigitMiddleDstX = m_pos.x - 8;
        iDigitRightDstX = m_pos.x + 8;
    }
}

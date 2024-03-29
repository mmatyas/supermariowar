#include "GameModeTimer.h"

#include "GameValues.h"
#include "GlobalConstants.h"
#include "ResourceManager.h"

extern CGameValues game_values;
extern CResourceManager* rm;


GameTimerDisplay::GameTimerDisplay()
{
    iFramesPerSecond = 1000 / WAITTIME;
}

void GameTimerDisplay::Init(short iTime, bool fCountDown)
{
    timeleft = iTime;
    countdown = fCountDown;

    SetDigitCounters();
    framesleft_persecond = iFramesPerSecond;

    if (game_values.scoreboardstyle == 0)
        iScoreOffsetX = 5;
    else
        iScoreOffsetX = 291;

    SetDigitCounters();
    framesleft_persecond = iFramesPerSecond;
}

short GameTimerDisplay::RunClock()
{
    if (timeleft > 0 || !countdown) {
        if (--framesleft_persecond < 1) {
            framesleft_persecond = iFramesPerSecond;

            if (countdown) {
                --timeleft;
            } else {
                ++timeleft;
            }

            SetDigitCounters();

            return timeleft;
        }
    }

    return -1;
}

void GameTimerDisplay::Draw()
{
    rm->spr_timershade.draw(iScoreOffsetX, 5);
    rm->spr_scoretext.draw(iDigitRightDstX, 13, iDigitRightSrcX, 0, 16, 16);

    if (iDigitLeftSrcX > 0) {
        rm->spr_scoretext.draw(iDigitMiddleDstX, 13, iDigitMiddleSrcX, 0, 16, 16);
        rm->spr_scoretext.draw(iDigitLeftDstX, 13, iDigitLeftSrcX, 0, 16, 16);
    } else if (iDigitMiddleSrcX > 0) {
        rm->spr_scoretext.draw(iDigitMiddleDstX, 13, iDigitMiddleSrcX, 0, 16, 16);
    }
}

void GameTimerDisplay::SetDigitCounters()
{
    short iDigits = timeleft;
    while (iDigits > 999)
        iDigits -= 1000;

    iDigitLeftSrcX = iDigits / 100 * 16;
    iDigitMiddleSrcX = iDigits % 100 / 10 * 16;
    iDigitRightSrcX = iDigits % 10 * 16;

    if (iDigitLeftSrcX == 0) {
        if (iDigitMiddleSrcX == 0) {
            iDigitRightDstX = iScoreOffsetX + 21;
        } else {
            iDigitMiddleDstX = iScoreOffsetX + 12;
            iDigitRightDstX = iScoreOffsetX + 30;
        }
    } else {
        iDigitLeftDstX = iScoreOffsetX + 3;
        iDigitMiddleDstX = iScoreOffsetX + 21;
        iDigitRightDstX = iScoreOffsetX + 39;
    }
}

void GameTimerDisplay::SetTime(short iTime)
{
    timeleft = iTime;
    SetDigitCounters();
}

void GameTimerDisplay::AddTime(short iTime)
{
    timeleft += iTime;
    SetDigitCounters();
}

#pragma once


class GameTimerDisplay {
public:
    GameTimerDisplay();

    void Init(short iTime, bool fCountDown);
    short RunClock();
    void Draw();
    void SetTime(short iTime);
    void AddTime(short iTime);

protected:
    void SetDigitCounters();

    short timeleft;
    bool countdown;

    short framesleft_persecond;
    short iDigitLeftSrcX;
    short iDigitMiddleSrcX;
    short iDigitRightSrcX;
    short iDigitLeftDstX;
    short iDigitMiddleDstX;
    short iDigitRightDstX;
    short iScoreOffsetX;

    short iFramesPerSecond;
};

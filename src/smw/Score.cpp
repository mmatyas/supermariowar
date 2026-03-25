#include "Score.h"

#include "GameMode.h"
#include "GameValues.h"

extern CGameValues game_values;


void CScore::AdjustScore(short iValue)
{
    if (game_values.gamemode->gameover)
        return;

    score += iValue;

    if (score < 0)
        score = 0;

    SetDigitCounters();
}

void CScore::SetDigitCounters() {
    short iDigits = score;
    while (iDigits > 999)
        iDigits -= 1000;

    iDigitLeft = iDigits / 100 * 16;
    iDigitMiddle = iDigits % 100 / 10 * 16;
    iDigitRight = iDigits % 10 * 16;
}

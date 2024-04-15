#pragma once

#include <array>


class CScore {
public:
    CScore(short iPlace)
        : place(iPlace)
        , displayorder(iPlace)
    {
        subscore.fill(0);
    }

    void AdjustScore(short iValue);

    void SetScore(short iValue) {
        score = iValue;
        SetDigitCounters();
    }

    //keeps track of what the actual score value is
    short score = 0;

    //keeps track of other scoring elements for some games (health, collected cards, etc)
    std::array<short, 3> subscore;

    //Where to display score
    short x = 0;
    short y = 0;

    short destx = 0;
    short desty = 0;

    short place = 0;
    short displayorder = 0;
    short order = 0;  //the order in which the team died

    short fromx = 0;
    short fromy = 0;

    //One less array dereference doing vars like this
    short iDigitRight = 0;
    short iDigitMiddle = 0;
    short iDigitLeft = 0;

private:
    void SetDigitCounters() {
        short iDigits = score;
        while (iDigits > 999)
            iDigits -= 1000;

        iDigitLeft = iDigits / 100 * 16;
        iDigitMiddle = iDigits % 100 / 10 * 16;
        iDigitRight = iDigits % 10 * 16;
    }
};

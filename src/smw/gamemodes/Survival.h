#pragma once

#include "Classic.h"


class CGM_Survival : public CGM_Classic {
public:
    CGM_Survival();
    virtual ~CGM_Survival() {}

    void init();
    void think();
    char* getMenuString(char* buffer64);

protected:
    short timer;
    short ratetimer;
    short rate;
    short iSelectedEnemy;
    short iEnemyWeightCount;
};

#pragma once

#include "Classic.h"


class CGM_Survival : public CGM_Classic {
public:
    CGM_Survival();

    void init() override;
    void think() override;

protected:
    short timer;
    short ratetimer;
    short rate;
    short iSelectedEnemy;
    short iEnemyWeightCount;
};

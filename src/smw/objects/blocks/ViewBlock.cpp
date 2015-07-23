#include "ViewBlock.h"

#include "RandomNumberGenerator.h"
#include "ResourceManager.h"

extern CResourceManager* rm;

B_ViewBlock::B_ViewBlock(gfxSprite *nspr1, short x, short y, bool fHidden, short * piSettings) :
    B_PowerupBlock(nspr1, x, y, 1, 32000, fHidden, piSettings)
{
    poweruptimer = 0;
    powerupindex = RANDOM_INT(NUM_POWERUPS);

    iw = 32;
    ih = 32;

    iCountWeight = 0;
    for (short iPowerup = 0; iPowerup < NUM_POWERUPS; iPowerup++)
        iCountWeight += settings[iPowerup];

    fNoPowerupsSelected = iCountWeight == 0;
    GetNextPowerup();
}


void B_ViewBlock::draw()
{
    if (hidden)
        return;

    //Draw powerup behind block
    if (state == 0 && !fNoPowerupsSelected)
        rm->spr_storedpoweruplarge.draw(ix, iy, powerupindex * 32, 0, 32, 32);

    B_PowerupBlock::draw();
}

void B_ViewBlock::update()
{
    B_PowerupBlock::update();

    if (state == 0 && !fNoPowerupsSelected) {
        if (++poweruptimer > settings[powerupindex] * 10) {
            poweruptimer = 0;
            GetNextPowerup();
        }
    }
}

short B_ViewBlock::SelectPowerup()
{
    if (fNoPowerupsSelected)
        return -1;

    return powerupindex;
}

void B_ViewBlock::GetNextPowerup()
{
    if (fNoPowerupsSelected)
        return;

    int iRandPowerup = RANDOM_INT(iCountWeight) + 1;
    powerupindex = 0;
    int iPowerupWeightCount = settings[powerupindex];

    while (iPowerupWeightCount < iRandPowerup)
        iPowerupWeightCount += settings[++powerupindex];
}

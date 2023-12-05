#include "MI_BonusWheel.h"

#include "eyecandy.h"
#include "Game.h"
#include "GameMode.h"
#include "GameValues.h"
#include "RandomNumberGenerator.h"
#include "ResourceManager.h"
#include "ui/MI_Button.h"
#include "ui/MI_Image.h"
#include "uimenu.h"

#include <cmath>

extern CGameValues game_values;
extern CResourceManager* rm;
extern CGame* smw;


MI_BonusWheel::MI_BonusWheel(short x, short y)
    : UI_Control(x, y)
{
    for (short iImage = 0; iImage < NUMBONUSITEMSONWHEEL; iImage++) {
        dSelectionSector[iImage] = (float)iImage * TWO_PI / (float)(NUMBONUSITEMSONWHEEL);

        short iPowerupX = x + 160 + (short)(110.0f * cos(dSelectionSector[iImage]));
        short iPowerupY = y + 208 + (short)(110.0f * sin(dSelectionSector[iImage]));

        miBonusImages[iImage] = new MI_Image(&rm->spr_storedpoweruplarge, iPowerupX, iPowerupY, 0, 0, 32, 32, 1, 1, 0);
    }

    //Fix the last sector to allow correct detection of sector for tick sound
    dSelectionSector[NUMBONUSITEMSONWHEEL] = TWO_PI;

    miContinueButton = new MI_Button(&rm->menu_plain_field, ix + 76, iy + 390, "Continue", 200, TextAlign::CENTER);
    miContinueButton->Show(false);
    miContinueButton->SetCode(MENU_CODE_BONUS_DONE);
}

MI_BonusWheel::~MI_BonusWheel()
{
    delete miContinueButton;

    for (int iImage = 0; iImage < NUMBONUSITEMSONWHEEL; iImage++)
        delete miBonusImages[iImage];

    if (miPlayerImages) {
        for (int iPlayer = 0; iPlayer < iNumPlayers; iPlayer++)
            delete miPlayerImages[iPlayer];

        delete [] miPlayerImages;
    }
}

MenuCodeEnum MI_BonusWheel::Modify(bool fModify)
{
    if (fModify && !fPressedSelect) {
        fPressedSelect = true;

        float dNumWinddownSteps = dSelectionSpeed / 0.0005f - 1;
        float dWinddownAngle = dSelectionSpeed / 2.0f * dNumWinddownSteps;
        float dFinalAngle = dSelectionAngle + dWinddownAngle;

               //Bring the radians back down to between 0 and TWO_PI to do comparisons to the powerups on the wheel
        while (dFinalAngle > TWO_PI)
            dFinalAngle -= TWO_PI;

        float dSectorSize = TWO_PI / NUMBONUSITEMSONWHEEL;
        for (short iSector = 0; iSector < NUMBONUSITEMSONWHEEL; iSector++) {
            if (dFinalAngle >= iSector * dSectorSize && dFinalAngle < (iSector + 1) * dSectorSize) {
                iSelectedPowerup = iChosenPowerups[iSector + 1 >= NUMBONUSITEMSONWHEEL ? 0 : iSector + 1];
                float dNewWinddownAngle = dWinddownAngle + (iSector + 1) * dSectorSize - dFinalAngle;

                       //Determine the speed we need to exactly hit the selected powerup when the selector winds down
                dSelectionWinddownSpeed = dSelectionSpeed / (dNewWinddownAngle * 2.0f / dSelectionSpeed + 1.0f);
                break;
            }
        }
    }

    if (fPowerupSelectionDone)
        return miContinueButton->Modify(fModify);

    return MENU_CODE_NONE;
}

void MI_BonusWheel::Update()
{
    if (iState == 0) {
        if (--iDisplayPowerupTimer <= 0) {
            iDisplayPowerupTimer = 20;

            short iPoofX = ix + 152 + (short)(110.0f * cos(dSelectionSector[iDisplayPowerupIndex]));
            short iPoofY = iy + 200 + (short)(110.0f * sin(dSelectionSector[iDisplayPowerupIndex]));

            uiMenu->AddEyeCandy(new EC_SingleAnimation(&rm->spr_poof, iPoofX, iPoofY, 4, 5));

            ifSoundOnPlay(rm->sfx_cannon);

            if (++iDisplayPowerupIndex >= NUMBONUSITEMSONWHEEL) {
                iState = 1;
            }
        }
    } else {
        if (!fPressedSelect && (fCpuControlled || ++iPressSelectTimer > 620))
            Modify(true);

        for (int iImage = 0; iImage < NUMBONUSITEMSONWHEEL; iImage++) {
            miBonusImages[iImage]->Update();
        }

        for (int iPlayer = 0; iPlayer < iNumPlayers; iPlayer++) {
            miPlayerImages[iPlayer]->Update();
        }

        miContinueButton->Update();

        if (++iSelectorAnimationCounter > 8) {
            iSelectorAnimationCounter = 0;

            if (++iSelectorAnimation > 1)
                iSelectorAnimation = 0;
        }

        if (iSelectionSpeedTimer > 0) {
            if (--iSelectionSpeedTimer <= 0) {
                dSelectionSpeedGoal = (float)(RANDOM_INT(100) + 200) * 0.0005f;
                iSelectionSpeedTimer = 0;
            }
        }

        if (fPressedSelect) {
            dSelectionSpeed -= dSelectionWinddownSpeed;
        } else {
            if (dSelectionSpeed < dSelectionSpeedGoal) {
                dSelectionSpeed += 0.0005f;

                if (dSelectionSpeed >= dSelectionSpeedGoal) {
                    dSelectionSpeed = dSelectionSpeedGoal;
                    iSelectionSpeedTimer = RANDOM_INT(60) + 30;
                }
            } else if (dSelectionSpeed > dSelectionSpeedGoal) {
                dSelectionSpeed -= 0.0005f;

                if (dSelectionSpeed <= dSelectionSpeedGoal) {
                    dSelectionSpeed = dSelectionSpeedGoal;
                    iSelectionSpeedTimer = RANDOM_INT(60) + 30;
                }
            }
        }

        if (dSelectionSpeed <= 0.0f) {
            dSelectionSpeed = 0.0f;

            if (!fPowerupSelectionDone) {
                fPowerupSelectionDone = true;
                miContinueButton->Show(true);
                miContinueButton->Select(true);

                       //Reset all player's stored item
                if (!game_values.keeppowerup) {
                    for (short iPlayer = 0; iPlayer < 4; iPlayer++)
                        game_values.storedpowerups[iPlayer] = -1;
                }

                       //Give the newly won stored item to the winning players
                for (short iPlayer = 0; iPlayer < game_values.teamcounts[iWinningTeam]; iPlayer++)
                    game_values.storedpowerups[game_values.teamids[iWinningTeam][iPlayer]] = iSelectedPowerup;

                ifSoundOnPlay(rm->sfx_collectpowerup);
            }
        }

        dSelectionAngle += dSelectionSpeed;

               //If we hit the next powerup, play a tick sound
        if (dSelectionAngle >= dSelectionSector[iNextSelectionSoundIndex]) {
            ifSoundOnPlay(rm->sfx_worldmove);

            if (++iNextSelectionSoundIndex > NUMBONUSITEMSONWHEEL)
                iNextSelectionSoundIndex = 1;
        }

        while (dSelectionAngle > TWO_PI)
            dSelectionAngle -= TWO_PI;
    }
}

void MI_BonusWheel::Draw()
{
    if (!fShow)
        return;

    rm->spr_tournament_powerup_splash.draw(ix, iy);

    short iSelectorX = ix + 144 + (short)(110.0f * cos(dSelectionAngle));
    short iSelectorY = iy + 190 + (short)(110.0f * sin(dSelectionAngle));

    if (iState > 0)
        rm->spr_powerupselector.draw(iSelectorX, iSelectorY, iSelectorAnimation * 64, 0, 64, 64);

    for (int iImage = 0; iImage < NUMBONUSITEMSONWHEEL; iImage++) {
        if (iImage >= iDisplayPowerupIndex)
            break;

        miBonusImages[iImage]->Draw();
    }

    for (int iPlayer = 0; iPlayer < iNumPlayers; iPlayer++) {
        miPlayerImages[iPlayer]->Draw();
    }

    miContinueButton->Draw();

    if (iState == 1 && !fPressedSelect)
        rm->menu_font_large.drawCentered(smw->ScreenWidth/2, iy + 390, "Press a Button To Stop The Wheel");
}

void MI_BonusWheel::Reset(bool fTournament)
{
    //Setup the state so that we make powerups appear one by one before the wheel starts spinning
    iState = 0;
    iDisplayPowerupIndex = 0;
    iDisplayPowerupTimer = 0;

    if (fTournament)
        iWinningTeam = game_values.tournamentwinner;
    else
        iWinningTeam = game_values.gamemode->winningteam;

           //Randomly display the powerups around the ring
    short iCountWeight = 0;
    for (short iPowerup = 0; iPowerup < NUM_POWERUPS; iPowerup++)
        iCountWeight += game_values.powerupweights[iPowerup];

           //Always have at least 1 poison mushroom to try to avoid
    short iPoisonMushroom = RANDOM_INT(NUMBONUSITEMSONWHEEL);

    for (short iPowerup = 0; iPowerup < NUMBONUSITEMSONWHEEL; iPowerup++) {
        int iChoosePowerup = 0;

        if (iCountWeight > 0 && iPoisonMushroom != iPowerup) {
            int iRandPowerup = RANDOM_INT(iCountWeight + 1);
            int iPowerupWeightCount = game_values.powerupweights[iChoosePowerup];

            while (iPowerupWeightCount < iRandPowerup)
                iPowerupWeightCount += game_values.powerupweights[++iChoosePowerup];
        }

        miBonusImages[iPowerup]->SetImage(iChoosePowerup << 5, 0, 32, 32);
        iChosenPowerups[iPowerup] = iChoosePowerup;
    }

           //Setup player images on wheel
    if (miPlayerImages) {
        for (short iPlayer = 0; iPlayer < iNumPlayers; iPlayer++)
            delete miPlayerImages[iPlayer];

        delete [] miPlayerImages;
    }

    iNumPlayers = game_values.teamcounts[iWinningTeam];

    miPlayerImages = new MI_Image * [iNumPlayers];

    short iPlayerX = ix + 160 - ((iNumPlayers - 1) * 17);
    for (short iPlayer = 0; iPlayer < iNumPlayers; iPlayer++) {
        miPlayerImages[iPlayer] = new MI_Image(rm->spr_player[game_values.teamids[iWinningTeam][iPlayer]][PGFX_JUMPING_R], iPlayerX, iy + 210, 0, 0, 32, 32, 1, 1, 0);
        iPlayerX += 34;
    }

           //Indicate that the player hasn't choosen a powerup yet
    iPressSelectTimer = 0;
    fPressedSelect = false;
    fPowerupSelectionDone = false;
    miContinueButton->Show(false);

           //Counters to animate the selector's wings
    iSelectorAnimation = 0;
    iSelectorAnimationCounter = 0;

           //Figure out the initial position and speed of the selector
    dSelectionSpeed = (float)(RANDOM_INT(100) + 200) * 0.0005f;
    dSelectionAngle = (float)RANDOM_INT(NUMBONUSITEMSONWHEEL) * TWO_PI / (float)(NUMBONUSITEMSONWHEEL);
    dSelectionSpeedGoal = (float)(RANDOM_INT(100) + 200) * 0.0005f;
    iSelectionSpeedTimer = 0;

    for (short iSector = 0; iSector < NUMBONUSITEMSONWHEEL; iSector++) {
        if (dSelectionAngle > dSelectionSector[iSector])
            iNextSelectionSoundIndex = iSector + 1;
        else
            break;
    }

           //Figure out if only cpus are on the winning team, if so, the wheel will be stopped early
    fCpuControlled = true;
    for (short iPlayer = 0; iPlayer < game_values.teamcounts[iWinningTeam]; iPlayer++) {
        if (game_values.playercontrol[game_values.teamids[iWinningTeam][iPlayer]] == 1)
            fCpuControlled = false;
    }
}

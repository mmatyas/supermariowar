#pragma once

#include "uicontrol.h"

#define NUMBONUSITEMSONWHEEL 10

class MI_Button;
class MI_Image;


class MI_BonusWheel : public UI_Control {
public:
    MI_BonusWheel(short x, short y);
    virtual ~MI_BonusWheel();

    MenuCodeEnum Modify(bool modify) override;
    void Update() override;
    void Draw() override;

    bool GetPowerupSelectionDone() const { return fPowerupSelectionDone; }
    void Reset(bool fTournament);

private:
    short iState = 0;
    short iDisplayPowerupIndex = 0;
    short iDisplayPowerupTimer = 0;

    MI_Image* miBonusImages[NUMBONUSITEMSONWHEEL];
    MI_Image** miPlayerImages = nullptr;

    MI_Button* miContinueButton = nullptr;

    short iChosenPowerups[NUMBONUSITEMSONWHEEL];

    short iPressSelectTimer = 0;
    bool fPressedSelect = false;
    bool fPowerupSelectionDone = false;

    short iSelectorAnimation = 0;
    short iSelectorAnimationCounter = 0;

    float dSelectionSpeed = 0.f;
    float dSelectionAngle = 0.f;
    short iSelectedPowerup = 0;
    short iNextSelectionSoundIndex = 0;
    float dSelectionSector[NUMBONUSITEMSONWHEEL + 1];

    float dSelectionWinddownSpeed = 0.f;

    float dSelectionSpeedGoal = 0.f;
    short iSelectionSpeedTimer = 0;

    short iNumPlayers = 0;
    short iWinningTeam = 0;

    bool fCpuControlled = false;
};

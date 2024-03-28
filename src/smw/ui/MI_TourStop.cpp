#include "MI_TourStop.h"

#include "gamemodes.h"
#include "GameValues.h"
#include "ResourceManager.h"
#include "gamemodes/MiniBoss.h"
#include "gamemodes/MiniBoxes.h"
#include "gamemodes/MiniPipe.h"
#include "ui/MI_Button.h"
#include "ui/MI_Image.h"
#include "ui/MI_ImageSelectField.h"
#include "ui/MI_MapField.h"
#include "ui/MI_SelectField.h"
#include "ui/MI_Text.h"

extern CGameValues game_values;
extern CResourceManager* rm;

extern CGameMode* gamemodes[GAMEMODE_LAST];
extern CGM_Pipe_MiniGame* pipegamemode;
extern CGM_Boss_MiniGame* bossgamemode;
extern CGM_Boxes_MiniGame* boxesgamemode;


//Call with x = 70 and y == 80
MI_TourStop::MI_TourStop(short x, short y, bool fWorld)
    : UI_Control(x, y)
    , fIsWorld(fWorld)
{
    if (fIsWorld) {
        miModeField = std::make_unique<MI_ImageSelectField>(&rm->spr_selectfielddisabled, &rm->menu_mode_small, 70, 85, "Mode", 305, 90, 16, 16);
        miGoalField = std::make_unique<MI_SelectField>(&rm->spr_selectfielddisabled, 380, 85, "Goal", 190, 90);
        miPointsField = std::make_unique<MI_SelectField>(&rm->spr_selectfielddisabled, 380, 125, "Score", 190, 90);

        miBonusField = std::make_unique<MI_SelectField>(&rm->spr_selectfielddisabled, 70, 125, "Bonus", 305, 90);
        miBonusField->Disable(true);

        miEndStageImage[0] = std::make_unique<MI_Image>(&rm->spr_worlditemsplace, 54, 201, 0, 20, 80, 248, 1, 1, 0);
        miEndStageImage[0]->Show(false);

        miEndStageImage[1] = std::make_unique<MI_Image>(&rm->spr_worlditemsplace, 506, 201, 0, 20, 80, 248, 1, 1, 0);
        miEndStageImage[1]->Show(false);

        for (short iBonus = 0; iBonus < 10; iBonus++) {
            miBonusIcon[iBonus] = std::make_unique<MI_Image>(&rm->spr_worlditemssmall, 170 + iBonus * 20, 133, 0, 0, 16, 16, 1, 1, 0);
            miBonusBackground[iBonus] = std::make_unique<MI_Image>(&rm->spr_worlditemsplace, 168 + iBonus * 20, 131, 0, 0, 20, 20, 1, 1, 0);

            miBonusIcon[iBonus]->Show(false);
            miBonusBackground[iBonus]->Show(false);
        }
    } else {
        miModeField = std::make_unique<MI_ImageSelectField>(&rm->spr_selectfielddisabled, &rm->menu_mode_small, 70, 85, "Mode", 500, 120, 16, 16);
        miGoalField = std::make_unique<MI_SelectField>(&rm->spr_selectfielddisabled, 70, 125, "Goal", 246, 120);
        miPointsField = std::make_unique<MI_SelectField>(&rm->spr_selectfielddisabled, 70 + 254, 125, "Score", 246, 120);

        miBonusField = 0;
    }

    miStartButton = std::make_unique<MI_Button>(&rm->spr_selectfield, 70, 45, "Start", 500);
    miStartButton->SetCode(MENU_CODE_TOUR_STOP_CONTINUE);
    miStartButton->Select(true);

    miMapField = std::make_unique<MI_MapField>(&rm->spr_selectfielddisabled, 70, 165, "Map", 500, 120, false);
    miMapField->Disable(true);

    miModeField->Disable(true);
    miGoalField->Disable(true);
    miPointsField->Disable(true);

    miTourStopLeftHeaderBar = std::make_unique<MI_Image>(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miTourStopMenuRightHeaderBar = std::make_unique<MI_Image>(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miTourStopMenuHeaderText = std::make_unique<MI_HeaderText>("Tour Stop", 320, 5);
}

MenuCodeEnum MI_TourStop::Modify(bool fModify)
{
    return miStartButton->Modify(fModify);
}

void MI_TourStop::Update()
{
    if (!fShow)
        return;

    miStartButton->Update();

    miModeField->Update();
    miGoalField->Update();
    miPointsField->Update();
    miMapField->Update();

    if (fIsWorld) {
        miBonusField->Update();
    }

    miTourStopLeftHeaderBar->Update();
    miTourStopMenuRightHeaderBar->Update();
    miTourStopMenuHeaderText->Update();
}

void MI_TourStop::Draw()
{
    if (!fShow)
        return;

    miStartButton->Draw();

    miModeField->Draw();
    miGoalField->Draw();
    miPointsField->Draw();
    miMapField->Draw();

    if (fIsWorld) {
        miBonusField->Draw();
        miEndStageImage[0]->Draw();
        miEndStageImage[1]->Draw();

        for (short iBonus = 0; iBonus < 10; iBonus++) {
            miBonusBackground[iBonus]->Draw();
            miBonusIcon[iBonus]->Draw();
        }
    }

    miTourStopLeftHeaderBar->Draw();
    miTourStopMenuRightHeaderBar->Draw();
    miTourStopMenuHeaderText->Draw();
}

void MI_TourStop::Refresh(short iTourStop)
{
    TourStop * tourstop = game_values.tourstops[iTourStop];

    if (tourstop->iStageType == 0) {
        miModeField->Clear();

        CGameMode * gamemode = NULL;
        short tourstopicon = 0;
        if (tourstop->iMode == game_mode_pipe_minigame) {
            gamemode = pipegamemode;
            tourstopicon = 25;
        } else if (tourstop->iMode == game_mode_boss_minigame) {
            bossgamemode->SetBossType(tourstop->gmsSettings.boss.bosstype);
            gamemode = bossgamemode;
            tourstopicon = 26;
        } else if (tourstop->iMode == game_mode_boxes_minigame) {
            gamemode = boxesgamemode;
            tourstopicon = 27;
        } else {
            gamemode = gamemodes[tourstop->iMode];
            tourstopicon = tourstop->iMode;
        }

        miModeField->Add(gamemode->GetModeName(), tourstopicon, "", false, false);

        miGoalField->Clear();
        char szTemp[16];
        sprintf(szTemp, "%d", tourstop->iGoal);
        miGoalField->Add(szTemp, 0, "", false, false);
        miGoalField->SetTitle(gamemode->GetGoalName());

        miPointsField->Clear();
        sprintf(szTemp, "%d", tourstop->iPoints);
        miPointsField->Add(szTemp, 0, "", false, false);

        if (tourstop->iMode == game_mode_pipe_minigame) {
            bool fFound = miMapField->SetMap(tourstop->pszMapFile, true);

            if (!fFound)
                miMapField->SetSpecialMap("Pipe Minigame", "maps/special/two52_special_pipe_minigame.map");
        } else if (tourstop->iMode == game_mode_boss_minigame) {
            bool fFound = miMapField->SetMap(tourstop->pszMapFile, true);

            if (!fFound) {
                short iBossType = tourstop->gmsSettings.boss.bosstype;
                if (iBossType == 0)
                    miMapField->SetSpecialMap("Hammer Boss Minigame", "maps/special/two52_special_hammerboss_minigame.map");
                else if (iBossType == 1)
                    miMapField->SetSpecialMap("Bomb Boss Minigame", "maps/special/two52_special_bombboss_minigame.map");
                else if (iBossType == 2)
                    miMapField->SetSpecialMap("Fire Boss Minigame", "maps/special/two52_special_fireboss_minigame.map");
            }
        } else if (tourstop->iMode == game_mode_boxes_minigame) {
            bool fFound = miMapField->SetMap(tourstop->pszMapFile, true);

            if (!fFound)
                miMapField->SetSpecialMap("Boxes Minigame", "maps/special/two52_special_boxes_minigame.map");
        } else {
            miMapField->SetMap(tourstop->pszMapFile, true);
        }

        miTourStopMenuHeaderText->SetText(tourstop->szName);

        if (fIsWorld) {
            miBonusField->Clear();
            miEndStageImage[0]->Show(tourstop->fEndStage);
            miEndStageImage[1]->Show(tourstop->fEndStage);

            for (short iBonus = 0; iBonus < 10; iBonus++) {
                bool fShowBonus = iBonus < tourstop->iNumBonuses;
                if (fShowBonus) {
                    short iBonusIcon = tourstop->wsbBonuses[iBonus].iBonus;
                    miBonusIcon[iBonus]->SetImageSource(iBonusIcon < NUM_POWERUPS ? &rm->spr_storedpowerupsmall : &rm->spr_worlditemssmall);
                    miBonusIcon[iBonus]->SetImage((iBonusIcon < NUM_POWERUPS ? iBonusIcon : iBonusIcon - NUM_POWERUPS) << 4, 0, 16, 16);
                    miBonusBackground[iBonus]->SetImage(tourstop->wsbBonuses[iBonus].iWinnerPlace * 20, 0, 20, 20);
                }

                miBonusIcon[iBonus]->Show(fShowBonus);
                miBonusBackground[iBonus]->Show(fShowBonus);
            }
        }
    }
}

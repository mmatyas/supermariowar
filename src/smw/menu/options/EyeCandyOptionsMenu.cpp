#include "EyeCandyOptionsMenu.h"

#include "uicontrol.h"
#include "ui/MI_SelectField.h"

#include "GameValues.h"
#include "ResourceManager.h"

extern CResourceManager* rm;
extern CGameValues game_values;

UI_EyeCandyOptionsMenu::UI_EyeCandyOptionsMenu() : UI_Menu()
{
    miSpawnStyleField = new MI_SelectField(&rm->spr_selectfield, 70, 80, "Spawn Style", 500, 220);
    miSpawnStyleField->Add("Instant", 0, "", false, false);
    miSpawnStyleField->Add("Door", 1, "", false, false);
    miSpawnStyleField->Add("Swirl", 2, "", false, false);
    miSpawnStyleField->SetData(&game_values.spawnstyle, NULL, NULL);
    miSpawnStyleField->SetKey(game_values.spawnstyle);

    miAwardStyleField = new MI_SelectField(&rm->spr_selectfield, 70, 120, "Award Style", 500, 220);
    miAwardStyleField->Add("None", 0, "", false, false);
    miAwardStyleField->Add("Fireworks", 1, "", false, false);
    miAwardStyleField->Add("Spiral", 2, "", false, false);
    miAwardStyleField->Add("Ring", 3, "", false, false);
    miAwardStyleField->Add("Souls", 4, "", false, false);
    miAwardStyleField->Add("Text", 5, "", false, false);
    miAwardStyleField->SetData(&game_values.awardstyle, NULL, NULL);
    miAwardStyleField->SetKey(game_values.awardstyle);

    miScoreStyleField = new MI_SelectField(&rm->spr_selectfield, 70, 160, "Score Location", 500, 220);
    miScoreStyleField->Add("Top", 0, "", false, false);
    miScoreStyleField->Add("Bottom", 1, "", false, false);
    miScoreStyleField->Add("Corners", 2, "", false, false);
    miScoreStyleField->SetData(&game_values.scoreboardstyle, NULL, NULL);
    miScoreStyleField->SetKey(game_values.scoreboardstyle);

    miCrunchField = new MI_SelectField(&rm->spr_selectfield, 70, 200, "Screen Crunch", 500, 220);
    miCrunchField->Add("Off", 0, "", false, false);
    miCrunchField->Add("On", 1, "", true, false);
    miCrunchField->SetData(NULL, NULL, &game_values.screencrunch);
    miCrunchField->SetKey(game_values.screencrunch ? 1 : 0);
    miCrunchField->SetAutoAdvance(true);

    miWinningCrownField = new MI_SelectField(&rm->spr_selectfield, 70, 240, "Leader Crown", 500, 220);
    miWinningCrownField->Add("Off", 0, "", false, false);
    miWinningCrownField->Add("On", 1, "", true, false);
    miWinningCrownField->SetData(NULL, NULL, &game_values.showwinningcrown);
    miWinningCrownField->SetKey(game_values.showwinningcrown ? 1 : 0);
    miWinningCrownField->SetAutoAdvance(true);

    miStartCountDownField = new MI_SelectField(&rm->spr_selectfield, 70, 280, "Start Countdown", 500, 220);
    miStartCountDownField->Add("Off", 0, "", false, false);
    miStartCountDownField->Add("On", 1, "", true, false);
    miStartCountDownField->SetData(NULL, NULL, &game_values.startgamecountdown);
    miStartCountDownField->SetKey(game_values.startgamecountdown ? 1 : 0);
    miStartCountDownField->SetAutoAdvance(true);

    miStartModeDisplayField = new MI_SelectField(&rm->spr_selectfield, 70, 320, "Show Mode", 500, 220);
    miStartModeDisplayField->Add("Off", 0, "", false, false);
    miStartModeDisplayField->Add("On", 1, "", true, false);
    miStartModeDisplayField->SetData(NULL, NULL, &game_values.startmodedisplay);
    miStartModeDisplayField->SetKey(game_values.startmodedisplay ? 1 : 0);
    miStartModeDisplayField->SetAutoAdvance(true);

    miDeadTeamNoticeField = new MI_SelectField(&rm->spr_selectfield, 70, 360, "Dead Team Notice", 500, 220);
    miDeadTeamNoticeField->Add("Off", 0, "", false, false);
    miDeadTeamNoticeField->Add("On", 1, "", true, false);
    miDeadTeamNoticeField->SetData(NULL, NULL, &game_values.deadteamnotice);
    miDeadTeamNoticeField->SetKey(game_values.deadteamnotice ? 1 : 0);
    miDeadTeamNoticeField->SetAutoAdvance(true);

    miEyeCandyOptionsMenuBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, 1);
    miEyeCandyOptionsMenuBackButton->SetCode(MENU_CODE_BACK_TO_OPTIONS_MENU);

    miEyeCandyOptionsMenuLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miEyeCandyOptionsMenuRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miEyeCandyOptionsMenuHeaderText = new MI_Text("Eye Candy Options Menu", 320, 5, 0, 2, 1);

    AddControl(miSpawnStyleField, miEyeCandyOptionsMenuBackButton, miAwardStyleField, NULL, miEyeCandyOptionsMenuBackButton);
    AddControl(miAwardStyleField, miSpawnStyleField, miScoreStyleField, NULL, miEyeCandyOptionsMenuBackButton);
    AddControl(miScoreStyleField, miAwardStyleField, miCrunchField, NULL, miEyeCandyOptionsMenuBackButton);
    AddControl(miCrunchField, miScoreStyleField, miWinningCrownField, NULL, miEyeCandyOptionsMenuBackButton);
    AddControl(miWinningCrownField, miCrunchField, miStartCountDownField, NULL, miEyeCandyOptionsMenuBackButton);
    AddControl(miStartCountDownField, miWinningCrownField, miStartModeDisplayField, NULL, miEyeCandyOptionsMenuBackButton);
    AddControl(miStartModeDisplayField, miStartCountDownField, miDeadTeamNoticeField, NULL, miEyeCandyOptionsMenuBackButton);
    AddControl(miDeadTeamNoticeField, miStartModeDisplayField, miEyeCandyOptionsMenuBackButton, NULL, miEyeCandyOptionsMenuBackButton);

    AddControl(miEyeCandyOptionsMenuBackButton, miDeadTeamNoticeField, miSpawnStyleField, miDeadTeamNoticeField, NULL);

    AddNonControl(miEyeCandyOptionsMenuLeftHeaderBar);
    AddNonControl(miEyeCandyOptionsMenuRightHeaderBar);
    AddNonControl(miEyeCandyOptionsMenuHeaderText);

    SetHeadControl(miSpawnStyleField);
    SetCancelCode(MENU_CODE_BACK_TO_OPTIONS_MENU);
};

UI_EyeCandyOptionsMenu::~UI_EyeCandyOptionsMenu() {
}

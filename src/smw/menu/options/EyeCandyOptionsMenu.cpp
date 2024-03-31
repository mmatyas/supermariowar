#include "EyeCandyOptionsMenu.h"

#include "GameValues.h"
#include "ResourceManager.h"
#include "ui/MI_Button.h"
#include "ui/MI_Image.h"
#include "ui/MI_SelectField.h"
#include "ui/MI_Text.h"

extern CResourceManager* rm;
extern CGameValues game_values;


UI_EyeCandyOptionsMenu::UI_EyeCandyOptionsMenu()
    : UI_Menu()
{
    miSpawnStyleField = new MI_SelectField<SpawnStyle>(&rm->spr_selectfield, 70, 80, "Spawn Style", 500, 220);
    miSpawnStyleField->add("Instant", SpawnStyle::Instant);
    miSpawnStyleField->add("Door", SpawnStyle::Door);
    miSpawnStyleField->add("Swirl", SpawnStyle::Swirl);
    miSpawnStyleField->setOutputPtr(&game_values.spawnstyle);
    miSpawnStyleField->setCurrentValue(game_values.spawnstyle);

    miAwardStyleField = new MI_SelectField<AwardStyle>(&rm->spr_selectfield, 70, 120, "Award Style", 500, 220);
    miAwardStyleField->add("None", AwardStyle::None);
    miAwardStyleField->add("Fireworks", AwardStyle::Fireworks);
    miAwardStyleField->add("Spiral", AwardStyle::Swirl);
    miAwardStyleField->add("Ring", AwardStyle::Halo);
    miAwardStyleField->add("Souls", AwardStyle::Souls);
    miAwardStyleField->add("Text", AwardStyle::Text);
    miAwardStyleField->setOutputPtr(&game_values.awardstyle);
    miAwardStyleField->setCurrentValue(game_values.awardstyle);

    miScoreStyleField = new MI_SelectField<ScoreboardStyle>(&rm->spr_selectfield, 70, 160, "Score Location", 500, 220);
    miScoreStyleField->add("Top", ScoreboardStyle::Top);
    miScoreStyleField->add("Bottom", ScoreboardStyle::Bottom);
    miScoreStyleField->add("Corners", ScoreboardStyle::Corners);
    miScoreStyleField->setOutputPtr(&game_values.scoreboardstyle);
    miScoreStyleField->setCurrentValue(game_values.scoreboardstyle);

    miCrunchField = new MI_SelectField<bool>(&rm->spr_selectfield, 70, 200, "Screen Crunch", 500, 220);
    miCrunchField->add("Off", false);
    miCrunchField->add("On", true);
    miCrunchField->setOutputPtr(&game_values.screencrunch);
    miCrunchField->setCurrentValue(game_values.screencrunch ? 1 : 0);
    miCrunchField->setAutoAdvance(true);

    miWinningCrownField = new MI_SelectField<bool>(&rm->spr_selectfield, 70, 240, "Leader Crown", 500, 220);
    miWinningCrownField->add("Off", false);
    miWinningCrownField->add("On", true);
    miWinningCrownField->setOutputPtr(&game_values.showwinningcrown);
    miWinningCrownField->setCurrentValue(game_values.showwinningcrown ? 1 : 0);
    miWinningCrownField->setAutoAdvance(true);

    miStartCountDownField = new MI_SelectField<bool>(&rm->spr_selectfield, 70, 280, "Start Countdown", 500, 220);
    miStartCountDownField->add("Off", false);
    miStartCountDownField->add("On", true);
    miStartCountDownField->setOutputPtr(&game_values.startgamecountdown);
    miStartCountDownField->setCurrentValue(game_values.startgamecountdown ? 1 : 0);
    miStartCountDownField->setAutoAdvance(true);

    miStartModeDisplayField = new MI_SelectField<bool>(&rm->spr_selectfield, 70, 320, "Show Mode", 500, 220);
    miStartModeDisplayField->add("Off", false);
    miStartModeDisplayField->add("On", true);
    miStartModeDisplayField->setOutputPtr(&game_values.startmodedisplay);
    miStartModeDisplayField->setCurrentValue(game_values.startmodedisplay ? 1 : 0);
    miStartModeDisplayField->setAutoAdvance(true);

    miDeadTeamNoticeField = new MI_SelectField<bool>(&rm->spr_selectfield, 70, 360, "Dead Team Notice", 500, 220);
    miDeadTeamNoticeField->add("Off", false);
    miDeadTeamNoticeField->add("On", true);
    miDeadTeamNoticeField->setOutputPtr(&game_values.deadteamnotice);
    miDeadTeamNoticeField->setCurrentValue(game_values.deadteamnotice ? 1 : 0);
    miDeadTeamNoticeField->setAutoAdvance(true);

    miEyeCandyOptionsMenuBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, TextAlign::CENTER);
    miEyeCandyOptionsMenuBackButton->SetCode(MENU_CODE_BACK_TO_OPTIONS_MENU);

    miEyeCandyOptionsMenuLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miEyeCandyOptionsMenuRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miEyeCandyOptionsMenuHeaderText = new MI_HeaderText("Eye Candy Options Menu", 320, 5);

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

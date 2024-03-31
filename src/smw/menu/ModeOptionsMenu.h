#pragma once

#include "uimenu.h"

class MI_Button;
class MI_Image;
class MI_FrenzyModeOptions;
class MI_PowerupSlider;
class MI_SliderField;
class MI_Text;
template<typename T> class MI_SelectFieldDyn;


class UI_ModeOptionsMenu : public UI_Menu {
public:
    UI_ModeOptionsMenu();

    void SetRandomGameModeSettings(short iMode);
    void SetControllingTeam(short iControlTeam);

    UI_Menu* GetOptionsMenu(short iMode) {
        return &mModeSettingsMenu[iMode];
    }
    UI_Menu* GetBossOptionsMenu() {
        return &mBossSettingsMenu;
    }

    void HealthModeStartLifeChanged();
    void HealthModeMaxLifeChanged();

    void Refresh();

private:
    // Game Mode Settings Menu
    UI_Menu mModeSettingsMenu[22];

    UI_Menu mBossSettingsMenu;

    // Classic
    MI_SelectFieldDyn<short>* miClassicModeStyleField;
    MI_SelectFieldDyn<short>* miClassicModeScoringField;
    MI_Button* miClassicModeBackButton;

    MI_Image* miClassicModeLeftHeaderBar;
    MI_Image* miClassicModeRightHeaderBar;
    MI_Text* miClassicModeHeaderText;

    // Frag
    MI_SelectFieldDyn<short>* miFragModeStyleField;
    MI_SelectFieldDyn<short>* miFragModeScoringField;
    MI_Button* miFragModeBackButton;

    MI_Image* miFragModeLeftHeaderBar;
    MI_Image* miFragModeRightHeaderBar;
    MI_Text* miFragModeHeaderText;

    // Time Limit
    MI_SelectFieldDyn<short>* miTimeLimitModeStyleField;
    MI_SelectFieldDyn<short>* miTimeLimitModeScoringField;
    MI_SliderField* miTimeLimitModePercentExtraTime;
    MI_Button* miTimeLimitModeBackButton;

    MI_Image* miTimeLimitModeLeftHeaderBar;
    MI_Image* miTimeLimitModeRightHeaderBar;
    MI_Text* miTimeLimitModeHeaderText;

    // Jail
    MI_SelectFieldDyn<short>* miJailModeStyleField;
    MI_SelectFieldDyn<bool>* miJailModeTagFreeField;
    MI_SelectFieldDyn<short>* miJailModeTimeFreeField;
    MI_SliderField* miJailModeJailKeyField;
    MI_Button* miJailModeBackButton;

    MI_Image* miJailModeLeftHeaderBar;
    MI_Image* miJailModeRightHeaderBar;
    MI_Text* miJailModeHeaderText;

    // Coins
    MI_SelectFieldDyn<bool>* miCoinModePenaltyField;
    MI_SelectFieldDyn<short>* miCoinModeQuantityField;
    MI_SliderField* miCoinModePercentExtraCoin;
    MI_Button* miCoinModeBackButton;

    MI_Image* miCoinModeLeftHeaderBar;
    MI_Image* miCoinModeRightHeaderBar;
    MI_Text* miCoinModeHeaderText;

    // Yoshi's Eggs
    MI_PowerupSlider* miEggModeEggQuantityField[4];
    MI_PowerupSlider* miEggModeYoshiQuantityField[4];
    MI_SelectFieldDyn<short>* miEggModeExplosionTimeField;
    MI_Button* miEggModeBackButton;

    MI_Image* miEggModeLeftHeaderBar;
    MI_Image* miEggModeRightHeaderBar;
    MI_Text* miEggModeHeaderText;

    // Capture The Flag
    MI_SliderField* miFlagModeSpeedField;
    MI_SelectFieldDyn<bool>* miFlagModeTouchReturnField;
    MI_SelectFieldDyn<bool>* miFlagModePointMoveField;
    MI_SelectFieldDyn<short>* miFlagModeAutoReturnField;
    MI_SelectFieldDyn<bool>* miFlagModeHomeScoreField;
    MI_SelectFieldDyn<bool>* miFlagModeCenterFlagField;
    MI_Button* miFlagModeBackButton;

    MI_Image* miFlagModeLeftHeaderBar;
    MI_Image* miFlagModeRightHeaderBar;
    MI_Text* miFlagModeHeaderText;

    // Chicken
    MI_SelectFieldDyn<bool>* miChickenModeShowTargetField;
    MI_SelectFieldDyn<bool>* miChickenModeGlideField;
    MI_Button* miChickenModeBackButton;

    MI_Image* miChickenModeLeftHeaderBar;
    MI_Image* miChickenModeRightHeaderBar;
    MI_Text* miChickenModeHeaderText;

    // Tag
    MI_SelectFieldDyn<bool>* miTagModeTagOnTouchField;
    MI_Button* miTagModeBackButton;

    MI_Image* miTagModeLeftHeaderBar;
    MI_Image* miTagModeRightHeaderBar;
    MI_Text* miTagModeHeaderText;

    // Star
    MI_SelectFieldDyn<short>* miStarModeTimeField;
    MI_SelectFieldDyn<short>* miStarModeShineField;
    MI_SliderField* miStarModePercentExtraTime;
    MI_Button* miStarModeBackButton;

    MI_Image* miStarModeLeftHeaderBar;
    MI_Image* miStarModeRightHeaderBar;
    MI_Text* miStarModeHeaderText;

    // Domination
    MI_SelectFieldDyn<short>* miDominationModeQuantityField;
    MI_SelectFieldDyn<bool>* miDominationModeLoseOnDeathField;
    MI_SelectFieldDyn<bool>* miDominationModeRelocateOnDeathField;
    MI_SelectFieldDyn<bool>* miDominationModeStealOnDeathField;
    MI_SelectFieldDyn<short>* miDominationModeRelocateFrequencyField;

    MI_Text* miDominationModeDeathText;

    MI_Button* miDominationModeBackButton;

    MI_Image* miDominationModeLeftHeaderBar;
    MI_Image* miDominationModeRightHeaderBar;
    MI_Text* miDominationModeHeaderText;

    // King of the Hill
    MI_SelectFieldDyn<short>* miKingOfTheHillModeSizeField;
    MI_SelectFieldDyn<short>* miKingOfTheHillModeRelocateFrequencyField;
    MI_SelectFieldDyn<short>* miKingOfTheHillModeMultiplierField;

    MI_Button* miKingOfTheHillModeBackButton;

    MI_Image* miKingOfTheHillModeLeftHeaderBar;
    MI_Image* miKingOfTheHillModeRightHeaderBar;
    MI_Text* miKingOfTheHillModeHeaderText;

    // Race
    MI_SelectFieldDyn<short>* miRaceModeQuantityField;
    MI_SelectFieldDyn<short>* miRaceModeSpeedField;
    MI_SelectFieldDyn<short>* miRaceModePenaltyField;
    MI_Button* miRaceModeBackButton;

    MI_Image* miRaceModeLeftHeaderBar;
    MI_Image* miRaceModeRightHeaderBar;
    MI_Text* miRaceModeHeaderText;

    // Stomp
    MI_SelectFieldDyn<short>* miStompModeRateField;
    MI_PowerupSlider* miStompModeEnemySlider[NUMSTOMPENEMIES];
    MI_Button* miStompModeBackButton;

    MI_Image* miStompModeLeftHeaderBar;
    MI_Image* miStompModeRightHeaderBar;
    MI_Text* miStompModeHeaderText;

    // Frenzy
    MI_FrenzyModeOptions* miFrenzyModeOptions;

    MI_Image* miFrenzyModeLeftHeaderBar;
    MI_Image* miFrenzyModeRightHeaderBar;
    MI_Text* miFrenzyModeHeaderText;

    // Survival
    MI_PowerupSlider* miSurvivalModeEnemySlider[NUMSURVIVALENEMIES];
    MI_SelectFieldDyn<short>* miSurvivalModeDensityField;
    MI_SelectFieldDyn<short>* miSurvivalModeSpeedField;
    MI_SelectFieldDyn<bool>* miSurvivalModeShieldField;
    MI_Button* miSurvivalModeBackButton;

    MI_Image* miSurvivalModeLeftHeaderBar;
    MI_Image* miSurvivalModeRightHeaderBar;
    MI_Text* miSurvivalModeHeaderText;

    // Greed
    MI_SelectFieldDyn<short>* miGreedModeCoinLife;
    MI_SelectFieldDyn<bool>* miGreedModeOwnCoins;
    MI_SelectFieldDyn<short>* miGreedModeMultiplier;
    MI_SliderField* miGreedModePercentExtraCoin;
    MI_Button* miGreedModeBackButton;

    MI_Image* miGreedModeLeftHeaderBar;
    MI_Image* miGreedModeRightHeaderBar;
    MI_Text* miGreedModeHeaderText;

    // Health
    MI_SelectFieldDyn<short>* miHealthModeStartLife;
    MI_SelectFieldDyn<short>* miHealthModeMaxLife;
    MI_SliderField* miHealthModePercentExtraLife;
    MI_Button* miHealthModeBackButton;

    MI_Image* miHealthModeLeftHeaderBar;
    MI_Image* miHealthModeRightHeaderBar;
    MI_Text* miHealthModeHeaderText;

    // Card Collection
    MI_SelectFieldDyn<short>* miCollectionModeQuantityField;
    MI_SelectFieldDyn<short>* miCollectionModeRateField;
    MI_SelectFieldDyn<short>* miCollectionModeBankTimeField;
    MI_SelectFieldDyn<short>* miCollectionModeCardLifeField;
    MI_Button* miCollectionModeBackButton;

    MI_Image* miCollectionModeLeftHeaderBar;
    MI_Image* miCollectionModeRightHeaderBar;
    MI_Text* miCollectionModeHeaderText;

    // Phanto Chase
    MI_SelectFieldDyn<short>* miChaseModeSpeedField;
    MI_PowerupSlider* miChaseModeQuantitySlider[3];
    MI_Button* miChaseModeBackButton;

    MI_Image* miChaseModeLeftHeaderBar;
    MI_Image* miChaseModeRightHeaderBar;
    MI_Text* miChaseModeHeaderText;

    // Shyguy Tag
    MI_SelectFieldDyn<bool>* miShyGuyTagModeTagOnSuicideField;
    MI_SelectFieldDyn<short>* miShyGuyTagModeTagOnStompField;
    MI_SelectFieldDyn<short>* miShyGuyTagModeFreeTimeField;
    MI_Button* miShyGuyTagModeBackButton;

    MI_Image* miShyGuyTagModeLeftHeaderBar;
    MI_Image* miShyGuyTagModeRightHeaderBar;
    MI_Text* miShyGuyTagModeHeaderText;

    // Boss
    MI_SelectFieldDyn<short>* miBossModeTypeField;
    MI_SelectFieldDyn<short>* miBossModeDifficultyField;
    MI_SelectFieldDyn<short>* miBossModeHitPointsField;
    MI_Button* miBossModeBackButton;

    MI_Image* miBossModeLeftHeaderBar;
    MI_Image* miBossModeRightHeaderBar;
    MI_Text* miBossModeHeaderText;
};

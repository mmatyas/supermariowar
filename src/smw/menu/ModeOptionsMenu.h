#pragma once

#include "uimenu.h"
#include "GameplayStyles.h"
#include "MatchTypes.h"

class MI_Button;
class MI_Image;
class MI_FrenzyModeOptions;
class MI_PowerupSlider;
class MI_SliderField;
class MI_Text;
template<typename T> class MI_SelectField;


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
    MI_SelectField<DeathStyle>* miClassicModeStyleField;
    MI_SelectField<short>* miClassicModeScoringField;
    MI_Button* miClassicModeBackButton;

    MI_Image* miClassicModeLeftHeaderBar;
    MI_Image* miClassicModeRightHeaderBar;
    MI_Text* miClassicModeHeaderText;

    // Frag
    MI_SelectField<DeathStyle>* miFragModeStyleField;
    MI_SelectField<short>* miFragModeScoringField;
    MI_Button* miFragModeBackButton;

    MI_Image* miFragModeLeftHeaderBar;
    MI_Image* miFragModeRightHeaderBar;
    MI_Text* miFragModeHeaderText;

    // Time Limit
    MI_SelectField<DeathStyle>* miTimeLimitModeStyleField;
    MI_SelectField<short>* miTimeLimitModeScoringField;
    MI_SliderField* miTimeLimitModePercentExtraTime;
    MI_Button* miTimeLimitModeBackButton;

    MI_Image* miTimeLimitModeLeftHeaderBar;
    MI_Image* miTimeLimitModeRightHeaderBar;
    MI_Text* miTimeLimitModeHeaderText;

    // Jail
    MI_SelectField<JailStyle>* miJailModeStyleField;
    MI_SelectField<bool>* miJailModeTagFreeField;
    MI_SelectField<short>* miJailModeTimeFreeField;
    MI_SliderField* miJailModeJailKeyField;
    MI_Button* miJailModeBackButton;

    MI_Image* miJailModeLeftHeaderBar;
    MI_Image* miJailModeRightHeaderBar;
    MI_Text* miJailModeHeaderText;

    // Coins
    MI_SelectField<bool>* miCoinModePenaltyField;
    MI_SelectField<short>* miCoinModeQuantityField;
    MI_SliderField* miCoinModePercentExtraCoin;
    MI_Button* miCoinModeBackButton;

    MI_Image* miCoinModeLeftHeaderBar;
    MI_Image* miCoinModeRightHeaderBar;
    MI_Text* miCoinModeHeaderText;

    // Yoshi's Eggs
    MI_PowerupSlider* miEggModeEggQuantityField[4];
    MI_PowerupSlider* miEggModeYoshiQuantityField[4];
    MI_SelectField<short>* miEggModeExplosionTimeField;
    MI_Button* miEggModeBackButton;

    MI_Image* miEggModeLeftHeaderBar;
    MI_Image* miEggModeRightHeaderBar;
    MI_Text* miEggModeHeaderText;

    // Capture The Flag
    MI_SliderField* miFlagModeSpeedField;
    MI_SelectField<bool>* miFlagModeTouchReturnField;
    MI_SelectField<bool>* miFlagModePointMoveField;
    MI_SelectField<short>* miFlagModeAutoReturnField;
    MI_SelectField<bool>* miFlagModeHomeScoreField;
    MI_SelectField<bool>* miFlagModeCenterFlagField;
    MI_Button* miFlagModeBackButton;

    MI_Image* miFlagModeLeftHeaderBar;
    MI_Image* miFlagModeRightHeaderBar;
    MI_Text* miFlagModeHeaderText;

    // Chicken
    MI_SelectField<bool>* miChickenModeShowTargetField;
    MI_SelectField<bool>* miChickenModeGlideField;
    MI_Button* miChickenModeBackButton;

    MI_Image* miChickenModeLeftHeaderBar;
    MI_Image* miChickenModeRightHeaderBar;
    MI_Text* miChickenModeHeaderText;

    // Tag
    MI_SelectField<bool>* miTagModeTagOnTouchField;
    MI_Button* miTagModeBackButton;

    MI_Image* miTagModeLeftHeaderBar;
    MI_Image* miTagModeRightHeaderBar;
    MI_Text* miTagModeHeaderText;

    // Star
    MI_SelectField<short>* miStarModeTimeField;
    MI_SelectField<StarStyle>* miStarModeShineField;
    MI_SliderField* miStarModePercentExtraTime;
    MI_Button* miStarModeBackButton;

    MI_Image* miStarModeLeftHeaderBar;
    MI_Image* miStarModeRightHeaderBar;
    MI_Text* miStarModeHeaderText;

    // Domination
    MI_SelectField<short>* miDominationModeQuantityField;
    MI_SelectField<bool>* miDominationModeLoseOnDeathField;
    MI_SelectField<bool>* miDominationModeRelocateOnDeathField;
    MI_SelectField<bool>* miDominationModeStealOnDeathField;
    MI_SelectField<short>* miDominationModeRelocateFrequencyField;

    MI_Text* miDominationModeDeathText;

    MI_Button* miDominationModeBackButton;

    MI_Image* miDominationModeLeftHeaderBar;
    MI_Image* miDominationModeRightHeaderBar;
    MI_Text* miDominationModeHeaderText;

    // King of the Hill
    MI_SelectField<short>* miKingOfTheHillModeSizeField;
    MI_SelectField<short>* miKingOfTheHillModeRelocateFrequencyField;
    MI_SelectField<short>* miKingOfTheHillModeMultiplierField;

    MI_Button* miKingOfTheHillModeBackButton;

    MI_Image* miKingOfTheHillModeLeftHeaderBar;
    MI_Image* miKingOfTheHillModeRightHeaderBar;
    MI_Text* miKingOfTheHillModeHeaderText;

    // Race
    MI_SelectField<short>* miRaceModeQuantityField;
    MI_SelectField<short>* miRaceModeSpeedField;
    MI_SelectField<short>* miRaceModePenaltyField;
    MI_Button* miRaceModeBackButton;

    MI_Image* miRaceModeLeftHeaderBar;
    MI_Image* miRaceModeRightHeaderBar;
    MI_Text* miRaceModeHeaderText;

    // Stomp
    MI_SelectField<short>* miStompModeRateField;
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
    MI_SelectField<short>* miSurvivalModeDensityField;
    MI_SelectField<short>* miSurvivalModeSpeedField;
    MI_SelectField<bool>* miSurvivalModeShieldField;
    MI_Button* miSurvivalModeBackButton;

    MI_Image* miSurvivalModeLeftHeaderBar;
    MI_Image* miSurvivalModeRightHeaderBar;
    MI_Text* miSurvivalModeHeaderText;

    // Greed
    MI_SelectField<short>* miGreedModeCoinLife;
    MI_SelectField<bool>* miGreedModeOwnCoins;
    MI_SelectField<short>* miGreedModeMultiplier;
    MI_SliderField* miGreedModePercentExtraCoin;
    MI_Button* miGreedModeBackButton;

    MI_Image* miGreedModeLeftHeaderBar;
    MI_Image* miGreedModeRightHeaderBar;
    MI_Text* miGreedModeHeaderText;

    // Health
    MI_SelectField<short>* miHealthModeStartLife;
    MI_SelectField<short>* miHealthModeMaxLife;
    MI_SliderField* miHealthModePercentExtraLife;
    MI_Button* miHealthModeBackButton;

    MI_Image* miHealthModeLeftHeaderBar;
    MI_Image* miHealthModeRightHeaderBar;
    MI_Text* miHealthModeHeaderText;

    // Card Collection
    MI_SelectField<short>* miCollectionModeQuantityField;
    MI_SelectField<short>* miCollectionModeRateField;
    MI_SelectField<short>* miCollectionModeBankTimeField;
    MI_SelectField<short>* miCollectionModeCardLifeField;
    MI_Button* miCollectionModeBackButton;

    MI_Image* miCollectionModeLeftHeaderBar;
    MI_Image* miCollectionModeRightHeaderBar;
    MI_Text* miCollectionModeHeaderText;

    // Phanto Chase
    MI_SelectField<short>* miChaseModeSpeedField;
    MI_PowerupSlider* miChaseModeQuantitySlider[3];
    MI_Button* miChaseModeBackButton;

    MI_Image* miChaseModeLeftHeaderBar;
    MI_Image* miChaseModeRightHeaderBar;
    MI_Text* miChaseModeHeaderText;

    // Shyguy Tag
    MI_SelectField<bool>* miShyGuyTagModeTagOnSuicideField;
    MI_SelectField<short>* miShyGuyTagModeTagOnStompField;
    MI_SelectField<short>* miShyGuyTagModeFreeTimeField;
    MI_Button* miShyGuyTagModeBackButton;

    MI_Image* miShyGuyTagModeLeftHeaderBar;
    MI_Image* miShyGuyTagModeRightHeaderBar;
    MI_Text* miShyGuyTagModeHeaderText;

    // Boss
    MI_SelectField<Boss>* miBossModeTypeField;
    MI_SelectField<short>* miBossModeDifficultyField;
    MI_SelectField<short>* miBossModeHitPointsField;
    MI_Button* miBossModeBackButton;

    MI_Image* miBossModeLeftHeaderBar;
    MI_Image* miBossModeRightHeaderBar;
    MI_Text* miBossModeHeaderText;
};

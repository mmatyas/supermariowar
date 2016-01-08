#ifndef MENU_MODEOPTIONS_H
#define MENU_MODEOPTIONS_H

#include "uimenu.h"
#include "uicontrol.h"
#include "ui/MI_PowerupSlider.h"
#include "ui/MI_FrenzyModeOptions.h"

class UI_ModeOptionsMenu: public UI_Menu
{
	public:
		UI_ModeOptionsMenu();
		~UI_ModeOptionsMenu() {}

		void SetRandomGameModeSettings(short iMode);
		void SetControllingTeam(short iControlTeam);

        UI_Menu * GetOptionsMenu(short iMode) {
            return &mModeSettingsMenu[iMode];
        }
        UI_Menu * GetBossOptionsMenu() {
            return &mBossSettingsMenu;
        }

		void HealthModeStartLifeChanged();
		void HealthModeMaxLifeChanged();

		void Refresh();

	private:

		//Game Mode Settings Menu
		UI_Menu mModeSettingsMenu[22];

		UI_Menu mBossSettingsMenu;

		//Classic
		MI_SelectField * miClassicModeStyleField;
		MI_SelectField * miClassicModeScoringField;
		MI_Button * miClassicModeBackButton;

		MI_Image * miClassicModeLeftHeaderBar;
		MI_Image * miClassicModeRightHeaderBar;
		MI_Text * miClassicModeHeaderText;

		//Frag
		MI_SelectField * miFragModeStyleField;
		MI_SelectField * miFragModeScoringField;
		MI_Button * miFragModeBackButton;

		MI_Image * miFragModeLeftHeaderBar;
		MI_Image * miFragModeRightHeaderBar;
		MI_Text * miFragModeHeaderText;

		//Time Limit
		MI_SelectField * miTimeLimitModeStyleField;
		MI_SelectField * miTimeLimitModeScoringField;
		MI_SliderField * miTimeLimitModePercentExtraTime;
		MI_Button * miTimeLimitModeBackButton;

		MI_Image * miTimeLimitModeLeftHeaderBar;
		MI_Image * miTimeLimitModeRightHeaderBar;
		MI_Text * miTimeLimitModeHeaderText;

		//Jail
		MI_SelectField * miJailModeStyleField;
		MI_SelectField * miJailModeTagFreeField;
		MI_SelectField * miJailModeTimeFreeField;
		MI_SliderField * miJailModeJailKeyField;
		MI_Button * miJailModeBackButton;

		MI_Image * miJailModeLeftHeaderBar;
		MI_Image * miJailModeRightHeaderBar;
		MI_Text * miJailModeHeaderText;

		//Coins
		MI_SelectField * miCoinModePenaltyField;
		MI_SelectField * miCoinModeQuantityField;
		MI_SelectField * miCoinModePercentExtraCoin;
		MI_Button * miCoinModeBackButton;

		MI_Image * miCoinModeLeftHeaderBar;
		MI_Image * miCoinModeRightHeaderBar;
		MI_Text * miCoinModeHeaderText;

		//Yoshi's Eggs
		MI_PowerupSlider * miEggModeEggQuantityField[4];
		MI_PowerupSlider * miEggModeYoshiQuantityField[4];
		MI_SelectField * miEggModeExplosionTimeField;
		MI_Button * miEggModeBackButton;

		MI_Image * miEggModeLeftHeaderBar;
		MI_Image * miEggModeRightHeaderBar;
		MI_Text * miEggModeHeaderText;

		//Capture The Flag
		MI_SliderField * miFlagModeSpeedField;
		MI_SelectField * miFlagModeTouchReturnField;
		MI_SelectField * miFlagModePointMoveField;
		MI_SelectField * miFlagModeAutoReturnField;
		MI_SelectField * miFlagModeHomeScoreField;
		MI_SelectField * miFlagModeCenterFlagField;
		MI_Button * miFlagModeBackButton;

		MI_Image * miFlagModeLeftHeaderBar;
		MI_Image * miFlagModeRightHeaderBar;
		MI_Text * miFlagModeHeaderText;

		//Chicken
		MI_SelectField * miChickenModeShowTargetField;
		MI_SelectField * miChickenModeGlideField;
		MI_Button * miChickenModeBackButton;

		MI_Image * miChickenModeLeftHeaderBar;
		MI_Image * miChickenModeRightHeaderBar;
		MI_Text * miChickenModeHeaderText;

		//Tag
		MI_SelectField * miTagModeTagOnTouchField;
		MI_Button * miTagModeBackButton;

		MI_Image * miTagModeLeftHeaderBar;
		MI_Image * miTagModeRightHeaderBar;
		MI_Text * miTagModeHeaderText;

		//Star
		MI_SelectField * miStarModeTimeField;
		MI_SelectField * miStarModeShineField;
		MI_SliderField * miStarModePercentExtraTime;
		MI_Button * miStarModeBackButton;

		MI_Image * miStarModeLeftHeaderBar;
		MI_Image * miStarModeRightHeaderBar;
		MI_Text * miStarModeHeaderText;

		//Domination
		MI_SelectField * miDominationModeQuantityField;
		MI_SelectField * miDominationModeLoseOnDeathField;
		MI_SelectField * miDominationModeRelocateOnDeathField;
		MI_SelectField * miDominationModeStealOnDeathField;
		MI_SelectField * miDominationModeRelocateFrequencyField;

		MI_Text * miDominationModeDeathText;

		MI_Button * miDominationModeBackButton;

		MI_Image * miDominationModeLeftHeaderBar;
		MI_Image * miDominationModeRightHeaderBar;
		MI_Text * miDominationModeHeaderText;

		//King of the Hill
		MI_SelectField * miKingOfTheHillModeSizeField;
		MI_SelectField * miKingOfTheHillModeRelocateFrequencyField;
		MI_SelectField * miKingOfTheHillModeMultiplierField;

		MI_Button * miKingOfTheHillModeBackButton;

		MI_Image * miKingOfTheHillModeLeftHeaderBar;
		MI_Image * miKingOfTheHillModeRightHeaderBar;
		MI_Text * miKingOfTheHillModeHeaderText;

		//Race
		MI_SelectField * miRaceModeQuantityField;
		MI_SelectField * miRaceModeSpeedField;
		MI_SelectField * miRaceModePenaltyField;
		MI_Button * miRaceModeBackButton;

		MI_Image * miRaceModeLeftHeaderBar;
		MI_Image * miRaceModeRightHeaderBar;
		MI_Text * miRaceModeHeaderText;

		//Stomp
		MI_SelectField * miStompModeRateField;
		MI_PowerupSlider * miStompModeEnemySlider[NUMSTOMPENEMIES];
		MI_Button * miStompModeBackButton;

		MI_Image * miStompModeLeftHeaderBar;
		MI_Image * miStompModeRightHeaderBar;
		MI_Text * miStompModeHeaderText;

		//Frenzy
		MI_FrenzyModeOptions * miFrenzyModeOptions;

		MI_Image * miFrenzyModeLeftHeaderBar;
		MI_Image * miFrenzyModeRightHeaderBar;
		MI_Text * miFrenzyModeHeaderText;

		//Survival
		MI_PowerupSlider * miSurvivalModeEnemySlider[NUMSURVIVALENEMIES];
		MI_SelectField * miSurvivalModeDensityField;
		MI_SelectField * miSurvivalModeSpeedField;
		MI_SelectField * miSurvivalModeShieldField;
		MI_Button * miSurvivalModeBackButton;

		MI_Image * miSurvivalModeLeftHeaderBar;
		MI_Image * miSurvivalModeRightHeaderBar;
		MI_Text * miSurvivalModeHeaderText;

		//Greed
		MI_SelectField * miGreedModeCoinLife;
		MI_SelectField * miGreedModeOwnCoins;
		MI_SelectField * miGreedModeMultiplier;
		MI_SelectField * miGreedModePercentExtraCoin;
		MI_Button * miGreedModeBackButton;

		MI_Image * miGreedModeLeftHeaderBar;
		MI_Image * miGreedModeRightHeaderBar;
		MI_Text * miGreedModeHeaderText;

		//Health
		MI_SelectField * miHealthModeStartLife;
		MI_SelectField * miHealthModeMaxLife;
		MI_SliderField * miHealthModePercentExtraLife;
		MI_Button * miHealthModeBackButton;

		MI_Image * miHealthModeLeftHeaderBar;
		MI_Image * miHealthModeRightHeaderBar;
		MI_Text * miHealthModeHeaderText;

		//Card Collection
		MI_SelectField * miCollectionModeQuantityField;
		MI_SelectField * miCollectionModeRateField;
		MI_SelectField * miCollectionModeBankTimeField;
		MI_SelectField * miCollectionModeCardLifeField;
		MI_Button * miCollectionModeBackButton;

		MI_Image * miCollectionModeLeftHeaderBar;
		MI_Image * miCollectionModeRightHeaderBar;
		MI_Text * miCollectionModeHeaderText;

		//Phanto Chase
		MI_SelectField * miChaseModeSpeedField;
		MI_PowerupSlider * miChaseModeQuantitySlider[3];
		MI_Button * miChaseModeBackButton;

		MI_Image * miChaseModeLeftHeaderBar;
		MI_Image * miChaseModeRightHeaderBar;
		MI_Text * miChaseModeHeaderText;

		//Shyguy Tag
		MI_SelectField * miShyGuyTagModeTagOnSuicideField;
		MI_SelectField * miShyGuyTagModeTagOnStompField;
		MI_SelectField * miShyGuyTagModeFreeTimeField;
		MI_Button * miShyGuyTagModeBackButton;

		MI_Image * miShyGuyTagModeLeftHeaderBar;
		MI_Image * miShyGuyTagModeRightHeaderBar;
		MI_Text * miShyGuyTagModeHeaderText;

		//Boss
		MI_SelectField * miBossModeTypeField;
		MI_SelectField * miBossModeDifficultyField;
		MI_SelectField * miBossModeHitPointsField;
		MI_Button * miBossModeBackButton;

		MI_Image * miBossModeLeftHeaderBar;
		MI_Image * miBossModeRightHeaderBar;
		MI_Text * miBossModeHeaderText;
};

#endif // MENU_MODEOPTIONS_H

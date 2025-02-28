#pragma once

#include "EditorBase.h"

#include "GameMode.h"
#include "uimenu.h"

class MI_Button;
class MI_Image;
class MI_ImageSelectField;
class MI_MapField;
class MI_Text;
class MI_TextField;
template<typename T> class MI_SelectField;
class UI_ModeOptionsMenu;

class TourStop;


class EditorStages : public EditorBase {
public:
    EditorStages();

    void setupMenu(CResourceManager& rm);

protected:
    bool isSetupTransparent() const override { return true; }

    void onEnter(const WorldMap& world) override;
    void updateMenu(WorldMap& world) override;

    void renderSetup(CResourceManager& rm, const WorldMap& world) override;
    void onSetupKeypress(const SDL_KeyboardEvent& event, WorldMap& world) override;
    void onSetupMouseClick(const SDL_MouseButtonEvent& event, WorldMap& world) override;
    void onSetupMouseMotion(const SDL_MouseMotionEvent& event, WorldMap& world) override;

    bool onTileClicked(WorldMap& world, Vec2s pos, uint8_t button) override;

private:
    short iStageDisplay = -1;
    short iEditStage = -1;
    short m_selectedTileId = 0;

    SDL_Rect rStageBonusDst[10];
    SDL_Rect rHouseBonusDst[MAX_BONUS_CHESTS];
    SDL_Rect rItemDst[NUM_WORLD_ITEMS];

    MenuCodeEnum code = MENU_CODE_NONE;

    UI_Menu mStageSettingsMenu;
    UI_Menu mBonusItemPicker;
    UI_Menu* mCurrentMenu;

    MI_ImageSelectField* miModeField;
    MI_SelectField<short>* miGoalField[GAMEMODE_LAST];
    MI_Button* miModeSettingsButton;
    MI_Button* miBonusItemsButton;
    MI_SelectField<short>* miSpecialGoalField[3];

    MI_SelectField<bool>* miFinalStageField;
    MI_SelectField<short>* miPointsField;

    MI_TextField* miNameField;
    MI_SelectField<short>* miBonusType;
    MI_TextField* miBonusTextField[5];

    MI_MapField* miMapField;

    MI_Button* miDeleteStageButton;

    MI_Image* miDeleteStageDialogImage;
    MI_Text* miDeleteStageDialogAreYouText;
    MI_Text* miDeleteStageDialogSureText;
    MI_Button* miDeleteStageDialogYesButton;
    MI_Button* miDeleteStageDialogNoButton;

    UI_ModeOptionsMenu* mModeOptionsMenu;

    void NewStage(WorldMap& world, short* iEditStage);
    void EditStage(short iEditStage);
    void EnableStageMenu(bool fEnable);
    void TestAndSetBonusItem(TourStop& ts, short iPlace);
};

#pragma once

#include "EditorBase.h"

#include "uimenu.h"
#include "world.h"

class MI_Button;
class MI_ImageSelectField;
class MI_Text;
template<typename T> class MI_SelectField;


class EditorVehicles : public EditorBase {
public:
    EditorVehicles();

    void setupMenu(CResourceManager& rm);
    void setStamp(const WorldVehicle& vehicle);
    void onStageDeleted(WorldMap& world, short stageId);

protected:
    void loadAssets() override;
    bool isSetupTransparent() const override { return true; }
    void updateMenu() override;

    void onEnter(const WorldMap& world) override;
    void renderSetup(CResourceManager& rm, const WorldMap& world) override;
    void onSetupKeypress(const SDL_KeyboardEvent& event, WorldMap& world) override;
    void onSetupMouseClick(const SDL_MouseButtonEvent& event, WorldMap& world) override;

    bool onTileClicked(WorldMap& world, Vec2s pos, uint8_t button) override;

private:
    UI_Menu mVehicleMenu;
    MI_ImageSelectField* miVehicleSpriteField;
    MI_ImageSelectField* miVehicleStageField;
    MI_SelectField<short>* miVehicleMinMovesField;
    MI_SelectField<short>* miVehicleMaxMovesField;
    MI_SelectField<bool>* miVehiclePacesField;
    MI_SelectField<short>* miVehicleDirectionField;
    MI_SelectField<short>* miVehicleBoundaryField;
    MI_Button* miVehicleCreateButton;
    MI_Text* miTitleText;
    MenuCodeEnum mMenuCode = MENU_CODE_NONE;

    gfxSprite m_sprVehicleIcons;

    // Vehicle structure that holds the current vehicle "stamp"
    WorldVehicle m_vehicleStamp;
};

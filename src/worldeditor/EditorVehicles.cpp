#include "EditorVehicles.h"

#include "GameValues.h"
#include "path.h"
#include "ResourceManager.h"
#include "WorldTourStop.h"

#include "ui/MI_Button.h"
#include "ui/MI_ImageSelectField.h"
#include "ui/MI_Text.h"

extern CGameValues game_values;


namespace {
void addVehicle(WorldMap& world, Vec2s pos, const WorldVehicle& newVehicle)
{
    const auto it = std::find_if(
        world.getVehicles().begin(),
        world.getVehicles().end(),
        [pos](const WorldVehicle& vehicle) { return vehicle.getCurrentTile() == pos; });
    if (it != world.getVehicles().end()) {
        *it = newVehicle;
        it->SetPosition(pos.x, pos.y);
    } else {
        world.getVehicles().emplace_back(newVehicle);
        world.getVehicles().back().SetPosition(pos.x, pos.y);
    }
}

void removeVehicle(WorldMap& world, Vec2s pos)
{
    const auto eraseFrom = std::remove_if(
        world.getVehicles().begin(),
        world.getVehicles().end(),
        [pos](const WorldVehicle& vehicle) { return vehicle.getCurrentTile() == pos; });
    world.getVehicles().erase(eraseFrom, world.getVehicles().end());
}
} // namespace


EditorVehicles::EditorVehicles()
    : EditorBase()
{}


void EditorVehicles::loadAssets()
{
    m_sprVehicleIcons.init(convertPath("gfx/leveleditor/vehicle_icons.png"), colors::MAGENTA);
}


void EditorVehicles::setupMenu(CResourceManager& rm)
{
    miVehicleSpriteField = new MI_ImageSelectField(&rm.spr_selectfield, &m_sprVehicleIcons, 70, 80, "Sprite", 500, 150, 16, 16);
    miVehicleSpriteField->add("Hammer Brother", 0);
    miVehicleSpriteField->add("Boomerang Brother", 1);
    miVehicleSpriteField->add("Fire Brother", 2);
    miVehicleSpriteField->add("Tank 1", 3);
    miVehicleSpriteField->add("Boat 1", 4);
    miVehicleSpriteField->add("Boat 2", 5);
    miVehicleSpriteField->add("Airship 1", 6);
    miVehicleSpriteField->add("Airship 2", 7);
    miVehicleSpriteField->add("Tank 2", 8);
    miVehicleSpriteField->setOutputPtr(&m_vehicleStamp.iDrawSprite);
    miVehicleSpriteField->setCurrentValue(m_vehicleStamp.iDrawSprite);

    miVehicleStageField = new MI_ImageSelectField(&rm.spr_selectfield, &rm.menu_mode_small, 70, 120, "Stage", 500, 150, 16, 16);
    miVehicleStageField->setOutputPtr(&m_vehicleStamp.iActionId);

    miVehicleMinMovesField = new MI_SelectField<short>(&rm.spr_selectfield, 70, 160, "Min Moves", 500, 150);

    for (short iMinMoves = 0; iMinMoves <= 100; iMinMoves++) {
        char szMinMoves[8];
        sprintf(szMinMoves, "%d", iMinMoves);
        miVehicleMinMovesField->add(szMinMoves, iMinMoves);
    }

    miVehicleMinMovesField->setOutputPtr(&m_vehicleStamp.iMinMoves);
    miVehicleMinMovesField->setCurrentValue(m_vehicleStamp.iMinMoves);
    miVehicleMinMovesField->setItemChangedCode(MENU_CODE_VEHICLE_MIN_MOVES_CHANGED);
    miVehicleMinMovesField->allowWrap(false);
    miVehicleMinMovesField->allowFastScroll(true);

    miVehicleMaxMovesField = new MI_SelectField<short>(&rm.spr_selectfield, 70, 200, "Max Moves", 500, 150);

    for (short iMaxMoves = 0; iMaxMoves <= 100; iMaxMoves++) {
        char szMaxMoves[8];
        sprintf(szMaxMoves, "%d", iMaxMoves);
        miVehicleMaxMovesField->add(szMaxMoves, iMaxMoves);
    }

    miVehicleMaxMovesField->setOutputPtr(&m_vehicleStamp.iMaxMoves);
    miVehicleMaxMovesField->setCurrentValue(m_vehicleStamp.iMaxMoves);
    miVehicleMaxMovesField->setItemChangedCode(MENU_CODE_VEHICLE_MAX_MOVES_CHANGED);
    miVehicleMaxMovesField->allowWrap(false);
    miVehicleMaxMovesField->allowFastScroll(true);

    miVehiclePacesField = new MI_SelectField<bool>(&rm.spr_selectfield, 70, 240, "Paces", 500, 150);
    miVehiclePacesField->add("No", false);
    miVehiclePacesField->add("Yes", true);
    miVehiclePacesField->setOutputPtr(&m_vehicleStamp.fSpritePaces);
    miVehiclePacesField->setCurrentValue(m_vehicleStamp.fSpritePaces ? 1 : 0);
    miVehiclePacesField->setAutoAdvance(true);

    miVehicleDirectionField = new MI_SelectField<short>(&rm.spr_selectfield, 70, 280, "Direction", 500, 150);
    miVehicleDirectionField->add("Left", false);
    miVehicleDirectionField->add("Right", true);
    miVehicleDirectionField->setOutputPtr(&m_vehicleStamp.iDrawDirection);
    miVehicleDirectionField->setCurrentValue(m_vehicleStamp.iDrawDirection);
    miVehicleDirectionField->setAutoAdvance(true);

    miVehicleBoundaryField = new MI_SelectField<short>(&rm.spr_selectfield, 70, 320, "Boundary", 500, 150);
    miVehicleBoundaryField->add("No Boundary", 0);

    for (short iBoundary = 1; iBoundary <= 100; iBoundary++) {
        char szBoundary[8];
        sprintf(szBoundary, "%d", iBoundary);
        miVehicleBoundaryField->add(szBoundary, iBoundary);
    }

    miVehicleBoundaryField->setOutputPtr(&m_vehicleStamp.iBoundary);
    miVehicleBoundaryField->setCurrentValue(m_vehicleStamp.iBoundary);
    miVehicleBoundaryField->allowFastScroll(true);

    miVehicleCreateButton = new MI_Button(&rm.spr_selectfield, 430, 360, "OK", 140, TextAlign::CENTER);
    miVehicleCreateButton->SetCode(MENU_CODE_CREATE_VEHICLE);

    miTitleText = new MI_Text("Clicking on the map will add the vehicle configured below", 320, 50, 640, true, TextAlign::CENTER);
    mVehicleMenu.AddNonControl(miTitleText);

    mVehicleMenu.AddControl(miVehicleSpriteField, miVehicleCreateButton, miVehicleStageField, NULL, NULL);
    mVehicleMenu.AddControl(miVehicleStageField, miVehicleSpriteField, miVehicleMinMovesField, NULL, NULL);
    mVehicleMenu.AddControl(miVehicleMinMovesField, miVehicleStageField, miVehicleMaxMovesField, NULL, NULL);
    mVehicleMenu.AddControl(miVehicleMaxMovesField, miVehicleMinMovesField, miVehiclePacesField, NULL, NULL);
    mVehicleMenu.AddControl(miVehiclePacesField, miVehicleMaxMovesField, miVehicleDirectionField, NULL, NULL);
    mVehicleMenu.AddControl(miVehicleDirectionField, miVehiclePacesField, miVehicleBoundaryField, NULL, NULL);
    mVehicleMenu.AddControl(miVehicleBoundaryField, miVehicleDirectionField, miVehicleCreateButton, NULL, NULL);
    mVehicleMenu.AddControl(miVehicleCreateButton, miVehicleBoundaryField, miVehicleSpriteField, NULL, NULL);

    mVehicleMenu.setInitialFocus(miVehicleSpriteField);
    mVehicleMenu.SetCancelCode(MENU_CODE_EXIT_APPLICATION);
}


void EditorVehicles::setStamp(const WorldVehicle& vehicle)
{
    m_vehicleStamp = vehicle;

    miVehicleSpriteField->setCurrentValue(m_vehicleStamp.iDrawSprite);
    miVehicleStageField->setCurrentValue(m_vehicleStamp.iActionId);
    miVehicleMinMovesField->setCurrentValue(m_vehicleStamp.iMinMoves);
    miVehicleMaxMovesField->setCurrentValue(m_vehicleStamp.iMaxMoves);
    miVehiclePacesField->setCurrentValue(m_vehicleStamp.fSpritePaces ? 1 : 0);
    miVehicleDirectionField->setCurrentValue(m_vehicleStamp.iDrawDirection);
    miVehicleBoundaryField->setCurrentValue(m_vehicleStamp.iBoundary);
}


void EditorVehicles::onEnter(const WorldMap& world)
{
    EditorBase::onEnter(world);

    mVehicleMenu.ResetMenu();
    miVehicleStageField->clear();

    for (short iStage = 0; iStage < world.stageCount(); iStage++) {
        TourStop* ts = game_values.tourstops[iStage];
        char szStageName[256];
        sprintf(szStageName, "(%d) %s", iStage + 1, ts->szName);

        SF_ListItem<short>& item = miVehicleStageField->add(szStageName, iStage);
        item.iconOverride = ts->iStageType == 1 ? 24 : (ts->iMode >= 1000 ? ts->iMode - 975 : ts->iMode);
    }

    miVehicleStageField->setCurrentValue(m_vehicleStamp.iActionId);
}


void EditorVehicles::onSetupKeypress(const SDL_KeyboardEvent& event, WorldMap& world)
{
    switch (event.keysym.sym) {
        case SDLK_v:
            if (!mVehicleMenu.IsModifying()) {
                newlyEntered = false;
                return;
            }
    }
}


void EditorVehicles::onSetupMouseClick(const SDL_MouseButtonEvent& event, WorldMap& world)
{
    if (event.state == SDL_PRESSED && event.button == SDL_BUTTON_LEFT) {
        mMenuCode = mVehicleMenu.MouseClick(event.x, event.y);
    }
}


void EditorVehicles::updateMenu()
{
    switch (mMenuCode) {
        case MENU_CODE_EXIT_APPLICATION:
        case MENU_CODE_CREATE_VEHICLE:
            newlyEntered = false;
            return;
        case MENU_CODE_VEHICLE_MIN_MOVES_CHANGED: {
            const short iMaxMoves = miVehicleMaxMovesField->currentValue();
            if (miVehicleMinMovesField->currentValue() > iMaxMoves) {
                miVehicleMinMovesField->setCurrentValue(iMaxMoves);
            }
            break;
        }
        case MENU_CODE_VEHICLE_MAX_MOVES_CHANGED: {
            const short iMinMoves = miVehicleMinMovesField->currentValue();
            if (miVehicleMaxMovesField->currentValue() < iMinMoves) {
                miVehicleMaxMovesField->setCurrentValue(iMinMoves);
            }
            break;
        }
        default:
            break;
    }
}


void EditorVehicles::renderSetup(CResourceManager& rm, const WorldMap& world)
{
    mVehicleMenu.Update();
    mVehicleMenu.Draw();
}


bool EditorVehicles::onTileClicked(WorldMap& world, Vec2s pos, uint8_t button)
{
    if (button == SDL_BUTTON_LEFT) {
        addVehicle(world, pos, m_vehicleStamp);
    } else {
        removeVehicle(world, pos);
    }
    return false;
}

void EditorVehicles::onStageDeleted(WorldMap& world, short stageId)
{
    // Scan vehicles and remove references to deleted stage
    for (WorldVehicle& vehicle : world.getVehicles()) {
        if (vehicle.iActionId == stageId) {
            removeVehicle(world, vehicle.getCurrentTile());
        } else if (vehicle.iActionId > stageId) {
            vehicle.iActionId--;
        }
    }
}

#include "GraphicsOptionsMenu.h"

#include "FileList.h"
#include "GameValues.h"
#include "ResourceManager.h"

extern CResourceManager* rm;
extern CGameValues game_values;
extern GraphicsList *menugraphicspacklist;
extern GraphicsList *worldgraphicspacklist;
extern GraphicsList *gamegraphicspacklist;

UI_GraphicsOptionsMenu::UI_GraphicsOptionsMenu() : UI_Menu()
{
    miTopLayerField = new MI_SelectField(&rm->spr_selectfield, 70, 120, "Draw Top Layer", 500, 220);
    miTopLayerField->Add("Background", 0, "", false, false);
    miTopLayerField->Add("Foreground", 1, "", true, false);
    miTopLayerField->SetData(NULL, NULL, &game_values.toplayer);
    miTopLayerField->SetKey(game_values.toplayer ? 1 : 0);
    miTopLayerField->SetAutoAdvance(true);

    miFrameLimiterField = new MI_SelectField(&rm->spr_selectfield, 70, 160, "Frame Limit", 500, 220);
    miFrameLimiterField->Add("10 FPS", 100, "", false, false);
    miFrameLimiterField->Add("15 FPS", 67, "", false, false);
    miFrameLimiterField->Add("20 FPS", 50, "", false, false);
    miFrameLimiterField->Add("25 FPS", 40, "", false, false);
    miFrameLimiterField->Add("30 FPS", 33, "", false, false);
    miFrameLimiterField->Add("35 FPS", 28, "", false, false);
    miFrameLimiterField->Add("40 FPS", 25, "", false, false);
    miFrameLimiterField->Add("45 FPS", 22, "", false, false);
    miFrameLimiterField->Add("50 FPS", 20, "", false, false);
    miFrameLimiterField->Add("55 FPS", 18, "", false, false);
    miFrameLimiterField->Add("62 FPS (Normal)", 16, "", false, false);
    miFrameLimiterField->Add("66 FPS", 15, "", false, false);
    miFrameLimiterField->Add("71 FPS", 14, "", false, false);
    miFrameLimiterField->Add("77 FPS", 13, "", false, false);
    miFrameLimiterField->Add("83 FPS", 12, "", false, false);
    miFrameLimiterField->Add("90 FPS", 11, "", false, false);
    miFrameLimiterField->Add("100 FPS", 10, "", false, false);
    miFrameLimiterField->Add("111 FPS", 9, "", false, false);
    miFrameLimiterField->Add("125 FPS", 8, "", false, false);
    miFrameLimiterField->Add("142 FPS", 7, "", false, false);
    miFrameLimiterField->Add("166 FPS", 6, "", false, false);
    miFrameLimiterField->Add("200 FPS", 5, "", false, false);
    miFrameLimiterField->Add("250 FPS", 4, "", false, false);
    miFrameLimiterField->Add("333 FPS", 3, "", false, false);
    miFrameLimiterField->Add("500 FPS", 2, "", false, false);
    miFrameLimiterField->Add("No Limit", 0, "", false, false);
    miFrameLimiterField->SetData(&game_values.framelimiter, NULL, NULL);
    miFrameLimiterField->SetKey(game_values.framelimiter);

#ifdef _XBOX
    miScreenSettingsButton = new MI_Button(&rm->spr_selectfield, 70, 200, "Screen Settings", 500, 0);
    miScreenSettingsButton->SetCode(MENU_CODE_TO_SCREEN_SETTINGS);
#else
    miFullscreenField = new MI_SelectField(&rm->spr_selectfield, 70, 200, "Screen Size", 500, 220);
    miFullscreenField->Add("Windowed", 0, "", false, false);
    miFullscreenField->Add("Fullscreen", 1, "", true, false);
    miFullscreenField->SetData(NULL, NULL, &game_values.fullscreen);
    miFullscreenField->SetKey(game_values.fullscreen ? 1 : 0);
    miFullscreenField->SetAutoAdvance(true);
    miFullscreenField->SetItemChangedCode(MENU_CODE_TOGGLE_FULLSCREEN);
#endif //_XBOX

    miMenuGraphicsPackField = new MI_PacksField(&rm->spr_selectfield, 70, 240, "Menu Graphics", 500, 220, menugraphicspacklist, MENU_CODE_MENU_GRAPHICS_PACK_CHANGED);
    miWorldGraphicsPackField = new MI_PacksField(&rm->spr_selectfield, 70, 280, "World Graphics", 500, 220, worldgraphicspacklist, MENU_CODE_WORLD_GRAPHICS_PACK_CHANGED);
    miGameGraphicsPackField = new MI_PacksField(&rm->spr_selectfield, 70, 320, "Game Graphics", 500, 220, gamegraphicspacklist, MENU_CODE_GAME_GRAPHICS_PACK_CHANGED);

    miGraphicsOptionsMenuBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, 1);
    miGraphicsOptionsMenuBackButton->SetCode(MENU_CODE_BACK_TO_OPTIONS_MENU);

    miGraphicsOptionsMenuLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miGraphicsOptionsMenuRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miGraphicsOptionsMenuHeaderText = new MI_Text("Graphics Options Menu", 320, 5, 0, 2, 1);

    AddControl(miTopLayerField, miGraphicsOptionsMenuBackButton, miFrameLimiterField, NULL, miGraphicsOptionsMenuBackButton);

#ifdef _XBOX
    AddControl(miFrameLimiterField, miTopLayerField, miScreenSettingsButton, NULL, miGraphicsOptionsMenuBackButton);
    AddControl(miScreenSettingsButton, miFrameLimiterField, miMenuGraphicsPackField, NULL, miGraphicsOptionsMenuBackButton);
    AddControl(miMenuGraphicsPackField, miScreenSettingsButton, miWorldGraphicsPackField, NULL, miGraphicsOptionsMenuBackButton);
#else
    AddControl(miFrameLimiterField, miTopLayerField, miFullscreenField, NULL, miGraphicsOptionsMenuBackButton);
    AddControl(miFullscreenField, miFrameLimiterField, miMenuGraphicsPackField, NULL, miGraphicsOptionsMenuBackButton);
    AddControl(miMenuGraphicsPackField, miFullscreenField, miWorldGraphicsPackField, NULL, miGraphicsOptionsMenuBackButton);
#endif

    AddControl(miWorldGraphicsPackField, miMenuGraphicsPackField, miGameGraphicsPackField, NULL, miGraphicsOptionsMenuBackButton);
    AddControl(miGameGraphicsPackField, miWorldGraphicsPackField, miGraphicsOptionsMenuBackButton, NULL, miGraphicsOptionsMenuBackButton);
    AddControl(miGraphicsOptionsMenuBackButton, miGameGraphicsPackField, miTopLayerField, miGameGraphicsPackField, NULL);

    AddNonControl(miGraphicsOptionsMenuLeftHeaderBar);
    AddNonControl(miGraphicsOptionsMenuRightHeaderBar);
    AddNonControl(miGraphicsOptionsMenuHeaderText);

    SetHeadControl(miTopLayerField);
    SetCancelCode(MENU_CODE_BACK_TO_OPTIONS_MENU);
};

UI_GraphicsOptionsMenu::~UI_GraphicsOptionsMenu() {
}

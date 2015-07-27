#include "NetEditServersMenu.h"

#include "ResourceManager.h"

extern CResourceManager* rm;

UI_NetEditServersMenu::UI_NetEditServersMenu() : UI_Menu()
{
    miBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, 1);
    miBackButton->SetCode(MENU_CODE_TO_NET_SERVERS_MENU);

    miLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miHeaderText = new MI_Text("Add/Remove Servers Menu", 320, 5, 0, 2, 1);

    AddControl(miBackButton, NULL, NULL, NULL, NULL);

    AddNonControl(miLeftHeaderBar);
    AddNonControl(miRightHeaderBar);
    AddNonControl(miHeaderText);

    SetHeadControl(miBackButton);
    SetCancelCode(MENU_CODE_TO_NET_SERVERS_MENU);
};

UI_NetEditServersMenu::~UI_NetEditServersMenu() {
}

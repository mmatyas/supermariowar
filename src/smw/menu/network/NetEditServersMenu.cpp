#include "NetEditServersMenu.h"

#include "ResourceManager.h"

extern CResourceManager* rm;

UI_NetEditServersMenu::UI_NetEditServersMenu() : UI_Menu()
{
    miBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, 1);
    miBackButton->SetCode(MENU_CODE_TO_NET_SERVERS_MENU);

    miAddButton = new MI_Button(&rm->spr_selectfield, 40, 40, "Add", 200, 1);

    miEditButton = new MI_Button(&rm->spr_selectfield, 40, 80, "Edit", 200, 1);

    miRemoveButton = new MI_Button(&rm->spr_selectfield, 40, 120, "Remove", 200, 1);

    miLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miHeaderText = new MI_Text("Add/Remove Servers Menu", 320, 5, 0, 2, 1);

    miServerScroll = new MI_StringScroll(&rm->menu_plain_field, 260, 32, 350, 11);
    miServerScroll->SetAutoModify(true);
    miServerScroll->Deactivate();

    AddControl(miAddButton, miBackButton, miEditButton, NULL, NULL);
    AddControl(miEditButton, miAddButton, miRemoveButton, NULL, NULL);
    AddControl(miRemoveButton, miEditButton, miBackButton, NULL, NULL);
    AddControl(miBackButton, miRemoveButton, miAddButton, NULL, NULL);

    AddNonControl(miLeftHeaderBar);
    AddNonControl(miRightHeaderBar);
    AddNonControl(miHeaderText);

    AddControl(miServerScroll, NULL, NULL, NULL, NULL);

    SetHeadControl(miBackButton);
    SetCancelCode(MENU_CODE_TO_NET_SERVERS_MENU);
};

UI_NetEditServersMenu::~UI_NetEditServersMenu() {
}

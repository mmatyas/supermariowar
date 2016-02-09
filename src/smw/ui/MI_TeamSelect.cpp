#include "MI_TeamSelect.h"

#include "GameValues.h"
#include "ResourceManager.h"
#include "FileList.h"

extern CGameValues game_values;
extern CResourceManager* rm;

extern SkinList *skinlist;

/**************************************
 * MI_TeamSelect Class
 **************************************/

MI_TeamSelect::MI_TeamSelect(gfxSprite * spr_background_ref, short x, short y) :
    UI_Control(x, y)
{
    spr = spr_background_ref;
    miImage = new MI_Image(spr, ix, iy, 0, 0, 416, 256, 1, 1, 0);

    for (short iTeam = 0; iTeam < 4; iTeam++) {
        iTeamCounts[iTeam] = game_values.teamcounts[iTeam];

        for (short iSlot = 0; iSlot < 3; iSlot++)
            iTeamIDs[iTeam][iSlot] = game_values.teamids[iTeam][iSlot];

        fReady[iTeam] = false;
    }

    iAnimationTimer = 0;
    iAnimationFrame = 0;
    iRandomAnimationFrame = 0;
}

MI_TeamSelect::~MI_TeamSelect()
{
    delete miImage;
}

void MI_TeamSelect::Update()
{
    if (++iAnimationTimer > 7) {
        iAnimationTimer = 0;

        iAnimationFrame += 2;
        if (iAnimationFrame > 2)
            iAnimationFrame = 0;

        iRandomAnimationFrame += 32;
        if (iRandomAnimationFrame >= 128)
            iRandomAnimationFrame = 0;
    }
}

void MI_TeamSelect::Draw()
{
    if (!fShow)
        return;

    miImage->Draw();

    short iPlayerCount = 0;

    for (short iPlayer = 0; iPlayer < 4; iPlayer++) {
        if (game_values.playercontrol[iPlayer] > 0)
            iPlayerCount++;
    }

    for (short iTeam = 0; iTeam < 4; iTeam++) {
        for (short iTeamItem = 0; iTeamItem < iTeamCounts[iTeam]; iTeamItem++) {
            short iPlayerID = iTeamIDs[iTeam][iTeamItem];

            if (game_values.randomskin[iPlayerID])
                spr->draw(iTeam * 96 + 43 + ix, iTeamItem * 36 + 52 + iy, 416, fReady[iPlayerID] ? 0 : iRandomAnimationFrame, 42, 32);
            else
                rm->spr_player[iPlayerID][fReady[iPlayerID] ? 0 : iAnimationFrame]->draw(iTeam * 96 + 48 + ix, iTeamItem * 36 + 52 + iy, 0, 0, 32, 32);

            rm->spr_menu_boxed_numbers.draw(iTeam * 96 + 44 + ix, iTeamItem * 36 + 72 + iy, iPlayerID * 16, game_values.colorids[iPlayerID] * 16, 16, 16);
        }

        if (game_values.playercontrol[iTeam] > 0) {
            rm->spr_player_select_ready.draw(iTeam * 160 + 16, 368, 0, 0, 128, 96);

            rm->spr_menu_boxed_numbers.draw(iTeam * 160 + 32, 388, iTeam * 16, game_values.colorids[iTeam] * 16, 16, 16);
            rm->menu_font_small.drawChopRight(iTeam * 160 + 52, 404 - rm->menu_font_small.getHeight(), 80, game_values.randomskin[iTeam] ? "Random" : skinlist->GetSkinName(game_values.skinids[iTeam]));

            rm->spr_player_select_ready.draw(iTeam * 160 + 64, 408, 128, (!fReady[iTeam] ? 0 : (game_values.playercontrol[iTeam] == 1 ? 32 : 64)), 34, 32);
        }
    }

    if (fAllReady) {
        rm->menu_plain_field.draw(ix + 108, iy + smw->ScreenHeight * 0.47f, 0, 160, 100, 32);
        rm->menu_plain_field.draw(ix + 208, iy + smw->ScreenHeight * 0.47f, 412, 160, 100, 32);
        rm->menu_font_large.drawCentered(smw->ScreenWidth/2, iy + smw->ScreenHeight * 0.48f, "Continue");
    }
}

MenuCodeEnum MI_TeamSelect::SendInput(CPlayerInput * playerInput)
{
    for (short iPlayer = 0; iPlayer < 4; iPlayer++) {
        COutputControl * playerKeys = &game_values.playerInput.outputControls[iPlayer];

        if (game_values.playercontrol[iPlayer] > 0 && !fReady[iPlayer]) { //if this player is player or cpu
            if (playerKeys->menu_left.fPressed) {
                if (playerKeys->menu_right.fDown)
                    game_values.randomskin[iPlayer] = !game_values.randomskin[iPlayer];
                else
                    FindNewTeam(iPlayer, -1);
            }

            if (playerKeys->menu_right.fPressed) {
                if (playerKeys->menu_left.fDown)
                    game_values.randomskin[iPlayer] = !game_values.randomskin[iPlayer];
                else
                    FindNewTeam(iPlayer, 1);
            }

            //Scroll up/down through player skins
            if (!game_values.randomskin[iPlayer]) {
                if (playerKeys->menu_up.fPressed) {
                    do {
                        if (playerKeys->menu_down.fDown) {
                            game_values.skinids[iPlayer] = RANDOM_INT(skinlist->GetCount());
                        } else {
                            if (--game_values.skinids[iPlayer] < 0)
                                game_values.skinids[iPlayer] = (short)skinlist->GetCount() - 1;
                        }
                    } while (!rm->LoadMenuSkin(iPlayer, game_values.skinids[iPlayer], game_values.colorids[iPlayer], false));
                } else if (playerKeys->menu_up.fDown) {
                    if (iFastScroll[iPlayer] == 0) {
                        if (++iFastScrollTimer[iPlayer] > 40) {
                            iFastScroll[iPlayer] = 1;
                        }
                    } else {
                        if (++iFastScrollTimer[iPlayer] > 5) {
                            do {
                                if (--game_values.skinids[iPlayer] < 0)
                                    game_values.skinids[iPlayer] = (short)skinlist->GetCount() - 1;
                            } while (!rm->LoadMenuSkin(iPlayer, game_values.skinids[iPlayer], game_values.colorids[iPlayer], false));

                            iFastScrollTimer[iPlayer] = 0;
                        }
                    }
                }

                if (playerKeys->menu_down.fPressed) {
                    do {
                        if (playerKeys->menu_up.fDown) {
                            game_values.skinids[iPlayer] = RANDOM_INT( skinlist->GetCount());
                        } else {
                            if (++game_values.skinids[iPlayer] >= skinlist->GetCount())
                                game_values.skinids[iPlayer] = 0;
                        }
                    } while (!rm->LoadMenuSkin(iPlayer, game_values.skinids[iPlayer], game_values.colorids[iPlayer], false));
                } else if (playerKeys->menu_down.fDown) {
                    if (iFastScroll[iPlayer] == 0) {
                        if (++iFastScrollTimer[iPlayer] > 40) {
                            iFastScroll[iPlayer] = 1;
                        }
                    } else {
                        if (++iFastScrollTimer[iPlayer] > 5) {
                            do {
                                if (++game_values.skinids[iPlayer] >= skinlist->GetCount())
                                    game_values.skinids[iPlayer] = 0;
                            } while (!rm->LoadMenuSkin(iPlayer, game_values.skinids[iPlayer], game_values.colorids[iPlayer], false));

                            iFastScrollTimer[iPlayer] = 0;
                        }
                    }
                }

                if ((!playerKeys->menu_up.fDown && !playerKeys->menu_down.fDown) ||
                        (playerKeys->menu_up.fDown && playerKeys->menu_down.fDown)) {
                    iFastScroll[iPlayer] = 0;
                    iFastScrollTimer[iPlayer] = 0;
                }
            } else {
                iFastScroll[iPlayer] = 0;
                iFastScrollTimer[iPlayer] = 0;
            }

            if (playerKeys->menu_random.fPressed) {
                if (playerKeys->menu_scrollfast.fDown) {
                    game_values.randomskin[iPlayer] = !game_values.randomskin[iPlayer];
                } else if (!game_values.randomskin[iPlayer]) {
                    do {
                        game_values.skinids[iPlayer] = RANDOM_INT(skinlist->GetCount());
                    } while (!rm->LoadMenuSkin(iPlayer, game_values.skinids[iPlayer], game_values.colorids[iPlayer], false));
                }
            }
        }

        if (playerInput->outputControls[iPlayer].menu_select.fPressed) {
            fReady[iPlayer] = true;

            if (fAllReady && (DEVICE_KEYBOARD != playerInput->inputControls[iPlayer]->iDevice || iPlayer == 0)) {
                fModifying = false;
                printf("MI_TeamSelect::SendInput MENU_CODE_TO_GAME_SETUP_MENU\n");
                return MENU_CODE_TO_GAME_SETUP_MENU;
            }

            fAllReady = true;
            for (short i = 0; i < 4; i++) {
                if (!fReady[i]) {
                    fAllReady = false;
                    break;
                }
            }
        }

        if (playerInput->outputControls[iPlayer].menu_cancel.fPressed) {
            if (game_values.playercontrol[iPlayer] > 0 && fReady[iPlayer]) {
                fReady[iPlayer] = false;
                fAllReady = false;
            } else {
                if (DEVICE_KEYBOARD != playerInput->inputControls[iPlayer]->iDevice || iPlayer == 0) {
                    fModifying = false;
                    return MENU_CODE_UNSELECT_ITEM;
                }
            }
        }
    }

    return MENU_CODE_NONE;
}

MenuCodeEnum MI_TeamSelect::Modify(bool modify)
{
    fModifying = modify;
    return MENU_CODE_MODIFY_ACCEPTED;
}

void MI_TeamSelect::FindNewTeam(short iPlayerID, short iDirection)
{
    for (short iTeam = 0; iTeam < 4; iTeam++) {
        for (short iTeamItem = 0; iTeamItem < iTeamCounts[iTeam]; iTeamItem++) {
            if (iTeamIDs[iTeam][iTeamItem] == iPlayerID) {
                iTeamCounts[iTeam]--;

                for (int iMovePlayer = iTeamItem; iMovePlayer < iTeamCounts[iTeam]; iMovePlayer++)
                    iTeamIDs[iTeam][iMovePlayer] = iTeamIDs[iTeam][iMovePlayer + 1];

                short iNewTeam = iTeam;
                bool fOnlyTeam = true;

                do {
                    iNewTeam += iDirection;

                    if (iNewTeam < 0)
                        iNewTeam = 3;
                    else if (iNewTeam > 3)
                        iNewTeam = 0;

                    fOnlyTeam = true;
                    for (int iMovePlayer = 0; iMovePlayer < 4; iMovePlayer++) {
                        if (iMovePlayer == iNewTeam)
                            continue;

                        if (iTeamCounts[iMovePlayer] > 0) {
                            fOnlyTeam = false;
                            break;
                        }
                    }
                } while (fOnlyTeam);

                iTeamIDs[iNewTeam][iTeamCounts[iNewTeam]] = iPlayerID;
                iTeamCounts[iNewTeam]++;

                if (game_values.teamcolors) {
                    game_values.colorids[iPlayerID] = iNewTeam;

                    //Skip skins that are invalid
                    while (!rm->LoadMenuSkin(iPlayerID, game_values.skinids[iPlayerID], iNewTeam, false)) {
                        if (++game_values.skinids[iPlayerID] >= skinlist->GetCount())
                            game_values.skinids[iPlayerID] = 0;
                    }
                }

                return;
            }
        }
    }
}

void MI_TeamSelect::Reset()
{
    for (short iPlayer = 0; iPlayer < 4; iPlayer++) {
        short iTeamID;
        short iSlotID;
        bool fFound = false;
        for (iTeamID = 0; iTeamID < 4; iTeamID++) {
            for (iSlotID = 0; iSlotID < iTeamCounts[iTeamID]; iSlotID++) {
                if (iTeamIDs[iTeamID][iSlotID] == iPlayer) {
                    fFound = true;
                    break;
                }
            }

            if (fFound)
                break;
        }

        if (fFound) {
            //Need to remove the player
            if (game_values.playercontrol[iPlayer] == 0) {
                iTeamCounts[iTeamID]--;

                if (iTeamCounts[iTeamID] > iSlotID) {
                    for (short iSlot = iSlotID; iSlot < iTeamCounts[iTeamID]; iSlot++) {
                        iTeamIDs[iTeamID][iSlot] = iTeamIDs[iTeamID][iSlot + 1];
                    }
                }
            }
        } else {
            //A new player was added so find a spot for him
            if (game_values.playercontrol[iPlayer] > 0) {
                short iLookForNewTeam = iPlayer;

                while (iTeamCounts[iLookForNewTeam] >= 3) {
                    if (++iLookForNewTeam >= 4)
                        iLookForNewTeam = 0;
                }

                iTeamIDs[iLookForNewTeam][iTeamCounts[iLookForNewTeam]] = iPlayer;
                iTeamCounts[iLookForNewTeam]++;

                if (game_values.teamcolors)
                    game_values.colorids[iPlayer] = iLookForNewTeam;
            }
        }
    }

    //Check to see if there is only one team and if so, split them up

    short iCountTeams = 0;
    short iLastTeam = 0;
    for (short iTeamID = 0; iTeamID < 4; iTeamID++) {
        if (iTeamCounts[iTeamID] > 0) {
            iCountTeams++;
            iLastTeam = iTeamID;
        }
    }

    if (iCountTeams == 1) {
        short iLookForNewTeam = iLastTeam;
        if (++iLookForNewTeam >= 4)
            iLookForNewTeam = 0;

        iTeamCounts[iLastTeam]--;
        short iPlayer = iTeamIDs[iLastTeam][iTeamCounts[iLastTeam]];
        iTeamIDs[iLookForNewTeam][iTeamCounts[iLookForNewTeam]] = iPlayer;
        iTeamCounts[iLookForNewTeam]++;

        if (game_values.teamcolors)
            game_values.colorids[iPlayer] = iLookForNewTeam;
    }

    iAnimationTimer = 0;
    iAnimationFrame = 0;

    fAllReady = true;

    for (short iPlayer = 0; iPlayer < 4; iPlayer++) {
        iFastScroll[iPlayer] = 0;
        iFastScrollTimer[iPlayer] = 0;

        if (game_values.playercontrol[iPlayer] == 1) {
            fReady[iPlayer] = false;
            fAllReady = false;
        } else {
            fReady[iPlayer] = true;
        }

        //If debug build, then ignore the player ready stuff
#ifdef _DEBUG
        fReady[iPlayer] = true;
        fAllReady = true;
#endif

        if (game_values.playercontrol[iPlayer] == 0)
            continue;

        if (game_values.teamcolors)
            game_values.colorids[iPlayer] = GetTeam(iPlayer);
        else
            game_values.colorids[iPlayer] = iPlayer;

        //Skip skins that are invalid
        //TODO: this loops forever in emscripten build
        while (!rm->LoadMenuSkin(iPlayer, game_values.skinids[iPlayer], game_values.colorids[iPlayer], false)) {
            if (++game_values.skinids[iPlayer] >= skinlist->GetCount())
                game_values.skinids[iPlayer] = 0;
        }
    }
}

short MI_TeamSelect::OrganizeTeams()
{
    iNumTeams = 0;
    for (short iTeam = 0; iTeam < 4; iTeam++) {
        game_values.teamcounts[iTeam] = 0;

        if (iTeamCounts[iTeam] > 0) {
            for (short iTeamSpot = 0; iTeamSpot < 3; iTeamSpot++)
                game_values.teamids[iNumTeams][iTeamSpot] = iTeamIDs[iTeam][iTeamSpot];

            game_values.teamcounts[iNumTeams] = iTeamCounts[iTeam];
            iNumTeams++;
        }
    }

    return iNumTeams;
}

short MI_TeamSelect::GetTeam(short iPlayerID)
{
    for (short iTeam = 0; iTeam < 4; iTeam++) {
        for (short iSlot = 0; iSlot < iTeamCounts[iTeam]; iSlot++) {
            if (iTeamIDs[iTeam][iSlot] == iPlayerID) {
                return iTeam;
            }
        }
    }

    return -1;
}

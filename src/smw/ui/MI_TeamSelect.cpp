#include "MI_TeamSelect.h"

#include "gfx.h"
#include "Game.h"
#include "GameValues.h"
#include "RandomNumberGenerator.h"
#include "ResourceManager.h"
#include "FileList.h"
#include "ui/MI_Image.h"

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

    for (short iTeam = 0; iTeam < MAX_PLAYERS; iTeam++) {
        iTeamCounts[iTeam] = game_values.teamcounts[iTeam];

        for (short iSlot = 0; iSlot < MAX_TEAM_SIZE; iSlot++)
            iTeamIDs[iTeam][iSlot] = game_values.teamids[iTeam][iSlot];

        fReady[iTeam] = false;
        iFastScroll[iTeam] = 0;
        iFastScrollTimer[iTeam] = 0;
    }

    iAnimationTimer = 0;
    iAnimationFrame = 0;
    iRandomAnimationFrame = 0;
}

MI_TeamSelect::~MI_TeamSelect()
{
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

namespace {
// Layout of the team select background image (player_select_background.png)
constexpr short BG_WIDTH = 416;
constexpr short BG_HEIGHT = 208;
constexpr short BG_FIRST_COLUMN_X = 14;   // the first team column, starting with its left border
constexpr short BG_COLUMN_WIDTH = 96;
constexpr short BG_COLUMN_TOP = 24;       // below the header with the team names
constexpr short BG_COLUMN_BOTTOM = 184;
constexpr short BG_HEADER_X = 90;         // an empty part of the header, between two team names
constexpr short BG_HEADER_FROM = 24;
constexpr short BG_HEADER_TO = 392;
constexpr short RANDOM_ICON_WIDTH = 42;

constexpr short WIDE_COLUMN_WIDTH = BG_COLUMN_WIDTH / 2;
static_assert(WIDE_COLUMN_WIDTH * MAX_PLAYERS == BG_COLUMN_WIDTH * NUM_BASE_TEAM_COLORS);
} // namespace

bool MI_TeamSelect::IsWideLayout() const
{
    for (short iPlayer = NUM_BASE_TEAM_COLORS; iPlayer < MAX_PLAYERS; iPlayer++) {
        if (game_values.playercontrol[iPlayer] > 0)
            return true;
    }
    return false;
}

short MI_TeamSelect::GetTeamSlotCount() const
{
    return IsWideLayout() ? MAX_PLAYERS : NUM_BASE_TEAM_COLORS;
}

void MI_TeamSelect::DrawBackground(bool fWide) const
{
    // The generated team columns are inserted after the 4th column in the image,
    // so everything to the right of that has moved
    const short iGeneratedWidth = (MAX_PLAYERS - NUM_BASE_TEAM_COLORS) * BG_COLUMN_WIDTH;
    const short iColumnsEnd = BG_FIRST_COLUMN_X + NUM_BASE_TEAM_COLORS * BG_COLUMN_WIDTH;
    const bool fHasGeneratedColumns = spr->getWidth() > BG_WIDTH + iGeneratedWidth;

    if (!fWide) {
        spr->draw(m_pos.x, m_pos.y, {0, 0, iColumnsEnd, BG_HEIGHT});
        spr->draw(m_pos.x + iColumnsEnd, m_pos.y, {iColumnsEnd + (fHasGeneratedColumns ? iGeneratedWidth : 0), 0, BG_WIDTH - iColumnsEnd, BG_HEIGHT});
        return;
    }

    // Header: keep the corners, but hide the team names (they belong to the 4 column layout)
    spr->draw(m_pos.x, m_pos.y, {0, 0, BG_HEADER_FROM, BG_COLUMN_TOP});
    for (short x = BG_HEADER_FROM; x < BG_HEADER_TO; x++)
        spr->draw(m_pos.x + x, m_pos.y, {BG_HEADER_X, 0, 1, BG_COLUMN_TOP});
    spr->draw(m_pos.x + BG_HEADER_TO, m_pos.y, {BG_HEADER_TO + (fHasGeneratedColumns ? iGeneratedWidth : 0), 0, BG_WIDTH - BG_HEADER_TO, BG_COLUMN_TOP});

    // Left and right side of the frame, and the footer
    spr->draw(m_pos.x, m_pos.y + BG_COLUMN_TOP, {0, BG_COLUMN_TOP, BG_FIRST_COLUMN_X, BG_HEIGHT - BG_COLUMN_TOP});
    spr->draw(m_pos.x + BG_FIRST_COLUMN_X, m_pos.y + BG_COLUMN_BOTTOM, {BG_FIRST_COLUMN_X, BG_COLUMN_BOTTOM, iColumnsEnd - BG_FIRST_COLUMN_X, BG_HEIGHT - BG_COLUMN_BOTTOM});
    spr->draw(m_pos.x + iColumnsEnd, m_pos.y + BG_COLUMN_TOP, {iColumnsEnd + (fHasGeneratedColumns ? iGeneratedWidth : 0), BG_COLUMN_TOP, BG_WIDTH - iColumnsEnd, BG_HEIGHT - BG_COLUMN_TOP});

    // The left half of every team's column (the generated colors follow the original ones in the image)
    const short iTeamColumns = fHasGeneratedColumns ? MAX_PLAYERS : NUM_BASE_TEAM_COLORS;
    for (short iTeam = 0; iTeam < MAX_PLAYERS; iTeam++) {
        spr->draw(m_pos.x + BG_FIRST_COLUMN_X + iTeam * WIDE_COLUMN_WIDTH, m_pos.y + BG_COLUMN_TOP,
            {BG_FIRST_COLUMN_X + (iTeam % iTeamColumns) * BG_COLUMN_WIDTH, BG_COLUMN_TOP, WIDE_COLUMN_WIDTH, BG_COLUMN_BOTTOM - BG_COLUMN_TOP});
    }
}

void MI_TeamSelect::DrawReadyPanel(short iPlayer, short x, short y, short w) const
{
    constexpr short PANEL_W = 128;
    constexpr short PANEL_H = 96;

    if (w >= PANEL_W) {
        rm->spr_player_select_ready.draw(x, y, {0, 0, PANEL_W, PANEL_H});
    } else {
        // Shrink the panel: keep the left and right edges, cut out the middle
        const short half = w / 2;
        rm->spr_player_select_ready.draw(x, y, {0, 0, half, PANEL_H});
        rm->spr_player_select_ready.draw(x + half, y, {PANEL_W - (w - half), 0, w - half, PANEL_H});
    }

    const short nameX = x + 36;
    rm->spr_menu_boxed_numbers.draw(x + 16, y + 20, {iPlayer * 16, game_values.colorids[iPlayer] * 16, 16, 16});
    rm->menu_font_small.drawChopRight(nameX, y + 36 - rm->menu_font_small.getHeight(), x + w - 10 - nameX,
        game_values.randomskin[iPlayer] ? "Random" : skinlist->at(game_values.skinids[iPlayer]).name.c_str());

    rm->spr_player_select_ready.draw(x + (w - 34) / 2, y + 40,
        {128, (!fReady[iPlayer] ? 0 : (game_values.playercontrol[iPlayer] == 1 ? 32 : 64)), 34, 32});
}

void MI_TeamSelect::Draw()
{
    if (!m_visible)
        return;

    const bool fWide = IsWideLayout();
    DrawBackground(fWide);

    const short iSlots = GetTeamSlotCount();
    const short iColumnWidth = fWide ? WIDE_COLUMN_WIDTH : BG_COLUMN_WIDTH;
    const short iRandomIconX = spr->getWidth() - RANDOM_ICON_WIDTH;

    for (short iTeam = 0; iTeam < iSlots; iTeam++) {
        // Spread the members of large teams evenly in the column
        short iItemSpacing = 36;
        short iFirstItemY = 52;
        if (iTeamCounts[iTeam] > 3) {
            iFirstItemY = BG_COLUMN_TOP + 4;
            iItemSpacing = (BG_COLUMN_BOTTOM - 4 - 36 - iFirstItemY) / (iTeamCounts[iTeam] - 1);
        }

        // Center of the colored part of the column
        const short iCenterX = m_pos.x + BG_FIRST_COLUMN_X + iTeam * iColumnWidth + 2 + iColumnWidth / 2;

        for (short iTeamItem = 0; iTeamItem < iTeamCounts[iTeam]; iTeamItem++) {
            short iPlayerID = iTeamIDs[iTeam][iTeamItem];
            const short iItemY = m_pos.y + iFirstItemY + iTeamItem * iItemSpacing;

            if (game_values.randomskin[iPlayerID])
                spr->draw(iCenterX - 21, iItemY, {iRandomIconX, fReady[iPlayerID] ? 0 : iRandomAnimationFrame, RANDOM_ICON_WIDTH, 32});
            else
                rm->spr_player[iPlayerID][fReady[iPlayerID] ? 0 : iAnimationFrame].draw(iCenterX - 16, iItemY, {0, 0, 32, 32});

            rm->spr_menu_boxed_numbers.draw(iCenterX - 20, iItemY + 20, {iPlayerID * 16, game_values.colorids[iPlayerID] * 16, 16, 16});
        }
    }

    // Ready panels at the bottom of the screen
    const short iPanels = fWide ? MAX_PLAYERS : NUM_BASE_TEAM_COLORS;
    const short iPanelPitch = App::screenWidth / iPanels;
    for (short iPlayer = 0; iPlayer < iPanels; iPlayer++) {
        if (game_values.playercontrol[iPlayer] > 0) {
            if (fWide)
                DrawReadyPanel(iPlayer, iPlayer * iPanelPitch + 1, 368, iPanelPitch - 2);
            else
                DrawReadyPanel(iPlayer, iPlayer * iPanelPitch + 16, 368, 128);
        }
    }

    if (fAllReady) {
        rm->menu_plain_field.draw(m_pos.x + 108, m_pos.y + App::screenHeight * 0.47f, {0, 160, 100, 32});
        rm->menu_plain_field.draw(m_pos.x + 208, m_pos.y + App::screenHeight * 0.47f, {412, 160, 100, 32});
        rm->menu_font_large.drawCentered(App::screenWidth/2, m_pos.y + App::screenHeight * 0.48f, "Continue");
    }
}

MenuCodeEnum MI_TeamSelect::SendInput(CPlayerInput * playerInput)
{
    for (short iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++) {
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
                            game_values.skinids[iPlayer] = RANDOM_INT(skinlist->count());
                        } else {
                            if (--game_values.skinids[iPlayer] < 0)
                                game_values.skinids[iPlayer] = (short)skinlist->count() - 1;
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
                                    game_values.skinids[iPlayer] = (short)skinlist->count() - 1;
                            } while (!rm->LoadMenuSkin(iPlayer, game_values.skinids[iPlayer], game_values.colorids[iPlayer], false));

                            iFastScrollTimer[iPlayer] = 0;
                        }
                    }
                }

                if (playerKeys->menu_down.fPressed) {
                    do {
                        if (playerKeys->menu_up.fDown) {
                            game_values.skinids[iPlayer] = RANDOM_INT( skinlist->count());
                        } else {
                            if (++game_values.skinids[iPlayer] >= static_cast<short>(skinlist->count()))
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
                                if (++game_values.skinids[iPlayer] >= static_cast<short>(skinlist->count()))
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
                        game_values.skinids[iPlayer] = RANDOM_INT(skinlist->count());
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
            for (short i = 0; i < MAX_PLAYERS; i++) {
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
                    return MENU_CODE_BACK_TO_MATCH_SELECTION_MENU;
                }
            }
        }
        if (DEVICE_KEYBOARD != playerInput->inputControls[iPlayer]->iDevice) {
            short team = GetTeam(iPlayer);
            gfx_setjoystickteamcolor(SDL_GetJoystickFromPlayerIndex(playerInput->inputControls[iPlayer]->iDevice), team, fReady[iPlayer] ? 1.0 : 0.5);
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
    const short iSlots = GetTeamSlotCount();

    for (short iTeam = 0; iTeam < MAX_PLAYERS; iTeam++) {
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
                        iNewTeam = iSlots - 1;
                    else if (iNewTeam > iSlots - 1)
                        iNewTeam = 0;

                    // Don't let everyone be on the same team
                    fOnlyTeam = true;
                    for (int iOtherTeam = 0; iOtherTeam < MAX_PLAYERS; iOtherTeam++) {
                        if (iOtherTeam == iNewTeam)
                            continue;

                        if (iTeamCounts[iOtherTeam] > 0) {
                            fOnlyTeam = false;
                            break;
                        }
                    }
                } while (fOnlyTeam);

                iTeamIDs[iNewTeam][iTeamCounts[iNewTeam]] = iPlayerID;
                iTeamCounts[iNewTeam]++;

                SetPlayerColor(iPlayerID, iNewTeam);
                return;
            }
        }
    }
}

void MI_TeamSelect::SetPlayerColor(short iPlayerID, short iTeam)
{
    if (!game_values.teamcolors)
        return;

    game_values.colorids[iPlayerID] = iTeam;

    //Skip skins that are invalid
    while (!rm->LoadMenuSkin(iPlayerID, game_values.skinids[iPlayerID], iTeam, false)) {
        if (++game_values.skinids[iPlayerID] >= static_cast<short>(skinlist->count()))
            game_values.skinids[iPlayerID] = 0;
    }
}

void MI_TeamSelect::Reset()
{
    const short iSlots = GetTeamSlotCount();

    for (short iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++) {
        short iTeamID;
        short iSlotID;
        bool fFound = false;
        for (iTeamID = 0; iTeamID < MAX_PLAYERS; iTeamID++) {
            for (iSlotID = 0; iSlotID < iTeamCounts[iTeamID]; iSlotID++) {
                if (iTeamIDs[iTeamID][iSlotID] == iPlayer) {
                    fFound = true;
                    break;
                }
            }

            if (fFound)
                break;
        }

        // Remove players who left, and players who are on a team slot that is not available anymore
        if (fFound && (game_values.playercontrol[iPlayer] == 0 || iTeamID >= iSlots)) {
            iTeamCounts[iTeamID]--;

            for (short iSlot = iSlotID; iSlot < iTeamCounts[iTeamID]; iSlot++)
                iTeamIDs[iTeamID][iSlot] = iTeamIDs[iTeamID][iSlot + 1];

            fFound = false;
        }

        //A new player was added so find a spot for him
        if (!fFound && game_values.playercontrol[iPlayer] > 0) {
            // By default everyone is on their own team, otherwise look for a team that is not full
            short iLookForNewTeam = iPlayer % iSlots;

            while (iTeamCounts[iLookForNewTeam] >= MAX_TEAM_SIZE) {
                if (++iLookForNewTeam >= iSlots)
                    iLookForNewTeam = 0;
            }

            iTeamIDs[iLookForNewTeam][iTeamCounts[iLookForNewTeam]] = iPlayer;
            iTeamCounts[iLookForNewTeam]++;

            if (game_values.teamcolors)
                game_values.colorids[iPlayer] = iLookForNewTeam;
        }
    }

    //Check to see if there is only one team and if so, split them up

    short iCountTeams = 0;
    short iLastTeam = 0;
    for (short iTeamID = 0; iTeamID < iSlots; iTeamID++) {
        if (iTeamCounts[iTeamID] > 0) {
            iCountTeams++;
            iLastTeam = iTeamID;
        }
    }

    if (iCountTeams == 1) {
        short iLookForNewTeam = iLastTeam;
        if (++iLookForNewTeam >= iSlots)
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

    for (short iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++) {
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
            if (++game_values.skinids[iPlayer] >= static_cast<short>(skinlist->count()))
                game_values.skinids[iPlayer] = 0;
        }
    }
}

short MI_TeamSelect::OrganizeTeams()
{
    iNumTeams = 0;
    for (short iTeam = 0; iTeam < MAX_PLAYERS; iTeam++) {
        game_values.teamcounts[iTeam] = 0;
    }

    for (short iTeam = 0; iTeam < MAX_PLAYERS; iTeam++) {
        if (iTeamCounts[iTeam] > 0) {
            for (short iTeamSpot = 0; iTeamSpot < MAX_TEAM_SIZE; iTeamSpot++)
                game_values.teamids[iNumTeams][iTeamSpot] = iTeamIDs[iTeam][iTeamSpot];

            game_values.teamcounts[iNumTeams] = iTeamCounts[iTeam];
            iNumTeams++;
        }
    }

    return iNumTeams;
}

short MI_TeamSelect::GetTeam(short iPlayerID)
{
    for (short iTeam = 0; iTeam < MAX_PLAYERS; iTeam++) {
        for (short iSlot = 0; iSlot < iTeamCounts[iTeam]; iSlot++) {
            if (iTeamIDs[iTeam][iSlot] == iPlayerID) {
                return iTeam;
            }
        }
    }

    return -1;
}

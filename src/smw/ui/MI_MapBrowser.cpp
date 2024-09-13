#include "MI_MapBrowser.h"

#include "Game.h"
#include "GameValues.h"
#include "map.h"
#include "MapList.h"
#include "path.h"
#include "ResourceManager.h"

#include "SDL_image.h"

extern CGameValues game_values;
extern CResourceManager* rm;
extern CMap* g_map;
extern MapList* maplist;

extern SDL_Surface* blitdest;

extern short LookupTeamID(short id);

namespace {
#ifndef __EMSCRIPTEN__
inline void smallDelay() { SDL_Delay(10); }
#else
inline void smallDelay() {}
#endif
} // namespace


MI_MapBrowser::MI_MapBrowser()
    : UI_Control(0, 0)
{
    for (short iSurface = 0; iSurface < 9; iSurface++)
        mapSurfaces[iSurface] = nullptr;

    srcRectBackground.x = 0;
    srcRectBackground.y = 0;
    srcRectBackground.w = App::screenWidth;
    srcRectBackground.h = App::screenHeight;

    dstRectBackground.x = 0;
    dstRectBackground.y = 0;
    dstRectBackground.w = 160;
    dstRectBackground.h = 120;
}

MI_MapBrowser::~MI_MapBrowser()
{
    for (short iSurface = 0; iSurface < 9; iSurface++) {
        if (mapSurfaces[iSurface]) {
            SDL_FreeSurface(mapSurfaces[iSurface]);
            mapSurfaces[iSurface] = nullptr;
        }
    }
}

void MI_MapBrowser::Update()
{
    if (++iFilterTagAnimationTimer > 8) {
        iFilterTagAnimationTimer = 0;

        iFilterTagAnimationFrame += 24;
        if (iFilterTagAnimationFrame > 24)
            iFilterTagAnimationFrame = 0;
    }
}

void MI_MapBrowser::Draw()
{
    if (!m_visible)
        return;

    SDL_Rect rSrc = {0, 0, 160, 120};
    SDL_Rect rDst = {0, 0, 160, 120};

    for (short iRow = 0; iRow < 3; iRow++) {
        rDst.y = iRow * 150 + 30;

        for (short iCol = 0; iCol < 3; iCol++) {
            if (iSelectedCol != iCol || iSelectedRow != iRow) {
                if (iRow * 3 + iCol + iPage * 9 >= iMapCount)
                    break;

                rDst.x = iCol * 200 + 40;

                SDL_BlitSurface(mapSurfaces[iRow * 3 + iCol], &rSrc, blitdest, &rDst);

                if (iType == 0) {
                    if (mapListNodes[iRow * 3 + iCol]->pfFilters[game_values.selectedmapfilter])
                        rm->menu_map_filter.draw(rDst.x, rDst.y, iFilterTagAnimationFrame, 24, 24, 24);
                }

                rm->menu_font_large.drawChopRight(rDst.x, rDst.y + 120, 165, mapNames[iRow * 3 + iCol].c_str());
            }
        }
    }

           //Draw the selected map
    rDst.y = iSelectedRow * 150 + 30;
    rDst.x = iSelectedCol * 200 + 40;

    rm->menu_dialog.draw(rDst.x - 16, rDst.y - 16, 0, 0, 176, 148);
    rm->menu_dialog.draw(rDst.x + 160, rDst.y - 16, 496, 0, 16, 148);
    rm->menu_dialog.draw(rDst.x - 16, rDst.y + 132, 0, 464, 176, 16);
    rm->menu_dialog.draw(rDst.x + 160, rDst.y + 132, 496, 464, 16, 16);

    SDL_BlitSurface(mapSurfaces[iSelectedRow * 3 + iSelectedCol], &rSrc, blitdest, &rDst);

    if (iType == 0) {
        if (mapListNodes[iSelectedRow * 3 + iSelectedCol]->pfFilters[game_values.selectedmapfilter])
            rm->menu_map_filter.draw(rDst.x, rDst.y, iFilterTagAnimationFrame, 24, 24, 24);
    }

    rm->menu_font_large.drawChopRight(rDst.x, rDst.y + 120, 165, mapNames[iSelectedRow * 3 + iSelectedCol].c_str());
}

MenuCodeEnum MI_MapBrowser::Modify(bool modify)
{
    fModifying = modify;
    return MENU_CODE_MODIFY_ACCEPTED;
}

MenuCodeEnum MI_MapBrowser::SendInput(CPlayerInput * playerInput)
{
    for (int iPlayer = 0; iPlayer < 4; iPlayer++) {
        //Only allow the controlling team to control the menu (if there is one)
        if (iControllingTeam != -1) {
            if (iControllingTeam != LookupTeamID(iPlayer) || game_values.playercontrol[iPlayer] != 1)
                continue;
        }

        if (playerInput->outputControls[iPlayer].menu_down.fPressed) {
            short iSkipRows = 1;
            if (playerInput->outputControls[iPlayer].menu_scrollfast.fDown)
                iSkipRows = 3;

            iSelectedIndex = (iSelectedRow + iSkipRows) * 3 + iSelectedCol + iPage * 9;

            if (iSelectedIndex >= iMapCount)
                iSelectedIndex = iMapCount - 1;

            iSelectedRow = (iSelectedIndex / 3) % 3;
            iSelectedCol = iSelectedIndex % 3;

            short iOldPage = iPage;
            iPage = iSelectedIndex / 9;

            if (iOldPage != iPage)
                LoadPage(iPage, iType == 1);

            return MENU_CODE_NONE;
        }

        if (playerInput->outputControls[iPlayer].menu_up.fPressed) {
            short iSkipRows = 1;
            if (playerInput->outputControls[iPlayer].menu_scrollfast.fDown)
                iSkipRows = 3;

            iSelectedIndex = (iSelectedRow - iSkipRows) * 3 + iSelectedCol + iPage * 9;

            if (iSelectedIndex < 0)
                iSelectedIndex = 0;

            iSelectedRow = (iSelectedIndex / 3) % 3;
            iSelectedCol = iSelectedIndex % 3;

            short iOldPage = iPage;
            iPage = iSelectedIndex / 9;

            if (iOldPage != iPage)
                LoadPage(iPage, iType == 1);

            return MENU_CODE_NONE;
        }

        if (playerInput->outputControls[iPlayer].menu_left.fPressed) {
            short iNextIndex = iSelectedRow * 3 + iSelectedCol - 1 + iPage * 9;

            if (iNextIndex < 0)
                return MENU_CODE_NONE;

            iSelectedIndex = iNextIndex;

            if (--iSelectedCol < 0) {
                iSelectedCol = 2;

                if (--iSelectedRow < 0) {
                    iSelectedRow = 2;
                    LoadPage(--iPage, iType == 1);
                }
            }

            return MENU_CODE_NONE;
        }

        if (playerInput->outputControls[iPlayer].menu_right.fPressed) {
            short iNextIndex = iSelectedRow * 3 + iSelectedCol + 1 + iPage * 9;

            if (iNextIndex >= iMapCount)
                return MENU_CODE_NONE;

            iSelectedIndex = iNextIndex;

            if (++iSelectedCol > 2) {
                iSelectedCol = 0;

                if (++iSelectedRow > 2) {
                    iSelectedRow = 0;
                    LoadPage(++iPage, iType == 1);
                }
            }

            return MENU_CODE_NONE;
        }

        if (playerInput->outputControls[iPlayer].menu_select.fPressed) {
            if (iType == 0) {
                mapListNodes[iSelectedRow * 3 + iSelectedCol]->pfFilters[game_values.selectedmapfilter] = !mapListNodes[iSelectedRow * 3 + iSelectedCol]->pfFilters[game_values.selectedmapfilter];
                game_values.fNeedWriteFilters = true;
            } else {
                maplist->SetCurrent(mapListItr[iSelectedRow * 3 + iSelectedCol]);
                return MENU_CODE_MAP_BROWSER_EXIT;
            }

            return MENU_CODE_NONE;
        }

        if (playerInput->outputControls[iPlayer].menu_cancel.fPressed) {
            return MENU_CODE_MAP_BROWSER_EXIT;
        }
    }

    return MENU_CODE_NONE;
}

void MI_MapBrowser::Reset(short type)
{
    iType = type;

    if (iType == 0) {
        iSelectedIndex = maplist->GetCurrent()->second->iIndex;

        iFilterTagAnimationTimer = 0;
        iFilterTagAnimationFrame = 72;

        iMapCount = maplist->count();
    } else {
        iSelectedIndex = maplist->GetCurrent()->second->iFilteredIndex;
        iMapCount = maplist->filteredCount();
    }

    iSelectedRow = (iSelectedIndex / 3) % 3;
    iSelectedCol = iSelectedIndex % 3;
    iPage = iSelectedIndex / 9;

    LoadPage(iPage, iType == 1);
}

void MI_MapBrowser::LoadPage(short page, bool fUseFilters)
{
    for (short iMap = 0; iMap < 9; iMap++) {
        short iIndex = iMap + page * 9;

        if (iIndex >= iMapCount)
            return;

        std::map<std::string, MapListNode*>::iterator itr = maplist->GetIteratorAt(iIndex, fUseFilters);

               //See if we already have a thumbnail saved for this map

        std::string szThumbnail("maps/cache/");
        szThumbnail += GetNameFromFileName((*itr).second->filename.c_str());

#ifdef PNG_SAVE_FORMAT
        szThumbnail += ".png";
#else
        szThumbnail += ".bmp";
#endif

        std::string sConvertedPath = convertPath(szThumbnail);

        if (!FileExists(sConvertedPath)) {
            g_map->loadMap((*itr).second->filename, read_type_preview);
            smallDelay();  //Sleeps to help the music from skipping
            g_map->saveThumbnail(sConvertedPath, false);
            smallDelay();
        }

        if (mapSurfaces[iMap])
            SDL_FreeSurface(mapSurfaces[iMap]);

        mapSurfaces[iMap] = IMG_Load(sConvertedPath.c_str());

        mapListNodes[iMap] = (*itr).second;
        mapNames[iMap] = (*itr).first.c_str();
        mapListItr[iMap] = itr;
    }
}

#include "MI_World.h"

#include "eyecandy.h"
#include "FileList.h"
#include "Game.h"
#include "GameValues.h"
#include "ResourceManager.h"
#include "uimenu.h"
#include "world.h"
#include "WorldTourStop.h"

#include <cmath>

extern CGameValues game_values;
extern CResourceManager* rm;
extern WorldMap g_worldmap;
extern WorldMusicList* worldmusiclist;

extern SDL_Surface* screen;
extern SDL_Surface* blitdest;

extern short LookupTeamID(short id);


MI_World::MI_World()
    : UI_Control(0, 0)
{
    sMapSurface[0] = SDL_CreateRGBSurface(screen->flags, 768, 608, screen->format->BitsPerPixel, 0, 0, 0, 0);
    sMapSurface[1] = SDL_CreateRGBSurface(screen->flags, 768, 608, screen->format->BitsPerPixel, 0, 0, 0, 0);

    rectSrcSurface.x = 0;
    rectSrcSurface.y = 0;
    rectSrcSurface.w = 768;
    rectSrcSurface.h = 608;

    rectDstSurface.x = 0;
    rectDstSurface.y = 0;
    rectDstSurface.w = App::screenWidth;
    rectDstSurface.h = App::screenHeight;
}

MI_World::~MI_World()
{
    if (sMapSurface[0]) {
        SDL_FreeSurface(sMapSurface[0]);
        sMapSurface[0] = NULL;
    }

    if (sMapSurface[1]) {
        SDL_FreeSurface(sMapSurface[1]);
        sMapSurface[1] = NULL;
    }
}

void MI_World::Init()
{
    iCycleIndex = 0;
    g_worldmap.ResetDrawCycle();

    iAnimationFrame = 0;
    iDrawFullRefresh = 0;

    g_worldmap.InitPlayer();

    iMapDrawOffsetCol = 0;
    iMapDrawOffsetRow = 0;

    iNextMapDrawOffsetCol = 0;
    iNextMapDrawOffsetRow = 0;

    iState = -2;

    for (short iTeam = 0; iTeam < 4; iTeam++) {
        iStateTransition[iTeam] = 0;
        iItemPopupDrawY[iTeam] = 0;
        iPopupFlag[iTeam] = false;
    }

    iNumPopups = 0;

    iStoredItemPopupDrawY = -48;

    iVehicleId = -1;

    SetMapOffset();
    RepositionMapImage();

    g_worldmap.DrawMapToSurface(-1, true, sMapSurface[0], iMapDrawOffsetCol, iMapDrawOffsetRow, iAnimationFrame);
    g_worldmap.DrawMapToSurface(-1, true, sMapSurface[1], iMapDrawOffsetCol, iMapDrawOffsetRow, iAnimationFrame);

    dTeleportStarRadius = 300.0f;
    dTeleportStarAngle = 0.0f;
    iTeleportStarAnimationFrame = 0;
    iTeleportStarAnimationTimer = 0;

    fForceStageStart = false;
    fNoInterestingMoves = false;
    iSleepTurns = 0;
    fUsingCloud = false;
    game_values.worldpointsbonus = -1;

    iPressSelectTimer = 0;
    pressSelectKeys = NULL;

    iDrawWidth = g_worldmap.iWidth < 20 ? g_worldmap.iWidth << 5 : App::screenWidth;
    iDrawHeight = g_worldmap.iHeight < 15 ? g_worldmap.iHeight << 5 : App::screenHeight;

    iSrcOffsetX = 0;
    iSrcOffsetY = 0;
    iDstOffsetX = 0;
    iDstOffsetY = 0;

    if (g_worldmap.iWidth < 20)
        iDstOffsetX = (20 - g_worldmap.iWidth) << 4;

    if (g_worldmap.iHeight < 15)
        iDstOffsetY = (15 - g_worldmap.iHeight) << 4;
}

void MI_World::SetControllingTeam(short iWinningTeam)
{
    iControllingTeam = iWinningTeam;
    iControllingPlayerId = game_values.teamids[iControllingTeam][RANDOM_INT(game_values.teamcounts[iControllingTeam])];
    g_worldmap.SetPlayerSprite(iControllingPlayerId);

    fNoInterestingMoves = false;
}

void MI_World::DisplayTeamControlAnnouncement()
{
    char szMessage[128];
    if (game_values.teamcounts[iControllingTeam] <= 1)
        sprintf(szMessage, "Player %d Is In Control", game_values.teamids[iControllingTeam][0] + 1);
    else
        sprintf(szMessage, "Team %d Is In Control", iControllingTeam + 1);

    uiMenu->AddEyeCandy(new EC_Announcement(&rm->menu_font_large, &rm->spr_announcementicons, szMessage, game_values.colorids[iControllingPlayerId], 120, 100));
}

void MI_World::SetCurrentStageToCompleted(short iWinningTeam)
{
    short iBonusAdd = 0;
    short iBonusMult = 1;

    if (game_values.worldpointsbonus == 0)
        iBonusMult = 0;
    else if (game_values.worldpointsbonus >= 1 && game_values.worldpointsbonus <= 3)
        iBonusAdd = game_values.worldpointsbonus;
    else if (game_values.worldpointsbonus >= 4 && game_values.worldpointsbonus <= 5)
        iBonusMult = game_values.worldpointsbonus - 2;

    if (iVehicleId >= 0) {
        game_values.tournament_scores[iWinningTeam].total += g_worldmap.GetVehicleStageScore(iVehicleId) * iBonusMult + iBonusAdd;
        g_worldmap.RemoveVehicle(iVehicleId);
    } else {
        short iPlayerCurrentTileX, iPlayerCurrentTileY;
        g_worldmap.GetPlayerCurrentTile(&iPlayerCurrentTileX, &iPlayerCurrentTileY);

        WorldMapTile * tile = &g_worldmap.tiles[iPlayerCurrentTileX][iPlayerCurrentTileY];
        //tile->iForegroundSprite = game_values.colorids[game_values.teamids[iWinningTeam][0]] + WORLD_WINNING_TEAM_SPRITE_OFFSET; //Update with team completed sprite
        //tile->fAnimated = false; //Update with team completed sprite
        tile->iCompleted = game_values.colorids[game_values.teamids[iWinningTeam][0]];

        g_worldmap.UpdateTile(sMapSurface[0], iPlayerCurrentTileX - iMapDrawOffsetCol, iPlayerCurrentTileY - iMapDrawOffsetRow, iMapDrawOffsetCol, iMapDrawOffsetRow, iAnimationFrame);
        g_worldmap.UpdateTile(sMapSurface[1], iPlayerCurrentTileX - iMapDrawOffsetCol, iPlayerCurrentTileY - iMapDrawOffsetRow, iMapDrawOffsetCol, iMapDrawOffsetRow, iAnimationFrame);

        game_values.tournament_scores[iWinningTeam].total += game_values.tourstops[tile->iType - 6]->iPoints * iBonusMult + iBonusAdd;
    }

           //Only advance the turn if the players played a real game, no bonus houses
    if (game_values.singleplayermode == -1)
        AdvanceTurn();
}

void MI_World::AdvanceTurn()
{
    if (iSleepTurns > 0) {
        if (--iSleepTurns <= 0) {
            rm->backgroundmusic[5].load(worldmusiclist->GetMusic(g_worldmap.GetMusicCategory(), g_worldmap.GetWorldName()));
            rm->backgroundmusic[5].play(false, false);
        }
    } else {
        g_worldmap.MoveVehicles();
    }

    g_worldmap.MoveBridges();

           //Update the map with the flipped bridges
           //TODO:: Need to update just bridge tiles across both surfaces

    fNoInterestingMoves = false;
}

void MI_World::Update()
{
    bool fPlayerVehicleCollision = false;
    bool fPlayerMoveDone = g_worldmap.Update(&fPlayerVehicleCollision);

    short iPlayerX, iPlayerY;
    g_worldmap.GetPlayerPosition(&iPlayerX, &iPlayerY);

    short iPlayerState = g_worldmap.GetPlayerState();

    UpdateMapSurface(iCycleIndex);

    if (++iCycleIndex > 16) {
        iCurrentSurfaceIndex = 1 - iCurrentSurfaceIndex;

        if (iDrawFullRefresh == 1)
            iDrawFullRefresh = 2;
        else
            iDrawFullRefresh = 0;

        iCycleIndex = 0;
        iAnimationFrame += TILESIZE;

        if (iAnimationFrame >= 128)
            iAnimationFrame = 0;
    }

    if (fPlayerMoveDone)
        RepositionMapImage();

    if (iSleepTurns <= 0 && !fUsingCloud && (fPlayerVehicleCollision || fPlayerMoveDone)) {
        short iStage = g_worldmap.GetVehicleInPlayerTile(&iVehicleId);
        if (iStage >= 0 && iStage < g_worldmap.iNumStages) {
            game_values.tourstopcurrent = iStage;
            fForceStageStart = true;
        }
    }

           //Player is moving from one tile to the next (up)
    if (iPlayerState == 1) {
        if (g_worldmap.iHeight > 15 && iMapOffsetY < 0 && iPlayerY < (g_worldmap.iHeight << 5) - (App::screenHeight * 0.53f))
            iMapOffsetY += 2;
    } else if (iPlayerState == 2) { //down
        if (g_worldmap.iHeight > 15 && iMapOffsetY > App::screenHeight - (g_worldmap.iHeight << 5) && iPlayerY > (App::screenHeight * 0.47f))
            iMapOffsetY -= 2;
    } else if (iPlayerState == 3) { //left
        if (g_worldmap.iWidth > 20 && iMapOffsetX < 0 && iPlayerX < (g_worldmap.iWidth << 5) - (App::screenWidth * 0.525f))
            iMapOffsetX += 2;
    } else if (iPlayerState == 4) { //right
        if (g_worldmap.iWidth > 20 && iMapOffsetX > App::screenWidth - (g_worldmap.iWidth << 5) && iPlayerX > (App::screenWidth * 0.475f))
            iMapOffsetX -= 2;
    }

    if (iState == -2 || iState >= 4) {
        dTeleportStarRadius += (iState == 4 || iState == 6 ? 5.0f : -5.0f);
        dTeleportStarAngle -= 0.15f;

        if (++iTeleportStarAnimationTimer >= 3) {
            iTeleportStarAnimationTimer = 0;
            iTeleportStarAnimationFrame += 32;

            if (iTeleportStarAnimationFrame > 32)
                iTeleportStarAnimationFrame = 0;
        }

        if (dTeleportStarRadius <= 0.0f) {
            if (iState == -2 || iState == 7)
                iState = -1;

            dTeleportStarRadius = 0.0f;
        }
    }

    if (iNumPopups > 0) {
        for (short iTeam = 0; iTeam < 4; iTeam++) {
            //Do inventory popup open effect
            if (iStateTransition[iTeam] == 1) {
                iItemPopupDrawY[iTeam] += 4;

                if (iItemPopupDrawY[iTeam] >= 32) {
                    iItemPopupDrawY[iTeam] = 32;
                    iStateTransition[iTeam] = 3;
                }
            } else if (iStateTransition[iTeam] == 2) { // Do close effect
                iItemPopupDrawY[iTeam] -= 4;

                if (iItemPopupDrawY[iTeam] <= 0) {
                    iItemPopupDrawY[iTeam] = 0;
                    iStateTransition[iTeam] = 0;

                           //Shift down popup menus if one was removed that was below
                    bool fStartShift = false;
                    for (short iMoveTeam = 0; iMoveTeam < iNumPopups - 1; iMoveTeam++) {
                        if (iPopupOrder[iMoveTeam] == iTeam) {
                            fStartShift = true;
                        }

                        if (fStartShift) {
                            iPopupOrder[iMoveTeam] = iPopupOrder[iMoveTeam + 1];
                            iPopupOffsets[iPopupOrder[iMoveTeam]] = iMoveTeam << 6;
                        }
                    }

                    iNumPopups--;
                }
            }
        }

        for (short iTeam = 0; iTeam < 4; iTeam++) {
            //Transition the inventory popup to it's new location if it needs to move
            if (iPopupOffsetsCurrent[iTeam] > iPopupOffsets[iTeam]) {
                iPopupOffsetsCurrent[iTeam] -= 4;
                if (iPopupOffsetsCurrent[iTeam] < iPopupOffsets[iTeam])
                    iPopupOffsetsCurrent[iTeam] = iPopupOffsets[iTeam];
            }
        }
    }

    bool iShowStoredItems = false;
    for (short iTeam = 0; iTeam < 4; iTeam++) {
        if ((iShowStoredItems = (iStateTransition[iTeam] == 1 || iStateTransition[iTeam] == 3)))
            break;
    }

    if (iShowStoredItems && iStoredItemPopupDrawY < 16)
        iStoredItemPopupDrawY += 8;
    else if (!iShowStoredItems && iStoredItemPopupDrawY > -48)
        iStoredItemPopupDrawY -= 8;


    if (iState == 4 || iState == 5) {
        iScreenfade += iScreenfadeRate;

        if (iState == 4 && iScreenfade > 255) {
            g_worldmap.SetPlayerPosition(iWarpCol, iWarpRow);
            SetMapOffset();
            RepositionMapImage();

                   //These 3 lines allow us to only refresh the entire map to one surface and sets it up so that the
                   //other surface will update with the normal flow
            iCurrentSurfaceIndex = 0;
            iCycleIndex = 0;
            g_worldmap.ResetDrawCycle();

            iDrawFullRefresh = 2; //Draw one full refresh to next surface

            g_worldmap.DrawMapToSurface(-1, true, sMapSurface[0], iMapDrawOffsetCol, iMapDrawOffsetRow, iAnimationFrame);
            //g_worldmap.DrawMapToSurface(-1, true, sMapSurface[1], iMapDrawOffsetCol, iMapDrawOffsetRow, iAnimationFrame);

            iState = 5;
            iScreenfade = 255;
            iScreenfadeRate = -8;
        } else if (iState == 5 && iScreenfade < 0) {
            iState = -1;
            iScreenfade = 0;
        }
    } else if (iState == 6) {
        if (dTeleportStarRadius > 150.0f)
            iState = 7;
    }
}

void MI_World::UpdateMapSurface(short iCycleIndex)
{
    if (iCycleIndex >= 0 && iCycleIndex <= 15)
        g_worldmap.DrawMapToSurface(iCycleIndex, iDrawFullRefresh > 0, sMapSurface[1 - iCurrentSurfaceIndex], iNextMapDrawOffsetCol, iNextMapDrawOffsetRow, iAnimationFrame);
}

void MI_World::SetMapOffset()
{
    short iPlayerX, iPlayerY;
    g_worldmap.GetPlayerPosition(&iPlayerX, &iPlayerY);

    if (g_worldmap.iWidth > 20) {
        if (iPlayerX < (g_worldmap.iWidth << 5) - 336 && iPlayerX > 304)
            iMapOffsetX = 304 - iPlayerX;
        else if (iPlayerX <= 304)
            iMapOffsetX = 0;
        else
            iMapOffsetX = App::screenWidth - (g_worldmap.iWidth << 5);
    } else {
        iMapOffsetX = (App::screenWidth - (g_worldmap.iWidth << 5)) >> 1;
    }

    if (g_worldmap.iHeight > 15) {
        if (iPlayerY < (g_worldmap.iHeight << 5) - 256 && iPlayerY > 224)
            iMapOffsetY = 224 - iPlayerY;
        else if (iPlayerY <= 224)
            iMapOffsetY = 0;
        else
            iMapOffsetY = App::screenHeight - (g_worldmap.iHeight << 5);
    } else {
        iMapOffsetY = (App::screenHeight - (g_worldmap.iHeight << 5)) >> 1;
    }
}

void MI_World::RepositionMapImage()
{
    short iPlayerCurrentTileX, iPlayerCurrentTileY;
    g_worldmap.GetPlayerCurrentTile(&iPlayerCurrentTileX, &iPlayerCurrentTileY);

    if (g_worldmap.iWidth > 24) {
        iMapDrawOffsetCol = iPlayerCurrentTileX - 11;
        if (iMapDrawOffsetCol < 0)
            iMapDrawOffsetCol = 0;
        else if (iMapDrawOffsetCol > g_worldmap.iWidth - 24)
            iMapDrawOffsetCol = g_worldmap.iWidth - 24;
    }

    if (g_worldmap.iHeight > 19) {
        iMapDrawOffsetRow = iPlayerCurrentTileY - 9;
        if (iMapDrawOffsetRow < 0)
            iMapDrawOffsetRow = 0;
        else if (iMapDrawOffsetRow > g_worldmap.iHeight - 19)
            iMapDrawOffsetRow = g_worldmap.iHeight - 19;
    }

    iNextMapDrawOffsetCol = iMapDrawOffsetCol;
    iNextMapDrawOffsetRow = iMapDrawOffsetRow;
}


void MI_World::Draw()
{
    if (!fShow)
        return;

    short iPlayerX, iPlayerY;
    g_worldmap.GetPlayerPosition(&iPlayerX, &iPlayerY);

    if (g_worldmap.iWidth > 20)
        iSrcOffsetX = -iMapOffsetX - (iMapDrawOffsetCol << 5);

    if (g_worldmap.iHeight > 15)
        iSrcOffsetY = -iMapOffsetY - (iMapDrawOffsetRow << 5);

    rectSrcSurface = {iSrcOffsetX, iSrcOffsetY, iDrawWidth, iDrawHeight};
    rectDstSurface = {iDstOffsetX, iDstOffsetY, iDrawWidth, iDrawHeight};

    SDL_BlitSurface(sMapSurface[iCurrentSurfaceIndex], &rectSrcSurface, blitdest, &rectDstSurface);

           //Draw the world, vehicles and player
    g_worldmap.Draw(iMapOffsetX, iMapOffsetY, iState == -1 && !fUsingCloud, iSleepTurns > 0);

           //Draw the cloud if the player is one
    if (fUsingCloud && iState == -1)
        rm->spr_worlditems.draw(iPlayerX + iMapOffsetX, iPlayerY + iMapOffsetY, 32, 0, 32, 32);

           //If a points modifier is in place, display it
    if (game_values.worldpointsbonus >= 0) {
        rm->spr_worlditems.draw(603, 5, (game_values.worldpointsbonus + 9) << 5, 0, 32, 32);
    }

           //Draw the teleport/warp stars effect
    if (iState == -2 || iState >= 4) {
        for (short iStar = 0; iStar < 10; iStar++) {
            float dAngle = dTeleportStarAngle + (TWO_PI / 10.0f) * (float)iStar;
            short iStarX = (short)(dTeleportStarRadius * cos(dAngle));
            short iStarY = (short)(dTeleportStarRadius * sin(dAngle));

            rm->spr_teleportstar.draw(iStarX + iPlayerX + iMapOffsetX, iStarY + iPlayerY + iMapOffsetY, iTeleportStarAnimationFrame, 0, 32, 32);
        }
    }

           //If the item selector for a player is displayed
    if (iNumPopups > 0) {
        //Draw Spots and Stored Powerups
        short iPlayerCount = 0;
        for (short iCountPlayers = 0; iCountPlayers < 4; iCountPlayers++) {
            iPlayerCount += game_values.teamcounts[iCountPlayers];
        }

        short iStoredPowerupBoxX = 296 - 48 * (iPlayerCount - 1);
        for (short iTeamStore = 0; iTeamStore < 4; iTeamStore++) {
            for (short iMemberStore = 0; iMemberStore < game_values.teamcounts[iTeamStore]; iMemberStore++) {
                short iPlayerId = game_values.teamids[iTeamStore][iMemberStore];

                rm->spr_worlditempopup.draw(iStoredPowerupBoxX, iStoredItemPopupDrawY, game_values.colorids[iPlayerId] * 48, 256, 48, 48);

                rm->spr_storedpoweruplarge.draw(iStoredPowerupBoxX + 8, iStoredItemPopupDrawY + 8, game_values.storedpowerups[iPlayerId] << 5, 0, 32, 32);

                iStoredPowerupBoxX += 96;
            }
        }

        for (short iTeam = 0; iTeam < 4; iTeam++) {
            if (iStateTransition[iTeam] != 0) {
                short iColorId = game_values.colorids[game_values.teamids[iTeam][0]];
                rm->spr_worlditempopup.draw(0, 448 - iItemPopupDrawY[iTeam] - iPopupOffsetsCurrent[iTeam], 0, iColorId * 64 + 32 - iItemPopupDrawY[iTeam], 320, iItemPopupDrawY[iTeam] << 1);
                rm->spr_worlditempopup.draw(320, 448 - iItemPopupDrawY[iTeam] - iPopupOffsetsCurrent[iTeam], 192, iColorId * 64 + 32 - iItemPopupDrawY[iTeam], 320, iItemPopupDrawY[iTeam] << 1);

                if (iStateTransition[iTeam] == 3) {
                    short iNumPowerups = game_values.worldpowerupcount[iTeam];

                    if (iNumPowerups > 0)
                        rm->spr_worlditempopup.draw(iItemCol[iTeam] * 52 + 114, 424 - iPopupOffsetsCurrent[iTeam], iColorId * 48, 256, 48, 48);

                    short iStartItem = iItemPage[iTeam] << 3;
                    for (short iItem = iStartItem; iItem < iStartItem + 8 && iItem < iNumPowerups; iItem++) {
                        short iPowerup = game_values.worldpowerups[iTeam][iItem];
                        if (iPowerup >= NUM_POWERUPS)
                            rm->spr_worlditems.draw((iItem - iItemPage[iTeam] * 8) * 52 + 122, 432 - iPopupOffsetsCurrent[iTeam], (iPowerup - NUM_POWERUPS) << 5, 0, 32, 32);
                        else
                            rm->spr_storedpoweruplarge.draw((iItem - iItemPage[iTeam] * 8) * 52 + 122, 432 - iPopupOffsetsCurrent[iTeam], iPowerup << 5, 0, 32, 32);
                    }
                }
            }
        }
    }

    if (iState == 4 || iState == 5) {
        rm->menu_shade.setalpha((Uint8)iScreenfade);
        rm->menu_shade.draw(0, 0);
    }
}

//TODO:: need a way for AI to use world items like keys
//Also, there is a problem if you put a key into a stage but someone exits before it is collected
MenuCodeEnum MI_World::SendInput(CPlayerInput * playerInput)
{
    //Don't allow any input while vehicles are moving
    if (g_worldmap.IsVehicleMoving())
        return MENU_CODE_NONE;

           //If the player and a vehicle collided, force the start of that level
    if (fForceStageStart) {
        iState = -1;

        for (short iTeam = 0; iTeam < 4; iTeam++) {
            iNumPopups = 0;
            iStateTransition[iTeam] = 0;
            iItemPopupDrawY[iTeam] = 0;
            iPopupFlag[iTeam] = false;
        }

        fForceStageStart = false;
        return MENU_CODE_TOUR_STOP_CONTINUE_FORCED;
    }

    short iPlayerState = g_worldmap.GetPlayerState();

           //Handle AI movement
    bool fNeedAiControl = false;
    if (iState == -1 && iPlayerState == 0 && iNumPopups == 0) {
        if (!fNoInterestingMoves) {
            fNeedAiControl = true;
            short iTeamMember;
            for (iTeamMember = 0; iTeamMember < game_values.teamcounts[iControllingTeam]; iTeamMember++) {
                if (game_values.playercontrol[game_values.teamids[iControllingTeam][iTeamMember]] == 1) {
                    fNeedAiControl = false;
                    break;
                }
            }

            if (fNeedAiControl) {
                if (iPressSelectTimer == 0) {
                    short iPlayerCurrentTileX, iPlayerCurrentTileY;
                    g_worldmap.GetPlayerCurrentTile(&iPlayerCurrentTileX, &iPlayerCurrentTileY);
                    short iNextMove = g_worldmap.GetNextInterestingMove(iPlayerCurrentTileX, iPlayerCurrentTileY);

                           //Clear out all input from cpu controlled team
                    COutputControl * playerKeys = NULL;
                    short iTeamMember;
                    for (iTeamMember = 0; iTeamMember < game_values.teamcounts[iControllingTeam]; iTeamMember++) {
                        playerKeys = &game_values.playerInput.outputControls[game_values.teamids[iControllingTeam][iTeamMember]];

                        playerKeys->menu_up.fPressed = false;
                        playerKeys->menu_down.fPressed = false;
                        playerKeys->menu_left.fPressed = false;
                        playerKeys->menu_right.fPressed = false;
                        playerKeys->menu_select.fPressed = false;
                        playerKeys->menu_random.fPressed = false;

                        if (iControllingTeam != 0)
                            playerKeys->menu_cancel.fPressed = false;

                    }

                    playerKeys = &game_values.playerInput.outputControls[game_values.teamids[iControllingTeam][0]];

                           //If there are no AI moves to make
                    if (iNextMove == -1) {
                        bool fUsedItem = false;

                               //See if we are touching a door
                        bool fDoor[4] = {false, false, false, false};
                        g_worldmap.IsTouchingDoor(iPlayerCurrentTileX, iPlayerCurrentTileY, fDoor);

                               //See if we can use a key to keep going
                        for (short iPowerup = 0; iPowerup < game_values.worldpowerupcount[iControllingTeam]; iPowerup++) {
                            short iType = game_values.worldpowerups[iControllingTeam][iPowerup];

                            if (iType >= NUM_POWERUPS + 5 && iType <= NUM_POWERUPS + 8) { //Door Keys
                                short iKeyType = iType - NUM_POWERUPS - 5;
                                if (fDoor[iKeyType]) {
                                    fUsedItem = UsePowerup(game_values.teamids[iControllingTeam][iTeamMember], iControllingTeam, iPowerup, false);
                                }
                            }
                        }

                               //if not, flag that there are no moves to make
                        fNoInterestingMoves = !fUsedItem;
                    }
                    if (iNextMove == 0)
                        playerKeys->menu_up.fPressed = true;
                    else if (iNextMove == 1)
                        playerKeys->menu_down.fPressed = true;
                    else if (iNextMove == 2)
                        playerKeys->menu_left.fPressed = true;
                    else if (iNextMove == 3)
                        playerKeys->menu_right.fPressed = true;
                    else if (iNextMove == 4) {
                        pressSelectKeys = playerKeys;
                        iPressSelectTimer = 60;
                    }
                } else {
                    if (--iPressSelectTimer <= 0) {
                        iPressSelectTimer = 0;
                        pressSelectKeys->menu_select.fPressed = true;
                    }
                }
            } else {
                iPressSelectTimer = 0;
            }
        }
    }

    for (short iPlayer = 0; iPlayer < 4; iPlayer++) {
        COutputControl * playerKeys = &game_values.playerInput.outputControls[iPlayer];

        short iPlayerCurrentTileX, iPlayerCurrentTileY;
        g_worldmap.GetPlayerCurrentTile(&iPlayerCurrentTileX, &iPlayerCurrentTileY);

        short iTeamId = LookupTeamID(iPlayer);

        if (iState == -1 && iNumPopups == 0 && iPopupFlag[0] == false && iPopupFlag[1] == false && iPopupFlag[2] == false && iPopupFlag[3] == false) {
            if (iControllingTeam == LookupTeamID(iPlayer) && iPlayerState == 0 && game_values.playercontrol[iPlayer] > 0) { //if this player is player or cpu
                WorldMapTile * tile = &g_worldmap.tiles[iPlayerCurrentTileX][iPlayerCurrentTileY];

                short iTemp; //Just a temp value so we can call the GetVehicleInPlayerTile method
                bool fVehicleInTile = g_worldmap.GetVehicleInPlayerTile(&iTemp) >= 0;

                if (playerKeys->menu_up.fPressed || playerKeys->menu_up.fDown) {
                    //Make sure there is a path connection and that there is no stage or vehicle blocking the way
                    if ((tile->fConnection[0] || tile->iConnectionType == 14) && !g_worldmap.IsDoor(iPlayerCurrentTileX, iPlayerCurrentTileY - 1) &&
                        (((tile->iCompleted >= -1 || (tile->iType >= 6 && game_values.tourstops[tile->iType - 6]->iStageType == 1)) && (!fVehicleInTile || iSleepTurns > 0)) || iReturnDirection == 0 || fUsingCloud)) {
                        if (fUsingCloud && (tile->iCompleted == -2 || fVehicleInTile) && iReturnDirection != 0)
                            UseCloud(false);

                        g_worldmap.MovePlayer(0);
                        iPlayerState = g_worldmap.GetPlayerState();

                        iReturnDirection = 1;

                        ifSoundOnPlay(rm->sfx_worldmove);

                               //Start draw cycle over
                        RestartDrawCycleIfNeeded();
                    } else {
                        if (playerKeys->menu_up.fPressed)
                            ifSoundOnPlay(rm->sfx_hit);
                    }
                } else if (playerKeys->menu_down.fPressed || playerKeys->menu_down.fDown) {
                    if ((tile->fConnection[1] || tile->iConnectionType == 14) && !g_worldmap.IsDoor(iPlayerCurrentTileX, iPlayerCurrentTileY + 1) &&
                        (((tile->iCompleted >= -1 || (tile->iType >= 6 && game_values.tourstops[tile->iType - 6]->iStageType == 1)) && (!fVehicleInTile || iSleepTurns > 0)) || iReturnDirection == 1 || fUsingCloud)) {
                        if (fUsingCloud && (tile->iCompleted == -2 || fVehicleInTile) && iReturnDirection != 1)
                            UseCloud(false);

                        g_worldmap.MovePlayer(1);
                        iPlayerState = g_worldmap.GetPlayerState();

                        iReturnDirection = 0;

                               //Start draw cycle over
                        RestartDrawCycleIfNeeded();

                        ifSoundOnPlay(rm->sfx_worldmove);
                    } else {
                        if (playerKeys->menu_down.fPressed)
                            ifSoundOnPlay(rm->sfx_hit);
                    }
                } else if (playerKeys->menu_left.fPressed || playerKeys->menu_left.fDown) {
                    if ((tile->fConnection[2] || tile->iConnectionType == 12) && !g_worldmap.IsDoor(iPlayerCurrentTileX - 1, iPlayerCurrentTileY) &&
                        (((tile->iCompleted >= -1 || (tile->iType >= 6 && game_values.tourstops[tile->iType - 6]->iStageType == 1)) && (!fVehicleInTile || iSleepTurns > 0)) || iReturnDirection == 2 || fUsingCloud)) {
                        if (fUsingCloud && (tile->iCompleted == -2 || fVehicleInTile) && iReturnDirection != 2)
                            UseCloud(false);

                        g_worldmap.MovePlayer(2);
                        iPlayerState = g_worldmap.GetPlayerState();

                        iReturnDirection = 3;

                               //Start draw cycle over
                        RestartDrawCycleIfNeeded();

                        ifSoundOnPlay(rm->sfx_worldmove);
                    } else {
                        g_worldmap.FacePlayer(1);

                        if (playerKeys->menu_left.fPressed)
                            ifSoundOnPlay(rm->sfx_hit);
                    }
                } else if (playerKeys->menu_right.fPressed || playerKeys->menu_right.fDown) {
                    if ((tile->fConnection[3] || tile->iConnectionType == 12) && !g_worldmap.IsDoor(iPlayerCurrentTileX + 1, iPlayerCurrentTileY) &&
                        (((tile->iCompleted >= -1 || (tile->iType >= 6 && game_values.tourstops[tile->iType - 6]->iStageType == 1)) && (!fVehicleInTile || iSleepTurns > 0)) || iReturnDirection == 3 || fUsingCloud)) {
                        if (fUsingCloud && (tile->iCompleted == -2 || fVehicleInTile) && iReturnDirection != 3)
                            UseCloud(false);

                        g_worldmap.MovePlayer(3);
                        iPlayerState = g_worldmap.GetPlayerState();

                        iReturnDirection = 2;

                               //Start draw cycle over
                        RestartDrawCycleIfNeeded();

                        ifSoundOnPlay(rm->sfx_worldmove);
                    } else {
                        g_worldmap.FacePlayer(0);

                        if (playerKeys->menu_right.fPressed)
                            ifSoundOnPlay(rm->sfx_hit);
                    }
                } else if (playerInput->outputControls[iPlayer].menu_select.fPressed) {
                    //Lookup current tile and see if it is a type of tile you can interact with
                    //If there is a vehicle on this tile, then load it's stage
                    short iStage = g_worldmap.GetVehicleInPlayerTile(&iVehicleId);
                    if (iStage >= 0 && iStage < g_worldmap.iNumStages) {
                        return InitGame(iStage, iPlayer, fNeedAiControl);
                    }

                           //if it is a stage, then load the stage
                    WorldMapTile * tile = &g_worldmap.tiles[iPlayerCurrentTileX][iPlayerCurrentTileY];

                    short iType = tile->iType - 6;
                    if (iType >= 0 && iType < g_worldmap.iNumStages && tile->iCompleted == -2) {
                        return InitGame(iType, iPlayer, fNeedAiControl);
                    }

                    if (g_worldmap.GetWarpInPlayerTile(&iWarpCol, &iWarpRow)) {
                        iState = 4;
                        iScreenfade = 0;
                        iScreenfadeRate = 8;

                        ifSoundOnPlay(rm->sfx_pipe);
                    }
                }
            }
        } else if (iStateTransition[iTeamId] == 3) { //not transitioning to or from the item popup menu
            if (playerKeys->menu_up.fPressed) {
                if (iItemPage[iTeamId] > 0) {
                    iItemPage[iTeamId]--;
                    iItemCol[iTeamId] = 0;
                    ifSoundOnPlay(rm->sfx_worldmove);
                } else {
                    ifSoundOnPlay(rm->sfx_hit);
                }
            } else if (playerKeys->menu_down.fPressed) {
                if (iItemPage[iTeamId] < 3 && (iItemPage[iTeamId] + 1) * 8 < game_values.worldpowerupcount[iTeamId]) {
                    iItemPage[iTeamId]++;
                    iItemCol[iTeamId] = 0;
                    ifSoundOnPlay(rm->sfx_worldmove);
                } else {
                    ifSoundOnPlay(rm->sfx_hit);
                }
            } else if (playerKeys->menu_left.fPressed) {
                if (iItemCol[iTeamId] > 0) {
                    iItemCol[iTeamId]--;
                    ifSoundOnPlay(rm->sfx_worldmove);
                } else if (iItemCol[iTeamId] == 0 && iItemPage[iTeamId] > 0) {
                    iItemCol[iTeamId] = 7;
                    iItemPage[iTeamId]--;
                    ifSoundOnPlay(rm->sfx_worldmove);
                } else {
                    ifSoundOnPlay(rm->sfx_hit);
                }
            } else if (playerKeys->menu_right.fPressed) {
                if (iItemPage[iTeamId] * 8 + iItemCol[iTeamId] + 1 < game_values.worldpowerupcount[iTeamId]) {
                    if (iItemCol[iTeamId] < 7) {
                        iItemCol[iTeamId]++;
                        ifSoundOnPlay(rm->sfx_worldmove);
                    } else if (iItemCol[iTeamId] == 7 && iItemPage[iTeamId] < 3) {
                        iItemCol[iTeamId] = 0;
                        iItemPage[iTeamId]++;
                        ifSoundOnPlay(rm->sfx_worldmove);
                    } else {
                        ifSoundOnPlay(rm->sfx_hit);
                    }
                }
            } else if (playerKeys->menu_select.fPressed) {
                if (game_values.worldpowerupcount[iTeamId] > 0) {
                    short iIndex = iItemPage[iTeamId] * 8 + iItemCol[iTeamId];
                    UsePowerup(iPlayer, iTeamId, iIndex, true);
                }
            }
        }

        if (iState == -1 && (iStateTransition[iTeamId] == 0 || iStateTransition[iTeamId] == 3)) {
            if (playerKeys->menu_cancel.fPressed) {
                if (DEVICE_KEYBOARD != playerInput->inputControls[iPlayer]->iDevice || iPlayer == 0) {
                    fModifying = false;
                    return MENU_CODE_BACK_TEAM_SELECT_MENU;
                }
            }

            if ((game_values.playercontrol[iPlayer] == 1 || fNoInterestingMoves) && (iPopupFlag[iTeamId] || playerKeys->menu_random.fPressed ||
                    (iPlayer != 0 && playerInput->inputControls[iPlayer]->iDevice == DEVICE_KEYBOARD && playerKeys->menu_cancel.fPressed))) {
                iPopupFlag[iTeamId] = true;

                if (iPlayerState == 0 && iStateTransition[iTeamId] == 0) {
                    iPopupFlag[iTeamId] = false;

                    iPopupOrder[iNumPopups] = iTeamId;
                    iPopupOffsets[iTeamId] = iNumPopups++ << 6;
                    iPopupOffsetsCurrent[iTeamId] = iPopupOffsets[iTeamId];

                    iStateTransition[iTeamId] = 1;

                    iItemPage[iTeamId] = 0;
                    iItemCol[iTeamId] = 0;

                    ifSoundOnPlay(rm->sfx_inventory);
                } else if (iStateTransition[iTeamId] == 3) {
                    iPopupFlag[iTeamId] = false;

                    iStateTransition[iTeamId] = 2;
                    ifSoundOnPlay(rm->sfx_inventory);
                }
            }
        }
    }

    return MENU_CODE_NONE;
}

void MI_World::RestartDrawCycleIfNeeded()
{
    iNextMapDrawOffsetCol = iMapDrawOffsetCol;
    iNextMapDrawOffsetRow = iMapDrawOffsetRow;

    short iPlayerDestTileX, iPlayerDestTileY;
    g_worldmap.GetPlayerDestTile(&iPlayerDestTileX, &iPlayerDestTileY);

    if (g_worldmap.iWidth > 24) {
        iNextMapDrawOffsetCol = iPlayerDestTileX - 11;
        if (iNextMapDrawOffsetCol < 0)
            iNextMapDrawOffsetCol = 0;
        else if (iNextMapDrawOffsetCol > g_worldmap.iWidth - 24)
            iNextMapDrawOffsetCol = g_worldmap.iWidth - 24;
    }

    if (g_worldmap.iHeight > 19) {
        iNextMapDrawOffsetRow = iPlayerDestTileY - 9;
        if (iNextMapDrawOffsetRow < 0)
            iNextMapDrawOffsetRow = 0;
        else if (iNextMapDrawOffsetRow > g_worldmap.iHeight - 19)
            iNextMapDrawOffsetRow = g_worldmap.iHeight - 19;
    }

           //Reset the draw cycle if the map offset col/row is going to move
    if (iMapDrawOffsetCol != iNextMapDrawOffsetCol || iMapDrawOffsetRow != iNextMapDrawOffsetRow) {
        iCycleIndex = 0;
        g_worldmap.ResetDrawCycle();
        iDrawFullRefresh = 1;
    }
}

bool MI_World::UsePowerup(short iPlayer, short iTeam, short iIndex, bool fPopupIsUp)
{
    short iPlayerCurrentTileX, iPlayerCurrentTileY;
    g_worldmap.GetPlayerCurrentTile(&iPlayerCurrentTileX, &iPlayerCurrentTileY);

    short iPowerup = game_values.worldpowerups[iTeam][iIndex];
    bool fUsedItem = false;

    if (iPowerup < NUM_POWERUPS) {
        /*
        //Comment this in to give the powerup to all members of the team
        for (short iPlayer = 0; iPlayer < game_values.teamcounts[iTeam]; iPlayer++)
        {
            game_values.storedpowerups[game_values.teamids[iTeam][iPlayer]] = iPowerup;
        }*/

               //Give the powerup only to the player that selected it
        game_values.storedpowerups[iPlayer] = iPowerup;

        ifSoundOnPlay(rm->sfx_collectpowerup);
        fUsedItem = true;
    } else if (iPowerup == NUM_POWERUPS) { //Music Box (put vehicles to sleep)
        iSleepTurns = RANDOM_INT(4) + 2;
        fUsedItem = true;
        ifSoundOnPlay(rm->sfx_collectpowerup);

        rm->backgroundmusic[5].load(worldmusiclist->GetMusic(WORLDMUSICSLEEP, ""));
        rm->backgroundmusic[5].play(false, false);
    } else if (iPowerup == NUM_POWERUPS + 1) { //Cloud (allows player to skip stages)
        if (!fUsingCloud && iState == -1) {
            UseCloud(true);
            fUsedItem = true;
        }
    } else if (iPowerup == NUM_POWERUPS + 2) { //Player Switch
        if (iControllingTeam != iTeam && iState == -1) {
            SetControllingTeam(iTeam);
            DisplayTeamControlAnnouncement();

            fUsedItem = true;
            ifSoundOnPlay(rm->sfx_switchpress);
            iState = 6;
            fNoInterestingMoves = false;
        }
    } else if (iPowerup == NUM_POWERUPS + 3 && iState == -1) { //Advance Turn
        short iDestX, iDestY;
        g_worldmap.GetPlayerDestTile(&iDestX, &iDestY);
        short iSprite = g_worldmap.tiles[iDestX][iDestY].iForegroundSprite;

        if (iSprite < WORLD_BRIDGE_SPRITE_OFFSET || iSprite > WORLD_BRIDGE_SPRITE_OFFSET + 3) {
            AdvanceTurn();
            fUsedItem = true;
            ifSoundOnPlay(rm->sfx_switchpress);
        }
    } else if (iPowerup == NUM_POWERUPS + 4 && iState == -1) { //Revive stage
        short iDestX, iDestY;
        g_worldmap.GetPlayerDestTile(&iDestX, &iDestY);
        WorldMapTile * tile = &g_worldmap.tiles[iDestX][iDestY];

        if (tile->iType >= 6 && tile->iCompleted >= 0) {
            tile->iCompleted = -2;

            g_worldmap.UpdateTile(sMapSurface[0], iDestX - iMapDrawOffsetCol, iDestY - iMapDrawOffsetRow, iMapDrawOffsetCol, iMapDrawOffsetRow, iAnimationFrame);
            g_worldmap.UpdateTile(sMapSurface[1], iDestX - iMapDrawOffsetCol, iDestY - iMapDrawOffsetRow, iMapDrawOffsetCol, iMapDrawOffsetRow, iAnimationFrame);

            uiMenu->AddEyeCandy(new EC_SingleAnimation(&rm->spr_poof, (iDestX << 5) + iMapOffsetX - 8, (iDestY << 5) + iMapOffsetY - 8, 4, 5));

            fUsedItem = true;
            ifSoundOnPlay(rm->sfx_transform);
        }
    } else if (iPowerup >= NUM_POWERUPS + 5 && iPowerup <= NUM_POWERUPS + 8 && iState == -1) { //Door Keys
        short iDoorsOpened = g_worldmap.UseKey(iPowerup - NUM_POWERUPS - 5, iPlayerCurrentTileX, iPlayerCurrentTileY, fUsingCloud);

        if (iDoorsOpened > 0) {
            ifSoundOnPlay(rm->sfx_transform);

            short iPlayerX = (iPlayerCurrentTileX << 5) + iMapOffsetX;
            short iPlayerY = (iPlayerCurrentTileY << 5) + iMapOffsetY;

            if (iDoorsOpened & 0x1) {
                uiMenu->AddEyeCandy(new EC_SingleAnimation(&rm->spr_fireballexplosion, iPlayerX - TILESIZE, iPlayerY, 3, 8));

                g_worldmap.UpdateTile(sMapSurface[0], iPlayerCurrentTileX - iMapDrawOffsetCol - 1, iPlayerCurrentTileY - iMapDrawOffsetRow, iMapDrawOffsetCol, iMapDrawOffsetRow, iAnimationFrame);
                g_worldmap.UpdateTile(sMapSurface[1], iPlayerCurrentTileX - iMapDrawOffsetCol - 1, iPlayerCurrentTileY - iMapDrawOffsetRow, iMapDrawOffsetCol, iMapDrawOffsetRow, iAnimationFrame);
            }

            if (iDoorsOpened & 0x2) {
                uiMenu->AddEyeCandy(new EC_SingleAnimation(&rm->spr_fireballexplosion, iPlayerX + TILESIZE, iPlayerY, 3, 8));

                g_worldmap.UpdateTile(sMapSurface[0], iPlayerCurrentTileX - iMapDrawOffsetCol + 1, iPlayerCurrentTileY - iMapDrawOffsetRow, iMapDrawOffsetCol, iMapDrawOffsetRow, iAnimationFrame);
                g_worldmap.UpdateTile(sMapSurface[1], iPlayerCurrentTileX - iMapDrawOffsetCol + 1, iPlayerCurrentTileY - iMapDrawOffsetRow, iMapDrawOffsetCol, iMapDrawOffsetRow, iAnimationFrame);
            }

            if (iDoorsOpened & 0x4) {
                uiMenu->AddEyeCandy(new EC_SingleAnimation(&rm->spr_fireballexplosion, iPlayerX, iPlayerY - TILESIZE, 3, 8));

                g_worldmap.UpdateTile(sMapSurface[0], iPlayerCurrentTileX - iMapDrawOffsetCol, iPlayerCurrentTileY - iMapDrawOffsetRow - 1, iMapDrawOffsetCol, iMapDrawOffsetRow, iAnimationFrame);
                g_worldmap.UpdateTile(sMapSurface[1], iPlayerCurrentTileX - iMapDrawOffsetCol, iPlayerCurrentTileY - iMapDrawOffsetRow - 1, iMapDrawOffsetCol, iMapDrawOffsetRow, iAnimationFrame);
            }

            if (iDoorsOpened & 0x8) {
                uiMenu->AddEyeCandy(new EC_SingleAnimation(&rm->spr_fireballexplosion, iPlayerX, iPlayerY + TILESIZE, 3, 8));

                g_worldmap.UpdateTile(sMapSurface[0], iPlayerCurrentTileX - iMapDrawOffsetCol, iPlayerCurrentTileY - iMapDrawOffsetRow + 1, iMapDrawOffsetCol, iMapDrawOffsetRow, iAnimationFrame);
                g_worldmap.UpdateTile(sMapSurface[1], iPlayerCurrentTileX - iMapDrawOffsetCol, iPlayerCurrentTileY - iMapDrawOffsetRow + 1, iMapDrawOffsetCol, iMapDrawOffsetRow, iAnimationFrame);
            }

            fUsedItem = true;
            fNoInterestingMoves = false;
        }
    } else if (iPowerup >= NUM_POWERUPS + 9 && iPowerup <= NUM_POWERUPS + 14) { //Stage Points Modifiers
        game_values.worldpointsbonus = iPowerup - NUM_POWERUPS - 9;
        fUsedItem = true;
        ifSoundOnPlay(rm->sfx_switchpress);
    }

    if (fUsedItem) {
        short iNumItems = --game_values.worldpowerupcount[iTeam];

        for (short iItem = iIndex; iItem < iNumItems; iItem++)
            game_values.worldpowerups[iTeam][iItem] = game_values.worldpowerups[iTeam][iItem + 1];

        if (fPopupIsUp)
            iStateTransition[iTeam] = 2;
    } else {
        ifSoundOnPlay(rm->sfx_stun);
    }

    return fUsedItem;
}

MenuCodeEnum MI_World::InitGame(short iStage, short iPlayer, bool fNeedAiControl)
{
    game_values.tourstopcurrent = iStage;
    game_values.worldskipscoreboard = false;

    bool fBonusHouse = game_values.tourstops[game_values.tourstopcurrent]->iStageType == 1;

    if (fBonusHouse)
        game_values.singleplayermode = iPlayer;

           //If player was trying to get to his inventory menu, cancel that because a game is about to start
    for (short iTeam = 0; iTeam < 4; iTeam++)
        iPopupFlag[iTeam] = false;

    if (fNeedAiControl || fBonusHouse)
        return MENU_CODE_TOUR_STOP_CONTINUE_FORCED;
    else
        return MENU_CODE_WORLD_STAGE_START;
}

MenuCodeEnum MI_World::Modify(bool modify)
{
    fModifying = modify;
    return MENU_CODE_MODIFY_ACCEPTED;
}

void MI_World::UseCloud(bool fUseCloud)
{
    fUsingCloud = fUseCloud;
    ifSoundOnPlay(rm->sfx_transform);

    short iPlayerX, iPlayerY;
    g_worldmap.GetPlayerPosition(&iPlayerX, &iPlayerY);
    uiMenu->AddEyeCandy(new EC_SingleAnimation(&rm->spr_poof, iPlayerX + iMapOffsetX - 8, iPlayerY + iMapOffsetY - 8, 4, 5));
}

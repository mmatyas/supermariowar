#pragma once

#include "uicontrol.h"

#include "SDL.h"

struct COutputControl;


class MI_World : public UI_Control {
public:
    MI_World();
    virtual ~MI_World();

    void Update() override;
    void Draw() override;

    MenuCodeEnum SendInput(CPlayerInput * playerInput) override;
    MenuCodeEnum Modify(bool modify) override;

    void Init();
    void SetControllingTeam(short iPlayerID);
    void DisplayTeamControlAnnouncement();

    void SetCurrentStageToCompleted(short iWinningTeam);
    void ClearCloud() {
        fUsingCloud = false;
    }

private:
    MenuCodeEnum InitGame(short iStage, short iPlayer, bool fNeedAiControl);

    void RestartDrawCycleIfNeeded();
    bool UsePowerup(short iPlayer, short iTeam, short iIndex, bool fPopupIsUp);

    void SetMapOffset();
    void RepositionMapImage();

    void AdvanceTurn();
    void UpdateMapSurface(short iCycleIndex);
    void UseCloud(bool fUseCloud);

    short iState = 0;
    short iStateTransition[4];
    short iItemPopupDrawY[4];
    short iPopupOffsets[4];
    short iPopupOffsetsCurrent[4];
    short iPopupOrder[4];
    short iNumPopups = 0;
    short iStoredItemPopupDrawY = 0;

    bool iPopupFlag[4];

    short iItemCol[4];
    short iItemPage[4];

    SDL_Surface* sMapSurface[2];
    SDL_Rect rectSrcSurface;
    SDL_Rect rectDstSurface;

    short iCurrentSurfaceIndex = 0;
    short iCycleIndex = 0;
    short iDrawFullRefresh = 0;

    short iAnimationFrame = 0;

    short iMapOffsetX = 0;
    short iMapOffsetY = 0;

    short iMapDrawOffsetCol = 0;
    short iMapDrawOffsetRow = 0;

    short iNextMapDrawOffsetCol = 0;
    short iNextMapDrawOffsetRow = 0;

    short iDrawWidth = 0;
    short iDrawHeight = 0;
    short iSrcOffsetX = 0;
    short iSrcOffsetY = 0;
    short iDstOffsetX = 0;
    short iDstOffsetY = 0;

    short iControllingTeam = 0;
    short iControllingPlayerId = 0;
    short iReturnDirection = 0;

    bool fForceStageStart = false;
    short iVehicleId = 0;

    short iWarpCol = 0;
    short iWarpRow = 0;

    short iScreenfade = 0;
    short iScreenfadeRate = 0;

    float dTeleportStarRadius = 0.f;
    float dTeleportStarAngle = 0.f;
    short iTeleportStarAnimationFrame = 0;
    short iTeleportStarAnimationTimer = 0;

    bool fNoInterestingMoves = false;

    short iSleepTurns = 0;
    bool fUsingCloud = false;

    short iPressSelectTimer = 0;
    COutputControl* pressSelectKeys = nullptr;
};

#pragma once

#include "world/WorldMovingObject.h"

#include <SDL3/SDL.h>

class WorldVehicle : public WorldMovingObject {
public:
    WorldVehicle();
    ~WorldVehicle() = default;

    void Init(short iCol, short iRow, short iAction, short iSprite, short iMinMoves, short iMaxMoves, bool fSpritePaces, short iInitialDirection, short iBoundary, short tilesize);
    void Move();

    bool Update();
    void Draw(short iWorldOffsetX, short iWorldOffsetY, bool fVehiclesSleeping) const;

    bool isEnabled() const { return fEnabled; }
    void setEnabled(bool fEnabledIn) { fEnabled = fEnabledIn; }

    short getActionId() const { return iActionId; }
    void setActionId(short iActionIdIn) { iActionId = iActionIdIn; }

    short getMinMoves() const { return iMinMoves; }
    void setMinMoves(short iMinMovesIn) { iMinMoves = iMinMovesIn; }

    short getMaxMoves() const { return iMaxMoves; }
    void setMaxMoves(short iMaxMovesIn) { iMaxMoves = iMaxMovesIn; }

    bool isSpritePacing() const { return fSpritePaces; }
    void setSpritePacing(bool fSpritePacesIn) { fSpritePaces = fSpritePacesIn; }

    short getBoundary() const { return iBoundary; }
    void setBoundary(short iBoundaryIn) { iBoundary = iBoundaryIn; }

private:
    void SetNextDest();

    SDL_Rect srcRects[5];

    short iMinMoves;
    short iMaxMoves;
    short iNumMoves;

    short iActionId;

    bool fEnabled;

    bool fSpritePaces;
    short iPaceOffset;
    short iPaceTimer;

    short iBoundary;
};

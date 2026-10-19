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

friend class WorldMap;
friend void AddVehicleToTile(short iCol, short iRow, short iType);
friend void RemoveVehicleFromTile(short iCol, short iRow);
friend void ReadVehiclesIntoEditor();
friend void WriteVehiclesIntoWorld();

friend int editor_stage();
friend int editor_edit();
friend int resize_world();
friend int editor_vehicles();
friend void inner_main();
};

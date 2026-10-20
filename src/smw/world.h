#ifndef WORLD_H
#define WORLD_H

#include "math/Vec2.h"
#include "util/Grid.h"
#include "world/WorldMovingObject.h"
#include "world/WorldPlayer.h"
#include "world/WorldVehicle.h"
#include "world/WorldWarp.h"

#include <SDL3/SDL.h>

#include <string>
#include <vector>

#define WORLD_BACKGROUND_SPRITE_SET_SIZE    60
#define WORLD_PATH_SPRITE_SET_SIZE          20

#define WORLD_FOREGROUND_STAGE_OFFSET           200
#define WORLD_WINNING_TEAM_SPRITE_OFFSET        600
#define WORLD_BRIDGE_SPRITE_OFFSET              604
#define WORLD_START_SPRITE_OFFSET               608
#define WORLD_FOREGROUND_SPRITE_OFFSET          700
#define WORLD_FOREGROUND_SPRITE_ANIMATED_OFFSET 900

enum class WorldMusicCategory : unsigned char;

struct WorldMapTile {
	//Id is used for searching for AI
	short iID;

	short iType;
	short iBackgroundWater;
	short iBackgroundSprite;
	short iForegroundSprite;

	short iConnectionType;
	bool fConnection[4];

	short iWarp;

	short iCompleted;
	bool fAnimated;

	short iCol;
	short iRow;

	short iVehicleBoundary;
};

class WorldMap {
public:
    WorldMap(short w, short h);
    WorldMap(const std::string& path, short iTileSize);

    WorldMap(const WorldMap&) = delete;
    WorldMap& operator=(const WorldMap&) = delete;
    WorldMap(WorldMap&&) = default;
    WorldMap& operator=(WorldMap&&) = default;

    bool Save(const std::string& szPath) const;

    void Resize(short iWidth, short iHeight);
    void Clear();

    bool Update(bool* fPlayerVehicleCollision);
    void Draw(short iMapOffsetX, short iMapOffsetY, bool fDrawPlayer, bool fVehiclesSleeping) const;

    void UpdateTile(SDL_Surface* surface, short iCol, short iRow, short iMapDrawOffsetCol, short iMapDrawOffsetRow, short iAnimationFrame);

    void ResetDrawCycle();
    void DrawMapToSurface(SDL_Surface* surface) const;
    void DrawMapToSurface(short iCycleIndex, bool fFullRefresh, SDL_Surface* surface, short iMapDrawOffsetCol, short iMapDrawOffsetRow, short iAnimationFrame);

    void GetWorldSize(short * w, short * h) const {
        *w = iWidth;
        *h = iHeight;
    }

    Vec2s GetPlayerCurrentTile() const { return player.getCurrentTile(); }
    Vec2s GetPlayerDestTile() const { return player.getDestTile(); };
    short GetPlayerState() const { return player.getState(); };
    Vec2s GetPlayerPosition() const { return player.getPosition(); }
    void SetPlayerPosition(short iPlayerCol, short iPlayerRow);
    void SetPlayerSprite(short iPlayerSprite);
    void MovePlayer(short iDirection);
    void FacePlayer(short iDirection);

    bool IsVehicleMoving() const;
    short GetVehicleInPlayerTile(short * iVehicleIndex) const;
    size_t NumVehiclesInTile(Vec2s iTile) const;
    short GetVehicleStageScore(short iVehicleIndex) const;
    void MoveVehicles();
    void RemoveVehicle(short iVehicleIndex);
    short GetVehicleBoundary(short iCol, short iRow) const;

    bool GetWarpInPlayerTile(short * iWarpCol, short * iWarpRow) const;
    void MoveBridges();

    void IsTouchingDoor(short iCol, short iRow, bool doors[4]) const;
    bool IsDoor(short iCol, short iRow) const;
    short UseKey(short iKeytype, short iCol, short iRow, bool fCloud);

    short GetNextInterestingMove(short iCol, short iRow) const;

    void SetInitialPowerups();

    WorldMusicCategory GetMusicCategory() const { return iMusicCategory; }
    const std::string& GetWorldName() { return worldName; }

    short getWidth() const { return iWidth; }
    short getHeight() const { return iHeight; }
    short getNumStages() const { return iNumStages; }
    void setNumStages(short iNumStages) { this->iNumStages = iNumStages; }
    const WorldMapTile& getTile(short iCol, short iRow) const { return tiles.at(iCol, iRow); }
    WorldMapTile& getTile(short iCol, short iRow) { return tiles.at(iCol, iRow); }

    short getNumInitialBonuses() const { return iNumInitialBonuses; }
    short getInitialBonus(short iIndex) const { return iInitialBonuses[iIndex]; }
    void addInitialBonus(short iBonus) {
        if (iNumInitialBonuses < 32)
            iInitialBonuses[iNumInitialBonuses++] = iBonus;
    }
    void removeInitialBonus(short iIndex) {
        for (short iAdjust = iIndex; iAdjust < iNumInitialBonuses - 1; iAdjust++)
            iInitialBonuses[iAdjust] = iInitialBonuses[iAdjust + 1];

        iNumInitialBonuses--;
    }

    void nextMusicCategory();

private:
    void SetTileConnections(short iCol, short iRow);

    void DrawTileToSurface(SDL_Surface* surface, short iCol, short iRow, short iMapDrawOffsetCol, short iMapDrawOffsetRow, bool fFullRefresh, short iAnimationFrame, short iLayer = 0) const;

    short iWidth = 0;
    short iHeight = 0;
    short iStartX = 0;
    short iStartY = 0;

    short iNumStages = 0;

    Grid<WorldMapTile> tiles;
    WorldPlayer player;
    std::vector<WorldVehicle> vehicles;
    std::vector<WorldWarp> warps;

    short iNumInitialBonuses;
    short iInitialBonuses[32];

    WorldMusicCategory iMusicCategory;

    short iTileSize;
    short iTileSizeShift;
    short iTileSheet;

    short iLastDrawRow;
    short iLastDrawCol;

    short iTilesPerCycle;

    std::string worldName;

    friend bool AutoSetTile(short iCol, short iRow);
    friend void AutoSetPath(short iCol, short iRow);
    friend void AutoSetPathSprite(short iCol, short iRow);
    friend short AdjustForeground(short iSprite, short iCol, short iRow);
    friend bool UpdateForeground(short iCol, short iRow);

    friend void GetForegroundTileValues(short iCol, short iRow, short iOldTiles[9]);
    friend bool ForegroundTileValuesChanged(short iCol, short iRow, short iOldTiles[9]);

    friend void ReadVehiclesIntoEditor();
    friend void WriteVehiclesIntoWorld();
    friend void ReadWarpsIntoEditor();
    friend void WriteWarpsIntoWorld();
};

#endif // WORLD_H

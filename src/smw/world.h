#ifndef WORLD_H
#define WORLD_H

#include "SDL.h"
#include "math/Vec2.h"
#include "util/Grid.h"

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

class WorldMovingObject {
public:
    WorldMovingObject();
    virtual ~WorldMovingObject() = default;

    void Init(short iCol, short iRow, short iSprite, short iInitialDirection, short tilesize);

    virtual void Move(short iDirection);
    virtual bool Update();

    void FaceDirection(short iDirection);
    void SetPosition(short iCol, short iRow);
    Vec2s getCurrentTile() const { return currentTile; }

protected:
    Vec2s pos;
    Vec2s currentTile;
    Vec2s destTile;

    short iState = 0;
    short iDrawSprite = 0;
    short iDrawDirection = 0;
    short iAnimationFrame = 0;
    short iAnimationTimer = 0;

    short iTileSize = 0;
    short iTileSheet = 0;

    friend class WorldMap;
    friend void takescreenshot();
};

class WorldPlayer : public WorldMovingObject {
public:
    WorldPlayer();
    WorldPlayer(short iCol, short iRow);

    void SetSprite(short iPlayer);
    void Draw(short iWorldOffsetX, short iWorldOffsetY) const;
};

class WorldVehicle : public WorldMovingObject
{
	public:

		WorldVehicle();
		~WorldVehicle();

		void Init(short iCol, short iRow, short iAction, short iSprite, short iMinMoves, short iMaxMoves, bool fSpritePaces, short iInitialDirection, short iBoundary, short tilesize);
		void Move();

		bool Update();
                void Draw(short iWorldOffsetX, short iWorldOffsetY, bool fVehiclesSleeping) const;

	private:

		void SetNextDest();

		SDL_Rect srcRects[5];

		short iMinMoves = 5;
		short iMaxMoves = 8;
		short iNumMoves = 0;

		short iActionId = 0;

		bool fEnabled = false;

		bool fSpritePaces = true;
		short iPaceOffset = 0;
		short iPaceTimer = 0;

		short iBoundary = 0;

	friend class WorldMap;
	friend class EditorVehicles;
        friend class EditorStages;

	friend void AddVehicleToTile(short iCol, short iRow, short iType);
	friend void RemoveVehicleFromTile(short iCol, short iRow);
	friend void ReadVehiclesIntoEditor();
	friend void WriteVehiclesIntoWorld();

	friend int editor_stage();
	friend int editor_edit();
	friend int resize_world();
	friend int editor_vehicles();
	friend int main(int argc, char *argv[]);
};

struct WorldWarp {
    static constexpr Vec2s NULL_POS {-1, -1};

    WorldWarp(short id, Vec2s posA, Vec2s posB);

    Vec2s getOtherSide(Vec2s target) const;

    short id;
    Vec2s posA;
    Vec2s posB;
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
    short w() const { return iWidth; }
    short h() const { return iHeight; }

    Vec2s GetPlayerCurrentTile() const { return player.currentTile; }
    Vec2s GetPlayerDestTile() const { return player.destTile; };
    short GetPlayerState() const { return player.iState; };
    Vec2s GetPlayerPosition() const { return player.pos; }
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

    short GetMusicCategory() const { return iMusicCategory; }
    const std::string& GetWorldName() { return worldName; }

    const Grid<WorldMapTile>& getTiles() const { return tiles; }
    Grid<WorldMapTile>& getTiles() { return tiles; }

    const std::vector<WorldWarp>& getWarps() const { return warps; }
    std::vector<WorldWarp>& getWarps() { return warps; }

    const std::vector<WorldVehicle>& getVehicles() const { return vehicles; }
    std::vector<WorldVehicle>& getVehicles() { return vehicles; }

    const std::vector<short>& getInitialBonuses() const { return m_initialBonuses; }
    std::vector<short>& getInitialBonuses() { return m_initialBonuses; }

    short stageCount() const { return iNumStages; }
    void setStageCount(short count) { iNumStages = count; }

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

    std::vector<short> m_initialBonuses;

    short iMusicCategory;

    short iTileSize;
    short iTileSizeShift;
    short iTileSheet;

    short iLastDrawRow;
    short iLastDrawCol;

    short iTilesPerCycle;

    std::string worldName;

    friend class MI_World;
    friend class MI_WorldPreviewDisplay;
    friend class WorldVehicle;

    friend int editor_edit();
    friend int editor_type();
    friend int editor_stage();
    friend int editor_start_items();
    friend int editor_vehicles();
    friend void NewStage(short * iEditStage);

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

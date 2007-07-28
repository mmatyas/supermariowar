#ifndef __WORLD_H_
#define __WORLD_H_

class MI_World;

struct WorldMapTile
{
	short iType;
	short iBackgroundSprite;
	short iForegroundSprite;

	short iConnectionType;
	bool fConnection[4];

	bool fCompleted;
	bool fAnimated;
};

class WorldMovingObject
{
	public:
		WorldMovingObject();
		virtual ~WorldMovingObject();

		void Init(short iCol, short iRow, short iSprite);
		virtual void Move(short iDirection);
		virtual bool Update();
		virtual void Draw(short iWorldOffsetX, short iWorldOffsetY) = 0;

	protected:

		short ix;
		short iy;
		short iCurrentTileX;
		short iCurrentTileY;
		short iDestTileX;
		short iDestTileY;
			
		short iState;
		short iDrawSprite;
		short iDrawDirection;
		short iAnimationFrame;
		short iAnimationTimer;
	
	friend class WorldMap;
};

class WorldPlayer : public WorldMovingObject
{
	public:
	
		WorldPlayer();
		~WorldPlayer();

		void Init(short iCol, short iRow);

		void SetSprite(short iPlayer);
		void Draw(short iWorldOffsetX, short iWorldOffsetY);

	friend class WorldMap;

};

class WorldVehicle : public WorldMovingObject
{
	public:
		
		WorldVehicle();
		~WorldVehicle();

		void Init(short iCol, short iRow, short iAction, short iSprite, short iMinMoves, short iMaxMoves);
		void Move();
		
		bool Update();
		void Draw(short iWorldOffsetX, short iWorldOffsetY);

	private:

		void SetNextDest();
					
		SDL_Rect srcRects[4];

		short iMinMoves;
		short iMaxMoves;
		short iNumMoves;

		short iActionId;

		bool fEnabled;

	friend class WorldMap;
};

class WorldMap
{
	public:

		WorldMap();
		~WorldMap();

		bool Load();
		bool Save(char * szFileName);

		void InitPlayer();

		bool Update();
		void Draw(short iMapOffsetX, short iMapOffsetY);

		void SetPlayerSprite(short iPlayerSprite);
		bool IsVehicleMoving();

		void GetPlayerPosition(short * iPlayerX, short * iPlayerY);
		void GetPlayerCurrentTile(short * iPlayerCurrentTileX, short * iPlayerCurrentTileY);
		short GetPlayerState();

		short GetVehicleInPlayerTile(short * iVehicleIndex);

		void MovePlayer(short iDirection);
		void MoveVehicles();

		void RemoveVehicle(short iVehicleIndex);

	private:

		void Cleanup();

		short iWidth;
		short iHeight;
		short iStartX, iStartY;

		short iNumStages;
		short iNumVehicles;

		WorldMapTile ** tiles;
		WorldPlayer player;
		WorldVehicle * vehicles;


	friend class MI_World;
	friend class WorldVehicle;

};

#endif //__WORLD_H_


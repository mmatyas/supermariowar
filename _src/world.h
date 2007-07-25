#ifndef __WORLD_H_
#define __WORLD_H_

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

class WorldVehicle
{
	public:
		
		WorldVehicle(short iCol, short iRow, short iAction, short iSprite);
		~WorldVehicle();

		void SetNextDest();
		
		void Update();
		void Draw(short iWorldOffsetX, short iWorldOffsetY);

	private:
		
		short iX;
		short iY;
		short iCurrentTileX;
		short iCurrentTileY;
		short iDestTileX;
		short iDestTileY;
			
		short iState;
		short iDrawSprite;
		short iDrawDirection;
		short iAnimationFrame;
		short iAnimationTimer;
		
		SDL_Rect srcRects[4];

		short iNumMoves;

		short iActionId;

	friend class MI_World;
};

class WorldMap
{
	public:

		WorldMap();
		~WorldMap();

		bool Load();
		bool Save(char * szFileName);

		short iStartX, iStartY;

	private:

		void Cleanup();

		short iWidth;
		short iHeight;

		WorldMapTile ** tiles;

	friend class MI_World;
	friend class WorldVehicle;

};

#endif //__WORLD_H_


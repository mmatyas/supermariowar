#ifndef _MOVINGPLATFORM_H_
#define _MOVINGPLATFORM_H_

class CPlayer;
class IO_MovingObject;
#include "map.h"

class MovingPlatform;

class MovingPlatformPath
{
	public:
		MovingPlatformPath(float vel, float startX, float startY, float endX, float endY, bool preview);
		virtual ~MovingPlatformPath() {}

		virtual bool Move(short type) {return false;}
		virtual void Reset();

		void SetPlatform(MovingPlatform * platform) {pPlatform = platform;}

	protected:

		MovingPlatform * pPlatform;

		float dVelocity;
		float dVelX, dVelY;

		float dPathPointX[2], dPathPointY[2];

		float dCurrentX, dCurrentY;

		short iOnStep[2];
		short iSteps;
		short iGoalPoint;

	friend class MovingPlatform;
	friend class CMap;
	friend void loadcurrentmap();
	friend void insert_platforms_into_map();

};

class StraightPath : public MovingPlatformPath
{
	public:
		StraightPath(float vel, float startX, float startY, float endX, float endY, bool preview);
		virtual ~StraightPath() {}

		bool Move(short type);

	private:
		void CalculateAngle();
		void SetVelocity();
		
		float dWidth, dHeight;
		float dAngle, dLength;

	friend class MovingPlatform;
};

/*
class ElipsePath : public MovingPlatformPath
{
	public:
		ElipsePath(float vel, float dAngle, float dRadiusX, float dRadiusY, bool preview);
		virtual ~ElipsePath() {}

		bool Move(short type);

	private:
		
		float dAngle, dRadiusX, dRadiusY;
		float dOldCurrentX, dOldCurrentY;

	friend class MovingPlatform;
};
*/

class FallingPath : public MovingPlatformPath
{
	public:
		FallingPath(float startX, float startY);
		virtual ~FallingPath() {}

		bool Move(short type);

	private:
	
	friend class MovingPlatform;
};

class MovingPlatform
{
	public:
		MovingPlatform(TilesetTile ** tiledata, MapTile ** tiletypes, short w, short h, MovingPlatformPath * path, bool preview);
		~MovingPlatform();

		void draw();
		void draw(short iOffsetX, short iOffsetY);
		void update();

		void ResetPath();

		void collide(CPlayer * player);
		bool coldec_player(CPlayer * player);

		void gettiletypes(CPlayer * player, int * lefttile, int * righttile);

		void collide(IO_MovingObject * object);
		bool coldec_object(IO_MovingObject * object);

		void xf(float xf){fx = xf; ix = (short)fx;}
 		void xi(short xi){ix = xi; fx = (float)ix;}
		void yf(float yf){fy = yf; iy = (short)fy;}
		void yi(short yi){iy = yi; fy = (float)iy;}

		void CalculateNoSpawnZone(float dPathTime);
		bool IsInNoSpawnZone(short x, short y, short w, short h);

	protected:

		TilesetTile ** iTileData;
		MapTile ** iTileType;
		short ix, iy;
		short iWidth, iHeight;
		short iTileWidth, iTileHeight;
		short iHalfWidth, iHalfHeight;

		bool fDead;

		float fx, fy;
		float fOldX, fOldY;
		float fOldVelX, fOldVelY;

		short iSteps;
		short iOnStep;

		SDL_Surface	* sSurface;

		SDL_Rect	rSrcRect;
		SDL_Rect    rDstRect;

		bool fForwardDirection;
		bool fStartDirection;

		MovingPlatformPath * pPath;

		float fVelX, fVelY;

		//Map::findspawnpoint uses this to figure out where not to spawn players
		short iNoSpawnZoneTop;
		short iNoSpawnZoneBottom;
		short iNoSpawnZoneLeft;
		short iNoSpawnZoneRight;

	friend class FallingPath;

	friend class CPlayer;
	friend class IO_MovingObject;
	friend class CMap;
	friend void loadcurrentmap();
	friend void takescreenshot();
	friend void loadmap(char * szMapFile);

};

#endif  //_MOVINGPLATFORM_H_


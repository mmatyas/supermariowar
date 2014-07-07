#ifndef MOVINGPLATFORM_H
#define MOVINGPLATFORM_H

#include "map.h"

class CPlayer;
class IO_MovingObject;
class MovingPlatform;

class MovingPlatformPath
{
	public:
		MovingPlatformPath(float vel, float startX, float startY, float endX, float endY, bool preview);
		virtual ~MovingPlatformPath() {}

		virtual bool Move(short type) = 0; //Never let this class be instanciated
		virtual void Reset();

    void SetPlatform(MovingPlatform * platform) {
        pPlatform = platform;
    }

    short GetType() {
        return iType;
    }

	protected:

		MovingPlatform * pPlatform;

		float dVelocity;
		float dVelX[2], dVelY[2];

		float dPathPointX[2], dPathPointY[2];

		float dCurrentX[2], dCurrentY[2];

		short iType;

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
		void Reset();

	protected:
		void SetVelocity(short type);

		short iOnStep[2];
		short iSteps;
		short iGoalPoint[2];

		float dAngle;

	friend class MovingPlatform;
	friend class CMap;
	friend void loadcurrentmap();
	friend void insert_platforms_into_map();
};

class StraightPathContinuous : public StraightPath
{
	public:
		StraightPathContinuous(float vel, float startX, float startY, float angle, bool preview);
		virtual ~StraightPathContinuous() {}

		bool Move(short type);
		void Reset();

	private:
		float dEdgeX, dEdgeY;

	friend class MovingPlatform;
	friend class CMap;
	friend void loadcurrentmap();
	friend void insert_platforms_into_map();
};

class EllipsePath : public MovingPlatformPath
{
	public:
		EllipsePath(float vel, float dAngle, float dRadiusX, float dRadiusY, float dCenterX, float dCenterY, bool preview);
		virtual ~EllipsePath() {}

		bool Move(short type);
		void SetPosition(short type);
		void Reset();

	private:

		float dRadiusX, dRadiusY;
		float dAngle[2], dStartAngle;

	friend class MovingPlatform;
	friend class CMap;
	friend void loadcurrentmap();
	friend void insert_platforms_into_map();
};

class FallingPath : public MovingPlatformPath
{
	public:
		FallingPath(float startX, float startY);
		virtual ~FallingPath() {}

		bool Move(short type);
		void Reset();

	private:

	friend class MovingPlatform;
};

class MovingPlatform
{
	public:
		MovingPlatform(TilesetTile ** tiledata, MapTile ** tiletypes, short w, short h, short drawlayer, MovingPlatformPath * path, bool preview);
		~MovingPlatform();

		void draw();
		void draw(short iOffsetX, short iOffsetY);
		void update();

		void ResetPath();

		void collide(CPlayer * player);

		void GetTileTypesFromPlayer(CPlayer * player, int * lefttile, int * righttile);
		int GetTileTypeFromCoord(short x, short y);

		void collide(IO_MovingObject * object);

    void setXf(float xf) {
        fx = xf;
        ix = (short)fx;
    }
    void setXi(short xi) {
        ix = xi;
        fx = (float)ix;
    }
    void setYf(float yf) {
        fy = yf;
        iy = (short)fy;
    }
    void setYi(short yi) {
        iy = yi;
        fy = (float)iy;
    }

		bool IsInNoSpawnZone(short x, short y, short w, short h);

    void SetPlayerId(short playerId) {
        iPlayerId = playerId;
    }

	protected:

		void check_map_collision_right(CPlayer * player);
		void check_map_collision_left(CPlayer * player);

		short coldec_player(CPlayer * player);
		short coldec_object(IO_MovingObject * object);

		bool collision_detection_check_sides(IO_MovingObject * object);

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

		SDL_Surface	* sSurface[2];

		SDL_Rect	rSrcRect;
		SDL_Rect    rDstRect;

		bool fForwardDirection;
		bool fStartDirection;

		short iDrawLayer;

		MovingPlatformPath * pPath;

		float fVelX, fVelY;

		short iPlayerId;

	friend class FallingPath;
	friend class StraightPathContinuous;

	friend class CPlayer;
	friend class IO_MovingObject;
	friend class CMap;
	friend void loadcurrentmap();
	friend void takescreenshot();
	friend void loadmap(char * szMapFile);
	friend int editor_platforms();

};

#endif // MOVINGPLATFORM_H

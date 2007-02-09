#ifndef _MOVINGPLATFORM_H_
#define _MOVINGPLATFORM_H_

class CPlayer;
class IO_MovingObject;
#include "map.h"

class MovingPlatformPath
{
	public:
		MovingPlatformPath();
		MovingPlatformPath(float vel, float startX, float startY, float endX, float endY);
		~MovingPlatformPath();

		float fVelocity;
		float fAngle;

		float fStartX, fStartY;
		float fEndX, fEndY;

	protected:

		void CalculateAngle();
};

class MovingPlatform
{
	public:
		MovingPlatform(short ** tiledata, short w, short h, MovingPlatformPath * path, bool forwardDirection, short startPathNode, bool preview);
		~MovingPlatform();

		void draw();
		void draw(short iOffsetX, short iOffsetY);
		void update();

		void ResetPath();
		void FlipDirection();

		void collide(CPlayer * player);
		bool coldec_player(CPlayer * player);

		void gettiletypes(CPlayer * player, TileType * lefttile, TileType * righttile);

		void collide(IO_MovingObject * object);
		bool coldec_object(IO_MovingObject * object);

		void xf(float xf){fx = xf; ix = (short)fx;}
 		void xi(short xi){ix = xi; fx = (float)ix;}
		void yf(float yf){fy = yf; iy = (short)fy;}
		void yi(short yi){iy = yi; fy = (float)iy;}

		void CalculateNoSpawnZone(float dPathTime);
		bool IsInNoSpawnZone(short x, short y, short w, short h);

	protected:

		short ** iTileData;
		short ix, iy;
		short iWidth, iHeight;
		short iTileWidth, iTileHeight;
		short iHalfWidth, iHalfHeight;

		float fx, fy;
		float fOldX, fOldY;
		float fOldVelX, fOldVelY;

		SDL_Surface	* sSurface;

		SDL_Rect	rSrcRect;
		SDL_Rect    rDstRect;

		bool fForwardDirection;
		bool fStartDirection;
		short iStartPathNode;

		MovingPlatformPath * pPath;

		float fVelX, fVelY;
		float fEndPointX, fEndPointY;

		//Map::findspawnpoint uses this to figure out where not to spawn players
		short iNoSpawnZoneTop;
		short iNoSpawnZoneBottom;
		short iNoSpawnZoneLeft;
		short iNoSpawnZoneRight;

	friend class CPlayer;
	friend class IO_MovingObject;
	friend class CMap;
	friend void loadcurrentmap();
	friend void takescreenshot();
	friend void loadmap(char * szMapFile);

};

#endif  //_MOVINGPLATFORM_H_


#ifndef MOVINGPLATFORM_H
#define MOVINGPLATFORM_H

#include "map.h"
#include "MovingPlatformPaths.h"

class CPlayer;
class IO_MovingObject;


class MovingPlatform
{
	public:
		MovingPlatform(TilesetTile ** tiledata, TileType ** tiletypes, short w, short h, short drawlayer, MovingPlatformPath * path, bool preview);
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
		TileType ** iTileType;
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

#ifndef __WORLD_H_
#define __WORLD_H_

struct WorldMapTile
{
	short iType;
	short iSprite;

	short iConnectionType;
	bool fConnection[4];

	bool fCompleted;
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

};

#endif //__WORLD_H_


#ifndef __LEVEL_H_
#define __LEVEL_H_

struct LevelMapTile
{
	short iType;
	bool fConnection[4];		
};

class LevelMap
{
	public:

		LevelMap();
		~LevelMap();

		bool Load();
		bool Save(char * szFileName);

	private:

		short iWidth;
		short iHeight;

		LevelMapTile ** tiles;

};

#endif //__LEVEL_H_


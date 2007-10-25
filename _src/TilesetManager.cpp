#include "global.h"

/*********************************
*  CTileset
*********************************/

CTileset::CTileset(const char * szname)
{
	strncpy(szName, szname, 256);
	szName[255] = 0;

	char szFile[256];

	strcpy(szFile, "gfx/tilesets/");
	strcat(szFile, szName);
	strcat(szFile, "/large.png");
	__load_gfxck(sSprites[0], convertPath(szFile));

	sSurfaces[0] = sSprites[0].getSurface(); //optimization for repeat surface use

	iWidth = sSprites[0].getWidth();
	iHeight = sSprites[0].getHeight();

	strcpy(szFile, "gfx/tilesets/");
	strcat(szFile, szName);
	strcat(szFile, "/medium.png");
	__load_gfxck(sSprites[1], convertPath(szFile));

	sSurfaces[1] = sSprites[1].getSurface();

	strcpy(szFile, "gfx/tilesets/");
	strcat(szFile, szName);
	strcat(szFile, "/small.png");
	__load_gfxck(sSprites[2], convertPath(szFile));

	sSurfaces[2] = sSprites[2].getSurface();

	strcpy(szFile, "gfx/tilesets/");
	strcat(szFile, szName);
	strcat(szFile, "/tileset.tls");
	ReadTileTypeFile(szFile);
}

bool CTileset::ReadTileTypeFile(char * szFile)
{
	FILE * tsf = fopen(szFile, "rb");
	if(tsf == NULL)
	{
		printf("ERROR: couldn't open tileset file: %s\n", szFile);
		return false;
	}
	
	short iTileTypeSize = ReadInt(tsf);

	if(iTileTypeSize <= 0 || iTileTypeSize > 1024)
	{
		fclose(tsf);
		return false;
	}

	tiletypes = new TileType[iTileTypeSize];

	for(short i = 0; i < iTileTypeSize; i++)
	{
		tiletypes[i] = (TileType)ReadInt(tsf);
	}

	fclose(tsf);
	return true;
}


CTileset::~CTileset()
{
	if(tiletypes)
		delete [] tiletypes;

	tiletypes = NULL;
}


/*********************************
*  CTilesetManager
*********************************/

CTilesetManager::CTilesetManager() :
	SimpleDirectoryList(convertPath("gfx/tilesets/"))
{
	short y = 0;
	for(short i = 0; i < 32; i++)
	{
		short x = 0;
		for(short j = 0; j < 32; j++)
		{
			gfx_setrect(&rRects[j][i], x, y, TILESIZE, TILESIZE);
			x += TILESIZE;
		}

		y += TILESIZE;
	}

	std::vector<std::string>::iterator itr = filelist.begin(), lim = filelist.end();

	while(itr != lim)
	{
		CTileset * tileset = new CTileset(itr->c_str());
		tilesetlist.push_back(tileset);

		itr++;
	}
}

CTilesetManager::~CTilesetManager()
{}

short CTilesetManager::GetIndexFromName(char * szName)
{
	std::vector<CTileset*>::iterator itr = tilesetlist.begin(), lim = tilesetlist.end();

	short iLength = tilesetlist.size();

	for(short i = 0; i < iLength; i++)
	{
		if(strcmp(tilesetlist[i]->GetName(), szName))
			return i;
	}

	return -1;
}


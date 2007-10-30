#include "global.h"

/*********************************
*  CTileset
*********************************/

CTileset::CTileset(const char * szpath)
{
	strncpy(szName, getFileFromPath(szpath).c_str(), 256);
	szName[255] = 0;

	char szFile[256];

	strcpy(szFile, szpath);
	strcat(szFile, "/large.png");
	__load_gfxck(sSprites[0], convertPath(szFile));

	sSurfaces[0] = sSprites[0].getSurface(); //optimization for repeat surface use

	iWidth = sSprites[0].getWidth();
	iHeight = sSprites[0].getHeight();

	strcpy(szFile, szpath);
	strcat(szFile, "/medium.png");
	__load_gfxck(sSprites[1], convertPath(szFile));

	sSurfaces[1] = sSprites[1].getSurface();

	strcpy(szFile, szpath);
	strcat(szFile, "/small.png");
	__load_gfxck(sSprites[2], convertPath(szFile));

	sSurfaces[2] = sSprites[2].getSurface();

	strcpy(szFile, szpath);
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

void CTileset::SetTileType(short iTileCol, short iTileRow, TileType type)
{
	tiletypes[iTileCol + iTileRow * iWidth] = type;
}

void CTileset::Draw(SDL_Surface * dstSurface, short iTileSize, SDL_Rect * srcRect, SDL_Rect * dstRect)
{
	SDL_BlitSurface(sSurfaces[iTileSize], srcRect, dstSurface, dstRect);
}

/*********************************
*  CTilesetManager
*********************************/

CTilesetManager::CTilesetManager() :
	SimpleDirectoryList(convertPath("gfx/tilesets/"))
{
	short y1 = 0, y2 = 0, y3 = 0;
	for(short i = 0; i < 32; i++)
	{
		short x1 = 0, x2 = 0, x3 = 0;
		for(short j = 0; j < 32; j++)
		{
			gfx_setrect(&rRects[0][j][i], x1, y1, TILESIZE, TILESIZE);
			gfx_setrect(&rRects[1][j][i], x2, y2, PREVIEWTILESIZE, PREVIEWTILESIZE);
			gfx_setrect(&rRects[2][j][i], x3, y3, THUMBTILESIZE, THUMBTILESIZE);
			
			x1 += TILESIZE;
			x2 += PREVIEWTILESIZE;
			x3 += THUMBTILESIZE;
		}

		y1 += TILESIZE;
		y2 += PREVIEWTILESIZE;
		y3 += THUMBTILESIZE;
	}
}

CTilesetManager::~CTilesetManager()
{}

void CTilesetManager::Init()
{
	std::vector<std::string>::iterator itr = filelist.begin(), lim = filelist.end();

	while(itr != lim)
	{
		CTileset * tileset = new CTileset(itr->c_str());
		tilesetlist.push_back(tileset);

		itr++;
	}
}

short CTilesetManager::GetIndexFromName(char * szName)
{
	std::vector<CTileset*>::iterator itr = tilesetlist.begin(), lim = tilesetlist.end();

	short iLength = tilesetlist.size();

	for(short i = 0; i < iLength; i++)
	{
		if(!strcmp(tilesetlist[i]->GetName(), szName))
			return i;
	}

	return -1;
}

void CTilesetManager::Draw(SDL_Surface * dstSurface, short iTilesetID, short iTileSize, short iSrcTileCol, short iSrcTileRow, short iDstTileCol, short iDstTileRow)
{
	tilesetlist[iTilesetID]->Draw(dstSurface, iTileSize, &rRects[iTileSize][iSrcTileCol][iSrcTileRow], &rRects[iTileSize][iDstTileCol][iDstTileRow]);
}

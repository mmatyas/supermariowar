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
	__load_gfxck(sSprites[0], convertPartialPath(szFile));

	sSurfaces[0] = sSprites[0].getSurface(); //optimization for repeat surface use

	iWidth = sSprites[0].getWidth() / TILESIZE;
	iHeight = sSprites[0].getHeight() / TILESIZE;

	strcpy(szFile, szpath);
	strcat(szFile, "/medium.png");
	__load_gfxck(sSprites[1], convertPartialPath(szFile));

	sSurfaces[1] = sSprites[1].getSurface();

	strcpy(szFile, szpath);
	strcat(szFile, "/small.png");
	__load_gfxck(sSprites[2], convertPartialPath(szFile));

	sSurfaces[2] = sSprites[2].getSurface();

	strcpy(szFile, szpath);
	strcat(szFile, "/tileset.tls");

	strcpy(szTilesetPath, convertPartialPath(szFile).c_str());
	ReadTileTypeFile(szTilesetPath);
}

bool CTileset::ReadTileTypeFile(char * szFile)
{
	//Detect if the tiletype file already exists, if not create it
	if(File_Exists(szFile))
	{
		FILE * tsf = fopen(szFile, "rb");
		if(tsf == NULL)
		{
			printf("ERROR: couldn't open tileset file: %s\n", szFile);
			return false;
		}
		
		iTileTypeSize = ReadInt(tsf);

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
	}
	else
	{
		iTileTypeSize = iWidth * iHeight;
		tiletypes = new TileType[iTileTypeSize];

		for(short i = 0; i < iTileTypeSize; i++)
		{
			tiletypes[i] = tile_nonsolid;
		}
	}

	return true;
}


CTileset::~CTileset()
{
	if(tiletypes)
		delete [] tiletypes;

	tiletypes = NULL;
}

TileType CTileset::GetTileType(short iTileCol, short iTileRow)
{
	return tiletypes[iTileCol + iTileRow * iWidth];
}

void CTileset::SetTileType(short iTileCol, short iTileRow, TileType type)
{
	tiletypes[iTileCol + iTileRow * iWidth] = type;
}

TileType CTileset::IncrementTileType(short iTileCol, short iTileRow)
{
	short iTile = iTileCol + iTileRow * iWidth;
	if(tiletypes[iTile] == tile_nonsolid)
		tiletypes[iTile] = tile_solid;
	else if(tiletypes[iTile] == tile_solid)
		tiletypes[iTile] = tile_solid_on_top;
	else if(tiletypes[iTile] == tile_solid_on_top)
		tiletypes[iTile] = tile_ice;
	else if(tiletypes[iTile] == tile_ice)
		tiletypes[iTile] = tile_death;
	else if(tiletypes[iTile] == tile_death)
		tiletypes[iTile] = tile_death_on_top;
	else if(tiletypes[iTile] == tile_death_on_top)
		tiletypes[iTile] = tile_death_on_bottom;
	else if(tiletypes[iTile] == tile_death_on_bottom)
		tiletypes[iTile] = tile_nonsolid;

	return tiletypes[iTile];
}

void CTileset::Draw(SDL_Surface * dstSurface, short iTileSize, SDL_Rect * srcRect, SDL_Rect * dstRect)
{
	SDL_BlitSurface(sSurfaces[iTileSize], srcRect, dstSurface, dstRect);
}

void CTileset::SaveTileset()
{
	FILE * tsf = fopen(szTilesetPath, "wb");
	if(tsf == NULL)
	{
		printf("ERROR: couldn't open tileset file to save tile types: %s\n", szTilesetPath);
		return;
	}
	
	WriteInt(iTileTypeSize, tsf);

	for(short i = 0; i < iTileTypeSize; i++)
	{
		WriteInt(tiletypes[i], tsf);
	}

	fclose(tsf);
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

	tClassicTileset = NULL;
}

CTilesetManager::~CTilesetManager()
{}

void CTilesetManager::Init()
{
	short iLength = filelist.size();

	for(short i = 0; i < iLength; i++)
	{
		CTileset * tileset = new CTileset(filelist[i].c_str());
		tilesetlist.push_back(tileset);

		if(!strcmp(tileset->GetName(), "Classic"))
		{
			tClassicTileset = tileset;
			iClassicTilesetIndex = i;
		}
	}
}

short CTilesetManager::GetIndexFromName(char * szName)
{
	short iLength = tilesetlist.size();

	for(short i = 0; i < iLength; i++)
	{
		if(!strcmp(tilesetlist[i]->GetName(), szName))
			return i;
	}

	return TILESETUNKNOWN;
}

void CTilesetManager::Draw(SDL_Surface * dstSurface, short iTilesetID, short iTileSize, short iSrcTileCol, short iSrcTileRow, short iDstTileCol, short iDstTileRow)
{
	tilesetlist[iTilesetID]->Draw(dstSurface, iTileSize, &rRects[iTileSize][iSrcTileCol][iSrcTileRow], &rRects[iTileSize][iDstTileCol][iDstTileRow]);
}


CTileset * CTilesetManager::GetTileset(short iTilesetID)
{
	if(iTilesetID < 0 || iTilesetID >= (short)tilesetlist.size())
		return NULL;

	return tilesetlist[iTilesetID];
}

void CTilesetManager::SaveTilesets()
{
	short iLength = tilesetlist.size();

	for(short i = 0; i < iLength; i++)
	{
		tilesetlist[i]->SaveTileset();
	}
}

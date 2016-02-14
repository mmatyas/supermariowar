#include "TilesetManager.h"

#include "FileIO.h"
#include "map.h"

#include <cstdio>
#include <cstring>

#if defined(__APPLE__)
#include <sys/stat.h>
#endif


/*********************************
*  CTileset
*********************************/
extern TileType GetIncrementedTileType(TileType type);

CTileset::CTileset(const char * szpath)
{
	strncpy(szName, getFileFromPath(szpath).c_str(), 256);
	szName[255] = 0;

	char szFile[256];

	strcpy(szFile, szpath);
	strcat(szFile, "/large.png");
	gfx_loadimage(&sSprites[0], convertPartialPath(szFile), false);

	sSurfaces[0] = sSprites[0].getSurface(); //optimization for repeat surface use

	iWidth = sSprites[0].getWidth() / TILESIZE;
	iHeight = sSprites[0].getHeight() / TILESIZE;

	strcpy(szFile, szpath);
	strcat(szFile, "/medium.png");
	gfx_loadimage(&sSprites[1], convertPartialPath(szFile), false);

	sSurfaces[1] = sSprites[1].getSurface();

	strcpy(szFile, szpath);
	strcat(szFile, "/small.png");
	gfx_loadimage(&sSprites[2], convertPartialPath(szFile), false);

	sSurfaces[2] = sSprites[2].getSurface();

	strcpy(szFile, szpath);
	strcat(szFile, "/tileset.tls");

	strcpy(szTilesetPath, convertPartialPath(szFile).c_str());
	ReadTileTypeFile(szTilesetPath);
}

bool CTileset::ReadTileTypeFile(char * szFile)
{
	//Detect if the tiletype file already exists, if not create it
    if (File_Exists(szFile)) {
		BinaryFile tsf(szFile, "rb");
        if (!tsf.is_open()) {
			printf("ERROR: couldn't open tileset file: %s\n", szFile);
			return false;
		}

		iTileTypeSize = tsf.read_i32();

        if (iTileTypeSize <= 0 || iTileTypeSize > 1024) {
			return false;
		}

		tiletypes = new TileType[iTileTypeSize];

        for (short i = 0; i < iTileTypeSize; i++) {
			tiletypes[i] = (TileType)tsf.read_i32();
		}
    } else {
		iTileTypeSize = iWidth * iHeight;
		tiletypes = new TileType[iTileTypeSize];

        for (short i = 0; i < iTileTypeSize; i++) {
			tiletypes[i] = tile_nonsolid;
		}
	}

	return true;
}


CTileset::~CTileset()
{
	if (tiletypes)
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
	tiletypes[iTile] = GetIncrementedTileType(tiletypes[iTile]);

	return tiletypes[iTile];
}

void CTileset::Draw(SDL_Surface * dstSurface, short iTileSize, SDL_Rect * srcRect, SDL_Rect * dstRect)
{
	SDL_BlitSurface(sSurfaces[iTileSize], srcRect, dstSurface, dstRect);
}

void CTileset::SaveTileset()
{
	BinaryFile tsf(szTilesetPath, "wb");
    if (!tsf.is_open()) {
		printf("ERROR: couldn't open tileset file to save tile types: %s\n", szTilesetPath);
		return;
	}

	tsf.write_i32(iTileTypeSize);

    for (short i = 0; i < iTileTypeSize; i++) {
		tsf.write_i32(tiletypes[i]);
	}

#if defined(__APPLE__)
	chmod(szTilesetPath, S_IRWXU | S_IRWXG | S_IROTH);
#endif
}

/*********************************
*  CTilesetManager
*********************************/

CTilesetManager::CTilesetManager() :
	SimpleDirectoryList(convertPath("gfx/packs/Classic/tilesets/"))
{
	short y1 = 0, y2 = 0, y3 = 0;
    for (short i = 0; i < 32; i++) {
		short x1 = 0, x2 = 0, x3 = 0;
        for (short j = 0; j < 32; j++) {
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

void CTilesetManager::Init(const char * szGfxPack)
{
	//Remove all existing tilesets
	std::vector<CTileset*>::iterator iter = tilesetlist.begin(), lim = tilesetlist.end();
    while (iter != lim) {
		delete (*iter);

		iter = tilesetlist.erase(iter);
		lim = tilesetlist.end();
	}

	//Add in tilesets from the new gfxpack (if the gfxpack isn't "Classic")
    if (strcmp(getFileFromPath(szGfxPack).c_str(), "Classic")) {
		std::string s = convertPath("gfx/packs/tilesets", szGfxPack) + getDirectorySeperator();
		SimpleDirectoryList dirlist(s);

		short iLength = dirlist.GetCount();
        for (short i = 0; i < iLength; i++) {
			CTileset * tileset = new CTileset(dirlist.current_name());
			tilesetlist.push_back(tileset);

			//If the tileset is "classic" then keep it's index for common operations
            if (!strcmp(tileset->GetName(), "Classic")) {
				tClassicTileset = tileset;
				iClassicTilesetIndex = i;
			}

			dirlist.next();
		}
	}

	//Add in tilesets from the classic tileset to fill the gaps
	short iLength = filelist.size();
	char szTilesetName[128];
    for (short i = 0; i < iLength; i++) {
		strncpy(szTilesetName, getFileFromPath(filelist[i]).c_str(), 128);
		szTilesetName[127] = 0;

		//See if the new tileset already exists and if it does, skip it
		short iTilesetSize = tilesetlist.size();
		bool fFound = false;
        for (short iTileset = 0; iTileset < iTilesetSize; iTileset++) {
            if (!strcmp(tilesetlist[iTileset]->GetName(), szTilesetName)) {
				fFound = true;
				break;
			}
		}

		//Add the tileset if another one by the same name isn't already in the tileset
        if (!fFound) {
			CTileset * tileset = new CTileset(filelist[i].c_str());
			tilesetlist.push_back(tileset);

            if (!strcmp(tileset->GetName(), "Classic")) {
				tClassicTileset = tileset;
				iClassicTilesetIndex = i;
			}
		}
	}
}

short CTilesetManager::GetIndexFromName(const char * szName)
{
	short iLength = tilesetlist.size();

    for (short i = 0; i < iLength; i++) {
		if (!strcmp(tilesetlist[i]->GetName(), szName))
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
	if (iTilesetID < 0 || iTilesetID >= (short)tilesetlist.size())
		return NULL;

	return tilesetlist[iTilesetID];
}

void CTilesetManager::SaveTilesets()
{
	short iLength = tilesetlist.size();

    for (short i = 0; i < iLength; i++) {
		tilesetlist[i]->SaveTileset();
	}
}

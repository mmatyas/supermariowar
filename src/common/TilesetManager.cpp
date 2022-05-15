#include "TilesetManager.h"

#include "FileIO.h"
#include "map.h"

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstring>

#if defined(__APPLE__)
#include <sys/stat.h>
#endif


// The map format supports tilesets with up to 128x128 tiles
constexpr int MAX_TILES_PER_AXIS = 128;


/*********************************
*  CTileset
*********************************/
extern TileType NextTileType(TileType type);
extern TileType PrevTileType(TileType type);

CTileset::CTileset(const char * szpath)
{
	strncpy(szName, getFileFromPath(szpath).c_str(), 256);
	szName[255] = 0;

	char szFile[256];

	strcpy(szFile, szpath);
	strcat(szFile, "/large.png");
	gfx_loadimage(&sSprites[0], convertPartialPath(szFile), false);

	sSurfaces[0] = sSprites[0].getSurface(); //optimization for repeat surface use

	iWidth = std::min(sSprites[0].getWidth() / TILESIZE, MAX_TILES_PER_AXIS);
	iHeight = std::min(sSprites[0].getHeight() / TILESIZE, MAX_TILES_PER_AXIS);
	tiletypes = std::vector<TileType>(iWidth * iHeight, tile_nonsolid);

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
	constexpr int MAX_TILES = MAX_TILES_PER_AXIS * MAX_TILES_PER_AXIS;

	//Detect if the tiletype file already exists, if not create it
	if (!File_Exists(szFile))
		return true;

	BinaryFile tsf(szFile, "rb");
	if (!tsf.is_open()) {
		printf("ERROR: couldn't open tileset file: %s\n", szFile);
		return false;
	}

	int tiletype_count = tsf.read_i32();
	if (tiletype_count <= 0 || tiletype_count > MAX_TILES)
		return false;

	tiletype_count = std::min<int>(tiletype_count, tiletypes.size());
	for (short i = 0; i < tiletype_count; i++)
		tiletypes[i] = static_cast<TileType>(tsf.read_i32());

	return true;
}


CTileset::~CTileset()
{}

TileType CTileset::GetTileType(short iTileCol, short iTileRow)
{
	assert(iTileCol >= 0 && iTileRow >= 0);
	assert(iTileCol + iTileRow * iWidth < tiletypes.size());
	return tiletypes[iTileCol + iTileRow * iWidth];
}

void CTileset::SetTileType(short iTileCol, short iTileRow, TileType type)
{
	assert(iTileCol >= 0 && iTileRow >= 0);
	assert(iTileCol + iTileRow * iWidth < tiletypes.size());
	tiletypes[iTileCol + iTileRow * iWidth] = type;
}

TileType CTileset::IncrementTileType(short iTileCol, short iTileRow)
{
	assert(iTileCol >= 0 && iTileRow >= 0);
	short iTile = iTileCol + iTileRow * iWidth;
	assert(iTile < tiletypes.size());
	tiletypes[iTile] = NextTileType(tiletypes[iTile]);

	return tiletypes[iTile];
}

TileType CTileset::DecrementTileType(short iTileCol, short iTileRow)
{
	assert(iTileCol >= 0 && iTileRow >= 0);
	short iTile = iTileCol + iTileRow * iWidth;
	assert(iTile < tiletypes.size());
	tiletypes[iTile] = PrevTileType(tiletypes[iTile]);

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

	tsf.write_i32(tiletypes.size());

	for (TileType tiletype : tiletypes)
		tsf.write_i32(tiletype);

#if defined(__APPLE__)
	chmod(szTilesetPath, S_IRWXU | S_IRWXG | S_IROTH);
#endif
}

/*********************************
*  CTilesetManager
*********************************/

CTilesetManager::CTilesetManager() :
	SimpleDirectoryList(convertPath("gfx/packs/Classic/tilesets/"))
{}

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

	InitTilesetRects();
}

void CTilesetManager::InitTilesetRects()
{
	size_t max_tileset_rows = 0;
	for (CTileset* tileset : tilesetlist) {
		max_tileset_rows = std::max<size_t>(max_tileset_rows, tileset->GetHeight());
		max_tileset_cols = std::max<size_t>(max_tileset_cols, tileset->GetWidth());
	}

	const size_t tile_count = max_tileset_rows * max_tileset_cols;
	rects_ingame.reserve(tile_count);
	rects_preview.reserve(tile_count);
	rects_thumb.reserve(tile_count);

	short y1 = 0, y2 = 0, y3 = 0;
	for (size_t row = 0; row < max_tileset_rows; row++) {
		short x1 = 0, x2 = 0, x3 = 0;
		for (size_t col = 0; col < max_tileset_cols; col++) {
			rects_ingame.emplace_back(SDL_Rect { x1, y1, TILESIZE, TILESIZE });
			rects_preview.emplace_back(SDL_Rect { x2, y2, PREVIEWTILESIZE, PREVIEWTILESIZE });
			rects_thumb.emplace_back(SDL_Rect { x3, y3, THUMBTILESIZE, THUMBTILESIZE });

			x1 += TILESIZE;
			x2 += PREVIEWTILESIZE;
			x3 += THUMBTILESIZE;
		}

		y1 += TILESIZE;
		y2 += PREVIEWTILESIZE;
		y3 += THUMBTILESIZE;
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
	assert(0 <= iTilesetID && static_cast<size_t>(iTilesetID) < tilesetlist.size());
	assert(iSrcTileCol >= 0 && iSrcTileRow >= 0);
	assert(iDstTileCol >= 0 && iDstTileRow >= 0);

	const size_t src_rect_idx = iSrcTileRow * max_tileset_cols + iSrcTileCol;
	const size_t dst_rect_idx = iDstTileRow * max_tileset_cols + iDstTileCol;

	SDL_Rect* src_rect = GetRect(iTileSize, src_rect_idx);
	SDL_Rect* dst_rect = GetRect(iTileSize, dst_rect_idx);

	tilesetlist[iTilesetID]->Draw(dstSurface, iTileSize, src_rect, dst_rect);
}

CTileset * CTilesetManager::GetTileset(short iTilesetID)
{
	if (iTilesetID < 0 || iTilesetID >= (short)tilesetlist.size())
		return NULL;

	return tilesetlist[iTilesetID];
}

SDL_Rect* CTilesetManager::GetRect(short size_id, short col, short row)
{
	const size_t rect_idx = row * max_tileset_cols + col;
	return GetRect(size_id, rect_idx);
}

SDL_Rect* CTilesetManager::GetRect(short size_id, size_t idx)
{
	assert(0 <= size_id && size_id < 3);
	assert(idx < rects_ingame.size());

	switch (size_id) {
		case 0: return &rects_ingame[idx];
		case 1: return &rects_preview[idx];
		case 2: return &rects_thumb[idx];
		default: return nullptr;
	}
}

void CTilesetManager::SaveTilesets()
{
	short iLength = tilesetlist.size();

    for (short i = 0; i < iLength; i++) {
		tilesetlist[i]->SaveTileset();
	}
}

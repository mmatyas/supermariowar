#include "TilesetManager.h"

#include "FileIO.h"
#include "path.h"

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <memory>

#if defined(__APPLE__)
#include <sys/stat.h>
#endif


// The map format supports tilesets with up to 128x128 tiles
constexpr int MAX_TILES_PER_AXIS = 128;

namespace {
std::vector<TileType> readTileTypeFile(const std::string& path)
{
    constexpr int MAX_TILES = MAX_TILES_PER_AXIS * MAX_TILES_PER_AXIS;

    //Detect if the tiletype file already exists, if not create it
    if (!FileExists(path))
        return {};

    BinaryFile tsf(path.c_str(), "rb");
    if (!tsf.is_open()) {
        printf("ERROR: couldn't open tileset file: %s\n", path.c_str());
        return {};
    }

    int tiletype_count = tsf.read_i32();
    if (tiletype_count <= 0 || tiletype_count > MAX_TILES)
        return {};

    tiletype_count = std::min<int>(tiletype_count, path.size());

    std::vector<TileType> tiletypes;
    tiletypes.reserve(tiletype_count);
    for (int i = 0; i < tiletype_count; i++)
        tiletypes.emplace_back(static_cast<TileType>(tsf.read_i32()));

    return tiletypes;
}
} // namespace


/*********************************
*  CTileset
*********************************/
CTileset::CTileset(const std::string& dir)
    : m_name(getFilenameFromPath(dir))
    , m_tilesetPath(dir + "/tileset.tls")
    , m_tiletypes(readTileTypeFile(m_tilesetPath))
{
    gfx_loadimage(m_sprites[0], dir + "/large.png", false);
    gfx_loadimage(m_sprites[1], dir + "/medium.png", false);
    gfx_loadimage(m_sprites[2], dir + "/small.png", false);

    m_width = std::min(m_sprites[0].getWidth() / TILESIZE, MAX_TILES_PER_AXIS);
    m_height = std::min(m_sprites[0].getHeight() / TILESIZE, MAX_TILES_PER_AXIS);
    m_tiletypes = std::vector<TileType>(m_width * m_height, tile_nonsolid);
}


SDL_Surface* CTileset::surface(size_t index) const
{
    return index < m_sprites.size()
        ? m_sprites[index].getSurface()
        : nullptr;
}


TileType CTileset::tileType(size_t tileCol, size_t tileRow) const
{
    assert(static_cast<size_t>(tileCol + tileRow * m_width) < m_tiletypes.size());
    return m_tiletypes[tileCol + tileRow * m_width];
}


void CTileset::setTileType(size_t tileCol, size_t tileRow, TileType type)
{
    assert(static_cast<size_t>(tileCol + tileRow * m_width) < m_tiletypes.size());
    m_tiletypes[tileCol + tileRow * m_width] = type;
}


TileType CTileset::incrementTileType(size_t tileCol, size_t tileRow)
{
    const size_t idx = tileCol + tileRow * m_width;
    assert(idx < m_tiletypes.size());
    m_tiletypes[idx] = NextTileType(m_tiletypes[idx]);
    return m_tiletypes[idx];
}


TileType CTileset::decrementTileType(size_t tileCol, size_t tileRow)
{
    const size_t idx = tileCol + tileRow * m_width;
    assert(idx < m_tiletypes.size());
    m_tiletypes[idx] = PrevTileType(m_tiletypes[idx]);
    return m_tiletypes[idx];
}


void CTileset::Draw(SDL_Surface* dstSurface, short tileSize, SDL_Rect* srcRect, SDL_Rect* dstRect) const
{
    SDL_BlitSurface(surface(tileSize), srcRect, dstSurface, dstRect);
}


void CTileset::saveTileset() const
{
    BinaryFile tsf(m_tilesetPath.c_str(), "wb");
    if (!tsf.is_open()) {
        printf("ERROR: couldn't open tileset file to save tile types: %s\n", m_tilesetPath.c_str());
        return;
    }

    tsf.write_i32(m_tiletypes.size());

    for (TileType tiletype : m_tiletypes)
        tsf.write_i32(tiletype);

#if defined(__APPLE__)
    chmod(m_tilesetPath.c_str(), S_IRWXU | S_IRWXG | S_IROTH);
#endif
}


/*********************************
*  CTilesetManager
*********************************/

CTilesetManager::CTilesetManager()
    : SimpleDirectoryList(convertPath("gfx/packs/Classic/tilesets/"))
{}


void CTilesetManager::init(const std::string& gfxPack)
{
    //Remove all existing tilesets
    m_tilesetlist.clear();

    //Add in tilesets from the new gfxpack (if the gfxpack isn't "Classic")
    if (getFilenameFromPath(gfxPack) != "Classic") {
        std::string s = convertPath("gfx/packs/tilesets", gfxPack) + '/';
        SimpleDirectoryList dirlist(s);

        for (size_t i = 0; i < dirlist.count(); i++) {
            std::unique_ptr<CTileset> tileset = std::make_unique<CTileset>(dirlist.currentPath());

            //If the tileset is "classic" then keep it's index for common operations
            if (tileset->name() == "Classic")
                m_classicTilesetIndex = i;

            m_tilesetlist.emplace_back(std::move(tileset));
            dirlist.next();
        }
    }

    //Add in tilesets from the classic tileset to fill the gaps
    for (size_t i = 0; i < m_filelist.size(); i++) {
        const std::string tilesetName = getFilenameFromPath(m_filelist[i]);

        //See if the new tileset already exists and if it does, skip it
        bool fFound = false;
        for (size_t iTileset = 0; iTileset < m_tilesetlist.size(); iTileset++) {
            if (m_tilesetlist[iTileset]->name() == tilesetName) {
                fFound = true;
                break;
            }
        }

        //Add the tileset if another one by the same name isn't already in the tileset
        if (!fFound) {
            std::unique_ptr<CTileset> tileset = std::make_unique<CTileset>(m_filelist[i]);
            if (tileset->name() == "Classic") {
                m_classicTilesetIndex = i;
            }
            m_tilesetlist.emplace_back(std::move(tileset));
        }
    }

    initTilesetRects();
}


void CTilesetManager::initTilesetRects()
{
    size_t max_tileset_rows = 0;
    for (const std::unique_ptr<CTileset>& tileset : m_tilesetlist) {
        max_tileset_rows = std::max<size_t>(max_tileset_rows, tileset->height());
        m_max_tileset_cols = std::max<size_t>(m_max_tileset_cols, tileset->width());
    }

    const size_t tile_count = max_tileset_rows * m_max_tileset_cols;
    m_rects_ingame.reserve(tile_count);
    m_rects_preview.reserve(tile_count);
    m_rects_thumb.reserve(tile_count);

    short y1 = 0, y2 = 0, y3 = 0;
    for (size_t row = 0; row < max_tileset_rows; row++) {
        short x1 = 0, x2 = 0, x3 = 0;
        for (size_t col = 0; col < m_max_tileset_cols; col++) {
            m_rects_ingame.emplace_back(SDL_Rect { x1, y1, TILESIZE, TILESIZE });
            m_rects_preview.emplace_back(SDL_Rect { x2, y2, PREVIEWTILESIZE, PREVIEWTILESIZE });
            m_rects_thumb.emplace_back(SDL_Rect { x3, y3, THUMBTILESIZE, THUMBTILESIZE });

            x1 += TILESIZE;
            x2 += PREVIEWTILESIZE;
            x3 += THUMBTILESIZE;
        }

        y1 += TILESIZE;
        y2 += PREVIEWTILESIZE;
        y3 += THUMBTILESIZE;
    }
}


size_t CTilesetManager::indexFromName(const std::string& name) const
{
    for (size_t i = 0; i < m_tilesetlist.size(); i++) {
        if (m_tilesetlist[i]->name() == name)
            return i;
    }

    return TILESETUNKNOWN;
}


void CTilesetManager::Draw(SDL_Surface * dstSurface, short iTilesetID, short iTileSize, short iSrcTileCol, short iSrcTileRow, short iDstTileCol, short iDstTileRow)
{
    assert(0 <= iTilesetID && static_cast<size_t>(iTilesetID) < m_tilesetlist.size());
    assert(iSrcTileCol >= 0 && iSrcTileRow >= 0);
    assert(iDstTileCol >= 0 && iDstTileRow >= 0);

    const size_t src_rect_idx = iSrcTileRow * m_max_tileset_cols + iSrcTileCol;
    const size_t dst_rect_idx = iDstTileRow * m_max_tileset_cols + iDstTileCol;

    SDL_Rect* src_rect = rect(iTileSize, src_rect_idx);
    SDL_Rect* dst_rect = rect(iTileSize, dst_rect_idx);

    m_tilesetlist[iTilesetID]->Draw(dstSurface, iTileSize, src_rect, dst_rect);
}


CTileset* CTilesetManager::tileset(size_t index)
{
    return index < m_tilesetlist.size()
        ? m_tilesetlist[index].get()
        : nullptr;
}


SDL_Rect* CTilesetManager::rect(short size_id, short col, short row)
{
    const size_t rect_idx = row * m_max_tileset_cols + col;
    return rect(size_id, rect_idx);
}


SDL_Rect* CTilesetManager::rect(short size_id, size_t idx)
{
    assert(0 <= size_id && size_id < 3);
    assert(idx < m_rects_ingame.size());

    switch (size_id) {
    case 0: return &m_rects_ingame[idx];
    case 1: return &m_rects_preview[idx];
    case 2: return &m_rects_thumb[idx];
        default: return nullptr;
    }
}


void CTilesetManager::saveTilesets() const
{
    for (const std::unique_ptr<CTileset>& tileset : m_tilesetlist)
        tileset->saveTileset();
}

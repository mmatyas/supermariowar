#include "TilesetManager.h"

#include "FileIO.h"
#include "path.h"
#include "util/ContainerHelpers.h"
#include "util/DirIterator.h"

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <memory>
#include <unordered_set>

#if defined(__APPLE__)
#include <sys/stat.h>
#endif

namespace fs = std::filesystem;


// The map format supports tilesets with up to 128x128 tiles
constexpr int MAX_TILES_PER_AXIS = 128;

namespace {
std::vector<TileType> readTileTypeFile(const std::string& path, int expectedCount)
{
    constexpr int MAX_TILES = MAX_TILES_PER_AXIS * MAX_TILES_PER_AXIS;

    //Detect if the tiletype file already exists, if not create it
    if (!FileExists(path))
        return {};

    BinaryFile tsf(path, "rb");
    if (!tsf.is_open()) {
        printf("ERROR: couldn't open tileset file: %s\n", path.c_str());
        return {};
    }

    int tiletype_count = tsf.read_i32();
    if (tiletype_count <= 0 || tiletype_count > MAX_TILES)
        return {};

    tiletype_count = std::min<int>(tiletype_count, expectedCount);

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
CTileset::CTileset(const std::filesystem::path& dir)
    : m_name(dir.filename().string())
    , m_tilesetPath(dir / "tileset.tls")
{
    m_sprite_large = SpriteBuilder(dir / "large.png").create();
    m_sprite_medium = SpriteBuilder(dir / "medium.png").create();
    m_sprite_small = SpriteBuilder(dir / "small.png").create();

    m_width = std::min(m_sprite_large.getWidth() / TILESIZE, MAX_TILES_PER_AXIS);
    m_height = std::min(m_sprite_large.getHeight() / TILESIZE, MAX_TILES_PER_AXIS);
    m_tiletypes = readTileTypeFile(m_tilesetPath, m_width * m_height);
}


SDL_Surface* CTileset::surface(DrawSize size) const
{
    switch (size) {
        case DrawSize::Ingame: return m_sprite_large.getSurface();
        case DrawSize::Preview: return m_sprite_medium.getSurface();
        case DrawSize::Thumbnail: return m_sprite_small.getSurface();
        default: return nullptr;
    }
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


void CTileset::Draw(SDL_Surface* dstSurface, DrawSize tileSize, SDL_Rect* srcRect, SDL_Rect* dstRect) const
{
    SDL_BlitSurface(surface(tileSize), srcRect, dstSurface, dstRect);
}


void CTileset::saveTileset() const
{
    BinaryFile tsf(m_tilesetPath, "wb");
    if (!tsf.is_open()) {
        printf("ERROR: couldn't open tileset file to save tile types: %s\n", m_tilesetPath.c_str());
        return;
    }

    tsf.write_i32(m_tiletypes.size());

    for (TileType tiletype : m_tiletypes)
        tsf.write_i32(static_cast<int>(tiletype));

#if defined(__APPLE__)
    chmod(m_tilesetPath.c_str(), S_IRWXU | S_IRWXG | S_IROTH);
#endif
}


/*********************************
*  CTilesetManager
*********************************/

CTilesetManager::CTilesetManager(const fs::path& gfxPack)
{
    std::unordered_set<std::string> found_tileset_names;

    SubdirsIterator dir(convertPath("gfx/packs/tilesets", gfxPack) + '/');
    while (auto path = dir.next()) {
        auto tileset = std::make_unique<CTileset>(std::move(*path));
        found_tileset_names.insert(tileset->name());
        m_tilesetlist.emplace_back(std::move(tileset));
    }

    //Add tilesets from the Classic pack to fill the gaps
    if (gfxPack.filename() != "Classic") {
        dir = SubdirsIterator(convertPath("gfx/packs/Classic/tilesets/"));
        while (auto path = dir.next()) {
            if (!found_tileset_names.contains(path->filename().string())) {
                auto tileset = std::make_unique<CTileset>(std::move(*path));
                m_tilesetlist.emplace_back(std::move(tileset));
            }
        }
    }

    utils::sort(m_tilesetlist, [](const auto& a, const auto& b) {
        return a->name() < b->name();
    });

    const auto it = utils::find_if(
        m_tilesetlist,
        [](const std::unique_ptr<CTileset>& tileset){ return tileset->name() == "Classic"; });
    if (it != m_tilesetlist.cend()) {
        m_classicTilesetIndex = std::distance(m_tilesetlist.cbegin(), it);
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


void CTilesetManager::Draw(SDL_Surface* dstSurface, size_t iTilesetID, DrawSize iTileSize, short iSrcTileCol, short iSrcTileRow, short iDstTileCol, short iDstTileRow)
{
    CTileset* tileset_ptr = tileset(iTilesetID);
    if (!tileset_ptr)
        return;

    assert(iSrcTileCol >= 0 && iSrcTileRow >= 0);
    assert(iDstTileCol >= 0 && iDstTileRow >= 0);

    const size_t src_rect_idx = iSrcTileRow * m_max_tileset_cols + iSrcTileCol;
    const size_t dst_rect_idx = iDstTileRow * m_max_tileset_cols + iDstTileCol;

    SDL_Rect* src_rect = rect(iTileSize, src_rect_idx);
    SDL_Rect* dst_rect = rect(iTileSize, dst_rect_idx);

    tileset_ptr->Draw(dstSurface, iTileSize, src_rect, dst_rect);
}


CTileset* CTilesetManager::tileset(size_t index) const
{
    return index < m_tilesetlist.size()
        ? m_tilesetlist[index].get()
        : nullptr;
}


SDL_Rect* CTilesetManager::rect(DrawSize size, short col, short row)
{
    const size_t rect_idx = row * m_max_tileset_cols + col;
    return rect(size, rect_idx);
}


SDL_Rect* CTilesetManager::rect(DrawSize size, size_t idx)
{
    assert(idx < m_rects_ingame.size());

    switch (size) {
        case DrawSize::Ingame: return &m_rects_ingame[idx];
        case DrawSize::Preview: return &m_rects_preview[idx];
        case DrawSize::Thumbnail: return &m_rects_thumb[idx];
        default: return nullptr;
    }
}


void CTilesetManager::saveTilesets() const
{
    for (const std::unique_ptr<CTileset>& tileset : m_tilesetlist)
        tileset->saveTileset();
}

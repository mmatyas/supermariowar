#include "TilesetManager.h"

#include "FileIO.h"
#include "path.h"
#include "util/ContainerHelpers.h"
#include "util/DirIterator.h"

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <unordered_set>

#if defined(__APPLE__)
#include <sys/stat.h>
#endif

namespace fs = std::filesystem;


namespace {
constexpr std::array<SDL_Rect, CTileset::MAX_TILES> generateTilesetRects(int tilesize)
{
    std::array<SDL_Rect, CTileset::MAX_TILES> rects {};
    for (int row = 0; row < CTileset::MAX_TILES_PER_AXIS; row++) {
        for (int col = 0; col < CTileset::MAX_TILES_PER_AXIS; col++) {
            rects[row * CTileset::MAX_TILES_PER_AXIS + col] = SDL_Rect {
                col * tilesize,
                row * tilesize,
                tilesize,
                tilesize,
            };
        }
    }
    return rects;
}

std::vector<TileType> readTileTypeFile(const fs::path& path)
{
    //Detect if the tiletype file already exists, if not create it
    if (!fs::exists(path))
        return {};

    BinaryFile tsf(path, "rb");
    if (!tsf.is_open()) {
        printf("ERROR: couldn't open tileset file: %s\n", path.string().c_str());
        return {};
    }

    int tiletype_count = tsf.read_i32();
    if (tiletype_count <= 0 || tiletype_count > CTileset::MAX_TILES)
        return {};

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
    , m_tileset_path(dir / "tileset.tls")
    , m_tiletypes(readTileTypeFile(m_tileset_path))
{}


void CTileset::ensureLoaded()
{
    if (m_sprite_large.getSurface())
        return;

    m_sprite_large = ImageLoader(m_tileset_path.parent_path() / "large.png").create();
    m_sprite_medium = ImageLoader(m_tileset_path.parent_path() / "medium.png").create();
    m_sprite_small = ImageLoader(m_tileset_path.parent_path() / "small.png").create();

    m_width = std::min(m_sprite_large.getWidth() / TILESIZE, MAX_TILES_PER_AXIS);
    m_height = std::min(m_sprite_large.getHeight() / TILESIZE, MAX_TILES_PER_AXIS);
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


void CTileset::draw(DrawSize drawsize, const SDL_Rect& srcRect, SDL_Surface* dstSurface, const SDL_Rect& dstRect) const
{
    sprite(drawsize).draw(srcRect, dstSurface, dstRect);
}


void CTileset::saveTileset() const
{
    BinaryFile tsf(m_tileset_path, "wb");
    if (!tsf.is_open()) {
        printf("ERROR: couldn't open tileset file to save tile types: %s\n", m_tileset_path.string().c_str());
        return;
    }

    tsf.write_i32(m_tiletypes.size());

    for (TileType tiletype : m_tiletypes)
        tsf.write_i32(static_cast<int>(tiletype));

#if defined(__APPLE__)
    chmod(m_tileset_path.string().c_str(), S_IRWXU | S_IRWXG | S_IROTH);
#endif
}


/*********************************
*  CTilesetManager
*********************************/

const std::array<SDL_Rect, CTileset::MAX_TILES> CTilesetManager::s_rects_ingame = generateTilesetRects(TILESIZE);
const std::array<SDL_Rect, CTileset::MAX_TILES> CTilesetManager::s_rects_preview = generateTilesetRects(PREVIEWTILESIZE);
const std::array<SDL_Rect, CTileset::MAX_TILES> CTilesetManager::s_rects_thumb = generateTilesetRects(THUMBTILESIZE);

CTilesetManager::CTilesetManager(const fs::path& gfxPack)
{
    std::unordered_set<std::string> found_tileset_names;

    SubdirsIterator dir(convertPath("gfx/packs/tilesets", gfxPack) + '/');
    while (auto path = dir.next()) {
        CTileset tileset(*path);
        found_tileset_names.insert(tileset.name());
        m_tilesets.emplace_back(std::move(tileset));
    }

    //Add tilesets from the Classic pack to fill the gaps
    if (gfxPack.filename() != "Classic") {
        dir = SubdirsIterator(convertPath("gfx/packs/Classic/tilesets/"));
        while (auto path = dir.next()) {
            if (!found_tileset_names.contains(path->filename().string())) {
                CTileset tileset(*path);
                m_tilesets.emplace_back(std::move(tileset));
            }
        }
    }

    utils::sort(m_tilesets, [](const CTileset& a, const CTileset& b) {
        return a.name() < b.name();
    });

    const auto it = utils::find_if(m_tilesets, [](const CTileset& tileset){ return tileset.name() == "Classic"; });
    if (it != m_tilesets.cend()) {
        m_classicTilesetIndex = std::distance(m_tilesets.cbegin(), it);
    }
}


size_t CTilesetManager::indexFromName(const std::string& name) const
{
    for (size_t i = 0; i < m_tilesets.size(); i++) {
        if (m_tilesets[i].name() == name)
            return i;
    }

    return TILESETUNKNOWN;
}


void CTilesetManager::Draw(
    SDL_Surface* dstSurface, size_t iTilesetID, DrawSize drawsize,
    short iSrcTileCol, short iSrcTileRow,
    short iDstTileCol, short iDstTileRow)
{
    const CTileset* tileset_ptr = tileset(iTilesetID);
    if (!tileset_ptr)
        return;

    const SDL_Rect& src_rect = rect(drawsize, iSrcTileCol, iSrcTileRow);
    const SDL_Rect& dst_rect = rect(drawsize, iDstTileCol, iDstTileRow);

    tileset_ptr->draw(drawsize, src_rect, dstSurface, dst_rect);
}


CTileset* CTilesetManager::tileset(size_t index)
{
    if (index < m_tilesets.size()) {
        m_tilesets[index].ensureLoaded();
        return &m_tilesets[index];
    }
    return nullptr;
}


const SDL_Rect& CTilesetManager::rect(DrawSize size, size_t col, size_t row)
{
    const size_t rect_idx = row * CTileset::MAX_TILES_PER_AXIS + col;
    return rect(size, rect_idx);
}


const SDL_Rect& CTilesetManager::rect(DrawSize size, size_t idx)
{
    switch (size) {
        case DrawSize::Ingame: return s_rects_ingame.at(idx);
        case DrawSize::Preview: return s_rects_preview.at(idx);
        case DrawSize::Thumbnail: return s_rects_thumb.at(idx);
    }
    return s_rects_ingame.at(idx);
}


void CTilesetManager::saveTilesets() const
{
    for (const CTileset& tileset : m_tilesets)
        tileset.saveTileset();
}

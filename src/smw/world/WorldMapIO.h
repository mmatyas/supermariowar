#pragma once

#include <string>

class WorldMap;

/// Serializes a world map to a file. Returns false when the file could not be written.
bool saveWorldMap(const WorldMap& world, const std::string& path);

/// Deserializes a world map from a file. Throws std::runtime_error when the file cannot be opened or is invalid.
WorldMap loadWorldMap(const std::string& path, short tilesize);

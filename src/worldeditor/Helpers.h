#pragma once

#include "math/Vec2.h"

class WorldMap;
struct WorldMapTile;
class CResourceManager;


/// Convert foreground sprite to match the background sprite.
short adjustedForeground(short fgSprite, short bgSprite);

bool updateForeground(WorldMapTile& tile);
bool updateCoastline(WorldMap& world, Vec2s center);
bool autoSetTile(WorldMap& world, Vec2s center);
bool setTileFgWithConnection(WorldMapTile& tile, short newTileId);

void DisplayStageDetails(short iStageId, short iMouseX, short iMouseY, CResourceManager& rm);

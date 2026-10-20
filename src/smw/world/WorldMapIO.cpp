#include "WorldMapIO.h"

#include "FileList.h"
#include "GameValues.h"
#include "GlobalConstants.h"
#include "Version.h"
#include "WorldTourStop.h"
#include "linfunc.h"
#include "path.h"
#include "world.h"

#include <algorithm>
#include <charconv>
#include <cstdio>
#include <fstream>
#include <list>
#include <string>

#if defined(__APPLE__)
#include <sys/stat.h>
#endif

extern CGameValues game_values;

namespace {
std::string_view popNext(std::list<std::string_view>& list)
{
    std::string_view result;
    if (!list.empty()) {
        result = std::move(list.front());
        list.pop_front();
    }
    return result;
}

int toInt(std::string_view text, int defval = 0)
{
    int value = defval;
    std::from_chars(text.data(), text.data() + text.size(), value);  // TODO: Handle errors
    return value;
}

int popNextInt(std::list<std::string_view>& list, int defval = 0)
{
    return toInt(popNext(list));
}
} // namespace

WorldMap loadWorldMap(const std::string& path, short tilesize)
{
    WorldMap result(0, 0);

    result.ResetDrawCycle();
    result.setTileSize(tilesize);

    result.worldName = stripPathAndExtension(path);

    std::ifstream file(path);
    if (!file)
        throw std::runtime_error("Could not open the world file");

    ResetTourStops();

    std::string line;
    short iReadType = 0;
    Version version;
    short iMapTileReadRow = 0;
    short iWorldWidth = 0;
    short iWorldHeight = 0;
    short iCurrentStage = 0;
    short iNumWarps = 0;
    short iNumVehicles = 0;

    while (std::getline(file, line)) {
        if (line.empty())
            continue;

        if (line[0] == '#' || line[0] == '\r' || line[0] == ' ' || line[0] == '\t')
            continue;

        if (iReadType == 0) { //Read version number
            std::list<std::string_view> tokens = tokenize(line, '.');
            version.major = popNextInt(tokens);
            version.minor = popNextInt(tokens);
            version.patch = popNextInt(tokens);
            version.build = popNextInt(tokens);
            iReadType = 1;
        } else if (iReadType == 1) { //music category
            result.iMusicCategory = static_cast<WorldMusicCategory>(std::stoi(line));  // FIXME
            iReadType = 2;
        } else if (iReadType == 2) { //world width
            iWorldWidth = std::stoi(line);
            iReadType = 3;
        } else if (iReadType == 3) { //world height
            iWorldHeight = std::stoi(line);

            result.Resize(iWorldWidth, iWorldHeight);

            iReadType = 4;
        } else if (iReadType == 4) { //background water
            std::list<std::string_view> tokens = tokenize(line, ',');
            if (tokens.size() < result.getWidth())
                goto RETURN;

            for (short iMapTileReadCol = 0; iMapTileReadCol < result.getWidth(); iMapTileReadCol++) {
                WorldMapTile& tile = result.tiles.at(iMapTileReadCol, iMapTileReadRow);
                tile.iBackgroundWater = popNextInt(tokens);
            }

            if (++iMapTileReadRow == result.getHeight()) {
                iReadType = 5;
                iMapTileReadRow = 0;
            }
        } else if (iReadType == 5) { //background sprites
            std::list<std::string_view> tokens = tokenize(line, ',');
            if (tokens.size() < result.getWidth())
                goto RETURN;

            for (short iMapTileReadCol = 0; iMapTileReadCol < result.getWidth(); iMapTileReadCol++) {
                WorldMapTile& tile = result.tiles.at(iMapTileReadCol, iMapTileReadRow);
                tile.iBackgroundSprite = popNextInt(tokens);
                tile.fAnimated = (tile.iBackgroundSprite % WORLD_BACKGROUND_SPRITE_SET_SIZE) != 1;

                tile.iID = iMapTileReadRow * result.getWidth() + iMapTileReadCol;
                tile.iCol = iMapTileReadCol;
                tile.iRow = iMapTileReadRow;
            }

            if (++iMapTileReadRow == result.getHeight()) {
                iReadType = 6;
                iMapTileReadRow = 0;
            }
        } else if (iReadType == 6) { //foreground sprites
            std::list<std::string_view> tokens = tokenize(line, ',');
            if (tokens.size() < result.getWidth())
                goto RETURN;

            for (short iMapTileReadCol = 0; iMapTileReadCol < result.getWidth(); iMapTileReadCol++) {
                WorldMapTile& tile = result.tiles.at(iMapTileReadCol, iMapTileReadRow);
                tile.iForegroundSprite = popNextInt(tokens);

                short iForegroundSprite = tile.iForegroundSprite;

                //Animated parts of paths
                if (!tile.fAnimated && iForegroundSprite >= 0 && iForegroundSprite <= 8 * WORLD_PATH_SPRITE_SET_SIZE) {
                    short iForeground = iForegroundSprite % WORLD_PATH_SPRITE_SET_SIZE;
                    tile.fAnimated = iForeground >= 3 && iForeground <= 10;
                }

                //Animated 1-100 stages
                if (!tile.fAnimated)
                    tile.fAnimated = iForegroundSprite >= WORLD_FOREGROUND_STAGE_OFFSET && iForegroundSprite <= WORLD_FOREGROUND_STAGE_OFFSET + 399;

                //Animated foreground tiles
                if (!tile.fAnimated)
                    tile.fAnimated = iForegroundSprite >= WORLD_FOREGROUND_SPRITE_ANIMATED_OFFSET && iForegroundSprite <= WORLD_FOREGROUND_SPRITE_ANIMATED_OFFSET + 29;
            }

            if (++iMapTileReadRow == result.getHeight()) {
                iReadType = 7;
                iMapTileReadRow = 0;
            }
        } else if (iReadType == 7) { //path connections
            std::list<std::string_view> tokens = tokenize(line, ',');
            if (tokens.size() < result.getWidth())
                goto RETURN;

            for (short iMapTileReadCol = 0; iMapTileReadCol < result.getWidth(); iMapTileReadCol++) {
                WorldMapTile& tile = result.tiles.at(iMapTileReadCol, iMapTileReadRow);
                tile.iConnectionType = popNextInt(tokens);
            }

            if (++iMapTileReadRow == result.getHeight()) {
                iReadType = 8;
                iMapTileReadRow = 0;

                //1 == |  2 == -  3 == -!  4 == L  5 == ,-  6 == -,
                //7 == -|  8 == -`-  9 == |-  10 == -,-  11 == +
                //12 == horizontal bridge starts open,  13 == horizontal bridge closed
                //14 == vertical bridge starts open,  15 == vertical bridge closed
                for (short iRow = 0; iRow < result.getHeight(); iRow++) {
                    for (short iCol = 0; iCol < result.getWidth(); iCol++) {
                        result.SetTileConnections(iCol, iRow);
                    }
                }
            }
        } else if (iReadType == 8) { //stages
            std::list<std::string_view> tokens = tokenize(line, ',');
            if (tokens.size() < result.getWidth())
                goto RETURN;

            for (short iMapTileReadCol = 0; iMapTileReadCol < result.getWidth(); iMapTileReadCol++) {
                WorldMapTile& tile = result.tiles.at(iMapTileReadCol, iMapTileReadRow);
                tile.iType = popNextInt(tokens);
                tile.iWarp = -1;

                if (tile.iType == 1) {
                    result.player.SetPosition(iMapTileReadCol, iMapTileReadRow);
                }

                tile.iCompleted = tile.iType <= 5 ? -1 : -2;
            }

            if (++iMapTileReadRow == result.getHeight()) {
                iReadType = 9;
                iMapTileReadRow = 0;
            }
        } else if (iReadType == 9) { //vehicle boundaries
            std::list<std::string_view> tokens = tokenize(line, ',');
            if (tokens.size() < result.getWidth())
                goto RETURN;

            for (short iMapTileReadCol = 0; iMapTileReadCol < result.getWidth(); iMapTileReadCol++) {
                WorldMapTile& tile = result.tiles.at(iMapTileReadCol, iMapTileReadRow);
                tile.iVehicleBoundary = popNextInt(tokens);
            }

            if (++iMapTileReadRow == result.getHeight())
                iReadType = 10;
        } else if (iReadType == 10) { //number of stages
            result.iNumStages = std::stoi(line);

            iReadType = result.iNumStages == 0 ? 12 : 11;
        } else if (iReadType == 11) { //stage details
            TourStop* ts = new TourStop();
            char* buffer = new char[line.size() + 1];
            std::copy(line.begin(), line.end(), buffer);
            buffer[line.size()] = '\0';
            *ts = ParseTourStopLine(buffer, version, true);
            delete[] buffer;

            game_values.tourstops.push_back(ts);

            if (++iCurrentStage >= result.iNumStages) {
                //Scan stage IDs and make sure we have a stage for each one
                short iMaxStage = game_values.tourstops.size() + 5;
                for (short iRow = 0; iRow < result.getHeight(); iRow++) {
                    for (short iCol = 0; iCol < result.getWidth(); iCol++) {
                        short iType = result.tiles.at(iCol, iRow).iType;
                        if (iType < 0 || iType > iMaxStage)
                            goto RETURN;
                    }
                }

                iReadType = 12;
            }
        } else if (iReadType == 12) { //number of warps
            iNumWarps = std::stoi(line);

            if (iNumWarps < 0)
                iNumWarps = 0;

            if (iNumWarps > 0)
                result.warps.reserve(iNumWarps);

            iReadType = iNumWarps == 0 ? 14 : 13;
        } else if (iReadType == 13) { //warp details
            std::list<std::string_view> tokens = tokenize(line, ',');

            short iCol1 = std::max(0, popNextInt(tokens));
            short iRow1 = std::max(0, popNextInt(tokens));
            short iCol2 = std::max(0, popNextInt(tokens));
            short iRow2 = std::max(0, popNextInt(tokens));

            short warpId = result.warps.size();
            result.warps.emplace_back(WorldWarp(warpId, {iCol1, iRow1}, {iCol2, iRow2}));

            result.tiles.at(iCol1, iRow1).iWarp = warpId;
            result.tiles.at(iCol2, iRow2).iWarp = warpId;

            if (result.warps.size() >= iNumWarps)
                iReadType = 14;
        } else if (iReadType == 14) { //number of vehicles
            iNumVehicles = std::stoi(line);

            if (iNumVehicles < 0)
                iNumVehicles = 0;

            if (iNumVehicles > 0)
                result.vehicles.reserve(iNumVehicles);

            iReadType = iNumVehicles == 0 ? 16 : 15;
        } else if (iReadType == 15) { //vehicles
            std::list<std::string_view> tokens = tokenize(line, ',');

            short iSprite = popNextInt(tokens);

            short iStage = popNextInt(tokens);
            if (iStage > result.iNumStages)
                iStage = 0;

            short iCol = popNextInt(tokens);
            short iRow = popNextInt(tokens);

            short iMinMoves = std::max(0, popNextInt(tokens));
            short iMaxMoves = std::max<short>(iMinMoves, popNextInt(tokens));

            bool fSpritePaces = popNextInt(tokens) == 1;

            short iInitialDirection = popNextInt(tokens);
            if (iInitialDirection != 0)
                iInitialDirection = 1;

            short iBoundary = popNextInt(tokens);

            result.vehicles.emplace_back(WorldVehicle());
            result.vehicles.back().Init(iCol, iRow, iStage, iSprite, iMinMoves, iMaxMoves, fSpritePaces, iInitialDirection, iBoundary, result.iTileSize);

            if (result.vehicles.size() >= iNumVehicles)
                iReadType = 16;
        } else if (iReadType == 16) { //initial bonus items
            std::list<std::string_view> tokens = tokenize(line, ',');

            result.iNumInitialBonuses = 0;

            while (!tokens.empty()) {
                std::string_view token = popNext(tokens);
                if (token.empty())
                    break;

                //0 indicates no initial bonuses
                if (token[0] == '0')
                    break;

                short iBonusOffset = 0;
                if (token[0] == 'w' || token[0] == 'W')
                    iBonusOffset += NUM_POWERUPS;

                short iBonus = toInt(token.substr(1)) + iBonusOffset;
                if (iBonus < 0 || iBonus >= NUM_POWERUPS + NUM_WORLD_POWERUPS)
                    iBonus = 0;

                if (result.iNumInitialBonuses < 32)
                    result.iInitialBonuses[result.iNumInitialBonuses++] = iBonus;
                else
                    result.iInitialBonuses[31] = iBonus;
            }

            iReadType = 17;
        }
    }

RETURN:
    if (iReadType != 17)
        throw std::runtime_error("Invalid world file");

    return result;
}

//Saves world to file
bool saveWorldMap(const WorldMap& world, const std::string& szPath)
{
    const std::vector<WorldWarp>& warps = world.getWarps();
    const std::vector<WorldVehicle>& vehicles = world.getVehicles();

    FILE * file = fopen(szPath.c_str(), "w");

    if (!file)
        return false;

    fprintf(file, "#Version\n");
    // For compatibility, let's use the final 1.8 version until
    // there's no actual change in the world format.
    fprintf(file, "1.8.0.4\n\n");
    //fprintf(file, "%d.%d.%d.%d\n\n", g_iVersion[0], g_iVersion[1], g_iVersion[2], g_iVersion[3]);

    fprintf(file, "#Music Category\n");
    fprintf(file, "%d\n\n", world.GetMusicCategory());

    fprintf(file, "#Width\n");
    fprintf(file, "%d\n\n", world.getWidth());

    fprintf(file, "#Height\n");
    fprintf(file, "%d\n\n", world.getHeight());

    fprintf(file, "#Sprite Water Layer\n");

    for (short iMapTileReadRow = 0; iMapTileReadRow < world.getHeight(); iMapTileReadRow++) {
        for (short iMapTileReadCol = 0; iMapTileReadCol < world.getWidth(); iMapTileReadCol++) {
            const WorldMapTile& tile = world.getTile(iMapTileReadCol, iMapTileReadRow);
            fprintf(file, "%d", tile.iBackgroundWater);

            if (iMapTileReadCol == world.getWidth() - 1)
                fprintf(file, "\n");
            else
                fprintf(file, ",");
        }
    }
    fprintf(file, "\n");

    fprintf(file, "#Sprite Background Layer\n");

    for (short iMapTileReadRow = 0; iMapTileReadRow < world.getHeight(); iMapTileReadRow++) {
        for (short iMapTileReadCol = 0; iMapTileReadCol < world.getWidth(); iMapTileReadCol++) {
            const WorldMapTile& tile = world.getTile(iMapTileReadCol, iMapTileReadRow);
            fprintf(file, "%d", tile.iBackgroundSprite);

            if (iMapTileReadCol == world.getWidth() - 1)
                fprintf(file, "\n");
            else
                fprintf(file, ",");
        }
    }
    fprintf(file, "\n");

    fprintf(file, "#Sprite Foreground Layer\n");

    for (short iMapTileReadRow = 0; iMapTileReadRow < world.getHeight(); iMapTileReadRow++) {
        for (short iMapTileReadCol = 0; iMapTileReadCol < world.getWidth(); iMapTileReadCol++) {
            const WorldMapTile& tile = world.getTile(iMapTileReadCol, iMapTileReadRow);
            fprintf(file, "%d", tile.iForegroundSprite);

            if (iMapTileReadCol == world.getWidth() - 1)
                fprintf(file, "\n");
            else
                fprintf(file, ",");
        }
    }
    fprintf(file, "\n");

    fprintf(file, "#Connections\n");

    for (short iMapTileReadRow = 0; iMapTileReadRow < world.getHeight(); iMapTileReadRow++) {
        for (short iMapTileReadCol = 0; iMapTileReadCol < world.getWidth(); iMapTileReadCol++) {
            const WorldMapTile& tile = world.getTile(iMapTileReadCol, iMapTileReadRow);
            fprintf(file, "%d", tile.iConnectionType);

            if (iMapTileReadCol == world.getWidth() - 1)
                fprintf(file, "\n");
            else
                fprintf(file, ",");
        }
    }
    fprintf(file, "\n");

    fprintf(file, "#Tile Types (Stages, Doors, Start Tiles)\n");

    for (short iMapTileReadRow = 0; iMapTileReadRow < world.getHeight(); iMapTileReadRow++) {
        for (short iMapTileReadCol = 0; iMapTileReadCol < world.getWidth(); iMapTileReadCol++) {
            const WorldMapTile& tile = world.getTile(iMapTileReadCol, iMapTileReadRow);
            fprintf(file, "%d", tile.iType);

            if (iMapTileReadCol == world.getWidth() - 1)
                fprintf(file, "\n");
            else
                fprintf(file, ",");
        }
    }
    fprintf(file, "\n");

    fprintf(file, "#Vehicle Boundaries\n");

    for (short iMapTileReadRow = 0; iMapTileReadRow < world.getHeight(); iMapTileReadRow++) {
        for (short iMapTileReadCol = 0; iMapTileReadCol < world.getWidth(); iMapTileReadCol++) {
            const WorldMapTile& tile = world.getTile(iMapTileReadCol, iMapTileReadRow);
            fprintf(file, "%d", tile.iVehicleBoundary);

            if (iMapTileReadCol == world.getWidth() - 1)
                fprintf(file, "\n");
            else
                fprintf(file, ",");
        }
    }
    fprintf(file, "\n");

    fprintf(file, "#Stages\n");
    fprintf(file, "#Stage Type 0,Map,Mode,Goal,Points,Bonus List(Max 10),Name,End World, then mode settings (see sample tour file for details)\n");
    fprintf(file, "#Stage Type 1,Bonus House Name,Sequential/Random Order,Display Text,Powerup List(Max 5)\n");

    fprintf(file, "%d\n", game_values.tourstops.size());

    for (short iStage = 0; iStage < game_values.tourstops.size(); iStage++) {
        std::string line = WriteTourStopLine(*game_values.tourstops[iStage], true);
        fprintf(file, "%s", line.c_str());
    }
    fprintf(file, "\n");

    fprintf(file, "#Warps\n");
    fprintf(file, "#location 1 x, y, location 2 x, y\n");

    fprintf(file, "%d\n", warps.size());

    for (const WorldWarp& warp : warps) {
        fprintf(file, "%d,", warp.posA.x);
        fprintf(file, "%d,", warp.posA.y);
        fprintf(file, "%d,", warp.posB.x);
        fprintf(file, "%d\n", warp.posB.y);
    }
    fprintf(file, "\n");

    fprintf(file, "#Vehicles\n");
    fprintf(file, "#Sprite,Stage Type, Start Column, Start Row, Min Moves, Max Moves, Sprite Paces, Sprite Direction, Boundary\n");

    fprintf(file, "%d\n", vehicles.size());

    for (short iVehicle = 0; iVehicle < vehicles.size(); iVehicle++) {
        fprintf(file, "%d,", vehicles[iVehicle].getDrawSprite());
        fprintf(file, "%d,", vehicles[iVehicle].getActionId());
        fprintf(file, "%d,", vehicles[iVehicle].getCurrentTile().x);
        fprintf(file, "%d,", vehicles[iVehicle].getCurrentTile().y);
        fprintf(file, "%d,", vehicles[iVehicle].getMinMoves());
        fprintf(file, "%d,", vehicles[iVehicle].getMaxMoves());
        fprintf(file, "%d,", vehicles[iVehicle].isSpritePacing());
        fprintf(file, "%d,", vehicles[iVehicle].getDrawDirection());
        fprintf(file, "%d\n", vehicles[iVehicle].getBoundary());
    }
    fprintf(file, "\n");

    fprintf(file, "#Initial Items\n");

    for (short iItem = 0; iItem < world.getNumInitialBonuses(); iItem++) {
        if (iItem != 0)
            fprintf(file, ",");

        short iBonus = world.getInitialBonus(iItem);
        char cBonusType = 'p';
        if (iBonus >= NUM_POWERUPS) {
            iBonus -= NUM_POWERUPS;
            cBonusType = 'w';
        }

        fprintf(file, "%c%d", cBonusType, iBonus);
    }

    if (world.getNumInitialBonuses() == 0)
        fprintf(file, "0");

    fprintf(file, "\n");

    fclose(file);

#if defined(__APPLE__)
    chmod(szPath.c_str(), S_IRWXU | S_IRWXG | S_IROTH);
#endif

    return true;
}

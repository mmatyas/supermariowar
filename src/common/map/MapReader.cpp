#include "map/MapReader.h"

extern bool VersionIsEqualOrAfter(int iVersion[], short iMajor, short iMinor, short iMicro, short iBuild);

MapLoader* MapLoader::getLoaderByVersion(int (&mapversion)[4])
{
    /* v1.8.x readers */

    if (VersionIsEqualOrAfter(mapversion, 1, 8, 0, 2))
        return new MapLoader1802();

    if (VersionIsEqualOrAfter(mapversion, 1, 8, 0, 0))
        return new MapLoader1800();

    /* v1.7.x readers */

    if (VersionIsEqualOrAfter(mapversion, 1, 7, 0, 2))
        return new MapLoader1702();

    if (VersionIsEqualOrAfter(mapversion, 1, 7, 0, 1))
        return new MapLoader1701();

    if (VersionIsEqualOrAfter(mapversion, 1, 7, 0, 0))
        return new MapLoader1700();

    /* 1.6.x readers */

    if (VersionIsEqualOrAfter(mapversion, 1, 6, 1, 0))
        return new MapLoader1610();

    if (VersionIsEqualOrAfter(mapversion, 1, 6, 0, 10))
        return new MapLoader160A();

    if (VersionIsEqualOrAfter(mapversion, 1, 6, 0, 0))
        return new MapLoader1600();

    /* Return 1.5 reader */

    return new MapLoader1500();
}

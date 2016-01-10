#include "map/MapReader.h"

extern bool VersionIsEqualOrAfter(int32_t iVersion[], short iMajor, short iMinor, short iMicro, short iBuild);

MapReader* MapReader::getLoaderByVersion(int32_t (&mapversion)[4])
{
    /* v1.8.x readers */

    if (VersionIsEqualOrAfter(mapversion, 1, 8, 0, 2))
        return new MapReader1802();

    if (VersionIsEqualOrAfter(mapversion, 1, 8, 0, 1))
        return new MapReader1801();

    if (VersionIsEqualOrAfter(mapversion, 1, 8, 0, 0))
        return new MapReader1800();

    /* v1.7.x readers */

    if (VersionIsEqualOrAfter(mapversion, 1, 7, 0, 2))
        return new MapReader1702();

    if (VersionIsEqualOrAfter(mapversion, 1, 7, 0, 1))
        return new MapReader1701();

    if (VersionIsEqualOrAfter(mapversion, 1, 7, 0, 0))
        return new MapReader1700();

    /* 1.6.x readers */

    if (VersionIsEqualOrAfter(mapversion, 1, 6, 1, 0))
        return new MapReader1610();

    if (VersionIsEqualOrAfter(mapversion, 1, 6, 0, 10))
        return new MapReader160A();

    if (VersionIsEqualOrAfter(mapversion, 1, 6, 0, 0))
        return new MapReader1600();

    /* Return 1.5 reader */

    return new MapReader1500();
}

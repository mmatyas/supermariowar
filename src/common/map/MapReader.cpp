#include "map/MapReader.h"

#include "Version.h"


MapReader* MapReader::getLoaderByVersion(const Version& mapversion)
{
    /* v1.8.x readers */

    if (mapversion >= Version {1, 8, 0, 2})
        return new MapReader1802();

    if (mapversion >= Version {1, 8, 0, 1})
        return new MapReader1801();

    if (mapversion >= Version {1, 8, 0, 0})
        return new MapReader1800();

    /* v1.7.x readers */

    if (mapversion >= Version {1, 7, 0, 2})
        return new MapReader1702();

    if (mapversion >= Version {1, 7, 0, 1})
        return new MapReader1701();

    if (mapversion >= Version {1, 7, 0, 0})
        return new MapReader1700();

    /* 1.6.x readers */

    if (mapversion >= Version {1, 6, 1, 0})
        return new MapReader1610();

    if (mapversion >= Version {1, 6, 0, 10})
        return new MapReader160A();

    if (mapversion >= Version {1, 6, 0, 0})
        return new MapReader1600();

    /* Return 1.5 reader */

    return new MapReader1500();
}

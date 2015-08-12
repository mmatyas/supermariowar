#ifndef SMW_MAP_LOADER_H
#define SMW_MAP_LOADER_H

#include <cstdio>

class CMap;
enum ReadType: unsigned char;

class MapReader {
public:
    MapReader() {}
    virtual ~MapReader() {}

    virtual bool load(CMap&, FILE*, ReadType) = 0;

    static MapReader* getLoaderByVersion(int (&mapversion)[4]);
};

/*
    v1.5 reader
*/

class MapReader1500 : public MapReader {
public:
    MapReader1500() : MapReader() {}
    virtual ~MapReader1500() {}
    virtual bool load(CMap&, FILE*, ReadType);

protected:
    virtual void read_autofilters(CMap&, FILE*); // compat: autofilters disabled
    virtual void read_tiles(CMap&, FILE*); // one-tileset mapdata, separately stored objects
    virtual void read_background(CMap&, FILE*); // background by ID
    virtual void read_music_category(CMap&, FILE*); // compat: music guessed by background
};

/*
    v1.6 readers
*/

class MapReader1600 : public MapReader1500 {
public:
    MapReader1600();
    virtual ~MapReader1600() {}
    virtual bool load(CMap&, FILE*, ReadType);

protected:
    virtual void read_tiles(CMap&, FILE*); // tiles also contain block and warp data
    virtual void read_eyecandy(CMap&, FILE*); // simple eyecandy support
    virtual bool read_spawn_areas(CMap&, FILE*);
    virtual void read_warp_exits(CMap&, FILE*);
    virtual bool read_draw_areas(CMap&, FILE*);

private:
    bool parse_nospawn;
    bool fix_spawnareas;

    friend class MapReader160A;
    friend class MapReader1610;
};

class MapReader160A : public MapReader1600 {
public:
    MapReader160A();
    virtual ~MapReader160A() {}
};

class MapReader1610 : public MapReader160A {
public:
    MapReader1610();
    virtual ~MapReader1610() {}
    virtual bool load(CMap&, FILE*, ReadType);
};

/*
    v1.7 readers
*/

class MapReader1700 : public MapReader1600 /* ignore 1.6 patches */ {
public:
    MapReader1700();
    virtual ~MapReader1700() {}
    virtual bool load(CMap&, FILE*, ReadType);

protected:
    virtual void read_tiles(CMap&, FILE*); // one-tileset mapdata
    virtual void set_preview_switches(CMap&, FILE*); // compat: switches disabled
    virtual void read_warp_locations(CMap&, FILE*);
    virtual void read_switches(CMap&, FILE*); // switches stored inverted
    virtual bool read_spawn_areas(CMap&, FILE*);

    unsigned char patch_version;
};

class MapReader1701 : public MapReader1700 {
public:
    MapReader1701();
    virtual ~MapReader1701() {}

protected:
    virtual void read_background(CMap&, FILE*); // background by underscore-conversion
    virtual void read_music_category(CMap&, FILE*); // music by category ID
};

class MapReader1702 : public MapReader1701 {
public:
    MapReader1702();
    virtual ~MapReader1702() {}

protected:
    virtual void read_autofilters(CMap& map, FILE* mapfile); // 9 autofilter support
    virtual void read_background(CMap&, FILE*); // background as string
};

/*
    v1.8 readers
*/

class MapReader1800 : public MapReader1702 {
public:
    MapReader1800();
    virtual ~MapReader1800() {}
    virtual bool load(CMap&, FILE*, ReadType);

protected:
    virtual void read_autofilters(CMap& map, FILE* mapfile); // 13 autofilters
    virtual void read_tileset(FILE*); // custom tileset support
    virtual void read_tiles(CMap&, FILE*); // multi-tileset mapdata
    virtual void read_switches(CMap&, FILE*); // switches stored as-is
    virtual void read_items(CMap&, FILE*); // map item support
    virtual void read_hazards(CMap&, FILE*); // map hazard support
    virtual void read_warp_locations(CMap&, FILE*);
    virtual void read_switchable_blocks(CMap&, FILE*);
    virtual bool read_spawn_areas(CMap&, FILE*);
    virtual void read_extra_tiledata(CMap&, FILE*);
    virtual void read_gamemode_settings(CMap&, FILE*);

private:
    short iMaxTilesetID;
    short* translationid;
    short* tilesetwidths;
    short* tilesetheights;
};

class MapReader1801 : public MapReader1800 {
public:
    MapReader1801();
    virtual ~MapReader1801() {}
};

class MapReader1802 : public MapReader1801 {
public:
    MapReader1802();
    virtual ~MapReader1802() {}

protected:
    virtual void read_eyecandy(CMap&, FILE*); // 3-layer eyecandy support
};

#endif // SMW_MAP_LOADER_H

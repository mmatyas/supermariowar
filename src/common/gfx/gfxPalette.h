#ifndef GFX_PALETTE
#define GFX_PALETTE

#include <stdint.h>

#include "gfx.h"

class gfxPalette {
public:
    gfxPalette();
    ~gfxPalette();

    bool load(const char*);
    void clear();
    unsigned short colorCount() { return numcolors; }
    bool matchesColorAtID(unsigned short id, uint8_t r, uint8_t g, uint8_t b);
    void copyColorSchemeTo(
        unsigned short teamID, unsigned short schemeID, unsigned short colorID,
        uint8_t& r, uint8_t& g, uint8_t& b);

private:
    uint8_t* colorcodes[3]; //[colorcomponents][numcolors]

    //[numplayers][colorscheme][colorcomponents][numcolors]
    uint8_t* colorschemes[4][PlayerPalette::NUM_PALETTES][3];
    unsigned short numcolors;
};

#endif // GFX_PALETTE

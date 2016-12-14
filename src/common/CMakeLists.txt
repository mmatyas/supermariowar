set(COMMON_SRCS
    dirlist.cpp
    eyecandy.cpp
    FileIO.cpp
    FileList.cpp
    Game.cpp
    GameModeSettings.cpp
    GameValues.cpp
    gfx.cpp
    gfx/gfxFont.cpp
    gfx/gfxPalette.cpp
    gfx/gfxSDL.cpp
    gfx/gfxSprite.cpp
    gfx/SFont.cpp
    global.cpp
    input.cpp
    linfunc.cpp
    map.cpp
    map/MapReader.cpp
    map/MapReader15xx.cpp
    map/MapReader16xx.cpp
    map/MapReader17xx.cpp
    map/MapReader18xx.cpp
    MapList.cpp
    ObjectBase.cpp
    path.cpp
    RandomNumberGenerator.cpp
    ResourceManager.cpp
    sfx.cpp
    TilesetManager.cpp
    ui/MI_MapPreview.cpp
    ui/MI_MapField.cpp
    ui/MI_SelectField.cpp
    ui/MI_ImageSelectField.cpp
    ui/MI_SliderField.cpp
)

if (USE_PNG_SAVE)
    list(APPEND COMMON_SRCS src/savepng.cpp)
endif()

add_library(CommonFiles ${COMMON_SRCS})

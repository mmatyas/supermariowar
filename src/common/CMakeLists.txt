add_library(CommonFiles STATIC
    CmdArgs.cpp
    CmdArgs.h
    eyecandy.cpp
    eyecandy.h
    EyecandyStyles.h
    FileIO.cpp
    FileIO.h
    FileList.cpp
    FileList.h
    Game.cpp
    Game.h
    GameMode.h
    GameModeSettings.cpp
    GameModeSettings.h
    GameValues.cpp
    GameValues.h
    GameSystems.h
    gfx.cpp
    gfx.h
    gfx/Color.h
    gfx/gfxFont.cpp
    gfx/gfxFont.h
    gfx/gfxPalette.cpp
    gfx/gfxPalette.h
    gfx/gfxSDL.cpp
    gfx/gfxSDL.h
    gfx/gfxSprite.cpp
    gfx/gfxSprite.h
    gfx/SFont.cpp
    gfx/SFont.h
    global.cpp
    input.cpp
    input.h
    linfunc.cpp
    linfunc.h
    map.cpp
    map.h
    map/MapReader.cpp
    map/MapReader.h
    map/MapReader15xx.cpp
    map/MapReader16xx.cpp
    map/MapReader17xx.cpp
    map/MapReader18xx.cpp
    map/MapReaderConstants.h
    math/Vec2.h
    MapList.cpp
    MapList.h
    MatchTypes.h
    MovingPlatformPaths.h
    MovingPlatformPaths.cpp
    ObjectBase.cpp
    ObjectBase.h
    path.cpp
    path.h
    RandomNumberGenerator.cpp
    RandomNumberGenerator.h
    ResourceManager.cpp
    ResourceManager.h
    sfx.cpp
    sfx.h
    TilesetManager.cpp
    TilesetManager.h
    TileTypes.cpp
    TileTypes.h
    uicontrol.cpp
    uicontrol.h
    uimenu.cpp
    uimenu.h
    Version.h
    WorldTourStop.cpp
    WorldTourStop.h
    util/ContainerHelpers.h
    util/Grid.h
    util/DirIterator.cpp
    util/DirIterator.h
    util/SdlHelpers.cpp
    util/SdlHelpers.h
)

add_subdirectory(ui)
target_compile_features(CommonFiles PUBLIC cxx_std_20)

target_compile_definitions(CommonFiles PUBLIC USE_SDL2)  # TODO
if (SDL2_FORCE_GLES)
    target_compile_definitions(CommonFiles PUBLIC SDL2_FORCE_GLES)
endif()
if (SDL2_USE_MIXERX)
    target_compile_definitions(CommonFiles PUBLIC SDL2_USE_MIXERX)
endif()


if(NOT EMSCRIPTEN)
    target_link_libraries(CommonFiles
        SDL2::SDL2
        SDL2_image::SDL2_image
    )
    if (SDL2_USE_MIXERX)
        target_link_libraries(CommonFiles SDL2_mixer_ext::SDL2_mixer_ext)
    else()
        target_link_libraries(CommonFiles SDL2_mixer::SDL2_mixer)
    endif()
endif()

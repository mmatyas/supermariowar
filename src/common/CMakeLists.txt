add_library(CommonFiles STATIC
    CmdArgs.cpp
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

if (USE_PNG_SAVE AND NOT USE_SDL2_LIBS)
    target_sources(CommonFiles PRIVATE savepng.cpp)
endif()


if (USE_SDL2_LIBS)
    target_compile_definitions(CommonFiles PUBLIC USE_SDL2)

    if (SDL2_FORCE_GLES)
        target_compile_definitions(CommonFiles PUBLIC SDL2_FORCE_GLES)
    endif()
    if (SDL2_USE_MIXERX)
        target_compile_definitions(CommonFiles PUBLIC SDL2_USE_MIXERX)
    endif()
endif()
if (USE_PNG_SAVE)
    target_compile_definitions(CommonFiles PUBLIC PNG_SAVE_FORMAT)
endif()


if(NOT EMSCRIPTEN)
    if (USE_SDL2_LIBS)
        target_link_libraries(CommonFiles
            SDL2::SDL2
            SDL2_image::SDL2_image
        )
        if (SDL2_USE_MIXERX)
            target_link_libraries(CommonFiles SDL2_mixer_ext::SDL2_mixer_ext)
        else()
            target_link_libraries(CommonFiles SDL2_mixer::SDL2_mixer)
        endif()
    else()
        target_link_libraries(CommonFiles
            ${SDL_LIBRARY}
            ${SDLMIXER_LIBRARY}
            ${SDLIMAGE_LIBRARY}
            ${SDLMAIN_LIBRARY}
        )
    endif()
endif()

add_library(CommonFiles STATIC
    CmdArgs.cpp
    CmdArgs.h
    dirlist.cpp
    dirlist.h
    eyecandy.cpp
    eyecandy.h
    EyecandyStyles.h
    FileIO.cpp
    FileIO.h
    FileList.cpp
    FileList.h
    Game.cpp
    Game.h
    GameModeSettings.cpp
    GameModeSettings.h
    GameValues.cpp
    GameValues.h
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
    ui/MenuCode.h
    ui/MI_Button.cpp
    ui/MI_Button.h
    ui/MI_Image.cpp
    ui/MI_Image.h
    ui/MI_ImageSelectField.cpp
    ui/MI_ImageSelectField.h
    ui/MI_MapField.cpp
    ui/MI_MapField.h
    ui/MI_MapPreview.cpp
    ui/MI_MapPreview.h
    ui/MI_ScoreText.cpp
    ui/MI_ScoreText.h
    ui/MI_SelectField.cpp
    ui/MI_SelectField.h
    ui/MI_SliderField.cpp
    ui/MI_SliderField.h
    ui/MI_Text.cpp
    ui/MI_Text.h
    ui/MI_TextField.cpp
    ui/MI_TextField.h
)

if (USE_PNG_SAVE AND NOT USE_SDL2_LIBS)
    target_sources(CommonFiles PRIVATE savepng.cpp savepng.h)
endif()

target_compile_features(CommonFiles PUBLIC cxx_std_14)


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

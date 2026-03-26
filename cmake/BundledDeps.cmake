include(FetchContent)

set(FETCHCONTENT_QUIET OFF)


function(smw_declare_gitrepo name url tag)
    FetchContent_Declare(
        ${name}
        GIT_REPOSITORY ${url}
        GIT_TAG ${tag}
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
        OVERRIDE_FIND_PACKAGE
    )
endfunction()


if (USE_BUNDLED_SDL)
    message("Using bundled SDL")

    # SDL
    set(SDL_STATIC OFF CACHE BOOL "")
    smw_declare_gitrepo(
        SDL2
        https://github.com/libsdl-org/SDL.git
        release-2.32.10
    )

    # SDL_image
    set(SDL2IMAGE_VENDORED ON CACHE BOOL "")
    set(SDL2IMAGE_DEPS_SHARED OFF CACHE BOOL "")
    set(SDL2IMAGE_ANI OFF CACHE BOOL "")
    set(SDL2IMAGE_AVIF OFF CACHE BOOL "")
    set(SDL2IMAGE_GIF OFF CACHE BOOL "")
    set(SDL2IMAGE_JPG OFF CACHE BOOL "")
    set(SDL2IMAGE_JXL OFF CACHE BOOL "")
    set(SDL2IMAGE_LBM OFF CACHE BOOL "")
    set(SDL2IMAGE_PCX OFF CACHE BOOL "")
    set(SDL2IMAGE_PNM OFF CACHE BOOL "")
    set(SDL2IMAGE_QOI OFF CACHE BOOL "")
    set(SDL2IMAGE_SVG OFF CACHE BOOL "")
    set(SDL2IMAGE_TGA OFF CACHE BOOL "")
    set(SDL2IMAGE_TIF OFF CACHE BOOL "")
    set(SDL2IMAGE_WEBP OFF CACHE BOOL "")
    set(SDL2IMAGE_XCF OFF CACHE BOOL "")
    set(SDL2IMAGE_XPM OFF CACHE BOOL "")
    set(SDL2IMAGE_XV OFF CACHE BOOL "")
    FetchContent_Declare(
        SDL2_image
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
        GIT_TAG release-2.8.8
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
        GIT_SUBMODULES "external/libpng" "external/zlib"
        OVERRIDE_FIND_PACKAGE
    )

    # SDL_mixer
    set(SDL2MIXER_VENDORED ON CACHE BOOL "")
    set(SDL2MIXER_DEPS_SHARED OFF CACHE BOOL "")
    set(SDL2MIXER_AIFF OFF CACHE BOOL "")
    set(SDL2MIXER_VOC OFF CACHE BOOL "")
    set(SDL2MIXER_AU OFF CACHE BOOL "")
    set(SDL2MIXER_FLAC OFF CACHE BOOL "")
    set(SDL2MIXER_GME OFF CACHE BOOL "")
    set(SDL2MIXER_MOD OFF CACHE BOOL "")
    set(SDL2MIXER_MP3 OFF CACHE BOOL "")
    set(SDL2MIXER_MIDI OFF CACHE BOOL "")
    set(SDL2MIXER_OPUS OFF CACHE BOOL "")
    set(SDL2MIXER_WAVPACK OFF CACHE BOOL "")
    FetchContent_Declare(
        SDL2_mixer
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_mixer.git
        GIT_TAG release-2.8.1
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
        GIT_SUBMODULES "external/ogg" "external/vorbis"
        OVERRIDE_FIND_PACKAGE
    )
endif()


if (USE_BUNDLED_YAMLCPP)
    message("Using bundled yaml-cpp")
    set(YAML_CPP_BUILD_CONTRIB OFF CACHE BOOL "")
    set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "")
    smw_declare_gitrepo(
        yaml-cpp
        https://github.com/jbeder/yaml-cpp.git
        yaml-cpp-0.9.0
    )
endif()


if (USE_BUNDLED_ENET)
    message("Using bundled enet")
    smw_declare_gitrepo(
        enet
        https://github.com/mmatyas/enet.git
        493c1b826b212a2d442c57b384cfe3101cfff3b7
    )
endif()


if (USE_BUNDLED_ZLIB)
    message("Using bundled zlib")
    set(ZLIB_BUILD_TESTING OFF CACHE BOOL "")
    set(ZLIB_BUILD_SHARED OFF CACHE BOOL "")
    set(ZLIB_INSTALL OFF CACHE BOOL "")
    smw_declare_gitrepo(
        zlib
        https://github.com/madler/zlib.git
        v1.3.2
    )
endif()

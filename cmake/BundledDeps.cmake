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
        SDL3
        https://github.com/libsdl-org/SDL.git
        release-3.4.2
    )

    # SDL_image
    set(SDL2IMAGE_VENDORED ON CACHE BOOL "")
    set(SDL2IMAGE_DEPS_SHARED OFF CACHE BOOL "")
    set(SDLIMAGE_ANI OFF CACHE BOOL "")
    set(SDLIMAGE_AVIF OFF CACHE BOOL "")
    set(SDLIMAGE_GIF OFF CACHE BOOL "")
    set(SDLIMAGE_JPG OFF CACHE BOOL "")
    set(SDLIMAGE_JXL OFF CACHE BOOL "")
    set(SDLIMAGE_LBM OFF CACHE BOOL "")
    set(SDLIMAGE_PCX OFF CACHE BOOL "")
    set(SDLIMAGE_PNM OFF CACHE BOOL "")
    set(SDLIMAGE_QOI OFF CACHE BOOL "")
    set(SDLIMAGE_SVG OFF CACHE BOOL "")
    set(SDLIMAGE_TGA OFF CACHE BOOL "")
    set(SDLIMAGE_TIF OFF CACHE BOOL "")
    set(SDLIMAGE_WEBP OFF CACHE BOOL "")
    set(SDLIMAGE_XCF OFF CACHE BOOL "")
    set(SDLIMAGE_XPM OFF CACHE BOOL "")
    set(SDLIMAGE_XV OFF CACHE BOOL "")
    smw_declare_gitrepo(
        SDL3_image
        https://github.com/libsdl-org/SDL_image.git
        release-3.4.0
    )

    # SDL_mixer
    set(SDLMIXER_VENDORED ON CACHE BOOL "")
    set(SDLMIXER_DEPS_SHARED OFF CACHE BOOL "")
    set(SDLMIXER_AIFF OFF CACHE BOOL "")
    set(SDLMIXER_VOC OFF CACHE BOOL "")
    set(SDLMIXER_AU OFF CACHE BOOL "")
    set(SDLMIXER_FLAC OFF CACHE BOOL "")
    set(SDLMIXER_GME OFF CACHE BOOL "")
    set(SDLMIXER_MOD OFF CACHE BOOL "")
    set(SDLMIXER_MP3 OFF CACHE BOOL "")
    set(SDLMIXER_MIDI OFF CACHE BOOL "")
    set(SDLMIXER_WAVPACK OFF CACHE BOOL "")
    smw_declare_gitrepo(
        SDL3_mixer
        https://github.com/libsdl-org/SDL_mixer.git
        release-3.2.0
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

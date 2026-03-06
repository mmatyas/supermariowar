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

    # SDL2
    set(SDL_STATIC OFF CACHE BOOL "")
    smw_declare_gitrepo(
        SDL2
        https://github.com/libsdl-org/SDL.git
        release-2.30.11
    )

    # SDL2 image
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
    smw_declare_gitrepo(
        SDL2_image
        https://github.com/libsdl-org/SDL_image
        release-2.8.4
    )

    # SDL2 mixer
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
    set(SDL2MIXER_WAVPACK OFF CACHE BOOL "")
    smw_declare_gitrepo(
        SDL2_mixer
        https://github.com/libsdl-org/SDL_mixer
        release-2.8.0
    )
endif()

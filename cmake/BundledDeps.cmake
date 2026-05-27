include(FetchContent)

set(FETCHCONTENT_QUIET OFF)


function(smw_declare_gitrepo name url tag)
    FetchContent_Declare(
        ${name}
        GIT_REPOSITORY ${url}
        GIT_TAG ${tag}
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
        FIND_PACKAGE_ARGS NAMES ${name}
    )
endfunction()


# SDL
set(SDL_STATIC OFF CACHE BOOL "")
smw_declare_gitrepo(
    SDL3
    https://github.com/libsdl-org/SDL.git
    release-3.4.8
)

# SDL_image
set(SDLIMAGE_VENDORED ON CACHE BOOL "")
set(SDLIMAGE_DEPS_SHARED OFF CACHE BOOL "")
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
FetchContent_Declare(
    SDL3_image
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
    GIT_TAG release-3.4.4
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
    GIT_SUBMODULES "external/libpng" "external/zlib"
    FIND_PACKAGE_ARGS NAMES SDL3_image
)

# SDL_mixer
set(SDLMIXER_VENDORED ON CACHE BOOL "")
if(EMSCRIPTEN)
    set(SDLMIXER_DEPS_SHARED OFF CACHE BOOL "")
    set(SDLMIXER_AIFF OFF CACHE BOOL "")
    set(SDLMIXER_VOC OFF CACHE BOOL "")
    set(SDLMIXER_AU OFF CACHE BOOL "")
    set(SDLMIXER_FLAC OFF CACHE BOOL "")
    set(SDLMIXER_GME OFF CACHE BOOL "")
    set(SDLMIXER_MOD OFF CACHE BOOL "")
    set(SDLMIXER_MP3 OFF CACHE BOOL "")
    set(SDLMIXER_MIDI OFF CACHE BOOL "")
    set(SDLMIXER_OPUS OFF CACHE BOOL "")
    set(SDLMIXER_WAVPACK OFF CACHE BOOL "")
    set(SDLMIXER_OPUS OFF CACHE BOOL "")
endif()
smw_declare_gitrepo(
    SDL3_mixer
    https://github.com/libsdl-org/SDL_mixer.git
    release-3.2.2
)


# toml11
FetchContent_Declare(
    toml11
    GIT_REPOSITORY https://github.com/ToruNiina/toml11.git
    GIT_TAG v4.4.0
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
    GIT_SUBMODULES ""
    FIND_PACKAGE_ARGS NAMES toml11
)

# enet
smw_declare_gitrepo(
    enet
    https://github.com/mmatyas/enet.git
    74d43e850f5abda0e5f565caa5d54f462bc210fd
)

# zlib
set(ZLIB_BUILD_TESTING OFF CACHE BOOL "")
set(ZLIB_BUILD_SHARED OFF CACHE BOOL "")
set(ZLIB_INSTALL OFF CACHE BOOL "")
smw_declare_gitrepo(
    ZLIB
    https://github.com/madler/zlib.git
    v1.3.2
)


FetchContent_MakeAvailable(SDL3 SDL3_image SDL3_mixer toml11)
if(NOT NO_NETWORK)
    FetchContent_MakeAvailable(enet)
endif()
# SDL *may* pull in its own vendored zlib
if (NOT TARGET ZLIB::ZLIB AND NOT TARGET ZLIB::ZLIBSTATIC)
    FetchContent_MakeAvailable(zlib)
endif()


# Link to static ZLIB when bundled, shared when using system
add_library(smw_zlib INTERFACE)
if (TARGET ZLIB::ZLIB)
    target_link_libraries(smw_zlib INTERFACE ZLIB::ZLIB)
else()
    target_link_libraries(smw_zlib INTERFACE ZLIB::ZLIBSTATIC)
endif()

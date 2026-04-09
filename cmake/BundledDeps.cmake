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
    SDL2
    https://github.com/libsdl-org/SDL.git
    release-2.32.10
)
smw_declare_gitrepo(
    SDL3
    https://github.com/libsdl-org/SDL.git
    release-3.4.2
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
    FIND_PACKAGE_ARGS NAMES SDL2_image
)
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
set(SDLIMAGE_ZLIB_VENDORED OFF CACHE BOOL "")
FetchContent_Declare(
    SDL3_image
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
    GIT_TAG release-3.4.0
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
    GIT_SUBMODULES "external/libpng" "external/zlib"
    FIND_PACKAGE_ARGS NAMES SDL3_image
)

# SDL_mixer
set(SDL2MIXER_VENDORED ON CACHE BOOL "")
smw_declare_gitrepo(
    SDL2_mixer
    https://github.com/libsdl-org/SDL_mixer.git
    release-2.8.1
)
smw_declare_gitrepo(
    SDL3_mixer
    https://github.com/libsdl-org/SDL_mixer.git
    release-3.2.0
)


# yaml-cpp
set(YAML_CPP_BUILD_CONTRIB OFF CACHE BOOL "")
set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "")
smw_declare_gitrepo(
    yaml-cpp
    https://github.com/jbeder/yaml-cpp.git
    yaml-cpp-0.9.0
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


if (SMW_USE_SDL3)
    FetchContent_MakeAvailable(SDL3 SDL3_image SDL3_mixer)
else()
    FetchContent_MakeAvailable(SDL2 SDL2_image SDL2_mixer)
endif()
# SDL *may* pull in its own vendored zlib
if (NOT TARGET ZLIB::ZLIB AND NOT TARGET ZLIB::ZLIBSTATIC)
    FetchContent_MakeAvailable(zlib)
endif()
if(NOT NO_NETWORK)
    FetchContent_MakeAvailable(enet)
endif()
FetchContent_MakeAvailable(yaml-cpp)


add_library(smw_sdl INTERFACE)
add_library(smw_sdl_image INTERFACE)
add_library(smw_sdl_mixer INTERFACE)
target_link_libraries(smw_sdl INTERFACE $<IF:$<BOOL:${SMW_USE_SDL3}>,SDL3,SDL2>)
target_link_libraries(smw_sdl_image INTERFACE $<IF:$<BOOL:${SMW_USE_SDL3}>,SDL3_image,SDL2_image>)
target_link_libraries(smw_sdl_mixer INTERFACE $<IF:$<BOOL:${SMW_USE_SDL3}>,SDL3_mixer,SDL2_mixer>)
target_compile_definitions(smw_sdl PUBLIC SMW_USE_SDL3)


# Link to static ZLIB when bundled, shared when using system
add_library(smw_zlib INTERFACE)
if (TARGET ZLIB::ZLIB)
    target_link_libraries(smw_zlib INTERFACE ZLIB::ZLIB)
else()
    target_link_libraries(smw_zlib INTERFACE ZLIB::ZLIBSTATIC)
endif()

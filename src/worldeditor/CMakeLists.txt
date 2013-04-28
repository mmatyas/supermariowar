
set(WORLDEDIT_SRCS  worldeditor.cpp )

add_executable(worldedit EXCLUDE_FROM_ALL ${COMMON_SRCS} ${WORLDEDIT_SRCS} ../common/global.h)

target_link_libraries(worldedit ${SDL_LIBRARY} ${SDLMIXER_LIBRARY} ${SDLIMAGE_LIBRARY} SDLmain)

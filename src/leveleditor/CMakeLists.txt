
set(LEVELEDIT_SRCS leveleditor.cpp)

add_executable(leveledit EXCLUDE_FROM_ALL ${COMMON_SRCS} ${LEVELEDIT_SRCS} ../common/global.h)

target_link_libraries(leveledit ${SDL_LIBRARY} ${SDLMIXER_LIBRARY} ${SDLIMAGE_LIBRARY} SDLmain)

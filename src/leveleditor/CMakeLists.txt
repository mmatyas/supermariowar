set(LEVELEDIT_SRCS leveleditor.cpp)

add_executable(leveledit ../common/movingplatform.cpp ${LEVELEDIT_SRCS})
target_link_libraries(leveledit CommonFiles)

# add .bc extension when building with emscripten
if(USE_EMSCRIPTEN)
	set_target_properties(leveledit PROPERTIES SUFFIX ".bc")
endif()

if(NOT USE_EMSCRIPTEN)
	target_link_libraries(leveledit ${SDL_LIBRARY} ${SDLMIXER_LIBRARY} ${SDLIMAGE_LIBRARY} SDLmain)
endif()

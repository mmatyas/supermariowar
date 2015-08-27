set(LEVELEDIT_SRCS leveleditor.cpp)

add_executable(smw-leveledit ../common/movingplatform.cpp ../smw/FPSLimiter.cpp ${LEVELEDIT_SRCS})
target_link_libraries(smw-leveledit CommonFiles)

# add .bc extension when building with emscripten
if(TARGET_EMSCRIPTEN)
	set_target_properties(smw-leveledit PROPERTIES SUFFIX ".bc")
endif()

if(NOT TARGET_EMSCRIPTEN)
	target_link_libraries(smw-leveledit ${SDL_LIBRARY} ${SDLMIXER_LIBRARY} ${SDLIMAGE_LIBRARY} SDLmain)
endif()

install(FILES "${PROJECT_BINARY_DIR}/Binaries/Release/smw-leveledit" DESTINATION ${SMW_BINDIR})

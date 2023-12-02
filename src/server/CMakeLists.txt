add_executable(smw-server
    mainServer.cpp
    Blob.cpp
    Blob.h
    Clock.h
    Log.cpp
    Log.h
    NetworkLayerENet.cpp
    NetworkLayerENet.h
    Player.cpp
    Player.h
    Room.cpp
    Room.h
    server.cpp
    server.h
)
if(WIN32)
    # Add EXE icon on Windows
    target_sources(smw-server PRIVATE "${PROJECT_SOURCE_DIR}/resources/windows/smw-server.rc")
endif()

target_link_libraries(smw-server NetplayFiles)
target_compile_definitions(smw-server PRIVATE IS_SERVER=1)
target_compile_options(smw-server PRIVATE "-Wall")

install(TARGETS smw-server DESTINATION "${SMW_BINDIR}")

# install Linux desktop item and icons
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux" AND NOT SMW_INSTALL_PORTABLE)
    install(FILES "${PROJECT_SOURCE_DIR}/resources/icons/smw-server.xpm" DESTINATION "${SMW_PIXMAPDIR}")
    install(FILES "${PROJECT_SOURCE_DIR}/resources/icons/smw-server.png" DESTINATION "${SMW_ICONDIR}/32x32/apps/")
endif()

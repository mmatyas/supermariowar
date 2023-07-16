add_executable(smw-server
    Blob.cpp
    Log.cpp
    NetworkLayerENet.cpp
    Player.cpp
    Room.cpp
    server.cpp
    mainServer.cpp
)
if(WIN32)
    # Add EXE icon on Windows
    target_sources(smw-server PRIVATE "${PROJECT_SOURCE_DIR}/resources/windows/smw-server.rc")
endif()

target_link_libraries(smw-server NetplayFiles)
target_compile_definitions(smw-server PRIVATE IS_SERVER=1)
target_compile_options(smw-server PRIVATE "-Wall")

install(TARGETS smw-server DESTINATION "${SMW_BINDIR}")

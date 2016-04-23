set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set (CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -Wall")

set (SERVER_SRCS
    Blob.cpp
    Log.cpp
    NetworkLayerENet.cpp
    Player.cpp
    Room.cpp
    server.cpp
    mainServer.cpp
)

add_definitions(-DIS_SERVER=1)

add_executable (smw-server ${SERVER_SRCS})
target_link_libraries(smw-server NetplayFiles)

target_link_libraries (smw-server enet)
if (WIN32)
  # WinSock2 libs, required by ENet on Windows
  target_link_libraries(smw-server ws2_32 winmm)
endif()

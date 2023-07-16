if(NOT NO_NETWORK)
    add_library(NetplayFiles platform_enet/NetPeerENet.cpp)
    target_link_libraries(NetplayFiles enet)
    if (WIN32)
        # WinSock2 libs, required by ENet on Windows
        target_link_libraries(NetplayFiles ws2_32 winmm)
    endif()
endif()

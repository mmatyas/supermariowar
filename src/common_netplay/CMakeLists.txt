if(NOT NO_NETWORK)
    add_library(NetplayFiles STATIC
        NetworkInterface.h
        ProtocolDefinitions.h
        ProtocolPackages.h
        platform_enet/NetPeerENet.cpp
        platform_enet/NetPeerENet.h
    )

    target_link_libraries(NetplayFiles enet)
    if (WIN32)
        # WinSock2 libs, required by ENet on Windows
        target_link_libraries(NetplayFiles ws2_32 winmm)
    endif()

    target_compile_features(NetplayFiles PUBLIC cxx_std_11)
endif()

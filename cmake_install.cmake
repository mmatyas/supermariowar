# Install script for directory: /media/NAS/Dropbox/Documents/steamlink-sdk/examples/supermariowar

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "TRUE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/local/share/games/smw/")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/usr/local/share/games/smw" TYPE DIRECTORY FILES "/media/NAS/Dropbox/Documents/steamlink-sdk/examples/supermariowar/data/")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/media/NAS/Dropbox/Documents/steamlink-sdk/examples/supermariowar/dependencies/enet/cmake_install.cmake")
  include("/media/NAS/Dropbox/Documents/steamlink-sdk/examples/supermariowar/dependencies/yaml-cpp-noboost/cmake_install.cmake")
  include("/media/NAS/Dropbox/Documents/steamlink-sdk/examples/supermariowar/src/common/cmake_install.cmake")
  include("/media/NAS/Dropbox/Documents/steamlink-sdk/examples/supermariowar/src/common_netplay/cmake_install.cmake")
  include("/media/NAS/Dropbox/Documents/steamlink-sdk/examples/supermariowar/src/smw/cmake_install.cmake")
  include("/media/NAS/Dropbox/Documents/steamlink-sdk/examples/supermariowar/src/leveleditor/cmake_install.cmake")
  include("/media/NAS/Dropbox/Documents/steamlink-sdk/examples/supermariowar/src/worldeditor/cmake_install.cmake")
  include("/media/NAS/Dropbox/Documents/steamlink-sdk/examples/supermariowar/src/server/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/media/NAS/Dropbox/Documents/steamlink-sdk/examples/supermariowar/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")

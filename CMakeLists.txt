cmake_minimum_required(VERSION 3.5)

project(VRPN-OpenVR)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

include(ExternalProject)
ExternalProject_Add(vrpn
        GIT_REPOSITORY https://github.com/vrpn/vrpn.git
        UPDATE_COMMAND "" # Prevents rebuilds on every update
        INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/vrpn-install
        )

set(VRPN_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/vrpn-install/include)
set(VRPN_LIBRARIES_DIR ${CMAKE_CURRENT_BINARY_DIR}/vrpn-install/lib)


include_directories(
        ${VRPN_INCLUDE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
        vendor/openvr/headers
)

link_directories(
        ${VRPN_LIBRARIES_DIR}
        vendor/openvr/lib/linux64
        vendor/openvr/bin/linux64
)

find_library(LIBQUAT libquat.a)
find_library(LIBVRPN libvrpn.a)

link_libraries(
        ${LIBQUAT}
        ${LIBVRPN}
)

add_executable(vrpnOpenVR
    VRPN-OpenVR/main.cpp
    VRPN-OpenVR/vrpn_Server_OpenVR.cpp
    VRPN-OpenVR/vrpn_Tracker_OpenVR.cpp
    VRPN-OpenVR/vrpn_Tracker_OpenVR_HMD.cpp
    VRPN-OpenVR/vrpn_Tracker_OpenVR_Controller.cpp
    VRPN-OpenVR/vrpn_Tracker_OpenVR_Tracker.cpp
    )

add_dependencies(vrpnOpenVR vrpn)

target_link_libraries(vrpnOpenVR
        PRIVATE pthread
        PRIVATE openvr_api)

cmake_minimum_required(VERSION 3.20)
project(Traffic_sdk)

set(CMAKE_CXX_STANDARD 17)


# Set a default build type if none was specified
set(DEFAULT_BUILD_TYPE "Release")
if(EXISTS "${CMAKE_SOURCE_DIR}/.git")
  set(DEFAULT_BUILD_TYPE "Debug")
endif()
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${DEFAULT_BUILD_TYPE}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${DEFAULT_BUILD_TYPE}" CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()


add_executable(traffic_processor traffic_processing_sdk.cpp)

if(WIN32) # Install dlls in the same directory as the executable on Windows
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

    # Copy required DLLs to the output directory
    add_custom_command(TARGET traffic_processor POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_FILE:RdKafka::rdkafka++>
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    # Copy required DLLs to the output directory
    add_custom_command(TARGET traffic_processor POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_FILE:RdKafka::rdkafka>
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    # Copy lz4d.dll to the output directory
    add_custom_command(TARGET traffic_processor POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/installed/x64-windows/debug/bin/lz4d.dll
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif()

include(FetchContent)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
                         GIT_TAG 3020c34ae2b732121f37433e61599c34535e68a8) # The commit hash for 1.10.x. Replace with the latest from: https://github.com/libcpr/cpr/releases
FetchContent_MakeAvailable(cpr)

target_link_libraries(traffic_processor PRIVATE cpr::cpr)

# Include vcpkg toolchain file
include(${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake)

# Use vcpkg to find librdkafka
find_package(RdKafka REQUIRED)

# Link librdkafka to your target
target_link_libraries(traffic_processor PRIVATE RdKafka::rdkafka++)
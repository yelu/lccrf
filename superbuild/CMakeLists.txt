cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

option(BUILD_BOOST "Build boost, default to ON" ON)
option(BUILD_PROTOBUF "Build protobuf, default to ON" ON)
option(BUILD_RAPIDJSON "Build rapidjson, default to ON" ON)
option(BUILD_GTEST "Build gtest, default to ON" ON)
option(BUILD_SPDLOG "Build spdlog, default to ON" ON)
option(BUILD_PYBIND "Build pybind11, default to ON" ON)

project(cmftk-superbuild NONE)

message("CMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME}, CMAKE_SYSTEM_VERSION=${CMAKE_SYSTEM_VERSION}, CROSSCOMPILING=${CMAKE_CROSSCOMPILING}")
if(${CMAKE_SYSTEM_NAME} MATCHES "Android")
    message("CMAKE_ANDROID_NDK=${CMAKE_ANDROID_NDK}")
    message("CMAKE_ANDROID_ARCH_ABI=${CMAKE_ANDROID_ARCH_ABI}")
endif()

get_filename_component(ABSOLUTE_INSTALL_PREFIX
    ${CMAKE_CURRENT_SOURCE_DIR}/../installed
    ABSOLUTE)
SET(CMAKE_INSTALL_PREFIX ${ABSOLUTE_INSTALL_PREFIX} CACHE PATH "cmake install prefix" FORCE)
message("CMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}")

IF(BUILD_BOOST)
    add_subdirectory(boost)
ENDIF()

IF(BUILD_GTEST)
    add_subdirectory(gtest)
ENDIF()

IF(BUILD_SPDLOG)
    add_subdirectory(spdlog)
ENDIF()

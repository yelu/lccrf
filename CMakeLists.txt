cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project(lccrf VERSION 0.1)

# set install prefix path. the default value is set as following. it can be overridden in command line
SET(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/installed/lccrf" CACHE PATH "cmake install prefix" FORCE)

# whether to skip building test code
# possible values:
# - ON: test code will be skipped
# - OFF: test code will be built
option(LCCRF_NO_TEST "skip test, default to OFF" OFF)

# verbose some environment information
message("CMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME}, CMAKE_SYSTEM_VERSION=${CMAKE_SYSTEM_VERSION}, CROSSCOMPILING=${CMAKE_CROSSCOMPILING}")
if(${CMAKE_SYSTEM_NAME} MATCHES "Android")
   message("CMAKE_ANDROID_NDK=${CMAKE_ANDROID_NDK}")
   message("CMAKE_ANDROID_ARCH_ABI=${CMAKE_ANDROID_ARCH_ABI}")
endif()

message("CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}, for single-configuration generators(makefile, ninja) only")
message("CMAKE_CONFIG=${CMAKE_CONFIG}, for multi-configuration generators(vs, xcode) only")

# process python versions
add_subdirectory(lccrf)

if(NOT ${LCCRF_NO_TEST} AND NOT ${CMAKE_CROSSCOMPILING})
    set(ENV{CTEST_OUTPUT_ON_FAILURE} "TRUE")
    enable_testing()
    add_subdirectory(tests)
endif()

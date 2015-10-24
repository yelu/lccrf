cmake_minimum_required(VERSION 2.8.9)
project(lccrf)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_BUILD_TYPE Release)

include_directories(lib)
file(GLOB LIB_SOURCES "lib/*.cpp")
add_library(lccrf STATIC ${LIB_SOURCES})
set_target_properties(lccrf PROPERTIES ARCHIVE_OUTPUT_DIRECTORY output/)

file(GLOB BIN_SOURCES "console/*.cpp")
add_executable(lccrf.exe ${BIN_SOURCES})
TARGET_LINK_LIBRARIES(lccrf.exe lccrf)
set_target_properties(lccrf.exe PROPERTIES RUNTIME_OUTPUT_DIRECTORY output/)

#install(TARGETS lccrf DESTINATION Debug)


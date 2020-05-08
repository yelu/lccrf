message("use toolchain file x64-windows.cmake")

# C++11
set(CMAKE_CXX_STANDARD 11 CACHE STRING "C++ Standard (toolchain)" FORCE)
set(CMAKE_CXX_STANDARD_REQUIRED YES CACHE BOOL "C++ Standard required" FORCE)
set(CMAKE_CXX_EXTENSIONS NO CACHE BOOL "C++ Standard extensions" FORCE)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_CONFIGURATION_TYPES Debug;Release;RelWithDebInfo CACHE STRING "VS build configurations" FORCE)
# in debug config, generate debug symbols, disable optimization and inline expansion.
set(CMAKE_CXX_FLAGS_DEBUG "/MDd /Zi /Ob0 /Od /MP" CACHE STRING "C++ build flags for debug" FORCE)
set(CMAKE_CXX_FLAGS_RELEASE "/MD /O2 /Ob2 /DNDEBUG /MP" CACHE STRING "C++ build flags for release" FORCE)
set(CMAKE_CXX_FLAGS_RELEASEWITHDEBUGINFO "/MD /O2 /Ob2 /g /DNDEBUG /MP" CACHE STRING "C++ build flags for release" FORCE)

SET(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "Makefile log verbose")

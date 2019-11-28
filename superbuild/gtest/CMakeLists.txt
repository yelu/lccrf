cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

include(ExternalProject)
set(URL https://github.com/google/googletest/archive/release-1.8.1.tar.gz)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    ExternalProject_Add(gtest
        URL ${URL}
        CMAKE_ARGS
            "-DBUILD_GMOCK=OFF"
            "-Dgtest_force_shared_crt=ON"
            "-Dgtest_build_tests=OFF"
            "-Dgtest_build_samples=OFF"
            "-DCMAKE_BUILD_TYPE=Release"
            "-DCMAKE_POSITION_INDEPENDENT_CODE=ON"
            "-DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}/gtest"
        BUILD_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --target install
        INSTALL_COMMAND ""
        TEST_COMMAND "")
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    ExternalProject_Add(gtest
        URL ${URL}
        CMAKE_ARGS
            "-DBUILD_GMOCK=OFF"
            "-Dgtest_force_shared_crt=ON"
            "-Dgtest_build_tests=OFF"
            "-Dgtest_build_samples=OFF"
            "-DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}/gtest"
        BUILD_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --config Release --target install
              COMMAND ${CMAKE_COMMAND} -E echo "Release build complete"
              COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --config Debug --target install
              COMMAND ${CMAKE_COMMAND} -E echo "Debug build complete"
        INSTALL_COMMAND ""
        TEST_COMMAND "")
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Android")
    message("cross compiling for android, skip gtest build")
    return()
endif()
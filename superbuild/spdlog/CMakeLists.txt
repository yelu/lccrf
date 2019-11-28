cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

set(GIT_REPO https://github.com/gabime/spdlog.git)
set(GIT_TAG 3adfeeec3ed0131c3f07bfc7bb1f4003bb2eb981)
set(SPDLOG_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/spdlog")

include(ExternalProject)
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    ExternalProject_Add(spdlog
        GIT_REPOSITORY ${GIT_REPO}
        GIT_TAG ${GIT_TAG}
        CMAKE_ARGS
            "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}"
            "-DCMAKE_INSTALL_PREFIX=${SPDLOG_INSTALL_DIR}"
        BUILD_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --config Release --target install
              COMMAND ${CMAKE_COMMAND} -E echo "Release build complete"
              COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --config Debug --target install
              COMMAND ${CMAKE_COMMAND} -E echo "Debug build complete"          
        INSTALL_COMMAND ""
        TEST_COMMAND "")
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "(Linux|Android)")
    ExternalProject_Add(spdlog
        GIT_REPOSITORY ${GIT_REPO}
        GIT_TAG ${GIT_TAG}
        CMAKE_ARGS
            "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}"
            "-DCMAKE_INSTALL_PREFIX=${SPDLOG_INSTALL_DIR}"
            "-DCMAKE_BUILD_TYPE=Release"
        BUILD_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --target install
        INSTALL_COMMAND ""
        TEST_COMMAND "")
endif()

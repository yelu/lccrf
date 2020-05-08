cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

# The debug version of the library file will be named libmylibd.lib (or mylibd.lib on Windows)
set(CMAKE_DEBUG_POSTFIX d)
IF (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
ENDIF()

find_package(Boost REQUIRED COMPONENTS headers PATHS ${CMAKE_INSTALL_PREFIX}/../boost)
message("Boost_LIBRARIES=${Boost_LIBRARIES}")
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_INSTALL_PREFIX}/../spdlog)
find_package(spdlog REQUIRED)

message("CMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}")

set(LCCRF_SOURCES
    "decoder.cpp"
    "evaluator.cpp"
    "lccrf.cpp"
    "perceptron.cpp"
    "viterbi.cpp")
set(LCCRF_HEADERS
    "evaluator.h"
    "features.h"
    "lccrf.h"
    "multi_array.h"
    "perceptron.h"
    "query.h"
    "viterbi.h")

add_library(lccrf STATIC ${LCCRF_SOURCES} ${LCCRF_HEADERS} ${PROTO_FILES})

target_compile_definitions(lccrf PUBLIC -DNOMINMAX -D_SCL_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_WARNINGS)
target_include_directories(lccrf 
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>)
target_link_libraries(lccrf 
    PUBLIC
        $<BUILD_INTERFACE:spdlog::spdlog_header_only>
        $<BUILD_INTERFACE:Boost::headers>)

if(UNIX)
    target_link_libraries(lccrf PRIVATE ${CMAKE_DL_LIBS})
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Android")
    find_library( log-lib log )
    target_link_libraries(lccrf PRIVATE ${log-lib})
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Android")
    add_custom_command(
        TARGET lccrf POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${ANDROID_NDK}/sources/cxx-stl/llvm-libc++/libs/${ANDROID_ABI}/libc++_shared.so
                $<TARGET_FILE_DIR:lccrf>)
endif()

include(GenerateExportHeader)
generate_export_header(lccrf
    EXPORT_FILE_NAME lccrf_export.h
    EXPORT_MACRO_NAME LCCRF_EXPORT)

# Instalation
# Introduce variables: CMAKE_INSTALL_LIBDIR, CMAKE_INSTALL_BINDIR, CMAKE_INSTALL_INCLUDEDIR
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
set(TARGETS_EXPORT_NAME "lccrf-targets")
set(CONFIG_INSTALL_DIR "share/cmake/lccrf")
set(LIB_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}")
set(INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_INCLUDEDIR}")
write_basic_package_version_file(
    lccrf-config-version.cmake COMPATIBILITY SameMajorVersion
)
configure_package_config_file(
    lccrf-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/lccrf-config.cmake
    INSTALL_DESTINATION ${CONFIG_INSTALL_DIR})
install(TARGETS lccrf EXPORT ${TARGETS_EXPORT_NAME}
    LIBRARY DESTINATION ${LIB_INSTALL_DIR}/$<$<CONFIG:Debug>:debug>
    ARCHIVE DESTINATION ${LIB_INSTALL_DIR}/$<$<CONFIG:Debug>:debug>
    RUNTIME DESTINATION ${LIB_INSTALL_DIR}/$<$<CONFIG:Debug>:debug>)
install(DIRECTORY $<TARGET_FILE_DIR:lccrf>/
    USE_SOURCE_PERMISSIONS
    DESTINATION ${LIB_INSTALL_DIR}/$<$<CONFIG:Debug>:debug>
    FILES_MATCHING PATTERN "*.so" PATTERN "*.dll")
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lccrf/
    DESTINATION ${INCLUDE_INSTALL_DIR}/lccrf
    FILES_MATCHING PATTERN "*.h")
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/lccrf_export.h
    DESTINATION ${INCLUDE_INSTALL_DIR}/lccrf)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/lccrf-config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/lccrf-config-version.cmake
    DESTINATION ${CONFIG_INSTALL_DIR})
install(EXPORT ${TARGETS_EXPORT_NAME}
    NAMESPACE lccrf:: 
    DESTINATION ${CONFIG_INSTALL_DIR})

# install system runtime libs: vcruntime[140|141|...].dll or glibc.so
include(InstallRequiredSystemLibraries)
if(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS)
    install(PROGRAMS 
        ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} 
        DESTINATION ${LIB_INSTALL_DIR} COMPONENT System)
endif( CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS )

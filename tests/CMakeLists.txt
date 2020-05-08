cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

# The debug version of the library file will be named libmylibd.lib (or mylibd.lib on Windows)
set(CMAKE_DEBUG_POSTFIX d)
IF (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
ENDIF()

list(APPEND CMAKE_PREFIX_PATH ${CMAKE_INSTALL_PREFIX}/../gtest)
find_package(GTest REQUIRED)

message("CMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}")

set(LCCRF_TESTS_SOURCES
    "test_lccrf.cpp"
    "test_perceptron.cpp")

add_executable(lccrf_tests ${LCCRF_TESTS_SOURCES})

target_compile_definitions(lccrf_tests PUBLIC -DNOMINMAX -D_SCL_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_WARNINGS)
target_link_libraries(lccrf_tests 
    PUBLIC
        lccrf
        GTest::GTest 
        GTest::Main)
add_custom_command(
    TARGET lccrf_tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_CURRENT_SOURCE_DIR}/lccrf.vw.dat
            $<TARGET_FILE_DIR:lccrf_tests>)

if(UNIX)
    target_link_libraries(lccrf PRIVATE ${CMAKE_DL_LIBS})
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Android")
    find_library( log-lib log )
    target_link_libraries(lccrf_tests PRIVATE ${log-lib})
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Android")
    add_custom_command(
        TARGET lccrf_tests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${ANDROID_NDK}/sources/cxx-stl/llvm-libc++/libs/${ANDROID_ABI}/libc++_shared.so
                $<TARGET_FILE_DIR:lccrf_tests>)
endif()

if(MSVC)
    set_target_properties(lccrf_tests PROPERTIES 
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/$(Configuration)")
endif()

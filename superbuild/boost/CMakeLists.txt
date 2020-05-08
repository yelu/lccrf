cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

include(ExternalProject)
set(BOOST_VER 1.70.0)
set(BOOST_COMPONENTS headers)
set(BOOST_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/boost")

list(TRANSFORM BOOST_COMPONENTS PREPEND "--with-" OUTPUT_VARIABLE BOOST_WITH_LIBRARIES)
string(REPLACE "." "_" BOOST_VER_1 ${BOOST_VER})
set(URL https://dl.bintray.com/boostorg/release/${BOOST_VER}/source/boost_${BOOST_VER_1}.tar.gz)
set(GIT_REPO https://github.com/moritz-wundke/Boost-for-Android.git)
set(GIT_TAG aed656a97fb3af7322fd2c3da5995a2d09d87d4b)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    ExternalProject_Add(Boost
        URL ${URL}
        BUILD_IN_SOURCE true
        CONFIGURE_COMMAND ./bootstrap.sh
        BUILD_COMMAND ./b2 install --prefix=${BOOST_INSTALL_DIR} ${BOOST_WITH_LIBRARIES} link=static threading=multi runtime-link=shared cxxflags=-fPIC cflags=-fPIC
        INSTALL_COMMAND ""
        TEST_COMMAND "")
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    ExternalProject_Add(Boost
        URL ${URL}
        BUILD_IN_SOURCE true
        CONFIGURE_COMMAND bootstrap.bat
#        BUILD_COMMAND bjam.exe -j4 install --prefix=${BOOST_INSTALL_DIR} --with-program_options --with-filesystem --with-thread link=static threading=multi runtime-link=shared architecture=x86 address-model=64
        BUILD_COMMAND bjam.exe -j4 install --prefix=${BOOST_INSTALL_DIR} ${BOOST_WITH_LIBRARIES} link=static threading=multi runtime-link=shared architecture=x86 address-model=64
        INSTALL_COMMAND ""
        TEST_COMMAND "")
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Android")
    list(JOIN BOOST_COMPONENTS "," BOOST_WITH_LIBRARIES)
    ExternalProject_Add(Boost
        GIT_REPOSITORY ${GIT_REPO}
        GIT_TAG ${GIT_TAG}
        BUILD_IN_SOURCE true
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ./build-android.sh ${ANDROID_NDK} --arch=${ANDROID_ABI} --with-libraries=${BOOST_WITH_LIBRARIES} --boost=${BOOST_VER}
        INSTALL_COMMAND mkdir -p ${BOOST_INSTALL_DIR}
                COMMAND cp -rf build/out/${ANDROID_ABI}/include ${BOOST_INSTALL_DIR}
                COMMAND cp -rf build/out/${ANDROID_ABI}/lib ${BOOST_INSTALL_DIR}
        COMMAND patch ${BOOST_INSTALL_DIR}/lib/cmake/BoostDetectToolset-${BOOST_VER}.cmake < ${CMAKE_CURRENT_SOURCE_DIR}/BoostDetectToolset-android-clang.patch
        TEST_COMMAND "")
endif()


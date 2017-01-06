cmake_minimum_required(VERSION 3.6)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED on)

set(PROJECT stream)
set(CMAKE_SHARED_LIBRARY_PREFIX "")

project(${PROJECT})

option(test "Build tests." OFF)

include_directories(include)
include_directories(src)

add_definitions(-DSTREAM_BUILD_DLL)

file(GLOB HEADERS include/*.hpp)
file(GLOB DETAIL_HEADERS include/detail/*.hpp)
file(GLOB TRANSFORMATIONS_HEADERS include/transformations/*.hpp)
source_group("include" FILES ${HEADERS})
source_group("include\\detail" FILES ${DETAIL_HEADERS})
source_group("include\\transformations" FILES ${TRANSFORMATIONS_HEADERS})

file(GLOB SOURCES src/*.cpp src/*.hpp)
source_group("src" FILES ${SOURCES})

set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)

if (MSVC)
    add_definitions(/Wall /wd4710 /wd4820 /wd4514 /wd4571)
endif()

add_library(${PROJECT} SHARED ${SOURCES} ${HEADERS} ${DETAIL_HEADERS} ${TRANSFORMATIONS_HEADERS})

if (test)
    include(ExternalProject)

    set(GTEST_PROJECT gtest)
    set(TEST_PROJECT test_stream)

    set(gtest_INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/gtest_install)
    set(gtest_INCLUDE ${gtest_INSTALL_DIR}/include)
    set(gtest_LIB ${gtest_INSTALL_DIR}/lib)

    ExternalProject_Add(
        ${GTEST_PROJECT}
        GIT_REPOSITORY "https://github.com/google/googletest.git"
        GIT_TAG "release-1.8.0"
        CMAKE_ARGS -DBUILD_GTEST=ON -DBUILD_GMOCK=ON -Dgtest_force_shared_crt=ON -DCMAKE_INSTALL_PREFIX:PATH=${gtest_INSTALL_DIR}
    )

    enable_testing()
    include_directories(${gtest_INCLUDE})
    link_directories(${gtest_LIB})

    ##############
    # Unit Tests #
    ##############
    file(GLOB TEST_SOURCES test/*.cpp test/*.hpp)
    add_executable(${TEST_PROJECT} ${TEST_SOURCES})
    target_link_libraries(${TEST_PROJECT} gtest.lib gmock.lib gmock_main.lib)

    #target_link_libraries(${TEST_PROJECT} stream_lib)
endif()
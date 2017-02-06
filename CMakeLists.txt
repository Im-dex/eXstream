cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED on)

set(PROJECT eXstream)
project(${PROJECT})

option(test "Build tests." OFF)

file(GLOB HEADERS include/*.hpp)
file(GLOB DETAIL_HEADERS include/detail/*.hpp)
file(GLOB TRANSFORMATIONS_HEADERS include/transformations/*.hpp)
file(GLOB COLLECTORS_HEADERS include/collectors/*.hpp)

source_group("lib" FILES ${HEADERS})
source_group("lib\\detail" FILES ${DETAIL_HEADERS})
source_group("lib\\transformations" FILES ${TRANSFORMATIONS_HEADERS})
source_group("lib\\collectors" FILES ${COLLECTORS_HEADERS})

add_library(${PROJECT} INTERFACE)
target_sources(${PROJECT} INTERFACE ${HEADERS} ${DETAIL_HEADERS} ${TRANSFORMATIONS_HEADERS} ${COLLECTORS_HEADERS})
target_include_directories(${PROJECT} INTERFACE include/)

if (MSVC)
    target_compile_options(${PROJECT} INTERFACE /Wall /WX /wd4710 /wd4820 /wd4514 /wd4571 /wd4503 /wd4505)
endif()

if (test)
    include(ExternalProject)

    set(GTEST_PROJECT gtest)
    set(TEST_PROJECT eXstream-test)

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
    source_group("tests" FILES ${TEST_SOURCES})

    add_executable(${TEST_PROJECT} ${TEST_SOURCES})
    target_link_libraries(${TEST_PROJECT} gtest.lib gmock.lib gmock_main.lib)
    add_dependencies(${TEST_PROJECT} ${GTEST_PROJECT})
    target_link_libraries(${TEST_PROJECT} ${PROJECT})
endif()
cmake_minimum_required(VERSION 3.16)
project(mediumtree LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Match the README's -stdlib=libc++; only Clang accepts this flag.
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-stdlib=libc++)
    add_link_options(-stdlib=libc++)
endif()

# Game logic shared by the apps and the tests.
add_library(mediumtree STATIC src/c4.cpp src/ttt.cpp)
target_include_directories(mediumtree PUBLIC src)

add_executable(c4_app src/c4_app.cpp)
target_link_libraries(c4_app PRIVATE mediumtree)
add_executable(ttt_app src/ttt_app.cpp)
target_link_libraries(ttt_app PRIVATE mediumtree)

option(MEDIUMTREE_BUILD_TESTS "Build the GoogleTest unit tests" ON)
if(MEDIUMTREE_BUILD_TESTS)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.tar.gz
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    # Keep GoogleTest from overriding the MSVC runtime, skip GoogleMock,
    # and don't install either with this project.
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    enable_testing()
    include(GoogleTest)

    add_executable(unit_tests tests/ttt_test.cpp tests/c4_test.cpp tests/uct_test.cpp)
    # GoogleTest 1.17 requires C++17; the library and apps stay on C++11.
    target_compile_features(unit_tests PRIVATE cxx_std_17)
    target_link_libraries(unit_tests PRIVATE mediumtree GTest::gtest_main)
    gtest_discover_tests(unit_tests)
endif()

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

add_executable(c4_app src/c4_app.cpp src/c4.cpp)
add_executable(ttt_app src/ttt_app.cpp src/ttt.cpp)

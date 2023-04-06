cmake_minimum_required(VERSION 3.14)
project(videodemo)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CXX_FLAGS
        -g
        # -DVALGRIND
        -DCHECK_PTHREAD_RETURN_VALUE
        -D_FILE_OFFSET_BITS=64
        -Wall
        -Wextra
        -Werror
        -Wconversion
        -Wno-unused-parameter
        -Wold-style-cast
        -Woverloaded-virtual
        -Wpointer-arith
        -Wshadow
        -Wwrite-strings
        -march=native
        # -MMD
        -rdynamic 
        )

find_package(SDL2 REQUIRED)
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    )
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)
include_directories(. ./third_part/)
add_library(videodemo "")
target_sources(videodemo
    PRIVATE
    version.h
    version.cpp
    ./ffmpeg_util.h 
    ./ffmpeg_util.cpp
    )

target_link_libraries(videodemo     
    avdevice
    avfilter
    avformat
    avcodec
    swresample
    swscale
    avutil 
    ${SDL2_LIBRARIES}
    pthread 
    lzma 
    z
    )
add_executable(easy_video ./mains/easy_video.cpp)

target_link_libraries(
    easy_video 
    videodemo
    )


# google test
enable_testing()

add_executable(    unit_test
    unit_test/any_test.cc
    )
target_link_libraries(
    unit_test
    GTest::gtest_main
    videodemo
    )

include(GoogleTest)
gtest_discover_tests(unit_test)

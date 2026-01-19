CMAKE_MINIMUM_REQUIRED(VERSION 3.20.0)
PROJECT(TinySTL)

message("current build type is ${CMAKE_BUILD_TYPE}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(${PROJECT_SOURCE_DIR}/code)

add_subdirectory(code)

if (ENABLE_TinySTL_TEST)
    find_package(GTest REQUIRED)
    add_subdirectory("test")
endif()
cmake_minimum_required(VERSION 3.20.0)

project(Test)

# Use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

FILE(GLOB_RECURSE TEST_SRC *.cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../code)

add_executable(${PROJECT_NAME} ${TEST_SRC})

target_link_libraries(${PROJECT_NAME} GTest::gtest GTest::gtest_main STL)
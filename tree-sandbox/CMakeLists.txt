cmake_minimum_required(VERSION 3.1.0)

project(tree-sandbox)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5Core CONFIG REQUIRED)
find_package(Qt5Widgets CONFIG REQUIRED)

add_subdirectory(third-party/googletest)

enable_testing()
set(CTEST_OUTPUT_ON_FAILURE TRUE)

include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/third-party/googletest/googletest/include)
include_directories(${PROJECT_SOURCE_DIR}/third-party/googletest/googlemock/include)

set(CMAKE_C_FLAGS  ${CMAKE_C_FLAGS} "-O3 -std=gnu99 -Wall -Werror -Wunused -Wextra -Wpedantic -pedantic -Wshadow -pedantic-errors -fprofile-arcs -ftest-coverage")
set(CMAKE_CXX_FLAGS  ${CMAKE_CXX_FLAGS} "-O3 -std=c++11 -Wall -Werror -Wunused -Wextra -Wpedantic -pedantic -Wshadow -pedantic-errors -Wold-style-cast -fprofile-arcs -ftest-coverage")

add_library(ctree STATIC cnode.c ctree.c)

add_subdirectory(tests)
add_subdirectory(qt-render-tree)
add_subdirectory(qt-render-ctree)


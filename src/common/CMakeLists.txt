cmake_minimum_required (VERSION 3.2)
project(common C CXX)

SET (MAIN_SOURCES
    AutoScope.h
    AutoScope.cpp
    utility.cpp
    utility.h
    GameUtil.hpp
)

SET (SOURCES ${MAIN_SOURCES})

# define project's build target with project's source files
add_library(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC trading-base cpprestsdk)
cmake_minimum_required(VERSION 3.4.1)

get_filename_component(FFSCRIPT_ROOT_ABSOLUTE "${FFSCRIPT_ROOT}" ABSOLUTE)

set (FFSCRIPT_INCLUDE_DIR ${FFSCRIPT_ROOT_ABSOLUTE}/ffscript)
set (FFSCRIPT_LIB_X64_R_DIR ${FFSCRIPT_ROOT_ABSOLUTE}/x64/Release)
set (FFSCRIPT_LIB_X64_D_DIR ${FFSCRIPT_ROOT_ABSOLUTE}/x64/Debug)

message(STATUS "variables begin")
message(STATUS ${FFSCRIPT_INCLUDE_DIR})
message(STATUS ${FFSCRIPT_LIB_X64_R_DIR})
message(STATUS ${FFSCRIPT_LIB_X64_R_DIR})
message(STATUS "variables end")

configure_file("${CMAKE_SOURCE_DIR}/ffscriptConfig.cmake.in" "${CMAKE_SOURCE_DIR}/ffscriptConfig.cmake" @ONLY)
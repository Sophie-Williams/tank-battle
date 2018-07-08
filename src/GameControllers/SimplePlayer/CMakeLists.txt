cmake_minimum_required (VERSION 3.2)
project(SimplePlayer C CXX)

SET (MAIN_SOURCES
    SimplePlayer.cpp
    SimplePlayer.h
    ../GameControllers.h
)

SET (SOURCES ${MAIN_SOURCES})

# define project's build target with project's source files
add_library(${PROJECT_NAME} SHARED ${SOURCES})

add_definitions(-DEXPORT_GAME_CONTROLLER)

target_include_directories(${PROJECT_NAME} PRIVATE ${GameControllerInterfaces_SOURCE_DIR} )
target_link_libraries(${PROJECT_NAME} GameControllerInterfaces)

if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    SET (PLATFORM x64)
else( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    SET (PLATFORM x86)
endif( CMAKE_SIZEOF_VOID_P EQUAL 8 )

SET (INSTALL_DEST ${CMAKE_BINARY_DIR}/bin/${PLATFORM}/)
file(MAKE_DIRECTORY ${INSTALL_DEST})

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} ARGS -E make_directory "${INSTALL_DEST}$<$<CONFIG:debug>:Debug>$<$<CONFIG:release>:Release>")
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} ARGS -E make_directory "${INSTALL_DEST}$<$<CONFIG:debug>:Debug>$<$<CONFIG:release>:Release>/controllers")
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} ARGS -E copy $<TARGET_FILE:${PROJECT_NAME}> "${INSTALL_DEST}$<$<CONFIG:debug>:Debug>$<$<CONFIG:release>:Release>/controllers/")
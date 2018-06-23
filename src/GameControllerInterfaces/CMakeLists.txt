cmake_minimum_required (VERSION 3.2)
project(GameControllerInterfaces C CXX)

SET (MAIN_SOURCES
    PlayerContext.cpp
    PlayerContext.h
    TankCommandsBuilder.cpp
    TankCommandsBuilder.h
    TankController.cpp
    TankController.h
    GameControllerInterfaces.h
    GameInterface.h
    GameInterface.cpp
)

SET (SOURCES ${MAIN_SOURCES})

# define project's build target with project's source files
add_library(${PROJECT_NAME} SHARED ${SOURCES})

add_definitions(-DEXPORT_GAME_CONTROLLER_INTERFACES)

if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    SET (PLATFORM x64)
else( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    SET (PLATFORM x86)
endif( CMAKE_SIZEOF_VOID_P EQUAL 8 )

SET (INSTALL_DEST ${CMAKE_BINARY_DIR}/bin/${PLATFORM}/)
install(FILES $<TARGET_FILE:${PROJECT_NAME}> DESTINATION ${INSTALL_DEST}/debug CONFIGURATIONS Debug)
install(FILES $<TARGET_FILE:${PROJECT_NAME}> DESTINATION ${INSTALL_DEST}/release CONFIGURATIONS Release)
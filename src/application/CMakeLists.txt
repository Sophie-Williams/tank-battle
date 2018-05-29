cmake_minimum_required (VERSION 3.2)
project(TankBattle C CXX)

SET (COMMON_SRC
    ../common/Utility.h
    ../common/Utility.cpp
    ../common/AutoScope.cpp
    ../common/AutoScope.h
    ../common/SyncMessageQueue.hpp
)

SET (ENGINE_SRC
    engine/GameEngine.h
    engine/GameEngine.cpp
    engine/GameObject.h
    engine/GameObject.cpp
    engine/Scene.h
    engine/Scene.cpp
    engine/Tank.h
    engine/Tank.cpp
    engine/TankBarrel.h
    engine/TankBarrel.cpp
    engine/TankBody.h
    engine/TankBody.cpp
    engine/TexturedObject.h
    engine/TexturedObject.cpp
    engine/PlayerController.h
    engine/PlayerController.cpp
    engine/Bullet.h
    engine/Bullet.cpp
    engine/CollisionDetector.h
    engine/CollisionDetector.cpp
    engine/GameController.h
    engine/GameController.cpp
    engine/LiveObject.h
    engine/LiveObject.cpp
    engine/Geometry.h
    engine/Barrier.h
    engine/Barrier.cpp
    engine/Animation.h
    engine/Animation.cpp
    engine/DrawableObject.h
    engine/DrawableObject.cpp
    engine/LifeTimeControlComponent.h
    engine/LifeTimeControlComponent.cpp
    engine/GameComponent.h
    engine/GameComponent.cpp
    engine/GameResource.h
    engine/GameResource.cpp
)

SET (UI_SRC
    UI/CiWidget.cpp
    UI/CiWidget.h
    UI/ImWidget.cpp
    UI/ImWidget.h
    UI/Spliter.cpp
    UI/Spliter.h
    UI/Widget.cpp
    UI/Widget.h
    UI/WxAppLog.cpp
    UI/WxAppLog.h
    UI/WxControlBoard.cpp
    UI/WxControlBoard.h
    UI/Panel.h
    UI/Panel.cpp
    UI/ImPopup.h
    UI/ImPopup.cpp
    UI/WxPlayerInfo.h
    UI/WxPlayerInfo.cpp
    UI/WxGameView.h
    UI/WxGameView.cpp
)

SET (CONTROLLERS_SRC
    Controllers/PlayerControllerUI.h
    Controllers/PlayerControllerUI.cpp
    Controllers/PlayerControllerTest.h
    Controllers/PlayerControllerTest.cpp
)

SET (BATTLE_SRC
    Battle/BattlePlatform.cpp
    Battle/BattlePlatform.h
    Battle/GameInterface.cpp
    Battle/GameInterface.h
    Battle/Radar.cpp
    Battle/Radar.h
    Battle/TankController.cpp
    Battle/TankController.h
)

SET (MAIN_SOURCES
    TankBattleApp.cpp
    TankBattle.h
    LogAdapter.h
    LogAdapter.cpp
    Resources/Resources.h
    Resources/Resources.rc
    ${UI_SRC}
    ${ENGINE_SRC}
    ${COMMON_SRC}
    ${CONTROLLERS_SRC}
    ${BATTLE_SRC}
)

SET (SOURCES ${MAIN_SOURCES})

if(MSVC)
    source_group(UI FILES ${UI_SRC})
    source_group(Common FILES ${COMMON_SRC})
    source_group(Engine FILES ${ENGINE_SRC})
    source_group(Controllers FILES ${CONTROLLERS_SRC})
    source_group(Battle FILES ${BATTLE_SRC})
endif()

# define project's build target with project's source files
add_executable(${PROJECT_NAME} WIN32  ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

include (${CMAKE_CURRENT_SOURCE_DIR}/../cmake/cinder.cmake)
include (${CMAKE_CURRENT_SOURCE_DIR}/../cmake/cinder-imgui.cmake)

if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    SET (PLATFORM x64)
else( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    SET (PLATFORM x86)
endif( CMAKE_SIZEOF_VOID_P EQUAL 8 )

if(WIN32)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} ARGS -E copy "$<$<CONFIG:debug>:${CINDER_BIN_D}>$<$<CONFIG:release>:${CINDER_BIN_R}>" ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIGURATION>/)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} ARGS -E copy "$<$<CONFIG:debug>:${CINDER_IMGUI_BIN_D}>$<$<CONFIG:release>:${CINDER_IMGUI_BIN_R}>" ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIGURATION>/)
endif()

SET (INSTALL_DEST ${CMAKE_BINARY_DIR}/bin/${PLATFORM}/)
install(FILES $<TARGET_FILE:${PROJECT_NAME}> DESTINATION ${INSTALL_DEST}/debug CONFIGURATIONS Debug)
install(FILES $<TARGET_FILE:${PROJECT_NAME}> DESTINATION ${INSTALL_DEST}/release CONFIGURATIONS Release)

if(WIN32)
install(FILES ${CINDER_BIN_D} DESTINATION ${INSTALL_DEST}/debug CONFIGURATIONS Debug)
install(FILES ${CINDER_BIN_R} DESTINATION ${INSTALL_DEST}/release CONFIGURATIONS Release)
install(FILES ${CINDER_IMGUI_BIN_D} DESTINATION ${INSTALL_DEST}/debug CONFIGURATIONS Debug)
install(FILES ${CINDER_IMGUI_BIN_R} DESTINATION ${INSTALL_DEST}/release CONFIGURATIONS Release)
endif()
cmake_minimum_required (VERSION 3.2)
project(TankBattle C CXX)

SET (COMMON_SRC
    ../common/Utility.h
    ../common/Utility.cpp
    ../common/AutoScope.cpp
    ../common/AutoScope.h
    ../common/SyncMessageQueue.hpp
    ../common/HandlerCollection.hpp
    ../common/ILogger.cpp
    ../common/SpinArray.hpp
)

SET (ENGINE_SRC
    engine/GameEngine.h
    engine/GameEngine.cpp
    engine/GameObject.h
    engine/GameObject.cpp
    engine/Scene.h
    engine/Scene.cpp
    engine/TexturedObject.h
    engine/TexturedObject.cpp
    engine/CollisionDetector.h
    engine/CollisionDetector.cpp
    engine/LiveObject.h
    engine/LiveObject.cpp
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
    engine/UIThreadRunner.h
    engine/UIThreadRunner.cpp
    engine/Timer.h
    engine/Timer.cpp
)

SET (ENGINE_SPECIFIC_SRC
    EngineSpecific/GameStateManager.h
    EngineSpecific/GameStateManager.cpp
    EngineSpecific/Tank.h
    EngineSpecific/Tank.cpp
    EngineSpecific/TankBarrel.h
    EngineSpecific/TankBarrel.cpp
    EngineSpecific/TankBody.h
    EngineSpecific/TankBody.cpp
    EngineSpecific/Barrier.h
    EngineSpecific/Barrier.cpp
    EngineSpecific/GameController.h
    EngineSpecific/GameController.cpp
    EngineSpecific/Bullet.h
    EngineSpecific/Bullet.cpp
    EngineSpecific/PlayerController.h
    EngineSpecific/PlayerController.cpp
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
    UI/WxGameView.h
    UI/WxGameView.cpp
    UI/WxRadarView.h
    UI/WxRadarView.cpp
    UI/WxTankPeripheralsView.h
    UI/WxTankPeripheralsView.cpp
    UI/WxGameStatistics.h
    UI/WxGameStatistics.cpp
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
    Battle/Radar.cpp
    Battle/Radar.h
    Battle/GameCapture.h
    Battle/GameCapture.cpp
    Battle/ObjectViewContainer.cpp
    Battle/ObjectViewContainer.h
    Battle/TankCamera.cpp
    Battle/TankCamera.h
    Battle/TankControllerWorker.h
    Battle/TankControllerWorker.cpp
    Battle/GameInterfaceImpl.cpp
    Battle/GameInterfaceImpl.h
    Battle/PlayerContextImpl.cpp
    Battle/PlayerContextImpl.h
    Battle/TankControllerModuleWrapper.cpp
    Battle/TankControllerModuleWrapper.h
)

SET (MAIN_SOURCES
    TankBattleApp.cpp
    TankBattle.h
    LogAdapter.h
    LogAdapter.cpp
    ${UI_SRC}
    ${ENGINE_SRC}
    ${COMMON_SRC}
    ${CONTROLLERS_SRC}
    ${BATTLE_SRC}
    ${ENGINE_SPECIFIC_SRC}
)

if(MSVC)
    SET (MAIN_SOURCES
        Resources/Resources.h
        Resources/Resources.rc
        ${MAIN_SOURCES}
    )
    source_group(UI FILES ${UI_SRC})
    source_group(Common FILES ${COMMON_SRC})
    source_group(Engine FILES ${ENGINE_SRC})
    source_group(EngineSpecific FILES ${ENGINE_SPECIFIC_SRC})
    source_group(Controllers FILES ${CONTROLLERS_SRC})
    source_group(Battle FILES ${BATTLE_SRC})
	
	# Override the default /MD with /MT
	foreach( 
		flag_var
		CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO 
		CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO 
	)
		if( ${flag_var} MATCHES "/MD" )
			string( REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}" )
		endif()
	endforeach()
endif()

SET (SOURCES ${MAIN_SOURCES})

# define project's build target with project's source files

if (WIN32)
	add_executable(${PROJECT_NAME} WIN32  ${SOURCES})
else ()
	add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${SOURCES})
endif ()


target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} PRIVATE ${GameControllerInterfaces_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../external/spdlog/include)
target_link_libraries(${PROJECT_NAME} GameControllerInterfaces ScriptedPlayer)

include (${CMAKE_CURRENT_SOURCE_DIR}/../cmake/cinder.cmake)
if (WIN32)
    include (${CMAKE_CURRENT_SOURCE_DIR}/../cmake/cinder-imgui.cmake)
else ()
    include (${CMAKE_CURRENT_SOURCE_DIR}/../cmake/cinder-imgui-unix.cmake)
endif ()    

if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    SET (PLATFORM x64)
else( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    SET (PLATFORM x86)
endif( CMAKE_SIZEOF_VOID_P EQUAL 8 )

SET (INSTALL_DEST ${CMAKE_BINARY_DIR}/bin/${PLATFORM}/)

file(MAKE_DIRECTORY ${INSTALL_DEST})

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} ARGS -E make_directory "${INSTALL_DEST}$<$<CONFIG:debug>:Debug>$<$<CONFIG:release>:Release>")
#if(WIN32)
#add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} ARGS -E copy "$<$<CONFIG:debug>:${CINDER_BIN_D}>$<$<CONFIG:release>:${CINDER_BIN_R}>" "${INSTALL_DEST}$<$<CONFIG:debug>:Debug>$<$<CONFIG:release>:Release>/")
#add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} ARGS -E copy "$<$<CONFIG:debug>:${CINDER_IMGUI_BIN_D}>$<$<CONFIG:release>:${CINDER_IMGUI_BIN_R}>" "${INSTALL_DEST}$<$<CONFIG:debug>:Debug>$<$<CONFIG:release>:Release>/")
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} ARGS -E copy $<TARGET_FILE:GameControllerInterfaces> "${INSTALL_DEST}$<$<CONFIG:debug>:Debug>$<$<CONFIG:release>:Release>/")
#endif()
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} ARGS -E copy $<TARGET_FILE:${PROJECT_NAME}> "${INSTALL_DEST}$<$<CONFIG:debug>:Debug>$<$<CONFIG:release>:Release>/")
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} ARGS -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/assets "${INSTALL_DEST}$<$<CONFIG:debug>:Debug>$<$<CONFIG:release>:Release>/assets")
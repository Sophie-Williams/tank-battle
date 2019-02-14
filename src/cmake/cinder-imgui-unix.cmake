cmake_minimum_required(VERSION 3.2.0)

target_include_directories(${PROJECT_NAME} PRIVATE /Users/apple/Documents/projects/tank-battle/src/external/Cinder/blocks/Cinder-ImGui/include)
target_include_directories(${PROJECT_NAME} PRIVATE /Users/apple/Documents/projects/tank-battle/src/external/Cinder/blocks/Cinder-ImGui/lib/imgui)

find_library(CINDER_IMGUI_TARGET Cinder-ImGui /Users/apple/Documents/projects/tank-battle/src/external/Cinder/blocks/Cinder-ImGui/build)
target_link_libraries(${PROJECT_NAME} ${CINDER_IMGUI_TARGET})

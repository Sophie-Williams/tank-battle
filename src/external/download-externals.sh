#!/bin/sh

CURR_DIR=$PWD
THIS_SCRIPT_PATH="$( cd "$(dirname "$0")" ; pwd -P )"
cd $THIS_SCRIPT_PATH

#download spdlog
git clone -b v1.x https://github.com/gabime/spdlog.git

#download Cinder at version v0.9.1
git clone --recursive https://github.com/cinder/Cinder.git
cd Cinder
git checkout tags/v0.9.1
cd ..

echo Download Cinder at version v0.9.1 completed!
#copy some patch files to fix compiling issue at version v0.9.1 of Cinder for Mac
yes | cp -rf $THIS_SCRIPT_PATH/patch/gcc/Cinder/* $THIS_SCRIPT_PATH/Cinder/

# download Cinder-ImGui at revision ccbf3a8
cd $THIS_SCRIPT_PATH/Cinder/blocks
git clone https://github.com/simongeilfus/Cinder-ImGui.git
cd Cinder-ImGui
git checkout ccbf3a8 .
cd ..
#copy some patch files of Cinder-ImGui to build with v1.65 of imgui
yes | cp -rf $THIS_SCRIPT_PATH/patch/global/Cinder-ImGui/* $THIS_SCRIPT_PATH/Cinder/blocks/Cinder-ImGui/

#download imgui at version v1.65
git clone https://github.com/ocornut/imgui.git
cd imgui
git checkout tags/v1.65
cd ..

#copy imgui source to Cinder-ImGui
yes | cp -rf ./imgui/* ./Cinder-ImGui/lib/imgui

echo Download Cinder block completed!

cd $CURR_DIR
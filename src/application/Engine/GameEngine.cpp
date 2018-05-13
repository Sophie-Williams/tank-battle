#ifdef WIN32
#include <filesystem>
using namespace std::experimental;
#else
#include <filesystem>
using namespace std;
#endif

#include "GameEngine.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <algorithm>
#include <string>

#ifdef min
#undef min
#endif

using namespace std::chrono;
using namespace std;

GameEngine::GameEngine(const char* configFile) : _runFlag(false) {
	
}

GameEngine::~GameEngine() {
}

void GameEngine::run() {
}

void GameEngine::stop() {
}

void GameEngine::doUpdate() {

}

void GameEngine::doDraw() {

}
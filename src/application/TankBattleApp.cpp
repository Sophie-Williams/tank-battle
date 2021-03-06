/******************************************************************
* File:        TankBattleApp.cpp
* Description: declare and implement TankBattleApp class. A class
*              is resonsible to connect all of application component
*              such as UI, game engine,... and make them work togheter.
*              game UI.
* Author:      Vincent Pham
*
* Copyright (c) 2018 VincentPT.
** Distributed under the MIT License (http://opensource.org/licenses/MIT)
**
*
**********************************************************************/

#ifdef WIN32
#include <Windows.h>
#include <experimental/filesystem> // C++-standard header file name  
#include <filesystem>
using namespace std::experimental::filesystem::v1;
#define PATH_TO_STRING(p) p.u8string()
#elif __APPLE__
#define PATH_TO_STRING(p) p.string()
#else
#include <unistd.h>
#include <filesystem>
using namespace std;
using fs = std::filesystem;
#define PATH_TO_STRING(p) p.u8string()
#endif

#include <exception>

#include <stdio.h>
//#include <io.h>
#include <fcntl.h>

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

#include "UI/WxAppLog.h"
#include "UI/Spliter.h"
#include "UI/WxControlBoard.h"
#include "UI/Panel.h"
#include "Engine/GameEngine.h"
#include "LogAdapter.h"
#include "../common/Utility.h"
#include "UI/CiWndCandle.h"
#include "UI/WxGameView.h"
#include "EngineSpecific/Tank.h"
#include "EngineSpecific/Barrier.h"
#include "Controllers/PlayerControllerUI.h"
#include "Engine/GameResource.h"
#include "Controllers/PlayerControllerTest.h"
#include "battle/BattlePlatform.h"
#include "UI/WxTankPeripheralsView.h"
#include "UI/WxGameStatistics.h"
#include "battle/GameCapture.h"
#include "battle/TankControllerWorker.h"
#include "battle/TankControllerModuleWrapper.h"
#include "Engine/Timer.h"
#include "EngineSpecific/GameStateManager.h"
#include "EngineSpecific/GameController.h"
#include "Engine/UIThreadRunner.h"
#include "../GameControllers/ScriptedPlayer/ScriptedPlayer.h"

using namespace ci;
using namespace ci::app;
using namespace std;

#define SCRIPT_EXT ".c955"

const char* startStopButtonTexts[] = {"Start", "Starting...", "Stop", "Stopping..."};
enum StartState : int {
	NOT_STARTED = 0,
	STARTING,
	STARTED,
	STOPING,
};

LogAdapter _logAdapter;


#define GAME_PREPARING_TIME 5.0f

class BasicApp : public App {

	typedef std::function<void()> Task;

	Spliter _spliter;
	Widget* _topCotrol;
	shared_ptr<WxAppLog> _applog;
	shared_ptr<WxGameView> _gameView;
	shared_ptr<WxControlBoard> _controlBoard;
	shared_ptr<WxGameStatistics> _gameStatistics;
	SyncMessageQueue<Task> _tasks;
	
	shared_ptr<GameEngine> _gameEngine;
	shared_ptr<GameResource> _gameResource;
	shared_ptr<BattlePlatform> _battlePlatform;
	vector<shared_ptr<TankControllerWorker>> _tankControllerWorkers;
	
	bool _runFlag;
	int _startStopButtonState = 0;
	shared_ptr<SignalAny> _controllerReadySignal;
	string _workingDir;

	shared_ptr<WxTankPeripheralsView> _peripheralsview1;
	shared_ptr<WxTankPeripheralsView> _peripheralsview2;
	shared_ptr<GameObject> _applicationComponentContainer;
	shared_ptr<GameStateManager> _gameStateManager;

	int _selectedPlayer1;
	int _selectedPlayer2;
	int _selectedRoundCount = -1;
	float _roundCountDownAt = -1;
	UIThreadRunner _uiThreadRunner;
	shared_ptr<Tank> _userTank;
	vector<bool> _playerFlags;

public:
	BasicApp();
	~BasicApp();
	void setup() override;
	void update() override;
	void draw() override;
	void cleanup() override;
	void startStopButtonClick();
	void setupGame();
	void loadPlayers();
	void generateGame();
	void startStopBots(bool start);

	void startRound();
	void stopRound();

	void addLog(LogLevel logLevel, const char* fmt, va_list args) {
		if (_applog) {
			_applog->addLogV( (WxAppLog::LogLevel) logLevel, fmt, args);
		}
	}

	static void intializeApp(App::Settings* settings);
	void onWindowSizeChanged();
	void setSSButtonState(int state);

	void applyController(shared_ptr<Tank> tank, int, std::shared_ptr<WxTankPeripheralsView> peripheralsview, int playerId);
};

void pushLog(int logLevel, const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	((BasicApp*)app::App::get())->addLog((LogLevel)logLevel, fmt, args);
	va_end(args);
}

void BasicApp::intializeApp(App::Settings* settings) {
	auto size = Display::getMainDisplay()->getSize();
	size.x -= 100;
	size.y -= 100;
	settings->setWindowSize(size);
}

BasicApp::BasicApp() :
	_runFlag(true) {
};

BasicApp::~BasicApp() {

	for (auto it = _tankControllerWorkers.begin(); it != _tankControllerWorkers.end(); it++) {
		auto& worker = *it;
		worker->stopAndWait(100);
	}

	_runFlag = false;

	_gameEngine->stop();
}

void BasicApp::cleanup() {
}

void BasicApp::onWindowSizeChanged() {
	// update root control size
	int w = getWindow()->getWidth();
	int h = getWindow()->getHeight();
	_topCotrol->setSize((float)w, (float)h);
	_topCotrol->setPos(0, 0);
	if (_topCotrol == _applog.get()) {
		return;
	}
}

std::string getExecutableAbsolutePath() {
	char buff[256];
#ifdef WIN32
	GetModuleFileNameA(nullptr, buff, sizeof(buff));
#else
	auto res = readlink("/proc/self/exe", buff, sizeof(buff));
#endif // WIN32
	return buff;
}

void BasicApp::setup()
{
	FUNCTON_LOG();

	fs::path path = fs::current_path();
	path.append("assets");
	if (fs::exists(path) && fs::is_directory(path)) {
		_workingDir = PATH_TO_STRING(path.parent_path());
	}
	else {
		path = fs::path(getExecutableAbsolutePath());
		path = path.parent_path();
		path.append("assets");
		if (fs::exists(path) && fs::is_directory(path)) {
			_workingDir = PATH_TO_STRING(path.parent_path());
		}
		else {
			quit();
			return;
		}
	}

	using namespace std::placeholders;
	ui::Options uiOptions;
	ui::initialize(uiOptions);
	setFrameRate(60);

	getWindow()->setTitle("Tank Battle");
	getWindow()->getSignalResize().connect(std::bind(&BasicApp::onWindowSizeChanged, this));

	int w = getWindow()->getWidth();
	int h = getWindow()->getHeight();

	// prepare controls and arrange layout
	_spliter.setSize((float)w, (float)h);
	_spliter.setPos(0, 0);
	_topCotrol = &_spliter;

	_applog = std::make_shared<WxAppLog>();
	
	auto bottomSpliter = std::make_shared<Spliter>();
	auto bottomLeftSpilter = std::make_shared<Spliter>();

	_controlBoard = std::make_shared<WxControlBoard>();
	_gameStatistics = make_shared<WxGameStatistics>();
	_logAdapter.setApplog(_applog);

	_gameView = std::make_shared<WxGameView>(getWindow());

	// arrange player information windows
	bottomLeftSpilter->setVertical(true);
	bottomLeftSpilter->setLayoutFixedSize(400);
	bottomLeftSpilter->setLayoutType(Spliter::LayoutType::Panel1Fixed);
	bottomLeftSpilter->setChild1(_controlBoard);
	bottomLeftSpilter->setChild2(_gameStatistics);

	bottomSpliter->setVertical(true);
	bottomSpliter->setLayoutFixedSize(700);
	bottomSpliter->setLayoutType(Spliter::LayoutType::Panel1Fixed);
	bottomSpliter->setChild1(bottomLeftSpilter);
	bottomSpliter->setChild2(_applog);

	_spliter.setLayoutFixedSize(300);
	_spliter.setLayoutType(Spliter::LayoutType::Panel2Fixed);
	_spliter.setChild1(_gameView);
	_spliter.setChild2(bottomSpliter);

	// bind events to handlers
	getWindow()->getSignalClose().connect([this]() {
		_runFlag = false;
	});

	getWindow()->getSignalKeyDown().connect([](ci::app::KeyEvent& e) {
		if (e.getCode() == app::KeyEvent::KEY_p) {
			auto gameEngine = GameEngine::getInstance();
			if (gameEngine->isPausing()) {
				gameEngine->resume();
			}
			else {
				gameEngine->pause();
			}
		}
	});

	_controlBoard->setOnStartStopButtonClickHandler([this](Widget*) {
		startStopButtonClick();
	});

	_controlBoard->setOnGenerateClickHandler([this](Widget*) {
		if (_startStopButtonState == StartState::NOT_STARTED) {
			GameEngine::getInstance()->accessEngineResource([this]() {
				generateGame();
			});
		}
	});

	auto playerChangedHandeler1 = [this](Widget*) {
		if (StartState::NOT_STARTED == _startStopButtonState) {
			_selectedPlayer1 = _controlBoard->getPlayer1();
			_selectedPlayer2 = _controlBoard->getPlayer2();

			GameEngine::getInstance()->accessEngineResource([this]() {
				generateGame();
			});
		}
	};

	auto playerChangedHandeler2 = playerChangedHandeler1;

	_controlBoard->setOnPlayer1ChangedHandler(playerChangedHandeler1);
	_controlBoard->setOnPlayer2ChangedHandler(playerChangedHandeler2);
	
	// set the first state of application
	setSSButtonState(StartState::NOT_STARTED);
	_controlBoard->setPauseResumeButtonText("Pause");

	setupGame();
}

std::shared_ptr<std::vector<std::shared_ptr<Tank>>> generateTanks(Scene* scene, int customTanks, int expectedAutoTank, float tankHealthCapacity) {
	float padding = 8;

	auto& sceneArea = scene->getSceneArea();
	auto sceneHeight = sceneArea.getHeight() - padding * 2;
	auto sceneWidth = sceneArea.getWidth() - padding * 2;

	Colorf tankColors[] = {
		{ 1,0,0 },
		{ 0,0,1 },
		
		{ 1,1,1 },
		{ 1,1,0 },
	};

	constexpr int tankColorCount = sizeof(tankColors) / sizeof(tankColors[0]);
	
	vec2 tankSize(5.7f, 6.6f);
	int colMax = (int)(sceneWidth / tankSize.x / 2);
	int rowMax = (int)(sceneHeight / tankSize.y / 2);
	int tankMaxCount = colMax * rowMax;

	int expectedTank = expectedAutoTank + customTanks;
	int tankCount = std::min(expectedTank, tankMaxCount);
	if (tankCount <= 0) {
		return nullptr;
	}

	Rand randomizer( (uint32_t) (GameEngine::getInstance()->getCurrentTime() * 10000) );

	float rowW = sceneHeight / colMax;
	float colW = sceneWidth / rowMax;

	auto baseCoordinateOfScene = sceneArea.getUpperLeft();

	std::map<int, bool> generatedPos;
	auto tanks = std::make_shared<std::vector<std::shared_ptr<Tank>>>(tankCount);

	for (int i = 0; i < tankCount; i++) {
		auto iPos = randomizer.nextInt(0, tankMaxCount);
		while (generatedPos.find(iPos) != generatedPos.end())
		{
			iPos = randomizer.nextInt(0, tankMaxCount);
		}
		generatedPos[iPos] = true;
		ci::vec3 tankPos((iPos % colMax) * colW + colW/2 + baseCoordinateOfScene.x + padding, (iPos / rowMax) * rowW + rowW/2 + baseCoordinateOfScene.y + padding, 0);
		auto tank = make_shared<Tank>();
		tank->setHealth(tankHealthCapacity);
		tank->setSize(tankSize);
		tank->translate(tankPos);
		tank->rotate(glm::half_pi<float>() * randomizer.nextInt(0, 4));

		if (i >= tankColorCount) {
			tank->setColor(tankColors[tankColorCount - 1]);
		}
		else {
			tank->setColor(tankColors[i]);
		}

		scene->addDrawbleObject(tank);
		tanks->at(i) = tank;
	}

	for (int i = customTanks; i < tankCount; i++) {
		auto playerController = make_shared<PlayerControllerTest>();
		tanks->at(i)->addComponent(playerController);
	}

	return tanks;
}

void BasicApp::applyController(shared_ptr<Tank> tankRef, int playerIdx,
	std::shared_ptr<WxTankPeripheralsView> peripheralsview, int playerId) {
	auto& gameArea = _battlePlatform->getMapArea();
	auto objectViewContainer = make_shared<ObjectViewContainer>(tankRef);

	auto radar = make_shared<Radar>(objectViewContainer, glm::pi<float>());
	radar->setRange(sqrt(gameArea.getWidth()*gameArea.getWidth() + gameArea.getHeight()*gameArea.getHeight()));

	auto camera = make_shared<TankCamera>(objectViewContainer, glm::pi<float>()*2.0f / 3);
	camera->setRange(radar->getRange());

	tankRef->addComponent(objectViewContainer);
	tankRef->addComponent(radar);
	tankRef->addComponent(camera);

	try {
		peripheralsview->setupPeripherals(tankRef);
	}
	catch (...) {
		quit();
	}
	shared_ptr<TankController> tankController;
	auto& playerNameConst = _controlBoard->getPlayerName(playerIdx);
	auto playerIdStr = playerNameConst + "(";
	playerIdStr.append(std::to_string(playerId));
	playerIdStr.append(")");

	if (_playerFlags[playerIdx] == false) {
		tankController = make_shared<TankControllerModuleWrapper>(playerNameConst.c_str());
		tankController->setName(playerIdStr.c_str());
	}
	else {
		auto tankControllerScript = make_shared<ScriptedPlayer>();
		tankControllerScript->setName(playerIdStr.c_str());

		string scriptFile("./controllers/");
		scriptFile += (playerNameConst + SCRIPT_EXT);
		auto message = tankControllerScript->setProgramScript(scriptFile.c_str());
		if (message) {
			ILogger::getInstance()->logV(LogLevel::Error, "compile program failed with error: %s\n", message);
		}

		tankController = tankControllerScript;
	}
	auto worker = make_shared<TankControllerWorker>(tankRef, tankController);

	std::string playerName = playerNameConst;
	playerName.append("(");
	playerName.append(std::to_string((int)_tankControllerWorkers.size()));
	playerName.append(")");
	worker->setName(playerName);

	_tankControllerWorkers.push_back(worker);
	worker->setSignalWaiter(_controllerReadySignal.get());
}

void BasicApp::loadPlayers() {
	vector<string> players;

	_playerFlags.clear();

	fs::path controllersPath(_workingDir);
	controllersPath.append("controllers");
	if (fs::exists(controllersPath) && fs::is_directory(controllersPath)) {
		for (auto & p : fs::directory_iterator(controllersPath)) {
			auto file = p.path().filename();
			auto ext = file.extension().string();
			std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
#ifdef WIN32
			if (ext == ".dll" || ext == SCRIPT_EXT)
#elif __APPLE__
            if (ext == ".dylib" || ext == SCRIPT_EXT)
#else
			if (ext == ".so" || ext == SCRIPT_EXT)
#endif

			{
				auto fileWithoutExt = file.string();
				fileWithoutExt = fileWithoutExt.substr(0, fileWithoutExt.size() - ext.size());
				players.push_back(fileWithoutExt);

				// set player flag to true if the file is a script file
				_playerFlags.push_back(ext == SCRIPT_EXT);
			}
		}
#ifdef WIN32
		BOOL res = SetDllDirectoryA(controllersPath.string().c_str());
#else
        char expression[256];
        sprintf(expression, "DYLD_LIBRARY_PATH=%s",controllersPath.string().c_str());
        putenv(expression);
		//throw std::runtime_error("dynamic library search path is not implemented");
#endif
	}

	_controlBoard->setPlayers(players);
}

void BasicApp::generateGame() {
	auto& gameScene = _gameEngine->getScene();
	_tankControllerWorkers.clear();

	// remove old tanks
	auto& physicalObjects = gameScene->getDrawableObjects();
	for (auto it = physicalObjects.begin(); it != physicalObjects.end();) {
		if (dynamic_cast<Tank*>(it->get())) {
			auto itTemp = it;
			it++;
			physicalObjects.erase(itTemp);
		}
		else {
			it++;
		}
	}

	auto tanks = generateTanks(gameScene.get(), 2, _controlBoard->getNumberOfBot(), (float)_controlBoard->getTankHeathCapacity());	

	_controllerReadySignal = make_shared<SignalAny>(true);

	applyController(tanks->at(0), _selectedPlayer1, _peripheralsview1, 0);
	applyController(tanks->at(1), _selectedPlayer2, _peripheralsview2, 1);

	//_userTank = tanks->at(2);
	//_userTank->setHealth(50);
}

void BasicApp::setupGame() {
	fs::path assetsPath(_workingDir);
	assetsPath.append("assets");
	addAssetDirectory(PATH_TO_STRING(assetsPath));

	_peripheralsview1 = make_shared<WxTankPeripheralsView>(getWindow());
	_peripheralsview2 = make_shared<WxTankPeripheralsView>(getWindow());

	_gameView->setTankView(_peripheralsview1);
	_gameView->setTankView(_peripheralsview2);

	_gameResource = std::shared_ptr<GameResource>(GameResource::createInstance());
	_gameResource->setTexture(TEX_ID_BULLET, "bulletBlue1_outline.png");
	_gameResource->setTexture(TEX_ID_EXPLOSION, "explosion.png");
	_gameResource->setTexture(TEX_ID_TANKBODY, "tankBody.png");
	_gameResource->setTexture(TEX_ID_TANKBARREL, "tankBarrel.png");
	_gameResource->setTexture(TEX_ID_TANKSHOT, "shotLarge.png");
	_gameResource->setTexture(TEX_ID_BACKGROUND, "background.jpg");

	_gameEngine = std::shared_ptr<GameEngine>(GameEngine::createInstance());

	const float sceneWidth = 70;
	const float sceneHeight = 70;

	_battlePlatform = make_shared<BattlePlatform>(sceneWidth, sceneHeight);
	auto gameCapture = make_shared<GameCapture>();

	auto& gameArea = _battlePlatform->getMapArea();
	auto gameScene = std::shared_ptr<Scene>(Scene::createScene(gameArea));
	gameScene->setBackgroundColor(ci::ColorA8u::gray(69, 255));

	_applicationComponentContainer = make_shared<GameObject>();
	_gameStateManager = make_shared<GameStateManager>();
	_applicationComponentContainer->addComponent(_gameStateManager);
	_applicationComponentContainer->addComponent(gameCapture);

	_gameEngine->addGameObject(_applicationComponentContainer);

	auto barrier1 = std::make_shared<Barrier>();
	auto barrier2 = std::make_shared<Barrier>();
	auto barrier3 = std::make_shared<Barrier>();
	auto barrier4 = std::make_shared<Barrier>();

	float wallDepth1 = 6;
	float wallDepth2 = 5;

	barrier1->setBound(Rectf(gameArea.x1, gameArea.y1, gameArea.x2, gameArea.y1 + wallDepth1));
	barrier2->setBound(Rectf(gameArea.x2 - wallDepth2, gameArea.y1 + wallDepth1, gameArea.x2, gameArea.y2 - wallDepth1));
	barrier3->setBound(Rectf(gameArea.x1, gameArea.y2 - wallDepth1, gameArea.x2, gameArea.y2));
	barrier4->setBound(Rectf(gameArea.x1, gameArea.y1 + wallDepth1, gameArea.x1 + wallDepth2, gameArea.y2 - wallDepth1));

	gameScene->addDrawbleObject(barrier1);
	gameScene->addDrawbleObject(barrier2);
	gameScene->addDrawbleObject(barrier3);
	gameScene->addDrawbleObject(barrier4);

	_gameEngine->setScene(gameScene);
	_gameView->setScene(gameScene);
	_gameView->setSceneViewRatio(gameArea.getAspectRatio());

	loadPlayers();	
	_controllerReadySignal = make_shared<SignalAny>(true);

	_selectedPlayer1 = _controlBoard->getPlayer1();
	_selectedPlayer2 = _controlBoard->getPlayer2();

	generateGame();
	_gameEngine->run();
}

void BasicApp::startStopBots(bool start) {
	auto& secne = _gameEngine->getScene();

	auto& objects = secne->getDrawableObjects();
	for (auto it = objects.begin(); it != objects.end(); it++) {
		auto tank = dynamic_cast<Tank*>(it->get());
		if (tank) {
			auto& components = tank->getComponents();
			for (auto jt = components.begin(); jt != components.end(); jt++) {
				auto controller = dynamic_cast<PlayerControllerTest*>(jt->get());
				if (controller) {
					if (start) {
						controller->startControl();
					}
					else {
						controller->stopControl();
					}
				}
			}
		}
	}
}

void BasicApp::startRound() {
	_gameEngine->postTask([this](float t) {
		_gameStateManager->initState();
		//_uiThreadRunner.postTask([this](float t) {
		//	// start user's controller
		//	for (auto it = _tankControllerWorkers.begin(); it != _tankControllerWorkers.end(); it++) {
		//		auto& worker = *it;
		//		_gameStateManager->addMonitorObject(worker->getAssociatedTank());
		//		worker->run();
		//	}
		//	_controllerReadySignal->signal();
		//});

		// start user's controller
		for (auto it = _tankControllerWorkers.begin(); it != _tankControllerWorkers.end(); it++) {
			auto& worker = *it;
			_gameStateManager->addMonitorObject(worker->getAssociatedTank());
			worker->run();
		}
		_controllerReadySignal->signal();
		
		if (_userTank) {
			auto controlByUser = make_shared<PlayerControllerUI>(getWindow());
			_userTank->addComponent(controlByUser);
			_gameStateManager->addMonitorObject(_userTank);
		}

		// start bot tanks
		startStopBots(true);
	});
}

void BasicApp::stopRound() {
	_uiThreadRunner.postTask([this](float t) {
		for (auto it = _tankControllerWorkers.begin(); it != _tankControllerWorkers.end(); it++) {
			auto& worker = *it;
			worker->stopAndWait(100);
		}
		_controllerReadySignal->resetState();
	});

	_gameEngine->accessEngineResource([this]() {
		startStopBots(false);
		_gameStateManager->initState();
	});
}

void BasicApp::startStopButtonClick() {
	LOG_SCOPE_ACCESS(&_logAdapter, __FUNCTION__);

	if (StartState::NOT_STARTED == _startStopButtonState) {
		setSSButtonState(StartState::STARTED);

		bool different = _selectedPlayer1 != _controlBoard->getPlayer1() || _selectedPlayer2 != _controlBoard->getPlayer2();
		_selectedRoundCount = _controlBoard->getRoundCount();

		if (different) {
			_selectedPlayer1 = _controlBoard->getPlayer1();
			_selectedPlayer2 = _controlBoard->getPlayer2();

			GameEngine::getInstance()->accessEngineResource([this]() {
				generateGame();
			});
		}

		_gameStatistics->clearPlayers();
		for (auto it = _tankControllerWorkers.begin(); it != _tankControllerWorkers.end(); it++) {
			_gameStatistics->addPlayer(it->get()->getName());
		}

		startRound();
	}
	else if (StartState::STARTED == _startStopButtonState) {
		setSSButtonState(StartState::NOT_STARTED);
		_roundCountDownAt = -1;
		stopRound();
	}
}

void BasicApp::setSSButtonState(int state) {
	_startStopButtonState = state;
	_controlBoard->setStarStopButtonText(startStopButtonTexts[state]);
}

void BasicApp::update()
{
	LOG_SCOPE_ACCESS(ILogger::getInstance(), __FUNCTION__);
	if (getWindow()->isHidden()) return;
	static int countt = 0;
	countt++;
	// use ImGUI to check double click event
	if (ImGui::IsMouseDoubleClicked(0)) {
		auto& io = ImGui::GetIO();
		if (io.MousePos.x >= _gameView->getX() && io.MousePos.x <= _gameView->getX() + _gameView->getWidth() &&
			io.MousePos.y >= _gameView->getY() && io.MousePos.y <= _gameView->getY() + _gameView->getHeight()) {
			if (_topCotrol == _gameView.get()) {
				_topCotrol = &_spliter;
			}
			else {
				_topCotrol = _gameView.get();
			}
			onWindowSizeChanged();
		}
	}

	_uiThreadRunner.executeTasks(0);

	if (_gameStateManager->isGameOver()) {
		auto gameController = GameController::getInstance();
		auto& records = _gameStatistics->records();
		auto& players = _gameStatistics->getPlayers();

		// push a empty record
		records.push_back({});
		auto& roundRecord = records.back();
		roundRecord.winner = -1;
		auto& playerRecords = roundRecord.playerRecords;

		for (auto it = _tankControllerWorkers.begin(); it != _tankControllerWorkers.end(); it++) {
			auto worker = it->get();
			auto& tankPlayer = worker->getAssociatedTank();
			auto playerIt = std::find(players.begin(), players.end(), worker->getName());
			int playerIdx = -1;

			if (playerIt != players.end()) {
				playerIdx = (int)(playerIt - players.begin());
			}
			else {
				continue;
			}

			if (tankPlayer->getId() == _gameStateManager->getWinner()) {
				roundRecord.winner = playerIdx;
			}

			PlayerRecord playerRecord;
			playerRecord.averageTimePerFrame = worker->getAverageProcessingTimeOfController();
			playerRecord.playerIdx = playerIdx;
			playerRecord.kills = gameController->getKills(tankPlayer->getId());

			playerRecords.push_back(playerRecord);
		}

		if ((int)records.size() < _selectedRoundCount) {
			stopRound();

			static bool generatedGame;
			generatedGame = false;
			auto timer = make_shared<TimerObject>(GAME_PREPARING_TIME, true);
			_roundCountDownAt = _gameEngine->getCurrentTime();

			timer->startTimer();
			timer->setTimeEventHandler([this, timer](bool timeOut, float t) {
				auto lifeTime = timer->getLifeTime();
				if (generatedGame == false && lifeTime >= (GAME_PREPARING_TIME - 1.0f)) {
					generateGame();
					generatedGame = true;
				}
				if (timeOut && _startStopButtonState != StartState::NOT_STARTED) {
					//_uiThreadRunner.postTask([this](float t){
					//	startRound();
					//	_roundCountDownAt = -1;
					//});

					startRound();
					_roundCountDownAt = -1;
				}
			});

			_applicationComponentContainer->addComponent(timer);
		}
		else {
			startStopButtonClick();
		}
	}

	_topCotrol->update();
}

void BasicApp::draw()
{
	LOG_SCOPE_ACCESS(ILogger::getInstance(), __FUNCTION__);
	if (getWindow()->isHidden()) return;

	gl::clear(ColorA::black());

	_topCotrol->draw();

	if (_roundCountDownAt >= 0) {
		auto t = _gameEngine->getCurrentTime();
		
		auto durationLeft = (int)std::ceil(GAME_PREPARING_TIME - (t - _roundCountDownAt));

		auto currentRound = (int)(_gameStatistics->records().size() + 1);

		ci::vec2 textPos(getWindow()->getWidth()/2, _gameView->getY() + _gameView->getHeight() / 2);
		string text = "Round ";
		text.append(std::to_string(currentRound));
		text.append(" start in ");
		text.append(std::to_string(durationLeft));
		text.append("s");
#ifdef _WIN32
		ci::Font font("Aria", 56);
#else
		ci::Font font = ci::Font::getDefault();
#endif
		gl::drawStringCentered(text, textPos, ci::ColorA(1, 1, 1, 1), font);
	}
}

CINDER_APP(BasicApp, RendererGl( RendererGl::Options().msaa( 16 ) ), BasicApp::intializeApp)

#ifdef WIN32
#include <Windows.h>
#include <filesystem>
using namespace std::experimental;
#else
#include <unistd.h>
#include <filesystem>
using namespace std;
#endif

#include <stdio.h>
#include <io.h>
#include <fcntl.h>

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

#include "UI/WxAppLog.h"
#include "UI/Spliter.h"
#include "UI/WxControlBoard.h"
#include "UI/Panel.h"
#include "UI/WxPlayerInfo.h"
#include "Engine/GameEngine.h"
#include "LogAdapter.h"
#include "../common/Utility.h"
#include "UI/CiWndCandle.h"
#include "UI/WxGameView.h"
#include "Engine/Tank.h"
#include "Engine/Barrier.h"
#include "Controllers/PlayerControllerUI.h"
#include "Engine/GameResource.h"
#include "Controllers/PlayerControllerTest.h"
#include "battle/BattlePlatform.h"
#include "UI/WxTankPeripheralsView.h"
#include "battle/GameCapture.h"

using namespace ci;
using namespace ci::app;
using namespace std;

const char* startStopButtonTexts[] = {"Start", "Starting...", "Stop", "Stopping..."};
enum StartState : int {
	NOT_STARTED = 0,
	STARTING,
	STARTED,
	STOPING,
};


class BasicApp : public App {

	typedef std::function<void()> Task;

	Spliter _spliter;
	Widget* _topCotrol;
	shared_ptr<WxAppLog> _applog;
	shared_ptr<WxGameView> _gameView;
	shared_ptr<WxControlBoard> _controlBoard;
	thread _asynTasksWorker;
	SyncMessageQueue<Task> _tasks;
	
	shared_ptr<GameEngine> _gameEngine;
	shared_ptr<GameResource> _gameResource;
	shared_ptr<BattlePlatform> _battlePlatform;
	
	bool _runFlag;
	LogAdapter* _logAdapter;
	int _startStopButtonState = 0;
public:
	BasicApp();
	~BasicApp();
	void setup() override;
	void update() override;
	void draw() override;
	void cleanup();
	void startServices();
	void stopServices();
	void setupGame();

	void addLog(LogLevel logLevel, const char* fmt, va_list args) {
		if (_applog) {
			_applog->addLogV( (WxAppLog::LogLevel) logLevel, fmt, args);
		}
	}

	static void intializeApp(App::Settings* settings);
	void onWindowSizeChanged();
	void setSSButtonState(int state);
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
}

BasicApp::~BasicApp() {
	stopServices();

	_runFlag = false;
	if (_logAdapter) {
		delete _logAdapter;
	}

	if (_asynTasksWorker.joinable()) {
		_asynTasksWorker.join();
	}
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
	readlink("/proc/self/exe", buff, sizeof(buff));
#endif // WIN32
	return buff;
}

void BasicApp::setup()
{
	FUNCTON_LOG();

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

	vector<string> players = {"player1", "player2"};

	auto bottomSpliter = std::make_shared<Spliter>();
	auto bottomLeftSpilter = std::make_shared<Spliter>();

	_controlBoard = std::make_shared<WxControlBoard>(players);
	_logAdapter = new LogAdapter(_applog.get());

	_gameView = std::make_shared<WxGameView>(getWindow());

	auto _playerInfo1 = std::make_shared<WxPlayerInfo>("player1");
	auto _playerInfo2 = std::make_shared<WxPlayerInfo>("player2");;

	// arrange player information windows
	bottomLeftSpilter->setVertical(true);
	bottomLeftSpilter->setLayoutType(Spliter::LayoutType::Auto);
	bottomLeftSpilter->setLayoutRelative(0.5f);
	bottomLeftSpilter->setChild1(_playerInfo1);
	bottomLeftSpilter->setChild2(_playerInfo2);

	bottomSpliter->setVertical(true);
	bottomSpliter->setLayoutFixedSize(600);
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

	getWindow()->getSignalKeyDown().connect([this](ci::app::KeyEvent& e) {
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
		if (StartState::NOT_STARTED == _startStopButtonState) {
			setSSButtonState(StartState::STARTING);
			Task task = std::bind(&BasicApp::startServices, this);
			_tasks.pushMessage(task);
		}
		else if (StartState::STOPING == _startStopButtonState) {
			pushLog((int)LogLevel::Error, "services are stoping, please wait\n");
		}
		else {
			setSSButtonState(StartState::STOPING);
			Task task = std::bind(&BasicApp::stopServices, this);
			_tasks.pushMessage(task);
		}
	});

	// start the asynchonous task handlers
	_asynTasksWorker = std::thread([this]() {
		while (_runFlag)
		{
			BasicApp::Task task;
			if (_tasks.popMessage(task, 1000)) {
				task();
			}
		}
	});

	// set the first state of application
	setSSButtonState(StartState::NOT_STARTED);

	//_controlBoard->showWindow(true);

	setupGame();
}

void generateTanks1(Scene* scene) {
	auto& sceneArea = scene->getSceneArea();
	auto sceneHeight = sceneArea.getHeight();
	auto sceneWidth = sceneArea.getWidth();

	auto tank1 = make_shared<Tank>();
	tank1->setSize(vec2(5.7f, 6.6f));
	auto playerController = std::make_shared<PlayerControllerUI>(getWindow());
	//auto playerController = make_shared<PlayerControllerTest>();
	tank1->addComponent(playerController);
	tank1->translate(vec3(0, sceneHeight / 2 - tank1->getBound().getHeight() - 7, 0));
	tank1->rotate(glm::pi<float>());

	auto tank2 = make_shared<Tank>();
	tank2->setSize(vec2(5.7f, 6.6f));
	auto playerController1 = make_shared<PlayerControllerTest>();
	tank2->addComponent(playerController1);
	tank2->translate(vec3(0, -sceneHeight / 2 + tank1->getBound().getHeight() + 7, 0));

	auto tank3 = make_shared<Tank>();
	tank3->setSize(vec2(5.7f, 6.6f));
	auto playerController3 = make_shared<PlayerControllerTest>();
	tank3->addComponent(playerController3);
	tank3->translate(vec3(-sceneWidth / 2 + tank1->getBound().getWidth() + 7, 0, 0));
	tank3->rotate(-glm::pi<float>() / 2);

	auto tank4 = make_shared<Tank>();
	tank4->setSize(vec2(5.7f, 6.6f));
	auto playerController4 = make_shared<PlayerControllerTest>();
	tank4->addComponent(playerController4);
	tank4->translate(vec3(sceneWidth / 2 - tank1->getBound().getWidth() - 7, 0, 0));
	tank4->rotate(glm::pi<float>() / 2);

	Colorf tankColors[] = {
		{ 0,0,1 },
		{ 0,1,0 },
		{ 1,0,0 },
		{ 1,1,0 },
	};

	int i = 0;
	tank1->setColor(tankColors[i++]);
	tank2->setColor(tankColors[i++]);
	tank3->setColor(tankColors[i++]);
	tank4->setColor(tankColors[i++]);

	scene->addDrawbleObject(tank1);
	scene->addDrawbleObject(tank2);
	scene->addDrawbleObject(tank3);
	scene->addDrawbleObject(tank4);
}

std::vector<Tank*> generateTanks2(Scene* scene) {
	float padding = 8;

	auto& sceneArea = scene->getSceneArea();
	auto sceneHeight = sceneArea.getHeight() - padding * 2;
	auto sceneWidth = sceneArea.getWidth() - padding * 2;

	Colorf tankColors[] = {
		{ 0,0,1 },
		{ 0,1,0 },
		{ 1,0,0 },
		{ 1,1,0 },
	};

	constexpr int tankColorCount = sizeof(tankColors) / sizeof(tankColors[0]);
	
	vec2 tankSize(5.7f, 6.6f);
	int colMax = (int)(sceneWidth / tankSize.x / 2);
	int rowMax = (int)(sceneHeight / tankSize.y / 2);
	int tankMaxCount = colMax * rowMax;

	int expectedTank = 10;
	int tankCount = std::min(expectedTank, tankMaxCount);
	if (tankCount <= 0) {
		std::vector<Tank*> tanks;
		return tanks;
	}

	Rand randomizer( (uint32_t) (GameEngine::getInstance()->getCurrentTime() * 10000) );

	float rowW = sceneHeight / colMax;
	float colW = sceneWidth / rowMax;

	auto baseCoordinateOfScene = sceneArea.getUpperLeft();

	std::map<int, bool> generatedPos;
	std::vector<Tank*> tanks(tankCount);

	for (int i = 0; i < tankCount; i++) {
		auto iPos = randomizer.nextInt(0, tankMaxCount);
		while (generatedPos.find(iPos) != generatedPos.end())
		{
			iPos = randomizer.nextInt(0, tankMaxCount);
		}
		generatedPos[iPos] = true;
		ci::vec3 tankPos((iPos % colMax) * colW + colW/2 + baseCoordinateOfScene.x + padding, (iPos / rowMax) * rowW + rowW/2 + baseCoordinateOfScene.y + padding, 0);
		auto tank = make_shared<Tank>();
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
		tanks[i] = tank.get();
	}

	auto playerController = std::make_shared<PlayerControllerUI>(getWindow());
	//auto playerController = std::make_shared<PlayerControllerTest>();
	tanks[0]->addComponent(playerController);
	for (int i = 1; i < tankCount; i++) {
		auto playerController = make_shared<PlayerControllerTest>();
		tanks[i]->addComponent(playerController);
	}

	return tanks;
}


void BasicApp::setupGame() {
	addAssetDirectory("E:/Projects/tank-battle/src/application/assets");
	_gameResource = std::shared_ptr<GameResource>(GameResource::createInstance());
	_gameResource->setTexture(TEX_ID_BULLET, "bulletBlue1_outline.png");
	_gameResource->setTexture(TEX_ID_EXPLOSION, "explosion.png");
	_gameResource->setTexture(TEX_ID_TANKBODY, "tankBody.png");
	_gameResource->setTexture(TEX_ID_TANKBARREL, "tankBarrel.png");
	_gameResource->setTexture(TEX_ID_TANKSHOT, "shotLarge.png");

	_gameEngine = std::shared_ptr<GameEngine>(GameEngine::createInstance());

	const float sceneWidth = 70;
	const float sceneHeight = 70;

	_battlePlatform = make_shared<BattlePlatform>(sceneWidth, sceneHeight);
	auto gameCapture = make_shared<GameCapture>();

	auto& gameArea = _battlePlatform->getMapArea();
	auto gameScene = std::shared_ptr<Scene>(Scene::createScene(gameArea));
	gameScene->setBackgroundColor(ci::ColorA8u::gray(69, 255));

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

	auto tanks = generateTanks2(gameScene.get());

	_gameEngine->setScene(gameScene);
	_gameView->setScene(gameScene);
	_gameView->setSceneViewRatio(gameArea.getAspectRatio());

	gameScene->addGameObject(gameCapture);

	auto tankRef = dynamic_pointer_cast<DrawableObject>(gameScene->findObjectRef(tanks[0]));

	auto objectViewContainer = make_shared<ObjectViewContainer>(tankRef);

	auto radar = make_shared<Radar>(objectViewContainer, glm::pi<float>());
	radar->setRange(std::max(gameArea.getWidth(), gameArea.getHeight())/2);

	auto camera = make_shared<TankCamera>(objectViewContainer, glm::pi<float>()*2.0f/3);
	camera->setRange(sqrt(gameArea.getWidth()*gameArea.getWidth() + gameArea.getHeight()*gameArea.getHeight()));

	tankRef->addComponent(objectViewContainer);
	tankRef->addComponent(radar);
	tankRef->addComponent(camera);

	auto peripheralsview = make_shared<WxTankPeripheralsView>(getWindow());

	try {
		peripheralsview->setupPeripherals(camera, radar);
	}
	catch (...) {
		quit();
	}

	_gameView->setTankView(peripheralsview);
}

void BasicApp::startServices() {
	LOG_SCOPE_ACCESS(_logAdapter, __FUNCTION__);

	std::unique_ptr<void, std::function<void(void*)>> scopedFinishState((void*)1, [this](void*) {
		setSSButtonState(StartState::STARTED);
	});
}

void BasicApp::stopServices() {
	LOG_SCOPE_ACCESS(_logAdapter, __FUNCTION__);
	std::unique_ptr<void, std::function<void(void*)>> scopedFinishState((void*)1, [this](void*) {
		setSSButtonState(StartState::NOT_STARTED);
	});
}

void BasicApp::setSSButtonState(int state) {
	_startStopButtonState = state;
	_controlBoard->setStarStopButtonText(startStopButtonTexts[state]);
}

void BasicApp::update()
{
	FUNCTON_LOG();
	if (getWindow()->isHidden()) return;

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

	_gameEngine->doUpdate();
	_topCotrol->update();

	// update popup window
	_controlBoard->update();
}

void BasicApp::draw()
{
	FUNCTON_LOG();
	if (getWindow()->isHidden()) return;

	gl::clear(ColorA::black());

	_topCotrol->draw();

	// draw popup window
	_controlBoard->draw();
}

CINDER_APP(BasicApp, RendererGl( RendererGl::Options().msaa( 16 ) ), BasicApp::intializeApp)

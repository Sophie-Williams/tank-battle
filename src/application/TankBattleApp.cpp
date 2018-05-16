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

	shared_ptr<PlayerControllerUI> _playerControllerUI;
	
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

void BasicApp::setupGame() {
	_gameEngine = std::shared_ptr<GameEngine>(GameEngine::createInstance());

	auto tank1 = make_shared<Tank>();
	tank1->setSize(vec2(3.8f, 4.4f));
	tank1->setComponentTexture("E:\\Projects\\TankBattle\\src\\application\\assets\\tankBody.png", "E:\\Projects\\TankBattle\\src\\application\\assets\\tankBarrel.png");

	const float sceneWidth = 70;
	const float sceneHeight = 70;

	Rectf gameArea(-sceneWidth/2, -sceneHeight/2, sceneWidth/2, sceneHeight/2);
	auto gameScene = std::shared_ptr<Scene>(Scene::createScene(gameArea));
	gameScene->setBackgroundColor(ci::ColorA8u::gray(69, 255));
	gameScene->addGameObject(tank1);

	auto barrier1 = std::make_shared<Barrier>();
	auto barrier2 = std::make_shared<Barrier>();
	auto barrier3 = std::make_shared<Barrier>();
	auto barrier4 = std::make_shared<Barrier>();

	float wallDepth1 = 6;
	float wallDepth2 = 5;

	barrier1->setBound(Rectf(gameArea.x1, gameArea.y1, gameArea.x2, gameArea.y1 + wallDepth1));
	barrier2->setBound(Rectf(gameArea.x2 - wallDepth2, gameArea.y1, gameArea.x2, gameArea.y2));
	barrier3->setBound(Rectf(gameArea.x1, gameArea.y2 - wallDepth1, gameArea.x2, gameArea.y2));
	barrier4->setBound(Rectf(gameArea.x1, gameArea.y1, gameArea.x1 + wallDepth2, gameArea.y2));

	gameScene->addGameObject(barrier1);
	gameScene->addGameObject(barrier2);
	gameScene->addGameObject(barrier3);
	gameScene->addGameObject(barrier4);

	_gameEngine->setScene(gameScene);
	_gameView->setScene(gameScene);
	_gameView->setSceneViewRatio(gameArea.getAspectRatio());

	_playerControllerUI = std::make_shared<PlayerControllerUI>(tank1, getWindow());
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

CINDER_APP(BasicApp, RendererGl( RendererGl::Options().msaa( 8 ) ), BasicApp::intializeApp)

//#include "../base/SoundCache.h"
//#include "../core/opengl/GLContext.h"
#include "../base/ApplicationPackage.h"
//#include "../base/InputManager.h"
#include "../base/Logger.h"
//#include "../base/SDLIncludes.h"
//#include "../base/GraphicsWindow.h"
//#include "../core/SDLUtils.h"
//#include "../base/Gu.h"
//#include "../core/opengl/OglErr.h"
#include "../base/OperatingSystem.h"
#include "../base/FileSystem.h"
#include "../base/EngineConfig.h"
#include "../math/MathAll.h"
// #include "../gfx/GraphicsApi.h"
// #include "../gfx/RenderUtils.h"
// #include "../core/opengl/OpenGLApi.h"
// #include "../gfx/VulkanApi.h"
// #include "../model/ModelCache.h"
// #include "../world/Scene.h"
// #include "../world/PhysicsWorld.h"
// #include "../bottle/BottleScript.h"
#include "../core/LuaScript.h"
#include "../core/CoreHeader.h"
#include "../core/Core.h"
#include "../core/SDLIncludes.h"
#include "../core/SDLApp.h"
#include "../core/SDLUtils.h"
#include "../core/GraphicsDriver.h"

#include <signal.h>
#include <chrono>
#include <thread>

namespace VG {
void SignalHandler(int signal) {
  BRThrowException(Stz "VC Access Violation. signal=" + signal + "  This shouldn't work in release build.");
}
SDLApp::SDLApp() {
}
SDLApp::~SDLApp() {
}
void SDLApp::freeFile(char*& pOutData) {
  SDLUtils::SDLFileFree(pOutData);
}
int SDLApp::readFile(const string_t& fname, char*& pOutData, int64_t& _iOutSizeBytes, bool addNull) {
  return SDLUtils::SDLFileRead(fname, pOutData, _iOutSizeBytes, addNull);
}
int SDLApp::writeFile(const string_t& fname, char* pData, size_t _iDataSizeBytes) {
  return SDLUtils::SDLFileWrite(fname, pData, _iDataSizeBytes);
}
void SDLApp::initSDLAndCreateGraphicsApi() {
  //Nix main()
  SDL_SetMainReady();

  //The Init isn't needed, but it is if we want to print the video diagnostics
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) == -1) {
    exitApp(Stz "SDL could not be initialized: " + SDL_GetError(), -1);
  }
  printVideoDiagnostics();

  //Create graphics API
  if (Base::config()->getRenderSystem() == RenderSystem::OpenGL) {
    BRThrowNotImplementedException();
    //TODO:
    //_pGraphicsApi = std::make_shared<OpenGLApi>();
  }
  else if (Base::config()->getRenderSystem() == RenderSystem::Vulkan) {
    BRThrowNotImplementedException();
    //_pGraphicsApi = std::make_shared<VulkanApi>();
  }
  else {
    BRThrowException("Invalid render engine.");
  }
  //Base::setGraphicsApi(_pGraphicsApi);

  // Run Tests
  std::shared_ptr<LuaScript> math_test = std::make_shared<LuaScript>();
  math_test->compile(FileSystem::combinePath(package()->getScriptsFolder(), "/test_math.lua"));
  math_test = nullptr;

  std::shared_ptr<LuaScript> editor = std::make_shared<LuaScript>();
  editor->compile(FileSystem::combinePath(package()->getAssetsFolder(), "/Editor.lua"));

  //** Editor.lua
  //
  //   std::shared_ptr<GraphicsWindow> mainwindow = _pGraphicsApi->createWindow(GraphicsWindowCreateParameters(Stz "Main Window",
  //                                                                                                           Base::getConfig()->getDefaultScreenWidth(),
  //                                                                                                           Base::getConfig()->getDefaultScreenHeight(),
  //                                                                                                           Base::getConfig()->getStartFullscreen(),
  //                                                                                                           true,
  //                                                                                                           Base::getConfig()->getForceAspectRatio(),
  //                                                                                                           nullptr));  //Just avoid title
  //
  //   initNet();
  //
  //   BRLogInfo("Creating Child Window ");
  //   std::shared_ptr<GraphicsWindow> child_win = _pGraphicsApi->createWindow(GraphicsWindowCreateParameters(Stz "Window 2", 300, 600, false, true, false, mainwindow));  //Just avoid title
  //
  //   BRLogInfo("Creating Scene");
  //   std::shared_ptr<Scene> pscene = Scene::create();
  //   mainwindow->setScene(pscene);
  //
  //   //**TODO: Multiple windows / scene
  //   //   child_win->setScene(pscene);
  //   //**TODO: Multiple windows / scene
  //
  //   //The game specific script
  //   pscene->addComponent(std::make_shared<LuaScript>());
  //   //pscene->addComponent(std::make_shared<BottleScript>());

  BRLogInfo("Apprunner complete.");
}
void SDLApp::checkErrors(bool bLog, bool bBreak) {
  SDLUtils::checkSDLErr(bLog, bBreak);
  if (_graphicsDriver != nullptr) {
    _graphicsDriver->checkErrors();
  }
}
void SDLApp::doShowError(const string_t& err, const Exception* const e) const {
  if (e != nullptr) {
    OperatingSystem::showErrorDialog(e->what() + err, Stz "Error");
  }
  else {
    OperatingSystem::showErrorDialog("No Error Message" + err, Stz "Error");
  }
}
void SDLApp::runUnitTests(std::vector<std::function<bool()>> unit_tests) {
  int x = 1;
  for (auto test : unit_tests) {
    if (!test()) {
      BRLogError("Unit test " + x + " failed.");
      Base::debugBreak();
    }
    x++;
  }
}

void SDLApp::attachToGameHost() {
  int GameHostPort = Base::config()->getGameHostPort();

  IPaddress ip;
  if (SDLNet_ResolveHost(&ip, NULL, GameHostPort) == -1) {
    exitApp(std::string("") + "SDLNet_ResolveHost: " + SDLNet_GetError(), -1);
  }

  _server_tcpsock = SDLNet_TCP_Open(&ip);
  if (!_server_tcpsock) {
    exitApp(std::string("") + "SDLNet_TCP_Open:" + SDLNet_GetError(), -1);
  }

  //Wait a few, then exit if we don't get a response.
  t_timeval t0 = Base::getMilliSeconds();

  while (true) {
    int timeout = Base::config()->getGameHostTimeoutMs();

    if (Base::getMilliSeconds() - t0 > timeout) {
      exitApp(Stz "Failed to connect to game host, timeout " + timeout + "ms exceeded.", -1);
      break;  //Unreachable, but just in case.
    }

    _gameHostSocket = SDLNet_TCP_Accept(_server_tcpsock);
    if (_gameHostSocket) {
      //  char data[512];

      //while(SDLNet_TCP_Recv(_gameHostSocket, data, 512) <= 0) {
      //    //DebugBreak();
      //    int n = 0;
      //    n++;
      //}
      //DebugBreak();

      // communicate over new_tcpsock
      break;
    }
    else {
      Base::sleepThread(50);
    }
  }
}
void SDLApp::printVideoDiagnostics() {
  //Init Video
  //SDL_Init(SDL_INIT_VIDEO);

  //Drivers (useless in sdl2)
  const char* driver = SDL_GetCurrentVideoDriver();
  if (driver) {
    BRLogInfo("Default Video Driver: " + driver);
  }
  BRLogInfo("Installed Video Drivers: ");
  int idrivers = SDL_GetNumVideoDrivers();
  for (int idriver = 0; idriver < idrivers; ++idriver) {
    driver = SDL_GetVideoDriver(idriver);
    BRLogInfo(" " + driver);
  }

  // Get current display mode of all displays.
  int nDisplays = SDL_GetNumVideoDisplays();
  BRLogInfo(nDisplays + " Displays:");
  for (int idisplay = 0; idisplay < nDisplays; ++idisplay) {
    SDL_DisplayMode current;
    int should_be_zero = SDL_GetCurrentDisplayMode(idisplay, &current);

    if (should_be_zero != 0) {
      // In case of error...
      BRLogInfo("  Could not get display mode for video display #%d: %s" + idisplay);
      Base::checkErrors();
    }
    else {
      // On success, print the current display mode.
      BRLogInfo("  Display " + idisplay + ": " + current.w + "x" + current.h + ", " + current.refresh_rate + "hz");
      Base::checkErrors();
    }
  }
}
void SDLApp::updateWindowHandleForGamehost() {
#if defined(BR2_OS_WINDOWS)
  //For the WPF app container we need to set the window handle to be the top window
  //https://docs.microsoft.com/en-us/dotnet/api/system.diagnostics.process.mainwindowhandle?view=netframework-4.8
  //SDL_SysWMinfo wmInfo;
  //SDL_VERSION(&wmInfo.version);
  //SDL_GetWindowWMInfo(Base::getWindow(), &wmInfo);
  //HWND hwnd = wmInfo.info.win.window;
  //HWND old = GetActiveWindow();
  //SetActiveWindow(hwnd);
#endif
}
void SDLApp::initAudio() {
  //AUDIO
  if (SDL_AudioInit(NULL) < 0) {
    exitApp(Stz "SDL Couldn't initialize audio driver: " + SDL_GetError(), -1);
  }
  Base::checkErrors();
}
void SDLApp::initNet() {
  BRLogInfo("Initializing SDL Net");
  if (SDLNet_Init() == -1) {
    exitApp(Stz "SDL Net could not be initialized: " + SDL_GetError(), -1);
  }

  if (Base::config()->getGameHostAttached()) {
    updateWindowHandleForGamehost();
    attachToGameHost();
  }

  Base::checkErrors();
}
void SDLApp::runGameLoopTryCatch() {
  typedef void (*SignalHandlerPointer)(int);

  //Attempt to catch segfaults that doesn't really work.
  //https://stackoverflow.com/questions/457577/catching-access-violation-exceptions
  SignalHandlerPointer previousHandler;
  previousHandler = signal(SIGSEGV, SignalHandler);

  //test the globals before starting the game loop
  Base::updateGlobals();

  //Print the setup time.
  BRLogInfo(Stz "**Total initialization time: " + Math::round((float)((Base::getMicroSeconds() - _tvInitStartTime) / 1000) / 1000.0f, 2) + " seconds" + OperatingSystem::newline());

  BRLogInfo("Entering Game Loop");
  try {
    _graphicsDriver->updateLoop();
  }
  catch (const Exception& e) {
    doShowError("Runtime Error", &e);
  }
  catch (...) {
    doShowError("Runtime Error, Unhandled exception.", nullptr);
  }
}

void SDLApp::exitApp(const string_t& error, int rc) {
  OperatingSystem::showErrorDialog(error + SDLNet_GetError(), Stz "Error");

  Base::debugBreak();

  _graphicsDriver->cleanup();

  SDLNet_Quit();
  SDL_Quit();

  exit(rc);
}
bool SDLApp::argMatch(const std::vector<string_t>& args, const string_t& arg1, int32_t iCount) {
  if (args.size() <= 1) {
    return false;
  }
  if (StringUtil::lowercase(arg1) == StringUtil::lowercase(args[1])) {
    return true;
  }
  return false;
}
bool SDLApp::runCommands(const std::vector<string_t>& args) {
  if (argMatch(args, "/c", 4)) {
    //Convert Mob
    string_t strMob = args[2];
    string_t strFriendlyName = args[3];
    BRThrowNotImplementedException();
    // Base::getModelCache()->convertMobToBin(strMob, false, strFriendlyName);
    return true;
  }
  else if (argMatch(args, "/s", 3)) {
    //Compile Shader
    // cw.exe /s "ShaderName"  "dir/ShaderFileDesc.dat"

    return true;
  }
  else {
    return false;
  }
}
void SDLApp::loadDebugPackage() {
  BRLogInfo("Building Debug ApplicationPackage");
  _package = std::make_shared<ApplicationPackage>();
  _package->build(FileSystem::getExecutableFullPath());
  Base::checkErrors();
}
void SDLApp::loadAppPackage(const std::vector<string_t>& args) {
  _package = nullptr;

  if (Base::checkArg(args, "package_loc", "_debug")) {
    BRLogInfo("Application package set to debug.");
    loadDebugPackage();
  }
  else {
    BRLogInfo("Select App Package: Use package_loc=_debug to skip the file selector.");

    auto files = OperatingSystem::showOpenFileDialog("Open Project", "(*.xml)\0*.xml\0\0", "*.xml", FileSystem::getExecutableDirectory());
    string_t file = files.size() ? files[0] : "";

    if (StringUtil::isNotEmpty(file)) {
      BRLogInfo("Building Package");
      _package = std::make_shared<ApplicationPackage>();
      _package->build(FileSystem::getExecutableFullPath());
    }
    else {
      BRLogWarn("Package not selected. Loading debug package.");
      loadDebugPackage();
    }
  }
}

void SDLApp::runApp(const std::vector<string_t>& args, std::vector<std::function<bool()>> unit_tests) {
  _tvInitStartTime = Base::getMicroSeconds();

  //Root the engine FIRST so we can find the EngineConfig.dat
  FileSystem::init(args[0]);

  _app = std::make_shared<SDLApp>();

  //Start logger
  Base::initGlobals(std::dynamic_pointer_cast<App>(_app), "./logs", args);

  loadAppPackage(args);
  Core::initGlobals();
  // static std::shared_ptr<ApplicationPackage> getPackage();
  //static void setPackage(std::shared_ptr<ApplicationPackage> x) { _pPackage = x; }

  //Load Engine Config, and make globals
  {
    initSDLAndCreateGraphicsApi();

    if (runCommands(args) == false) {
      runUnitTests(unit_tests);
      runGameLoopTryCatch();
    }
  }
}

}  // namespace VG

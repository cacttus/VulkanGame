#include "../base/AppRunner.h"
#include "../base/SoundCache.h"
#include "../base/GLContext.h"
#include "../base/ApplicationPackage.h"
#include "../base/InputManager.h"
#include "../base/Logger.h"
#include "../base/SDLIncludes.h"
#include "../base/GraphicsWindow.h"
#include "../base/SDLUtils.h"
#include "../base/Gu.h"
#include "../base/OglErr.h"
#include "../base/OperatingSystem.h"
#include "../base/FileSystem.h"
#include "../base/EngineConfig.h"
#include "../math/MathAll.h"
#include "../gfx/GraphicsApi.h"
#include "../gfx/RenderUtils.h"
#include "../gfx/OpenGLApi.h"
#include "../gfx/VulkanApi.h"
#include "../model/ModelCache.h"
#include "../world/Scene.h"
#include "../world/PhysicsWorld.h"
#include "../bottle/BottleScript.h"

#include <signal.h>
#include <chrono>
#include <thread>

namespace BR2 {
//////////////////////////////////////////////////////////////////////////
void SignalHandler(int signal) {
  BRThrowException(Stz "VC Access Violation. signal=" + signal + "  This shouldn't work in release build.");
}
//////////////////////////////////////////////////////////////////////////
class AppRunner_Internal {
public:
  SDL_AudioSpec _audioSpec;
  uint64_t _tvInitStartTime = 0;
  // accept a connection coming in on server_tcpsock
  TCPsocket _gameHostSocket;  //Send and receive data
  TCPsocket _server_tcpsock;  //Accept connections
  std::shared_ptr<GraphicsApi> _pGraphicsApi = nullptr;

  void initSDLAndCreateGraphicsApi();
  void doShowError(string_t err, const Exception* const e) const;
  void attachToGameHost();
  void printVideoDiagnostics();
  void updateWindowHandleForGamehost();
  void initAudio();
  void initNet();
  void runGameLoopTryCatch();
  void exitApp(string_t error, int rc);
  bool argMatch(const std::vector<string_t>& args, string_t arg1, int32_t iCount);
  bool runCommands(const std::vector<string_t>& args);
  void loadAppPackage(const std::vector<string_t>& args);
  void runUnitTests(std::vector<std::function<bool()>>);
};

void AppRunner_Internal::initSDLAndCreateGraphicsApi() {
  //Nix main()
  SDL_SetMainReady();

  //The Init isn't needed, but it is if we want to print the video diagnostics
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) == -1) {
    exitApp(Stz "SDL could not be initialized: " + SDL_GetError(), -1);
  }
  printVideoDiagnostics();

  //Create graphics API
  if (Gu::getEngineConfig()->getRenderSystem() == RenderSystem::OpenGL) {
    _pGraphicsApi = std::make_shared<OpenGLApi>();
  }
  else if (Gu::getEngineConfig()->getRenderSystem() == RenderSystem::Vulkan) {
    _pGraphicsApi = std::make_shared<VulkanApi>();
  }
  else {
    BRThrowException("Invalid render engine.");
  }
  Gu::setGraphicsApi(_pGraphicsApi);

  std::shared_ptr<GraphicsWindow> mainwindow = _pGraphicsApi->createWindow(GraphicsWindowCreateParameters(Stz "Main Window", Gu::getConfig()->getDefaultScreenWidth(), Gu::getConfig()->getDefaultScreenHeight(), Gu::getConfig()->getStartFullscreen(), true, Gu::getConfig()->getForceAspectRatio(), nullptr));  //Just avoid title

  //initAudio();//This is probably not needed - remove
  initNet();

  BRLogInfo("Creating Child Window ");
  std::shared_ptr<GraphicsWindow> child_win = _pGraphicsApi->createWindow(GraphicsWindowCreateParameters(Stz "Window 2", 300, 600, false, true, false, mainwindow));  //Just avoid title

  BRLogInfo("Creating Scene");
  std::shared_ptr<Scene> pscene = Scene::create();
  mainwindow->setScene(pscene);

  //**TODO: Multiple windows / scene
  //   child_win->setScene(pscene);
  //**TODO: Multiple windows / scene

  //The game specific script
  pscene->addComponent(std::make_shared<BottleScript>());

  BRLogInfo("Apprunner complete.");
}
void AppRunner_Internal::doShowError(string_t err, const Exception* const e) const {
  if (e != nullptr) {
    OperatingSystem::showErrorDialog(e->what() + err, Stz "Error");
  }
  else {
    OperatingSystem::showErrorDialog("No Error Message" + err, Stz "Error");
  }
}
void AppRunner_Internal::runUnitTests(std::vector<std::function<bool()>> unit_tests) {
  int x = 1;
  for (auto test : unit_tests) {
    if (!test()) {
      BRLogError("Unit test " + x + " failed.");
      Gu::debugBreak();
    }
    x++;
  }
}

void AppRunner_Internal::attachToGameHost() {
  int GameHostPort = Gu::getEngineConfig()->getGameHostPort();

  IPaddress ip;
  if (SDLNet_ResolveHost(&ip, NULL, GameHostPort) == -1) {
    exitApp(std::string("") + "SDLNet_ResolveHost: " + SDLNet_GetError(), -1);
  }

  _server_tcpsock = SDLNet_TCP_Open(&ip);
  if (!_server_tcpsock) {
    exitApp(std::string("") + "SDLNet_TCP_Open:" + SDLNet_GetError(), -1);
  }

  //Wait a few, then exit if we don't get a response.
  t_timeval t0 = Gu::getMilliSeconds();

  while (true) {
    int timeout = Gu::getEngineConfig()->getGameHostTimeoutMs();

    if (Gu::getMilliSeconds() - t0 > timeout) {
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
      Gu::sleepThread(50);
    }
  }
}
void AppRunner_Internal::printVideoDiagnostics() {
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
      SDLUtils::checkSDLErr();
    }
    else {
      // On success, print the current display mode.
      BRLogInfo("  Display " + idisplay + ": " + current.w + "x" + current.h + ", " + current.refresh_rate + "hz");
      SDLUtils::checkSDLErr();
    }
  }
}
void AppRunner_Internal::updateWindowHandleForGamehost() {
#if defined(BR2_OS_WINDOWS)
  //For the WPF app container we need to set the window handle to be the top window
  //https://docs.microsoft.com/en-us/dotnet/api/system.diagnostics.process.mainwindowhandle?view=netframework-4.8
  //SDL_SysWMinfo wmInfo;
  //SDL_VERSION(&wmInfo.version);
  //SDL_GetWindowWMInfo(Gu::getWindow(), &wmInfo);
  //HWND hwnd = wmInfo.info.win.window;
  //HWND old = GetActiveWindow();
  //SetActiveWindow(hwnd);
#endif
}
void AppRunner_Internal::initAudio() {
  //AUDIO
  if (SDL_AudioInit(NULL) < 0) {
    exitApp(Stz "SDL Couldn't initialize audio driver: " + SDL_GetError(), -1);
  }
  SDLUtils::checkSDLErr();
}
void AppRunner_Internal::initNet() {
  BRLogInfo("Initializing SDL Net");
  if (SDLNet_Init() == -1) {
    exitApp(Stz "SDL Net could not be initialized: " + SDL_GetError(), -1);
  }

  if (Gu::getEngineConfig()->getGameHostAttached()) {
    updateWindowHandleForGamehost();
    attachToGameHost();
  }

  SDLUtils::checkSDLErr();
}
void AppRunner_Internal::runGameLoopTryCatch() {
  typedef void (*SignalHandlerPointer)(int);

  //Attempt to catch segfaults that doesn't really work.
  //https://stackoverflow.com/questions/457577/catching-access-violation-exceptions
  SignalHandlerPointer previousHandler;
  previousHandler = signal(SIGSEGV, SignalHandler);

  //test the globals before starting the game loop
  Gu::updateManagers();

  //Print the setup time.
  BRLogInfo(Stz "**Total initialization time: " + MathUtils::round((float)((Gu::getMicroSeconds() - _tvInitStartTime) / 1000) / 1000.0f, 2) + " seconds" + OperatingSystem::newline());

  BRLogInfo("Entering Game Loop");
  try {
    _pGraphicsApi->updateLoop();
  }
  catch (const Exception& e) {
    doShowError("Runtime Error", &e);
  }
  catch (...) {
    doShowError("Runtime Error, Unhandled exception.", nullptr);
  }
}

void AppRunner_Internal::exitApp(string_t error, int rc) {
  OperatingSystem::showErrorDialog(error + SDLNet_GetError(), Stz "Error");

  Gu::debugBreak();

  Gu::getGraphicsApi()->cleanup();

  SDLNet_Quit();
  SDL_Quit();

  exit(rc);
}
bool AppRunner_Internal::argMatch(const std::vector<string_t>& args, string_t arg1, int32_t iCount) {
  if (args.size() <= 1) {
    return false;
  }
  if (StringUtil::lowercase(arg1) == StringUtil::lowercase(args[1])) {
    return true;
  }
  return false;
}
bool AppRunner_Internal::runCommands(const std::vector<string_t>& args) {
  if (argMatch(args, "/c", 4)) {
    //Convert Mob
    string_t strMob = args[2];
    string_t strFriendlyName = args[3];
    Gu::getModelCache()->convertMobToBin(strMob, false, strFriendlyName);
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
void AppRunner_Internal::loadAppPackage(const std::vector<string_t>& args) {
  if (Gu::checkArg(args, "package_loc", "_debug")) {
    BRLogInfo("Application package set to debug.");
  }
  else {
    BRLogInfo("Select App Package: Use package_loc=_debug to skip the file selector.");

    auto files = OperatingSystem::showOpenFileDialog("Open Project", "(*.xml)\0*.xml\0\0", "*.xml", FileSystem::getExecutableDirectory());
    string_t file = files.size() ? files[0] : "";

    if (StringUtil::isNotEmpty(file)) {
      BRLogInfo("Building Package");
      std::shared_ptr<ApplicationPackage> pack = std::make_shared<ApplicationPackage>();
      pack->build(FileSystem::getExecutableFullPath());
      Gu::setPackage(pack);
    }
    else {
      BRLogWarn("Package could not be loaded.  Folder not selected, or invalid path.");
      BRLogDebug("We do not yet support packages.");
    }
  }
}
//////////////////////////////////////////////////////////////////////////
AppRunner::AppRunner() {
  _pint = std::make_unique<AppRunner_Internal>();
}
AppRunner::~AppRunner() {
  _pint = nullptr;
}
void AppRunner::runApp(const std::vector<string_t>& args, std::vector<std::function<bool()>> unit_tests) {
  _pint->_tvInitStartTime = Gu::getMicroSeconds();

  //Root the engine FIRST so we can find the EngineConfig.dat
  FileSystem::init(args[0]);

  //Start logger
  Gu::createLogger("./logs", args);

  _pint->loadAppPackage(args);

  //Load Engine Config, and make globals
  Gu::initGlobals(args);
  {
    _pint->initSDLAndCreateGraphicsApi();

    if (_pint->runCommands(args) == false) {
      _pint->runUnitTests(unit_tests);
      _pint->runGameLoopTryCatch();
    }
  }
  Gu::deleteManagers();
}

//        //Init SDL ?? DO we need?
//        if (SDL_Init(0) != 0) {
//            fprintf(stderr, "Couldn't initialize video driver: %s\n",
//                    SDL_GetError());
//            return SDL_FALSE;
//        }
//
//
//        if (SDL_VideoInit(_pGlState->videodriver) < 0) {
//            fprintf(stderr, "Couldn't initialize video driver: %s\n",
//                    SDL_GetError());
//            return SDL_FALSE;
//        }
//Fullscreen
//        SDL_zero(fullscreen_mode);
//        switch (depthBits) {
//            case 16:
//                fullscreen_mode.format = SDL_PIXELFORMAT_RGB565;
//                break;
//            case 24:
//                fullscreen_mode.format = SDL_PIXELFORMAT_RGB24;
//                break;
//        }
//        fullscreen_mode.refresh_rate = _pGlState->refresh_rate;
//
//Create Window.
// char  etitle[1024];
// char* szTitle = NULL;

//SDL_ShowWindow(_pWindow);

//Not necessary - we need to make sure this it
//Create Renderer
// _pGlState->renderer = SDL_CreateRenderer(_pGlState->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
// if (!_pGlState->renderer) {
//     fprintf(stderr, "Couldn't create renderer: %s\n",
//         SDL_GetError());
//     return SDL_FALSE;
// }
// if (_pGlState->logical_w && _pGlState->logical_h) {
//     SDL_RenderSetLogicalSize(_pGlState->renderer, _pGlState->logical_w, _pGlState->logical_h);
// }
// else if (_pGlState->scale) {
//     SDL_RenderSetScale(_pGlState->renderer, _pGlState->scale, _pGlState->scale);
// }

//}

}  // namespace BR2

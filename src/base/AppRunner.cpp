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
  void doShowError(string_t err, const Exception * const e) const;
  void attachToGameHost();
  void printVideoDiagnostics();
  void updateWindowHandleForGamehost();
  SDL_bool initAudio();
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

  //Unfortunately we need a window for a graphics context.  We need contexts to init the managers.
  // This default window must come here.  We initialize the rendering system later.
  // After this window is made, more windows can be made since we have a core context.
  std::shared_ptr<GraphicsWindow> window = _pGraphicsApi->createWindow(Stz "");  //Just avoid title
  SDLUtils::checkSDLErr();
  Gu::getCoreContext()->chkErrRt();

  initAudio();
  SDLUtils::checkSDLErr();
  Gu::getCoreContext()->chkErrRt();

  initNet();
  SDLUtils::checkSDLErr();
  Gu::getCoreContext()->chkErrRt();

  if (Gu::getEngineConfig()->getGameHostAttached()) {
    updateWindowHandleForGamehost();
    attachToGameHost();
  }

  //Render System Static Data, which requires a prior context
  BRLogInfo("GraphicsContext - Making Vtx Formats.");
  RenderUtils::makeVertexFormats();
  SDLUtils::checkSDLErr();
  Gu::getCoreContext()->chkErrRt();

  BRLogInfo("Creating Managers.");
  Gu::createManagers();
  SDLUtils::checkSDLErr();
  Gu::getCoreContext()->chkErrRt();

  BRLogInfo("Creating Window Renderer");
  window->initRenderSystem();
  SDLUtils::checkSDLErr();
  Gu::getCoreContext()->chkErrRt();
  //
  //   BRLogInfo("Creating Window 2");
  //   std::shared_ptr<GraphicsWindow> window2 = _pGraphicsApi->createWindow(Stz "Window 2");  //Just avoid title
  //   window2->initRenderSystem();
  //   SDLUtils::checkSDLErr();
  //   Gu::getCoreContext()->chkErrRt();
}
void AppRunner_Internal::doShowError(string_t err, const Exception *  const e ) const {
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
SDL_bool AppRunner_Internal::initAudio() {
  //AUDIO
  if (SDL_AudioInit(NULL) < 0) {
    fprintf(stderr, "Couldn't initialize audio driver: %s\n", SDL_GetError());
    return SDL_FALSE;
  }
  return SDL_TRUE;
}
void AppRunner_Internal::initNet() {
  BRLogInfo("Initializing SDL Net");
  if (SDLNet_Init() == -1) {
    exitApp(Stz "SDL Net could not be initialized: " + SDL_GetError(), -1);
  }
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
  catch (const Exception&  e) {
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

}  // namespace BR2

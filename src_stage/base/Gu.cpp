#include "../base/Logger.h"
#include "../base/SDLIncludes.h"
#include "../base/SDLGLIncludes.h"
#include "../base/SDLUtils.h"
#include "../base/Img32.h"
#include "../base/Allocator.h"
#include "../base/FileSystem.h"
#include "../base/OglErr.h"
#include "../base/EngineConfig.h"
#include "../base/DiskFile.h"
#include "../base/OperatingSystem.h"
#include "../base/DebugHelper.h"
#include "../base/BinaryFile.h"
#include "../base/GraphicsWindow.h"
#include "../base/InputManager.h"
#include "../base/FpsMeter.h"
#include "../base/FrameSync.h"
#include "../base/ApplicationPackage.h"
#include "../base/Logger.h"
#include "../base/Sequencer.h"
#include "../base/SoundCache.h"
#include "../base/FileSystem.h"
#include "../base/EngineConfig.h"
#include "../base/GLContext.h"
#include "../base/Net.h"
#include "../base/ProjectFile.h"
#include "../base/ColoredConsole.h"
#include "../math/Algorithm.h"
#include "../gfx/TexCache.h"
#include "../gfx/RenderViewport.h"
#include "../gfx/GraphicsApi.h"
#include "../gfx/GraphicsContext.h"
#include "../gfx/RenderSettings.h"
#include "../gfx/ShaderMaker.h"
#include "../gfx/UiControls.h"
#include "../gfx/ParticleManager.h"
#include "../gfx/ShaderMaker.h"
#include "../gfx/OpenGLApi.h"
#include "../base/GLContext.h"
#include "../model/ModelCache.h"
#include "../model/VertexTypes.h"
#include "../model/VertexFormat.h"
#include "../ext/lodepng.h"
#include <chrono>
#include <thread>
#include <iostream>

#ifndef BR2_OS_WINDOWS
//For Sigtrap.
#include <signal.h>
#endif

extern "C" {
//nothings commented on Apr 12, 2016
//It's not meant to be #included. Don't #include it, just compile & link it.
extern int stb_vorbis_decode_filename(const char* filename, int* channels, int* sample_rate, short** output);
extern int stb_vorbis_decode_filename(const char* filename, int* channels, int* sample_rate, short** output);
}

//don't put this in a header file
#ifdef __APPLE__
#ifdef TARGET_OS_IPHONE
//#include <objc/objc-runtime.h>
#include <foundation/foundation.h>
//#include <corefoundation/CFBundle.h>
#endif
#endif

namespace VG {
std::shared_ptr<TexCache> Gu::_pTexCache = nullptr;
std::shared_ptr<Sequencer> Gu::_pSequencer = nullptr;
std::shared_ptr<SoundCache> Gu::_pSoundCache = nullptr;
std::shared_ptr<ShaderMaker> Gu::_pShaderMaker = nullptr;
std::shared_ptr<ModelCache> Gu::_pModelCache = nullptr;
std::shared_ptr<ApplicationPackage> Gu::_pPackage = nullptr;
std::shared_ptr<RenderSettings> Gu::_pRenderSettings = nullptr;
std::shared_ptr<GraphicsApi> Gu::_pGraphicsApi = nullptr;
std::shared_ptr<Logger> Gu::_pLogger = nullptr;
std::shared_ptr<EngineConfig> Gu::_pEngineConfig = nullptr;
std::shared_ptr<Net> Gu::_pNet = nullptr;
std::shared_ptr<InputManager> Gu::_pGlobalInput = nullptr;
std::shared_ptr<GLContext> Gu::_pActiveContext = nullptr;
std::shared_ptr<Random> Gu::_pRandom = nullptr;

template <class Tx>
std::shared_ptr<Tx> GetExistingManager(std::shared_ptr<Tx> global_manager) {
  bool manager_exists = global_manager != nullptr;
  if (!manager_exists) {
    int n = 0;
    n++;
  }
  AssertOrThrow2(manager_exists);
  //**TODO: verify that the calling thread is the main thread.  Deadlocks has been causing issues.
  return global_manager;
}

std::shared_ptr<RenderSettings> Gu::getRenderSettings() { return GetExistingManager(_pRenderSettings); }
std::shared_ptr<ApplicationPackage> Gu::getPackage() { return GetExistingManager(_pPackage); }
std::shared_ptr<ModelCache> Gu::getModelCache() { return GetExistingManager(_pModelCache); }
std::shared_ptr<Sequencer> Gu::getSequencer() { return GetExistingManager(_pSequencer); }
std::shared_ptr<SoundCache> Gu::getSound() { return GetExistingManager(_pSoundCache); }
std::shared_ptr<TexCache> Gu::getTexCache() { return GetExistingManager(_pTexCache); }
std::shared_ptr<ShaderMaker> Gu::getShaderMaker() { return GetExistingManager(_pShaderMaker); }
std::shared_ptr<EngineConfig> Core::config() { return GetExistingManager(_pEngineConfig); }
std::shared_ptr<Logger> Gu::getLogger() { return GetExistingManager(_pLogger); }
std::shared_ptr<GraphicsApi> Gu::getGraphicsApi() { return GetExistingManager(_pGraphicsApi); }
std::shared_ptr<EngineConfig> Gu::getConfig() { return GetExistingManager(_pEngineConfig); }
std::shared_ptr<Net> Gu::getNet() { return GetExistingManager(_pNet); }
std::shared_ptr<Random> Gu::getRandom() { return GetExistingManager(_pRandom); }
std::shared_ptr<InputManager> Gu::getGlobalInput() { return GetExistingManager(_pGlobalInput); }
std::shared_ptr<SoundInst> Gu::playSound(const string_t& filename, const SoundPlayInfo& inf){
  return Gu::getSound()->tryPlay(filename, inf);
}
std::shared_ptr<GLContext> Gu::getCoreContext() {
  std::shared_ptr<GraphicsApi> api = Gu::getGraphicsApi();
  std::shared_ptr<OpenGLApi> oglapi = std::dynamic_pointer_cast<OpenGLApi>(Gu::getGraphicsApi());
  if (!oglapi || oglapi->getCoreContext() == nullptr) {
    BRLogErrorOnce("Tried to retrieve a null Core Context. Application may fail.");
    return nullptr;
  }
  return oglapi->getCoreContext()->getThis<GLContext>();
}
void Gu::setGraphicsApi(std::shared_ptr<GraphicsApi> api) {
  AssertOrThrow2(api != nullptr);
  _pGraphicsApi = api;
}
void Gu::checkErrorsDbg(bool ignore) { Gu::getCoreContext()->chkErrDbg(false, false, "", ignore); }
void Gu::checkErrorsRt(bool ignore) { Gu::getCoreContext()->chkErrRt(false, false, "", ignore); }
bool Gu::is64Bit() {
  if (sizeof(size_t) == 8) {
    return true;
  }
  else if (sizeof(size_t) == 4) {
    return false;
  }
  else {
    BRThrowNotImplementedException();
  }
}
void parsearg(std::string key, std::string value) {
  if (key == "--show-console") {
    Core::config()->setShowConsole(VG::TypeConv::strToBool(value));
    BRLogInfo("Overriding show console window: " + value);
  }
  else if (key == "--game-host") {
    Core::config()->setGameHostAttached(VG::TypeConv::strToBool(value));
    BRLogInfo("Overriding game host: " + value);
  }
  else {
    BRLogWarn("Unrecognized parameter '" + key + "' value ='" + value + "'");
  }
}
//**TODO Move to AppRunner
void parsearg(std::string arg, string_t& __out_ out_key, string_t& __out_ out_value) {
  bool isvalue = false;
  std::string key = "";
  std::string value = "";
  //**TODO Move this to AppRunner

  for (int i = 0; i < arg.length(); ++i) {
    if (arg[i] == '=') {
      isvalue = true;
    }
    else if (isvalue) {
      value += arg[i];
    }
    else {
      key += arg[i];
    }
    //**TODO Move to AppRunner
  }

  out_key = StringUtil::trim(key);
  out_value = StringUtil::trim(value);
}
bool Gu::checkArg(const std::vector<string_t>& args, string_t inkey, string_t invalue) {
  string_t key, value;
  for (std::string arg : args) {
    parsearg(arg, key, value);
    if (StringUtil::equalsi(key, inkey)) {
      if (StringUtil::equalsi(value, invalue)) {
        return true;
      }
      else {
        return false;
      }
      break;
    }
  }
  return false;
}
void Gu::createLogger(const string_t& logfile_dir, const std::vector<string_t>& args) {
  //These are essentially the system defaults
  bool log_async = true;
  bool disabled = false;
  if (Gu::checkArg(args, "log_async", "true")) {
    log_async = true;
  }
  else if (Gu::checkArg(args, "log_async", "false")) {
    log_async = false;
  }
  if (Gu::checkArg(args, "log_disable", "true")) {
    disabled = true;
  }
  else if (Gu::checkArg(args, "log_disable", "false")) {
    disabled = false;
  }
  Gu::_pLogger = std::make_shared<Logger>(log_async, disabled);
  Gu::_pLogger->init(logfile_dir);
}
void processArg(std::string key, std::string value) {
  if (key == "--show-console") {
    Core::config()->setShowConsole(VG::TypeConv::strToBool(value));
    BRLogInfo("Overriding show console window: " + value);
  }
  else if (key == "--game-host") {
    Core::config()->setGameHostAttached(VG::TypeConv::strToBool(value));
    BRLogInfo("Overriding game host: " + value);
  }
  else {
    BRLogWarn("Unrecognized parameter '" + key + "' value ='" + value + "'");
  }
}
void Gu::initGlobals(const std::vector<std::string>& args) {
  //Config
  loadConfig(args);

  //Override EngineConfig
  for (std::string arg : args) {
    string_t key, value;
    parsearg(arg, key, value);
    processArg(key, value);
  }

  //Setup Global Configuration
  getLogger()->enableLogToFile(Core::config()->getEnableLogToFile());
  getLogger()->enableLogToConsole(Core::config()->getEnableLogToConsole());

  //Print some environment Diagnostics
  BRLogInfo(Stz "Operating System: " + Gu::getOperatingSystemName());
  BRLogInfo(Stz "C++ Version: " + Gu::getCPPVersion());

  if (Core::config()->getShowConsole() == false) {
    OperatingSystem::hideConsole();
  }
  else {
    OperatingSystem::showConsole();
  }
}
void Gu::loadConfig(const std::vector<std::string>& args) {
  string_t configPath = ApplicationPackage::getEngineConfigFilePath();
  BRLogInfo("Loading config from '" + configPath + "'");
  if (!FileSystem::fileExists(configPath)) {
    BRThrowException("Engine configuration file '" + configPath + "' does not exist.");
  }
  else {
    std::shared_ptr<EngineConfig> ef = std::make_shared<EngineConfig>();
    ef->loadAndParse(configPath);
    Gu::_pEngineConfig = ef;
  }

  //Override the EngineConfig
  for (std::string arg : args) {
    string_t key, value;
    parsearg(arg, key, value);
    processArg(key, value);
  }
}
//void Gu::setContext(std::shared_ptr<GraphicsContext> rb) {
//    Gu::_pContext = rb;
//}
void Gu::deleteManagers() {
  //This must be called in order to delete these in order.
  //Teh GL context needs to come at the end.
  _pTexCache = nullptr;
  _pSequencer = nullptr;

  _pSoundCache = nullptr;
  _pShaderMaker = nullptr;
  _pModelCache = nullptr;
  _pRenderSettings = nullptr;
  _pEngineConfig = nullptr;

  //System Level
  _pGlobalInput = nullptr;
  _pPackage = nullptr;
  _pLogger = nullptr;
  _pNet = nullptr;
  _pRandom = nullptr;

  _pGraphicsApi = nullptr;
}

bool Gu::isBigEndian() {
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  return true;
#else
  return false;
#endif
}


//
//float Gu::frand() {
//    return mint(
//    return ((float)(rand() % 1000)*0.001f);
//}
//float Gu::frand1() {
//    return ((float)(-1000 + (rand() % 2000))*0.001f);
//}
//vec3 Gu::rv3() {
//    return vec3(Gu::frand1(), Gu::frand1(), Gu::frand1());
//}

void Gu::inlineDrawBoxCont(const Box3f* b) {
#ifdef _WIN32
  glVertex3f(b->_min.x, b->_min.y, b->_min.z);
  glVertex3f(b->_max.x, b->_min.y, b->_min.z);

  glVertex3f(b->_min.x, b->_min.y, b->_min.z);
  glVertex3f(b->_min.x, b->_max.y, b->_min.z);

  glVertex3f(b->_min.x, b->_min.y, b->_min.z);
  glVertex3f(b->_min.x, b->_min.y, b->_max.z);

  glVertex3f(b->_max.x, b->_max.y, b->_max.z);
  glVertex3f(b->_min.x, b->_max.y, b->_max.z);

  glVertex3f(b->_max.x, b->_max.y, b->_max.z);
  glVertex3f(b->_max.x, b->_min.y, b->_max.z);

  glVertex3f(b->_max.x, b->_max.y, b->_max.z);
  glVertex3f(b->_max.x, b->_max.y, b->_min.z);

  glVertex3f(b->_max.x, b->_max.y, b->_min.z);
  glVertex3f(b->_min.x, b->_max.y, b->_min.z);
  glVertex3f(b->_max.x, b->_max.y, b->_min.z);
  glVertex3f(b->_max.x, b->_min.y, b->_min.z);

  glVertex3f(b->_min.x, b->_max.y, b->_max.z);
  glVertex3f(b->_min.x, b->_min.y, b->_max.z);
  glVertex3f(b->_min.x, b->_min.y, b->_max.z);
  glVertex3f(b->_max.x, b->_min.y, b->_max.z);

  glVertex3f(b->_min.x, b->_max.y, b->_min.z);
  glVertex3f(b->_min.x, b->_max.y, b->_max.z);

  glVertex3f(b->_max.x, b->_min.y, b->_min.z);
  glVertex3f(b->_max.x, b->_min.y, b->_max.z);
#endif
}

float Gu::fade(float t) {
  //T = [0,1]
  // Fade function as defined by Ken Perlin.  This eases coordinate values
  // so that they will ease towards integral values.  This ends up smoothing
  // the final output.
  return 6 * powf(t, 5) - 15 * powf(t, 4) + 10 * powf(t, 3);
  //  return 6*(t*t*t*t*t) - 15*(t*t*t*t) + 10*(t*t*t);         // 6t^5 - 15t^4 + 10t^3
}


void Gu::guiQuad2d(Box2f& pq, std::shared_ptr<RenderViewport> vp) {
  //Transforms a quad for the matrix-less Gui projection.

  //The resulting coordinates for the GPU are -0.5 +0.5 in both axes with the center being in the center of the screen
  //Translate a 2D screen quad to be rendered in a shader.
  //So* our quad is from TOP Left - OpenGL is Bottom Left - this fixes this.
  float w = (float)vp->getWidth();
  float w2 = w * 0.5f;
  float h = (float)vp->getHeight();
  float h2 = h * 0.5f;

  //Subtract from viewport center
  pq._p0.x -= w2;
  pq._p1.x -= w2;

  //Invert text to show rightsize up and divide by perspective
  pq._p0.x = pq._p0.x / w2;
  pq._p0.y = (h2 - pq._p0.y - 1) / h2;
  pq._p1.x = pq._p1.x / w2;
  pq._p1.y = (h2 - pq._p1.y - 1) / h2;
}


void Gu::createManagers(std::shared_ptr<GLContext> ct) {
  _pRenderSettings = RenderSettings::create();
  _pRandom = std::make_shared<Random>();

  BRLogInfo("GLContext - Creating TexCache");
  _pTexCache = std::make_shared<TexCache>(ct);
  SDLUtils::Base::checkErrors()();
  ct->chkErrRt();

  BRLogInfo("GLContext - Creating Sequencer");
  _pSequencer = std::make_shared<Sequencer>();
  SDLUtils::Base::checkErrors()();
  ct->chkErrRt();

  BRLogInfo("GLContext - Creating Global Input");
  _pGlobalInput = std::make_shared<InputManager>();
  SDLUtils::Base::checkErrors()();
  ct->chkErrRt();

  BRLogInfo("GLContext - Creating SoundCache");
  _pSoundCache = std::make_shared<SoundCache>();
  SDLUtils::Base::checkErrors()();
  ct->chkErrRt();

  BRLogInfo("GLContext - Creating ShaderMaker & base shaders");
  _pShaderMaker = std::make_shared<ShaderMaker>(ct);
  _pShaderMaker->initialize();
  SDLUtils::Base::checkErrors()();
  ct->chkErrRt();

  BRLogInfo("GLContext - Model Cache");
  _pModelCache = std::make_shared<ModelCache>(ct);
  SDLUtils::Base::checkErrors()();
  ct->chkErrRt();

  BRLogInfo("GLContext - Network");
  _pNet = std::make_shared<Net>();
  SDLUtils::Base::checkErrors()();
  ct->chkErrRt();

}
void Gu::updateManagers() {
  if (_pSequencer != nullptr) {
    _pSequencer->update();
  }

  if (_pSoundCache != nullptr) {
    _pSoundCache->update();
  }
  if (_pNet != nullptr) {
    _pNet->update();
  }

}


}  // namespace VG

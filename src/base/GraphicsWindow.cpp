#include "../base/OperatingSystemHeader.h"
#include "../base/Logger.h"
#include "../base/EngineConfig.h"
#include "../base/GLContext.h"
#include "../base/ApplicationPackage.h"
#include "../base/Gu.h"
#include "../base/SDLUtils.h"
#include "../base/GraphicsWindow.h"
#include "../base/Delta.h"
#include "../base/FpsMeter.h"
#include "../base/FrameSync.h"
#include "../base/Perf.h"
#include "../base/InputManager.h"
#include "../gfx/UiControls.h"
#include "../gfx/RenderUtils.h"
#include "../gfx/RenderViewport.h"
#include "../gfx/GraphicsApi.h"
#include "../gfx/RenderPipe.h"
#include "../base/SDLIncludes.h"
#include "../world/Scene.h"
#include "../world/PhysicsWorld.h"

namespace BR2 {
#pragma region GraphicsWindow_Internal
class GraphicsWindow_Internal {
public:
  std::shared_ptr<RenderViewport> _pViewport = nullptr;
  std::shared_ptr<Scene> _pScene = nullptr;
  std::shared_ptr<RenderPipe> _pRenderPipe = nullptr;  // This may be null if shared with a parent window.
  std::shared_ptr<GraphicsApi> _pApi = nullptr;
  std::shared_ptr<FrameSync> _pFrameSync = nullptr;
  std::shared_ptr<FpsMeter> _pFpsMeter = nullptr;
  std::shared_ptr<Delta> _pDelta = nullptr;
  std::shared_ptr<InputManager> _pInput = nullptr;

  FrameState _eFrameState = FrameState::NoFrameState;

  string_t _title = "window";

  SDL_Window* _pSDLWindow = nullptr;
  bool _bFullscreen = false;
  uint32_t _iLastWidth = 0;
  uint32_t _iLastHeight = 0;
  uint32_t _iFullscreenToggleWidth = 0;
  uint32_t _iFullscreenToggleHeight = 0;

  GraphicsWindow::ChildWindows _mapChildren;
  std::shared_ptr<GraphicsWindow> _pParent = nullptr;

  void printHelpfulDebug();
  void beginRender(std::shared_ptr<GraphicsWindow> cont);
  void endRender();
  void updateWidthHeight(uint32_t w, uint32_t h, bool force);
  void toggleFullscreen();
  void setFrameState(FrameState e) { _eFrameState = e; }
};

void GraphicsWindow_Internal::toggleFullscreen() {
  if (_bFullscreen == false) {
    //get the fullscreen resolution
    int32_t iFsW = Gu::getEngineConfig()->getFullscreenWidth();
    int32_t iFsH = Gu::getEngineConfig()->getFullscreenHeight();
    if (iFsW <= 0 || iFsH <= 0) {
      SDL_DisplayMode DM;
      SDL_GetCurrentDisplayMode(0, &DM);
      iFsW = DM.w;
      iFsH = DM.h;
    }
    //Save pre-fullscreen width/height
    _iFullscreenToggleWidth = _iLastWidth;
    _iFullscreenToggleHeight = _iLastHeight;
    SDL_SetWindowSize(_pSDLWindow, iFsW, iFsH);

    if (SDL_SetWindowFullscreen(_pSDLWindow, SDL_WINDOW_FULLSCREEN) != 0) {
      BRLogError("Failed to go fullscreen.");
      _bFullscreen = false;
    }
    else {
      _bFullscreen = true;
      //_pApp->screenChanged(iFsW, iFsH, _bFullscreen);
    }
  }
  else {
    if (_iFullscreenToggleWidth > 0 && _iFullscreenToggleHeight > 0) {
      //Restore pre-fullscreen width/height
      SDL_SetWindowSize(_pSDLWindow, _iFullscreenToggleWidth, _iFullscreenToggleHeight);
    }
    if (SDL_SetWindowFullscreen(_pSDLWindow, 0) != 0) {
      BRLogError("Failed to exit fullscreen.");
    }
    else {
      _bFullscreen = false;
      //_pApp->screenChanged(_iLastWidth, _iLastHeight, _bFullscreen);
    }
  }
}
void GraphicsWindow_Internal::updateWidthHeight(uint32_t w, uint32_t h, bool bForce) {
  //update view/cam
  if (_iLastWidth != w || bForce) {
    _pViewport->updateBox(0, 0, w, h);

    if (_iLastHeight != h || _iLastWidth != w || bForce) {
      if (_pRenderPipe != nullptr) {
        _pRenderPipe->resizeScreenBuffers((int32_t)w, (int32_t)h);
      }
      //   _pApp->screenChanged(w, h, _bFullscreen);

      //TODO: one Gui per window.
      if (_pScene != nullptr) {
        _pScene->updateWidthHeight(w, h, bForce);
      }
    }
    _iLastWidth = w;
    _iLastHeight = h;
  }
}
void GraphicsWindow_Internal::printHelpfulDebug() {
  int dw, dh;
  SDL_DisplayMode mode;
  SDL_Window* win = _pSDLWindow;

  SDL_GetCurrentDisplayMode(0, &mode);
  BRLogInfo("Screen BPP    : " + SDL_BITSPERPIXEL(mode.format));
  BRLogInfo("Swap Interval : " + SDL_GL_GetSwapInterval());
  SDL_GetWindowSize(win, &dw, &dh);
  BRLogInfo("Initial Window Size   : " + dw + "x" + dh);
  SDL_GL_GetDrawableSize(win, &dw, &dh);
  BRLogInfo("Draw Size     : " + dw + "x" + dh);

  SDLUtils::checkSDLErr();
}
void GraphicsWindow_Internal::beginRender(std::shared_ptr<GraphicsWindow> cont) {
  cont->getContext()->chkErrDbg();
  //Make this window current *critical*
  //OPTIMIZE:TODO:NOTE: if there is only 1 window we don't have to call this.
  _pApi->makeCurrent(cont.get());
  cont->getContext()->chkErrDbg();

  //Update the widnow size
  int w, h;
  _pApi->getDrawableSize(_pSDLWindow, &w, &h);
  cont->getContext()->chkErrDbg();

  updateWidthHeight(w, h, false);
  cont->getContext()->chkErrDbg();

  Perf::pushPerf();  //**BEGIN PERF
}
void GraphicsWindow_Internal::endRender() {
  _pApi->swapBuffers(_pSDLWindow);

  Perf::popPerf();  //**END PERF
}
#pragma endregion

#pragma region GraphicsWindow
//Called exclusively by the graphics API
GraphicsWindow::GraphicsWindow(std::shared_ptr<GLContext> ct) : RenderTarget(ct) {
  _pint = std::make_unique<GraphicsWindow_Internal>();
}
GraphicsWindow::~GraphicsWindow() {
  if (_pint->_pSDLWindow != nullptr) {
    _pint->_pSDLWindow = nullptr;
  }
  _pint = nullptr;
}
void GraphicsWindow::init(std::shared_ptr<GraphicsApi> api, SDL_Window* win, GraphicsWindowCreateParameters&& params) {
  _pint->_iLastWidth = params._width;
  _pint->_iLastHeight = params._height;
  _pint->_pViewport = std::make_shared<RenderViewport>(_pint->_iLastWidth, _pint->_iLastHeight, ViewportConstraint::Full);
  _pint->_pFrameSync = std::make_shared<FrameSync>(getThis<GraphicsWindow>());
  _pint->_pDelta = std::make_shared<Delta>();
  _pint->_pFpsMeter = std::make_shared<FpsMeter>();
  _pint->_pInput = std::make_shared<InputManager>();
  _pint->_pSDLWindow = win;
  _pint->_pApi = api;
  _pint->_pParent = params._parent;
  _pint->_title = params._title;

  if (_pint->_pParent) {
    _pint->_pParent->addChild(getThis<GraphicsWindow>());
  }

  if (params._show) {
    SDL_ShowWindow(_pint->_pSDLWindow);
  }

  SDLUtils::trySetWindowIcon(_pint->_pSDLWindow, Gu::getPackage()->getIconPath());
  SDLUtils::checkSDLErr();
  getContext()->chkErrRt();

  //Set W/H to match Desktop aspect ratio (e.g. 1920/1080)
  if (params._forceAspectRatio) {
    SDL_DisplayMode dm;
    if (SDL_GetDesktopDisplayMode(0, &dm) != 0) {
      BRLogError("SDL_GetDesktopDisplayMode failed: " + SDL_GetError());
    }
    else {
      float ar = (float)dm.h / (float)dm.w;
      int newHeight = (int)((float)_pint->_iLastWidth * ar);
      if (newHeight != _pint->_iLastHeight) {
        BRLogInfo("Adjusting window dims from " + _pint->_iLastWidth + " x " + _pint->_iLastHeight + " to " + _pint->_iLastWidth + " x " + newHeight);
        _pint->_iLastHeight = newHeight;
      }
    }
  }

  BRLogInfo("Setting window size to, " + _pint->_iLastWidth + " x " + _pint->_iLastHeight);
  SDL_SetWindowSize(_pint->_pSDLWindow, _pint->_iLastWidth, _pint->_iLastHeight);
  _pint->updateWidthHeight(_pint->_iLastWidth, _pint->_iLastHeight, true);

  //Fullscreen
  if (params._fullscreen == true) {
    BRLogInfo("Going fullscreen.");
    _pint->toggleFullscreen();
  }

  //Render Pipe
  if (_pint->_pParent) {
    BRLogInfo("'" + getTitle() + "'Sharing Render Pipeline with '" + _pint->_pParent->getTitle() + "'");
  }
  else {
    BRLogInfo("Creating Render Pipeline for '" + getTitle() + "'");
    _pint->_pRenderPipe = std::make_shared<RenderPipe>(getContext(), getThis<GraphicsWindow>());
    _pint->_pRenderPipe->init(getViewport()->getWidth(), getViewport()->getHeight(), Gu::getPackage()->makeAssetPath(Gu::getPackage()->getEnvTextureFolder()));
    _pint->printHelpfulDebug();
    SDLUtils::checkSDLErr();
    getContext()->chkErrRt();
  }
}
void GraphicsWindow::setTitle(const string_t& title) {
  //Update parent/child relations.
  string_t last = getTitle();
  getParent()->removeChildByTitle(last);
  _pint->_title = title;
  SDL_SetWindowTitle(_pint->_pSDLWindow, title.c_str());
  getParent()->addChild(getThis<GraphicsWindow>());
}
std::shared_ptr<GraphicsWindow> GraphicsWindow::removeChildByTitle(const string_t& title) {
  //Removes a child window and returns the removed window.
  std::shared_ptr<GraphicsWindow> ret = nullptr;
  auto ch = _pint->_mapChildren.find(title);
  if (ch == _pint->_mapChildren.end()) {
    BRLogError("Could not find child window named '" + title + "' in parent window '" + getTitle() + "'.");
  }
  else {
    ret = ch->second;
    _pint->_mapChildren.erase(title);
  }
  return ret;
}
void GraphicsWindow::addChild(std::shared_ptr<GraphicsWindow> child) {
  if (_pint->_mapChildren.find(child->getTitle()) != _pint->_mapChildren.end()) {
    BRLogError("Tried to add duplicate child window '" + child->getTitle() + "' to parent '" + getTitle() + "'");
  }
  else {
    _pint->_mapChildren.insert(std::make_pair(child->getTitle(), child));
  }
}
GraphicsWindow::ChildWindows& GraphicsWindow::getChildren() {
  return _pint->_mapChildren;
}
std::shared_ptr<GraphicsWindow> GraphicsWindow::getParent() {
  return _pint->_pParent;
}
FrameState GraphicsWindow::getFrameState() {
  return _pint->_eFrameState;
}
void GraphicsWindow::step() {
  getInput()->preUpdate();
  _pint->_pDelta->update();
  _pint->_pFpsMeter->update();

  _pint->beginRender(getThis<GraphicsWindow>());
  {
    if (getInput()->keyPress(SDL_SCANCODE_F11)) {
      _pint->toggleFullscreen();
    }

    _pint->setFrameState(FrameState::SyncBegin);
    _pint->_pFrameSync->syncBegin();
    {
      if (getScene() != nullptr) {
        _pint->setFrameState(FrameState::Update);
        getScene()->update(_pint->_pDelta->get());

        _pint->setFrameState(FrameState::Render);

        std::shared_ptr<RenderPipe> rp = nullptr;
        if (_pint->_pRenderPipe) {
          rp = _pint->_pRenderPipe;
        }
        else if (_pint->_pParent) {
          rp = _pint->_pParent->getRenderPipe();
        }
        else {
          BRLogErrorCycle("Parent window not found and render pipe not set for window '" + getTitle() + "'. No rendering will occur.");
        }

        if (rp) {
          PipeBits p;
          p.set();
          rp->renderScene(getScene(), getScene()->getPhysicsManager()->getRenderBucket(), getScene()->getActiveCamera(), getScene()->getLightManager(), p);
        }
      }
      else {
        BRLogErrorCycle("Scene was not set on graphics window " + getTitle());
      }
    }
    _pint->setFrameState(FrameState::SyncEnd);
    _pint->_pFrameSync->syncEnd();
  }
  _pint->endRender();

  getInput()->postUpdate();
}
void GraphicsWindow::idle(int64_t us) {
  if (getScene() != nullptr) {
    getScene()->idle(us);
  }
}
string_t GraphicsWindow::getTitle() { return _pint->_title; }
uint64_t GraphicsWindow::getFrameNumber() { return _pint->_pFpsMeter->getFrameNumber(); }
int32_t GraphicsWindow::getWidth() { return _pint->_iLastWidth; }
int32_t GraphicsWindow::getHeight() { return _pint->_iLastHeight; }
SDL_Window* GraphicsWindow::getSDLWindow() { return _pint->_pSDLWindow; }
std::shared_ptr<RenderViewport> GraphicsWindow::getViewport() { return _pint->_pViewport; }
std::shared_ptr<RenderPipe> GraphicsWindow::getRenderPipe() { return _pint->_pRenderPipe; }
std::shared_ptr<Scene> GraphicsWindow::getScene() { return _pint->_pScene; }
std::shared_ptr<FrameSync> GraphicsWindow::getFrameSync() { return _pint->_pFrameSync; }
std::shared_ptr<Delta> GraphicsWindow::getDelta() { return _pint->_pDelta; }
std::shared_ptr<FpsMeter> GraphicsWindow::getFpsMeter() { return _pint->_pFpsMeter; }
std::shared_ptr<InputManager> GraphicsWindow::getInput() { return _pint->_pInput; }
void GraphicsWindow::setScene(std::shared_ptr<Scene> scene) {
  scene->setWindow(getThis<GraphicsWindow>());
  _pint->_pScene = scene;
  static bool x = false;
  if (x == false) {
    scene->afterAttachedToWindow();
    x = true;
  }
}
bool GraphicsWindow::containsPoint_Global2D(const vec2& mp) {
  //Returns true if a point in screen coordinates lies within the window bounds in screen space.
  //TDOO: Check for window border
  // int top, left, bot, right;
  // int ret = SDL_GetWindowBordersSize(getSDLWindow(), &top, &left, &bot, &right);
  int posx, posy;
  SDL_GetWindowPosition(getSDLWindow(), &posx, &posy);
  int sizx, sizy;
  SDL_GetWindowSize(getSDLWindow(), &sizx, &sizy);

  int client_left = posx;
  int client_top = posy;
  int client_right = client_left + sizx;
  int client_bot = client_top + sizy;

  //mp must be a GLOBAL point (mouse pos) getting INputManager::getMousePos_Global
  if (mp.x < client_top) {
    return false;
  }
  if (mp.y < client_left) {
    return false;
  }
  if (mp.x > client_right - 1) {
    return false;
  }
  if (mp.y > client_bot - 1) {
    return false;
  }
  return true;
}

#pragma endregion
}  // namespace BR2

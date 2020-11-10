#include "../base/OglErr.h"
#include "../base/Logger.h"
#include "../base/EngineConfig.h"
#include "../base/Gu.h"
#include "../base/SDLUtils.h"
#include "../base/GLContext.h"
#include "../base/GraphicsWindow.h"
#include "../base/DebugHelper.h"
#include "../base/Delta.h"
#include "../base/Perf.h"
#include "../base/InputManager.h"
#include "../base/GLContext.h"
#include "../gfx/GraphicsApi.h"
#include "../gfx/GraphicsContext.h"
#include "../gfx/RenderPipe.h"

#include <SDL_video.h>

#if defined(BR2_OS_LINUX)
#if defined(SDL_VIDEO_DRIVER_X11)
//Xlib manual https://tronche.com/gui/x/xlib/
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <sys/utsname.h>
#endif
#if defined(SDL_VIDEO_DRIVER_WAYLAND)
//Wayland manual https://wayland.freedesktop.org/docs/html/
//https://gitlab.com/hdante/hello_wayland
#include <wayland-client.h>
#endif
#endif

namespace VG {
GraphicsApi::GraphicsApi() {
}
GraphicsApi::~GraphicsApi() {
  cleanup();
}
void GraphicsApi::updateLoop() {
#ifdef __WINDOWS__
  SDL_ShowCursor(SDL_DISABLE);
#endif
  //TODO: global input is needed.
  //Make the main window the global input.

  while (true) {
    Perf::beginPerf();
    Perf::pushPerf();
    {
      if (handleSDLEvents() == true) {
        break;  //SDL_QUIT
      }

      Gu::updateManagers();

      for (std::shared_ptr<GraphicsContext> ct : _contexts) {
        for (auto w : ct->getGraphicsWindows()) {
          w->step();
        }
        ct->chkErrRt();  //**End of loop error -- Don't Remove**
      }
    }
    Perf::popPerf();
    Perf::endPerf();
    DebugHelper::checkMemory();
  }

  DebugHelper::checkMemory();
}
bool GraphicsApi::handleSDLEvents() {
  SDL_Event event;
  bool done = false;
  while (SDL_PollEvent(&event)) {
    if (handleEvents(&event) == false) {
      done = true;
    }
  }
  return done;
}
std::shared_ptr<InputManager> GraphicsApi::getInputForWindow(uint32_t sdl_windowId) {
  for (std::shared_ptr<GraphicsContext> ct : _contexts) {
    for (auto w : ct->getGraphicsWindows()) {
      Uint32 wid = SDL_GetWindowID(w->getSDLWindow());
      if (sdl_windowId == wid) {
        return w->getInput();
      }
    }
  }
  return nullptr;
}

bool GraphicsApi::handleEvents(SDL_Event* event) {
  // Return false when we get a window_close message, or need to exit the app.
  int n = 0;
  vec2 delta;
  SDL_Scancode keyCode;

  if (event == nullptr) {
    return true;
  }

  std::shared_ptr<InputManager> pInput = getInputForWindow(event->window.windowID);
  if (pInput == nullptr) {
    if (event->type == SDL_AUDIODEVICEADDED) {
      BRLogDebug("SDL_AUDIODEVICEADDED called.");
    }
    else {
      string_t ze = Stz "Window input manager could not be found for window ID " + event->window.windowID + ". Known Ids:";
      string_t appz = "";
      for (auto wc : this->_contexts) {
        for (auto w : wc->getGraphicsWindows()) {
          ze += Stz appz + SDL_GetWindowID(w->getSDLWindow());
          appz = ",";
        }
      }
      ze += Stz " event=" + event->type;
      BRLogWarn(ze);
    }
    return true;
  }

  //event->key.keysym.sym;

  switch (event->type) {
    case SDL_MOUSEMOTION:
      //Don't use this.  Because of "SDL_warpMouse" which will mess stuff up.
      break;
    case SDL_KEYDOWN:
      keyCode = event->key.keysym.scancode;
      pInput->setKeyDown(keyCode);
      Gu::getGlobalInput()->setKeyDown(keyCode);
      break;
    case SDL_KEYUP:
      keyCode = event->key.keysym.scancode;
      pInput->setKeyUp(keyCode);
      Gu::getGlobalInput()->setKeyUp(keyCode);
      break;
    case SDL_MOUSEBUTTONDOWN:
      switch (event->button.button) {
        case SDL_BUTTON_LEFT:
          pInput->setLmbState(ButtonState::Press);
          Gu::getGlobalInput()->setLmbState(ButtonState::Press);
          break;
        case SDL_BUTTON_MIDDLE:
          break;
        case SDL_BUTTON_RIGHT:
          pInput->setRmbState(ButtonState::Press);
          Gu::getGlobalInput()->setRmbState(ButtonState::Press);
          break;
      }
      break;
    case SDL_MOUSEBUTTONUP:
      switch (event->button.button) {
        case SDL_BUTTON_LEFT:
          pInput->setLmbState(ButtonState::Release);
          Gu::getGlobalInput()->setLmbState(ButtonState::Release);
          break;
        case SDL_BUTTON_MIDDLE:
          break;
        case SDL_BUTTON_RIGHT:
          pInput->setRmbState(ButtonState::Release);
          Gu::getGlobalInput()->setRmbState(ButtonState::Release);
          break;
      }
      break;
    case SDL_WINDOWEVENT:
      switch (event->window.event) {
        case SDL_WINDOWEVENT_CLOSE:
          return false;
          break;
      }
      break;
    case SDL_MOUSEWHEEL:
      if (event->wheel.y != 0) {
        int n = Math::brMin(10, Math::brMax(-10, event->wheel.y));
        pInput->setMouseWheel(n);
        Gu::getGlobalInput()->setMouseWheel(n);
      }
      if (event->wheel.x != 0) {
        n++;
      }
      break;
    case SDL_QUIT:
      BRLogTODO(" only exit if user closed main window.");
      return false;
      break;
  }

  return true;
}
void GraphicsApi::destroyWindow(std::shared_ptr<GraphicsWindow> w) {
  SDL_DestroyWindow(w->getSDLWindow());
  w = nullptr;
}
SDL_Window* GraphicsApi::makeSDLWindow(const GraphicsWindowCreateParameters& params, int render_system, bool show) {
  string_t title;
  bool bFullscreen = false;
  SDL_Window* ret = nullptr;

  int style_flags = 0;
  style_flags |= (show ? SDL_WINDOW_SHOWN : SDL_WINDOW_HIDDEN);
  if (params._type == GraphicsWindowCreateParameters::Wintype_Desktop) {
    style_flags |= SDL_WINDOW_RESIZABLE;
  }
  else if (params._type == GraphicsWindowCreateParameters::Wintype_Utility) {
  }
  else if (params._type == GraphicsWindowCreateParameters::Wintype_Noborder) {
    style_flags |= SDL_WINDOW_BORDERLESS;
  }

  int x = params._x;
  int y = params._y;
  int w = params._width;
  int h = params._height;

#ifdef BR2_OS_IPHONE
  int flags = SDL_WINDOW_BORDERLESS | SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL;
  title = "";
#elif defined(BR2_OS_WINDOWS) || defined(BR2_OS_LINUX)
  int flags = style_flags | render_system;
  title = params._title;
#else
  OS_NOT_SUPPORTED_ERROR
#endif

  //Note: This calls SDL_GL_LOADLIBRARY if SDL_WINDOW_OPENGL is set.
  ret = SDL_CreateWindow(title.c_str(), x, y, w, h, flags);
  if (ret != nullptr) {
    //On Linux SDL will set an error if unable to represent a GL/Vulkan profile, as we try different ones. Ignore them for now.
    //Windows SDL sets an error when the context is created.
    SDLUtils::Base::checkErrors()();

    //Fullscreen nonsense
    if (bFullscreen) {
      SDL_SetWindowFullscreen(ret, SDL_WINDOW_FULLSCREEN);
    }
    SDLUtils::Base::checkErrors()();
    if (show) {
      SDL_ShowWindow(ret);
    }
    SDLUtils::Base::checkErrors()();
  }
  else {
    //Linux: Couldn't find matching GLX visual.
    SDLUtils::Base::checkErrors()(true, false);
  }

  //Customize window (per display system)
  //setWindowProps(ret, params);

  return ret;
}
void GraphicsApi::setWindowProps(SDL_Window* win, const GraphicsWindowCreateParameters& params) {
  //Customize the window based on the supported windowing system.

  //Best way to handle this is likely to just keep the buttons visible and stub their events.

  if (!win) {
    return;
  }
  SDL_SysWMinfo wmInfo;
  SDL_VERSION(&wmInfo.version);
  SDL_GetWindowWMInfo(win, &wmInfo);

  if (wmInfo.subsystem == SDL_SYSWM_X11) {
#if defined(BR2_OS_LINUX)
#if defined(SDL_VIDEO_DRIVER_X11)
    auto d = wmInfo.info.x11.display;
    auto w = wmInfo.info.x11.window;
    //_NET_WM_WINDOW_TYPE_TOOLBAR - Hides Maximize
    //_NET_WM_WINDOW_TYPE_DOCK - Hides border
    //_NET_WM_WINDOW_TYPE_UTILITY - Show only Close
    //_NET_WM_WINDOW_TYPE_SPLASH - hides maximize
    //_NET_WM_WINDOW_TYPE_DIALOG - show only close no resize
    //_NET_WM_WINDOW_TYPE_NOTIFICATION - no border
    //_NET_WM_WINDOW_TYPE_DESKTOP - sticks window to the desktop background..
    //https://www.geeks3d.com/20120102/programming-tutorial-simple-x11-x-window-code-sample-for-linux-and-mac-os-x/
    // Atom WM_DELETE_WINDOW = XInternAtom(d, "WM_DELETE_WINDOW", False);
    // XSetWMProtocols(d, w, &WM_DELETE_WINDOW, 1);

    // if (params._type == GraphicsWindowCreateParameters.Wintype_Desktop) {
    //   Atom window_type = XInternAtom(d, "_NET_WM_WINDOW_TYPE", False);
    //   long value = XInternAtom(d, "_NET_WM_WINDOW_TYPE_DESKTOP", False);
    //   XChangeProperty(d, w, window_type, XA_ATOM, 32, PropModeReplace, (unsigned char*)&value, 1);
    // }
    // else
    //Hide min/max for utility windows.
    // if (params._type == GraphicsWindowCreateParameters.Wintype_Utility) {
    //   Atom window_type = XInternAtom(d, "_NET_WM_WINDOW_TYPE", False);
    //   long value = XInternAtom(d, "_NET_WM_WINDOW_TYPE_UTILITY", False);
    //   XChangeProperty(d, w, window_type, XA_ATOM, 32, PropModeReplace, (unsigned char*)&value, 1);
    // }
    //Borderless handled in window create params._type == GraphicsWindowCreateParameters.Wintype_Borderless
#endif
#endif
  }
  else if (wmInfo.subsystem == SDL_SYSWM_WAYLAND) {
#if defined(BR2_OS_LINUX)
#if defined(SDL_VIDEO_DRIVER_WAYLAND)
    // auto d = wmInfo.info.wl.display;
    // auto h = wmInfo.info.wl.shell_surface;
    // auto s = wmInfo.info.wl.surface;
    // auto registry = wl_display_get_registry(d);
#endif
#endif
  }
  else if (wmInfo.subsystem == SDL_SYSWM_WINDOWS) {
#if defined(BR2_OR_WINDOWS)
#if defined(SDL_VIDEO_DRIVER_WINDOWS)
    //https://stackoverflow.com/questions/13189791/disable-x-button-icon-on-the-top-of-the-right-in-messagebox-using-c-win32-api
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(ret, &wmInfo);
    HWND hwnd = wmInfo.info.win.window;
    if (!params._closeButton) {
      LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);      //get current style
      SetWindowLongPtr(hwnd, GWL_STYLE, style & ~WS_SYSMENU);  //remove system menu
      WaitForSingleObject(thread, INFINITE);                   //view the effects until you close it
    }
    if (!params._minButton) {
      LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);          //get current style
      SetWindowLongPtr(hwnd, GWL_STYLE, style & ~WS_MINIMIZEBOX);  //remove system menu
      WaitForSingleObject(thread, INFINITE);                       //view the effects until you close it
    }
#endif
#endif
  }
  else {
    BRLogWarn("Failed to set window details. Unsupported windowing system: " + wmInfo.subsystem);
  }
}

}  // namespace VG

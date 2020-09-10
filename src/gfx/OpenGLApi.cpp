#include "../base/Exception.h"
#include "../base/Logger.h"
#include "../base/OglErr.h"
#include "../base/GLContext.h"
#include "../base/GraphicsWindow.h"
#include "../base/EngineConfig.h"
#include "../base/OpenGLWindow.h"
#include "../base/SDLUtils.h"
#include "../gfx/OpenGLApi.h"

namespace BR2 {
OpenGLApi::OpenGLApi() {
}
OpenGLApi::~OpenGLApi() {
}
void OpenGLApi::cleanup() {
}

std::shared_ptr<GraphicsWindow> OpenGLApi::createWindow(std::string title) {
  //Create GL Context.
  std::shared_ptr<GraphicsWindow> pRet = nullptr;

  int minGLVersion;
  int minGLSubversion;
  std::vector<std::shared_ptr<GLProfile>> profs;
  bool bVsync = false;

  int iProfile = SDL_GL_CONTEXT_PROFILE_CORE;
#ifdef BR2_OS_IPHONE
  minGLVersion = 3;
  minGLSubversion = 0;
  iProfile = SDL_GL_CONTEXT_PROFILE_ES;
#else
  minGLVersion = c_iCurrentOpenGLVersion;
  minGLSubversion = c_iCurrentOpenGLSubVersion;

  OpenGLProfile eConfigProfile = Gu::getEngineConfig()->openGLProfile();

  iProfile = SDL_GL_CONTEXT_PROFILE_CORE;
  if (eConfigProfile == OpenGLProfile::Compatibility) {
    iProfile = SDL_GL_CONTEXT_PROFILE_COMPATIBILITY;
  }
  else if (eConfigProfile == OpenGLProfile::Core) {
    iProfile = SDL_GL_CONTEXT_PROFILE_CORE;
  }
  else if (eConfigProfile == OpenGLProfile::ES) {
    iProfile = SDL_GL_CONTEXT_PROFILE_ES;
  }
#endif
  //Attempt to make a big depth buffer.
  std::vector<int> depth_sizes({32, 24, 16});

  int msaa_buf = 0;
  int msaa_samples = 0;
  if (Gu::getEngineConfig()->getEnableMsaa()) {
    msaa_buf = 1;
    msaa_samples = Gu::getEngineConfig()->getMsaaSamples();
  }

  //This is the 'optimal' context.
  if (iProfile == SDL_GL_CONTEXT_PROFILE_ES) {
    for (auto depth : depth_sizes) {
      for (int srgb = 1; srgb >= 0; --srgb) {
        profs.push_back(std::make_shared<GLProfile>(depth, 3, 3, iProfile, bVsync, msaa_buf, msaa_samples, (bool)srgb));
        profs.push_back(std::make_shared<GLProfile>(depth, 3, 0, iProfile, bVsync, msaa_buf, msaa_samples, (bool)srgb));
      }
    }
  }
  else {
    //Loop over all OpenGL versions and create highest one, with the highest bit Depth Buffer.
    for (int ver = minGLVersion; ver >= 0; --ver) {
      for (int sub = minGLSubversion; sub >= 0; --sub) {
        for (auto depth : depth_sizes) {
          for (int srgb = 1; srgb >= 0; --srgb) {
            profs.push_back(std::make_shared<GLProfile>(depth, ver, sub, iProfile, bVsync, msaa_buf, msaa_samples, (bool)srgb));
          }
        }
      }
    }
  }

  for (std::shared_ptr<GLProfile> prof : profs) {
    //in general you can't change the value of SDL_GL_CONTEXT_PROFILE_MASK without first destroying all windows created with the previous setting.
    BRLogInfo("Profile: " + prof->toString());
    try {
      SDL_Init(SDL_INIT_VIDEO);

      //This must be called before creating the window because this sets SDL's PixelFormatDescritpro
      GLContext::setWindowAndOpenGLFlags(prof);
      SDLUtils::checkSDLErr();

      //This fails on Linux for invalid profiles, but succeeds on windows.
      SDL_Window* win = makeSDLWindow(title, SDL_WINDOW_OPENGL, false);
      if (win != nullptr) {
        std::shared_ptr<GLContext> context = std::make_shared<GLContext>(getThis<GraphicsApi>(), prof, win);
        if (context->valid()) {
          pRet = context->getGraphicsWindow();
          if (pRet != nullptr) {
            SDL_GL_SetSwapInterval(prof->_bVsync ? 1 : 0);  //Vsync is automatic on IOS
            SDLUtils::checkSDLErr();

            SDL_ShowWindow(win);
            SDLUtils::checkSDLErr();

            //Context created successfully
            setMainContext(context);
            getContexts().push_back(context);
            break;
          }
        }
        SDL_DestroyWindow(win);
      }
    }
    catch (Exception* ex) {
      BRLogError("Error during GL context creation: " + ex->what());
    }
  }

  if (this->getCoreContext() == nullptr) {
    BRThrowException("Failed to create OpenGL context.  See errors in log.");
  }

  return pRet;
}
void OpenGLApi::makeCurrent(SDL_Window* win) {
  SDL_GL_MakeCurrent(win, Gu::getCoreContext()->getSDLGLContext());
}
void OpenGLApi::getDrawableSize(SDL_Window* win, int* w, int* h) {
  SDL_GL_GetDrawableSize(win, w, h);
}
void OpenGLApi::swapBuffers(SDL_Window* win) {
  SDL_GL_SwapWindow(win);
}

}  // namespace BR2

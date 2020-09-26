#include "../base/Exception.h"
#include "../base/Logger.h"
#include "../base/OglErr.h"
#include "../base/GLContext.h"
#include "../base/GraphicsWindow.h"
#include "../base/EngineConfig.h"
#include "../base/SDLUtils.h"
#include "../gfx/OpenGLApi.h"
#include "../gfx/RenderUtils.h"

namespace BR2 {
OpenGLApi::OpenGLApi() {
}
OpenGLApi::~OpenGLApi() {
}
void OpenGLApi::cleanup() {
}
std::vector<std::shared_ptr<GLProfile>> OpenGLApi::getProfiles() {
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
  //Reduce aliasing & z fighting.
  std::vector<int> depth_sizes({32, 24, 16});

  int msaa_buf = 0;
  int msaa_samples = 0;
  if (Gu::getEngineConfig()->getEnableMSAA()) {
    msaa_buf = 1;
    msaa_samples = Gu::getEngineConfig()->getMsaaSamples();
  }
  bVsync = Gu::getEngineConfig()->getVsyncEnabled();

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

  return profs;
}
std::shared_ptr<GraphicsWindow> OpenGLApi::createWindow(GraphicsWindowCreateParameters&& params) {
  std::shared_ptr<GraphicsWindow> pRet = nullptr;
  if (params._parent != nullptr) {
    if (params._parent->getContext()) {
      //Try to make window from the profile we used for a previous window.
      pRet = createWindowFromProfile(params._parent->getContext()->getProfile(), std::move(params));
      if (pRet == nullptr) {
        BRLogError("Couldn't create a window from the default window profile.");
      }
    }
    else {
      BRLogError("Window " + params._parent->getTitle() + " context had no profile.");
    }
  }

  if (pRet == nullptr) {
    auto profs = getProfiles();
    for (std::shared_ptr<GLProfile> prof : profs) {
      //in general you can't change the value of SDL_GL_CONTEXT_PROFILE_MASK without first destroying all windows created with the previous setting.
      pRet = createWindowFromProfile(prof, std::move(params));
      if (pRet != nullptr) {
        break;
      }
    }
  }

  if (this->getCoreContext() == nullptr) {
    BRThrowException("Failed to create OpenGL context.  See errors in log.");
  }

  if (getCoreContext()) {
    getCoreContext()->chkErrRt();
  }
  SDLUtils::checkSDLErr();

  return pRet;
}
std::shared_ptr<GraphicsWindow> OpenGLApi::createWindowFromProfile(std::shared_ptr<GLProfile> prof, GraphicsWindowCreateParameters&& params) {
  // Create an OpenGL enabled window from a spec profile.
  // @return An OpenGL enabled graphics window of the given profile, or nullptr if the profile wasn't compatible.
  AssertOrThrow2(prof != nullptr);
  BRLogInfo("Profile: " + prof->toString());
  std::shared_ptr<GraphicsWindow> pRet = nullptr;
  try {
    //This must be called before creating the window because this sets SDL's PixelFormatDescriptor.
    GLContext::setWindowAndOpenGLFlags(prof);
    SDLUtils::checkSDLErr();

    SDL_Window* win = makeSDLWindow(params._title, SDL_WINDOW_OPENGL, false);
    if (win != nullptr) {
      if (params._parent) {
        //Share a context, GBuffer
        std::shared_ptr<GLContext> context = getCoreContext()->getThis<GLContext>();
        pRet = context->createGraphicsWindow(win, std::move(params));
      }
      else {
        //Create a new context,and GBuffer.
        std::shared_ptr<GLContext> context = std::make_shared<GLContext>(getThis<GraphicsApi>(), prof);
        if (context->init(win)) {
          getContexts().push_back(context);

          if (getContexts().size() == 1) {
            //This is the core context.
            BRLogInfo("First Context Initialized.");
            BRLogInfo("GraphicsContext - Making Vtx Formats.");
            RenderUtils::makeVertexFormats();
            BRLogInfo("Creating Managers.");
            Gu::createManagers(context);
          }

          pRet = context->createGraphicsWindow(win, std::move(params));
        }
        else {
          SDL_DestroyWindow(win);
          pRet = nullptr;
        }
      }
    }
  }
  catch (const Exception& ex) {
    BRLogError("Error during GL context creation: " + ex.what());
    pRet = nullptr;
  }
  return pRet;
}
void OpenGLApi::makeCurrent(GraphicsWindow* win) {
  SDL_GL_MakeCurrent(win->getSDLWindow(), win->getContext()->getSDLGLContext());
  if (Gu::getActiveContext() != win->getContext()) {
    Gu::setActiveContext(getCoreContext()->getThis<GLContext>());
  }
}
void OpenGLApi::getDrawableSize(SDL_Window* win, int* w, int* h) {
  SDL_GL_GetDrawableSize(win, w, h);
}
void OpenGLApi::swapBuffers(SDL_Window* win) {
  SDL_GL_SwapWindow(win);
}
void OpenGLApi::destroyWindow(std::shared_ptr<GraphicsWindow> win) {
  //Destroy children
  for (auto child : win->getChildren()) {
    GraphicsApi::destroyWindow(child.second);
  }

  //SDL_GL_DeleteContext automatically happens in the window's context.
  GraphicsApi::destroyWindow(win);
}

}  // namespace BR2

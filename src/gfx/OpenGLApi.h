/**
*  @file OpenGLApi.h
*  @date August 5, 2019
*  @author MetalMario971
*/
#pragma once
#ifndef __OPENGLGraphicsApi_15650573213471462786_H__
#define __OPENGLGraphicsApi_15650573213471462786_H__

#include "../gfx/GfxHeader.h"
#include "../gfx/GraphicsApi.h"

namespace BR2 {
/**
*  @class OpenGLApi
*  @brief Handles the OpenGL API.
*/
class OpenGLApi : public GraphicsApi {
public:
  OpenGLApi();
  virtual ~OpenGLApi() override;

  const int c_iCurrentOpenGLVersion = 4;
  const int c_iCurrentOpenGLSubVersion = 5;

  virtual std::shared_ptr<GraphicsWindow> createWindow(const string_t& title, std::shared_ptr<GraphicsWindow> parent = nullptr) override;
  virtual void makeCurrent(GraphicsWindow*) override;
  virtual void getDrawableSize(SDL_Window* win, int* w, int* h) override;
  virtual void swapBuffers(SDL_Window* win) override;
  virtual void cleanup() override;
  virtual void destroyWindow(std::shared_ptr<GraphicsWindow> win) override;

private:
  std::vector<std::shared_ptr<GLProfile>> getProfiles();
  std::shared_ptr<GraphicsWindow> createWindowFromProfile(std::shared_ptr<GLProfile> prof, const string_t& title);
  std::shared_ptr<GLProfile> _pDefaultCompatibleProfile = nullptr;
};

}  // namespace BR2

#endif

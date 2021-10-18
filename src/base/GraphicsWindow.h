/**
*  @file Window.h
*  @date August 11, 2019
*  @author MetalMario971
*/
#pragma once
#ifndef __WINDOW_15655430083077514067_H__
#define __WINDOW_15655430083077514067_H__


#include "../gfx/RenderTarget.h"
#include "../world/WorldHeader.h"

namespace BR2 {
/**
*  @class GraphicsWindow
*  @brief Manages rendering to window.
*/
class GraphicsWindow_Internal;
class GraphicsWindow : public RenderTarget {
  friend class GraphicsApi;
  friend class OpenGLApi;
  friend class VulkanApi;
public:
  typedef std::unordered_map<std::string, std::shared_ptr<GraphicsWindow>> ChildWindows;

  GraphicsWindow(std::shared_ptr<GLContext> ct);
  virtual ~GraphicsWindow() override;

  void init(std::shared_ptr<GraphicsApi> api, SDL_Window* win, const GraphicsWindowCreateParameters& params);
  void step();
  void idle(int64_t us);

  virtual int32_t getWidth() override;
  virtual int32_t getHeight() override;
  string_t getTitle();
  void setTitle(const string_t& title);
  
  SDL_Window* getSDLWindow();
  std::shared_ptr<RenderViewport> getViewport();
  std::shared_ptr<RenderPipe> getRenderPipe();
  std::shared_ptr<Scene> getScene();
  std::shared_ptr<FrameSync> getFrameSync();
  std::shared_ptr<Delta> getDelta();
  std::shared_ptr<FpsMeter> getFpsMeter();
  uint64_t getFrameNumber();
  std::shared_ptr<InputManager> getInput();
  FrameState getFrameState();
  bool containsPoint_Global2D(const vec2& mp);

  void setScene(std::shared_ptr<Scene> scene);
  ChildWindows& getChildren();
  std::shared_ptr<GraphicsWindow> getParent();

private:
  void addChild(std::shared_ptr<GraphicsWindow> child);
  std::shared_ptr<GraphicsWindow>  removeChildByTitle(const string_t& title);
  std::unique_ptr<GraphicsWindow_Internal> _pint;
};

}//ns br2



#endif

/**
*  @file VulkanApi.h
*  @date August 6, 2019
*  @author MetalMario971
*/
#pragma once
#ifndef __VULKANAPI_15651062481484433440_H__
#define __VULKANAPI_15651062481484433440_H__

#include "../gfx/GraphicsApi.h"

namespace BR2 {
#define VkExtFn(_vkFn) PFN_##_vkFn _vkFn = nullptr;
/**
*  @class VulkanApi
*  @brief
*/
class VulkanApi_Internal;
class VulkanApi : public GraphicsApi {
public:
  VulkanApi();
  virtual ~VulkanApi() override;

  virtual std::shared_ptr<GraphicsWindow> createWindow(const string_t& title, std::shared_ptr<GraphicsWindow> parent = nullptr) override;
  virtual void makeCurrent(GraphicsWindow*) override;
  virtual void getDrawableSize(SDL_Window* win, int* w, int* h) override;
  virtual void swapBuffers(SDL_Window* win) override;
  virtual void cleanup() override;

  virtual void createRenderer() {}

private:
  std::unique_ptr<VulkanApi_Internal> _pint;
};

}  // namespace BR2

#endif

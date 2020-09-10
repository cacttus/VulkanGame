/**
*  @file VulkanContext.h
*  @date August 10, 2019
*  @author MetalMario971
*/
#pragma once
#ifndef __VULKANCONTEXT_15654533273371902995_H__
#define __VULKANCONTEXT_15654533273371902995_H__

#include "../gfx/GraphicsContext.h"

namespace BR2 {
/**
*  @class VulkanContext
*  @brief
*/
class VulkanContext : public GraphicsContext {
public:
  VulkanContext(std::shared_ptr<GraphicsApi> api);
  virtual ~VulkanContext() override;

  // virtual bool load(std::shared_ptr<AppBase> br) override { return true; }
  //virtual void update(float delta)  override {}
  virtual bool chkErrRt(bool bDoNotBreak = false, bool doNotLog = false, const string_t& shader = "") override { return true; }
  virtual bool chkErrDbg(bool bDoNotBreak = false, bool doNotLog = false, const string_t& shader = "") override { return true; }

  virtual void pushCullFace() override {}
  virtual void popCullFace() override {}
  virtual void pushBlend() override {}
  virtual void popBlend() override {}
  virtual void pushDepthTest() override {}
  virtual void popDepthTest() override {}
};

}  // namespace BR2

#endif

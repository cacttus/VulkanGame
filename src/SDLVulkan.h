

#pragma once
#ifndef __SDLVULKAN_H_2039482093842340928409238__
#define __SDLVULKAN_H_2039482093842340928409238__

#include "./BaseHeader.h"

namespace VulkanGame {

class SDLVulkan_Internal;
class SDLVulkan : public VirtualMemory {
public:
  SDLVulkan();
  virtual ~SDLVulkan() override;

  void makeSDLWindow(int32_t width, int32_t height);
  void cleanup();
private:
  std::unique_ptr<SDLVulkan_Internal> _pint;
};

} // namespace VulkanGameDemo

#endif
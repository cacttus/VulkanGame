

#pragma once
#ifndef __SDL_H__

#include "./BaseHeader.h"

namespace VulkanGame {

class SDL_Internal;
class SDLVulkan : public VirtualMemory {
public:
  SDLVulkan();
  virtual ~SDLVulkan() override;

  void makeSDLWindow(int32_t width, int32_t height);
  void cleanup();
private:
  std::unique_ptr<SDL_Internal> _pint;
};

} // namespace VulkanGameDemo

#endif
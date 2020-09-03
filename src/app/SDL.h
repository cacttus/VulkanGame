

#pragma once
#ifndef __SDL_H__

#include "../base/BaseHeader.h"

namespace VulkanGameDemo {

class SDL_Internal;
class SDL : public VirtualMemory {
public:
  SDL();
  virtual ~SDL() override;

  void makeSDLWindow(int32_t width, int32_t height);
  void cleanup();
private:
  std::unique_ptr<SDL_Internal> _pint;
};

} // namespace VulkanGameDemo

#endif


#ifndef __BASEHEADER_H__
#define __BASEHEADER_H__

//Basic stuff.
#include <iostream>
//Smart pointers
#include <memory>

namespace VulkanGame {
  class VirtualMemory { 
    public:
    virtual ~VirtualMemory() {}
  };
}
#endif
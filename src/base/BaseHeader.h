

#ifndef __BASEHEADER_H__
#define __BASEHEADER_H__

//Basic stuff.
#include <iostream>
//Smart pointers
#include <memory>

namespace VulkanGameDemo {
  class VirtualMemory { 
    public:
    virtual ~VirtualMemory() {}
  };
}
#endif
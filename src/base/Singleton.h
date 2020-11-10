/**
*  @file Singleton.h
*  @date 10/07/2020
*  @author MetalMario971
*/
#pragma once
#ifndef __SINGLETON_16047914072329720466646877649_H__
#define __SINGLETON_16047914072329720466646877649_H__

#include "../base/BaseHeader.h"

namespace VG {
/**
*  @class Singleton
*  @brief
*/

template <typename Tx>
class Singleton : public VirtualMemoryShared<Tx> {
public:
  Singleton() {}
  virtual ~Singleton() override {}
  std::shared_ptr<Tx> instance() {
    if (_instance == nullptr) {
      _instance = std::make_shared<Tx>();
    }
    return _instance;
  }

private:
  static std::shared_ptr<Tx> _instance;
};
template <typename Tx>
std::shared_ptr<Tx> Singleton<Tx>::_instance = nullptr;

}//ns Game



#endif

/**
*  @file Core.h
*  @date 10/10/2020
*  @author MetalMario971
*/
#pragma once
#ifndef __CORE_16050093968336945819257118408_H__
#define __CORE_16050093968336945819257118408_H__

#include "../core/CoreHeader.h"

namespace VG {
/**
*  @class Core
*  @brief
*/
class Core {
public:
  static std::shared_ptr<EngineConfig> config() { return _config; }

private:
  static std::shared_ptr<EngineConfig> _config;
  static void initGlobals();
};

}  // namespace VG

#endif

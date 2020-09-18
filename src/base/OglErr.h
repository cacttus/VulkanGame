/**
*  @file OglErr.h
*  @date November 22, 2016
*  @author MetalMario971
*/
#pragma once
#ifndef __OGLERR_1479868789525442042_H__
#define __OGLERR_1479868789525442042_H__

#include "../base/BaseHeader.h"
namespace BR2 {
/**
*  @class oglErr
*  @brief Handles OpenGL Errors.
*/
class OglErr_Internal;
class OglErr {
public:
  OglErr();
  virtual ~OglErr();
  bool chkErrRt(std::shared_ptr<GLContext> ctx, bool bDoNotBreak = false, bool doNotLog = true, const string_t& shaderName = "", bool clearOnly = false);
  bool chkErrDbg(std::shared_ptr<GLContext> ctx, bool bDoNotBreak = false, bool doNotLog = true, const string_t& shaderName = "", bool clearOnly = false);
  void clearGPULog(std::shared_ptr<GLContext> ctx);

private:
  std::unique_ptr<OglErr_Internal> _pint;
};

}  // namespace BR2

#endif

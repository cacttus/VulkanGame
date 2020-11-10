/**
*  @file DebugHelper.h
*  @date January 30, 2015
*  @author MetalMario971
*/
#pragma once
#ifndef __DEBUGHELPER_280862031362642290319991_H__
#define __DEBUGHELPER_280862031362642290319991_H__

#include "../base/BaseHeader.h"
namespace VG {
/**
*  @class DebugHelper
*  @brief  
*  1. Linux / Win32 Backtrace
*  2. MSVC CRT implementation of heap check debugging
*      checks consistency across memory blocks and
*      shows memory leaks.  Memory leaks are configured to
*      output to a file in the bin directory called crt.log.
*/
class DebugHelper : public VirtualMemory {
public:
  DebugHelper();
  virtual ~DebugHelper() override;


  static string_t getStackTrace();
  static std::vector<std::string> getCallStack(bool bIncludeFrameId = false);
  static string_t getCallingMethod();
  static void debugHeapBegin(bool bDoDebug);
  static void debugHeapEnd();
  static void setBreakAlloc(int allocNum);
  static void breakPoint();
  static void debugBreak();  //Breaks only on debuf
  static void checkMemory();
  static void setCheckAlways();
  static void setCheck16();

private:
  static string_t modList();
};

}  // namespace VG

#endif

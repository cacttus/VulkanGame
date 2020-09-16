/**
*  @file OperatingSystemHeader.h
*  @date October 9, 2014
*  @author MetalMario971
*/
#pragma once
#ifndef __OPERATINGSYSTEMHEADER_1315321194287381982523307_H__
#define __OPERATINGSYSTEMHEADER_1315321194287381982523307_H__

#include "../base/BuildConfig.h"

#define OS_METHOD_NOT_IMPLEMENTED BroLogError("The operating system method was not implemented.");

enum class OSErrorCode {
  //Note:Remember to add these codes to suppressError()
#ifdef BR2_OS_WINDOWS
  NoError = 0,
  FileNotFound = 2,
  PathNotFound = 3,
  InsufficientBuffer = 122L,// ERROR_INSUFFICIENT_BUFFER,
  ProcNotFound = 127,
  NoGuidTranslation = 560,
  ErrorNoToken = 1008,
#endif
#ifdef BR2_OS_ANDROID
  //TODO: fix this
  FileNotFound = 2,
  PathNotFound = 3,
  ProcNotFound = 127,
  NoGuidTranslation = 560,
#endif
#ifdef BR2_OS_LINUX

#endif
};

#endif

/**
*  @file TypeConv.hpp
*  @date August 23, 2010
*  @author MetalMario971
*/
#pragma once
#ifndef __TYPECONV_29189136032904720213337_H__
#define __TYPECONV_29189136032904720213337_H__

#include "../base/BaseHeader.h"
#include "../base/MachineTypes.h"
#include "../base/Exception.h"

namespace VG {
#define frameStampToStr(x) uint64ToStr(x)

#ifndef FORCE_INLINE
#define FORCE_INLINE inline
#endif

/**
*  @class TypeConv
*  @brief Wrapper for STL type casts, and some unorthodox casts.
*/
class TypeConv {
public:
  //ToVal
  static bool strToBool(const string_t& s, bool bThrowIfInvalid = false);
  static long strToLong(const string_t& s);
  static int_fast32_t strToInt(const string_t& s);
  static bool strToInt(const string_t& s, int32_t& out);  //Similar to .net' parseInt
  static uint_fast32_t strToUint(const string_t& s);
  //static int8_t strToByte(const string_t& s);
  static float strToFloat(const string_t& s);
  static double strToDouble(const string_t& s);

  //Silent Fail Verions
  static float strToFloat(const string_t& s, bool bFailed);
  static double strToDouble(const string_t& s, bool bFailed);

  //ToStr
  //static string_t intToStr(int_fast32_t i);
  static string_t intToStr(int_fast64_t i);
  static string_t uintToStr(uint_fast64_t i);
  //static string_t intToStr(uint_fast32_t i);
  static string_t sizetToStr(const size_t i);
  //static string_t longToStr(const long& i);
  //static string_t ulongToStr(const unsigned long& i);
  //static string_t int64ToStr(const int_fast64_t& i);
  //static string_t uint64ToStr(const uint_fast64_t& i);
  static string_t floatToStr(const float i);
  static string_t dblToStr(const double i);
  static string_t wstrToStr(wchar_t* wstr);

  //Format Verions
  static string_t intToStr(const int_fast64_t i, const char* const fmt);
};

}  // namespace VG

#endif

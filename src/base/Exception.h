/**
*  @file Exception.h
*  @date November 22, 2016
*  @author MetalMario971
*/
#pragma once
#ifndef __EXCEPTION_1479867204359778176_H__
#define __EXCEPTION_1479867204359778176_H__

#include "../base/MachineTypes.h"

namespace BR2 {
//TODO: override std::exception
class Exception {
public:
  Exception(const string_t& c, int line, const char* file, bool bLogError);
  string_t what() const;

private:
  int _line;
  string_t _msg;
  const char* _file;
};

}  // namespace BR2

#endif

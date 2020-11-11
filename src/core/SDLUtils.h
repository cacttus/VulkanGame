/**
*  @file SDLUtils.h
*  @date August 11, 2019
*  @author MetalMario971
*/
#pragma once
#ifndef __SDL_15655475192235209788_H__
#define __SDL_15655475192235209788_H__

#include "../base/BaseHeader.h"
#include "../core/SDLIncludes.h"

namespace VG {
/**
*  @class SDLUtils
*  @brief Static SDLUtils interface functions.
*/
class SDLUtils : public VirtualMemory {
public:
  static void trySetWindowIcon(SDL_Window* w, string_t iconPath);
  static void checkSDLErr(bool bLog = true, bool bBreak = true);
  static void SDLFileFree(char*& pOutData);
  static int SDLFileRead(const string_t& fname, char*& pOutData, int64_t& _iOutSizeBytes, bool addNull = false);
  static int SDLFileWrite(const string_t& fname, char* pData, size_t _iDataSizeBytes);

private:
  static void createSurfaceFromImage(const string_t& strImage, std::shared_ptr<Img32>& __out_ pImage, SDL_Surface*& __out_ pSurface);
  static SDL_Surface* createSurfaceFromImage(const std::shared_ptr<Img32> bi);
};

}  // namespace VG

#endif

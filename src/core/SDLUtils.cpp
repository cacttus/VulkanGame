#include "../base/Logger.h"
#include "../base/FileSystem.h"
#include "../base/EngineConfig.h"
#include "../base/Img32.h"
#include "../base/Allocator.h"
#include "../core/Core.h"
#include "../core/SDLUtils.h"

namespace VG {

void SDLUtils::trySetWindowIcon(SDL_Window* w, string_t iconPath) {
  if (iconPath.length()) {
    std::shared_ptr<Img32> img = nullptr;
    SDL_Surface* ss = nullptr;
    SDLUtils::createSurfaceFromImage(iconPath, img, ss);
    if (ss != nullptr) {
      SDL_SetWindowIcon(w, ss);
      SDLUtils::checkSDLErr();
      SDL_FreeSurface(ss);
      SDLUtils::checkSDLErr();
    }
    img = nullptr;
  }
}
void SDLUtils::createSurfaceFromImage(const string_t& strImage,
                                      std::shared_ptr<Img32>& __out_ pImage, SDL_Surface*& __out_ pSurface) {
  pImage = nullptr;
  pSurface = nullptr;
  if (FileSystem::fileExists(strImage)) {
    pImage = Base::loadImage(strImage);
    pSurface = createSurfaceFromImage(pImage);
  }
  else {
    BRLogError("Could not icon image '" + strImage + "'");
  }
}
SDL_Surface* SDLUtils::createSurfaceFromImage(const std::shared_ptr<Img32> pImage) {
  SDL_Surface* pSurface = SDL_CreateRGBSurfaceFrom(
      pImage->getData()->ptr(), pImage->getWidth(), pImage->getHeight(),
      pImage->getBitsPerPixel(), pImage->getPitch(),
      pImage->getRMask(), pImage->getGMask(), pImage->getBMask(), pImage->getAMask());

  checkSDLErr();

  return pSurface;
}
void SDLUtils::checkSDLErr(bool bLog, bool bBreak) {
  //Do SDL errors here as well
  const char* c;
  while ((c = SDL_GetError()) != nullptr && *c != 0) {
    //linux : GLXInfo
    if (bLog == true) {
      BRLogError("SDL: " + c);
    }

    if (Base::config()->getBreakOnSDLError() == true && bBreak) {
      Base::debugBreak();
    }
    SDL_ClearError();
  }
}

void SDLUtils::SDLFileFree(char*& pOutData) {
  delete[] pOutData;
  pOutData = nullptr;
}
int SDLUtils::SDLFileRead(const string_t& fname, char*& pOutData,
                            int64_t& _iOutSizeBytes, bool addNull) {
  //Return 1 = failure
  //Return 0 = success
  //Use SDLUtils::Base::checkErrors()or() to check the error.
  string_t full_path = FileSystem::getFilePath(fname);

  SDL_RWops* rw = SDL_RWFromFile(full_path.c_str(), "rb");
  if (rw == NULL) {
    return 1;
  }

  _iOutSizeBytes = SDL_RWsize(rw);
  if (addNull) {
    pOutData = new char[_iOutSizeBytes + 1];
  }
  else {
    pOutData = new char[_iOutSizeBytes];
  }

  Sint64 nb_read_total = 0, nb_read = 1;
  char* buf = pOutData;
  while (nb_read_total < _iOutSizeBytes && nb_read != 0) {
    nb_read = SDL_RWread(rw, buf, 1, _iOutSizeBytes - nb_read_total);
    nb_read_total += nb_read;
    buf += nb_read;
  }
  SDL_RWclose(rw);
  if (nb_read_total != _iOutSizeBytes) {
    delete[] pOutData;
    return 1;
  }

  // So yeah sometimes you just gotta..
  if (addNull) {
    pOutData[nb_read_total] = '\0';
  }
  return 0;
}
int SDLUtils::SDLFileWrite(const string_t& fname, char* pData,
                             size_t _iDataSizeBytes) {
  string_t full_path = FileSystem::getFilePath(fname);

  SDL_RWops* rw = SDL_RWFromFile(full_path.c_str(), "wb");
  if (rw == NULL) {
    return 1;
  }

  SDL_RWwrite(rw, pData, 1, _iDataSizeBytes);
  SDL_RWclose(rw);

  return 0;
}

}  // namespace VG

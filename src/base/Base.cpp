#include "../base/Base.h"

#include "../base/OperatingSystemHeader.h"
#include "../base/DiskFile.h"
#include "../base/ApplicationPackage.h"
#include "../base/BaseHeader.h"
#include "../base/Img32.h"
#include "../base/Allocator.h"
#include "../base/BinaryFile.h"
#include "../base/Logger.h"
#include "../base/ColoredConsole.h"
//#include "../base/Net.h"
#include "../ext/lodepng.h"

#include <chrono>
#include <thread>
#include <iostream>

#ifndef BR2_OS_WINDOWS
//For Sigtrap.
#include <signal.h>
#endif

extern "C" {
//nothings commented on Apr 12, 2016
//It's not meant to be #included. Don't #include it, just compile & link it.
extern int stb_vorbis_decode_filename(const char* filename, int* channels, int* sample_rate, short** output);
}

//don't put this in a header file
#ifdef __APPLE__
#ifdef TARGET_OS_IPHONE
//#include <objc/objc-runtime.h>
#include <foundation/foundation.h>
//#include <corefoundation/CFBundle.h>
#endif
#endif

namespace BR2 {

std::shared_ptr<Logger> Base::_logger = nullptr;
std::shared_ptr<ApplicationPackage> Base::_package = nullptr;
std::shared_ptr<App> Base::_app = nullptr;

void Base::checkErrors(bool a, bool b){
  Base::app()->checkErrors(a,b);
}
void Base::debugBreak() {
#if defined(BR2_OS_WINDOWS)
  DebugBreak();
#elif defined(BR2_OS_LINUX)
  raise(SIGTRAP);
#else
  OS_NOT_SUPPORTED_ERROR
#endif
}
int Base::loadSound(std::string path, int& __out_ iChannels, int& __out_ iSampleRate,
                    int16_t*& __out_ pData, int& __out_ nSamples, int& __out_ iDataLenBytes) {
  //  it returns the number of
  // samples decoded, and you told it the number of channels and the sample rate.Multiply
  // those three together, and you get the total number of shorts in the buffer;
  // multiply by sizeof(short), and voil�; size of that buffer in bytes.
  nSamples = stb_vorbis_decode_filename(path.c_str(), &iChannels, &iSampleRate, &pData);

  if (nSamples <= 0) {
    return nSamples;
  }

  iDataLenBytes = nSamples * iChannels * sizeof(int16_t);

  return 1;
}
void Base::checkMemory() {
#if defined(BR2_OS_WINDOWS)
#ifdef _DEBUG
  _CrtCheckMemory();
#endif
#endif
}
string_t Base::getOperatingSystemName() {
  string_t res;
#if defined(BR2_OS_WINDOWS)
  OSVERSIONINFOEX vex;
  vex.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
  GetVersionEx((OSVERSIONINFO*)&vex);

  // CheckOsErrorsDbg();

  if (vex.dwMajorVersion == 6 && vex.dwMinorVersion == 3) {
    res.append(" Windows 8.1");
  }
  else if (vex.dwMajorVersion == 6 && vex.dwMinorVersion == 2) {
    res.append(" Windows 8");
  }
  else if (vex.dwMajorVersion == 6 && vex.dwMinorVersion == 1) {
    res.append(" Windows 7");
  }
  else if (vex.dwMajorVersion == 6 && vex.dwMinorVersion == 0) {
    res.append(" Windows Vista");
  }
  else if (vex.dwMajorVersion == 5 && vex.dwMinorVersion == 2) {
    res.append(" Windows XP Pro 64 bit");
  }
  else if (vex.dwMajorVersion == 5 && vex.dwMinorVersion == 1) {
    res.append(" Windows XP");
  }
  else if (vex.dwMajorVersion == 5 && vex.dwMinorVersion == 0) {
    res.append(" Windows 2000");
  }
  else {
    res.append(" OS Unknown.  Note: Mac / Linux are not supported.");
  }

  if (vex.wServicePackMajor != 0)
    res += Stz ", Service Pack " + vex.wServicePackMajor + "." + vex.wServicePackMinor;
  else
    res += Stz ", No service pack";

  if (vex.wProductType == VER_NT_DOMAIN_CONTROLLER)
    res.append(", Domain Controller, note operating system may be incorrect as this is not supported");
  else if (vex.wProductType == VER_NT_SERVER)
    res.append(", Server, note operating system may be incorrect as this is not supported");
  else if (vex.wProductType == VER_NT_WORKSTATION)
    res.append(", Workstation");

#elif defined(BR2_OS_LINUX)
  res = "Linux";
#else
  OS_NOT_SUPPORTED_ERROR
#endif
  //CheckOsErrorsDbg();
  return res;
}
bool Base::isDebug() {
  //Reusing Windows debug flag for *nix.
#ifdef _DEBUG
  return true;
#else
  return false;
#endif
}
std::vector<string_t> Base::argsToVectorOfString(int argc, char** argv, char delimiter) {
  int squot = 0, dquot = 0;

  //todo - fix the delimiter thing
  std::vector<string_t> ret;
  for (int i = 0; i < argc; ++i) {
    string_t str(argv[i]);

    ret.push_back(str);
  }
  return ret;
}
void Base::print(char msg) {
  char c[2];
  c[0] = msg;
  c[1] = 0;
  print((char*)c);
}
void Base::print(const string_t& msg) {
  print(msg.c_str());
}
void Base::print(const char* msg) {
  //if (_pEngineConfig == nullptr) {
  ColoredConsole::print(std::string(msg), ColoredConsole::Color::FG_WHITE);
  //}
  //else if (Base::getEngineConfig()->getShowConsole()) {
  ColoredConsole::print(std::string(msg), ColoredConsole::Color::FG_WHITE);
  //}
}
std::string Base::getCPPVersion() {
  //https://stackoverflow.com/questions/2324658/how-to-determine-the-version-of-the-c-standard-used-by-the-compiler

  if (__cplusplus == 201703L) {
    return Stz "C++17";
  }
  else if (__cplusplus == 201403L) {
    return Stz "C++14";
  }
  else if (__cplusplus == 201103L) {
    return Stz "C++11";
  }
  else if (__cplusplus == 199711L) {
    return Stz "C++98";
  }
  return Stz "pre-standard C++";
}
void Base::sleepThread(uint64_t milliseconds) {
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
}
std::shared_ptr<Img32> Base::loadImage(std::string imgLoc) {
  unsigned char* image = nullptr;  //the raw pixels
  unsigned int width, height;
  int err;
  //char* imgData = 0;
  // uint32_t imgSize;
  std::shared_ptr<Img32> ret = nullptr;

  std::shared_ptr<BinaryFile> fb = std::make_shared<BinaryFile>("<none>");
  if (Base::package()->getFile(imgLoc, fb)) {
    //decode
    err = lodepng_decode32(&image, &width, &height, (unsigned char*)fb->getData().ptr(), fb->getData().count());
    if (err != 0) {
      //FB should free itself.
      //  Base::SDLFileFree(imgData);
      BRThrowException(Stz "Could not load image " + imgLoc + " err code = " + err);
    }
    else {
      Img32::flipImage20161206(image, width, height);
      // create(image, width, height, false);

      ret = std::make_shared<Img32>(width, height, (uint8_t*)image);

      //delete image;
    }
    //FileBuffer will free itself
    //Base::SDLFileFree(imgData);
  }

  return ret;
}
bool Base::saveImage(std::string path, std::shared_ptr<Img32> spec) {
  //unsigned char* image = nullptr; //the raw pixels
  //unsigned int width, height;
  //int err;
  //char* imgData = 0;
  //uint32_t imgSize;
  //err = Base::SDLFileRead(imgLoc.c_str(), imgData, imgSize);
  bool bRet = true;
  unsigned char* imageData = (unsigned char*)spec->getData()->ptr();
  unsigned int w = spec->getWidth();
  unsigned int h = spec->getHeight();
  unsigned char* buffer = 0;
  size_t buffersize = 0;
  //For some reason we have to call this twice, i'm not sure.  Maybe LODEPNG wants buffer to be allocated?
  // lodepng_encode_memory(&buffer, &buffersize, imageData, w, h, LCT_RGBA, 8);
  unsigned error = lodepng_encode_memory(&buffer, &buffersize, imageData, w, h, LCT_RGBA, 8);
  if (error == 0) {
    Allocator<char> allocr(buffersize, (char*)buffer);

    RetCode rc = DiskFile::writeAllBytes(path, allocr);
    if (rc != GR_OK) {
      BRLogError("Error'" + (int)rc + "' occurred while saving image.");
    }
    allocr.dealloc();
  }
  else {
    BRLogError("LodePng - Error encoding image '" + path + "'.");
    bRet = false;
  }
  free(buffer);  //lodepng_free

  return bRet;
}
void Base::freeImage(std::shared_ptr<Img32> b) {
  //b->dea
  //delete b;
}
t_timeval Base::getMicroSeconds() {
  int64_t ret;
  std::chrono::nanoseconds ns = std::chrono::high_resolution_clock::now().time_since_epoch();
  ret = std::chrono::duration_cast<std::chrono::microseconds>(ns).count();
  return ret;
}
t_timeval Base::getMilliSeconds() {
  return getMicroSeconds() / 1000;
}
#ifdef __APPLE__
#ifdef TARGET_OS_IPHONE
static CFStringRef stdStrToCFStr(std::string st) {
  CFStringRef str;
  //kCFStringEncodingUTF8
  str = CFStringCreateWithCString(NULL, st.c_str(), CFStringGetSystemEncoding());
  return str;
}
#endif
#endif

void Base::createManagers(const string_t& logfile_dir, const std::vector<string_t>& args) {
  BRLogInfo("GLContext - Creating Sequencer");
  Base::createLogger(logfile_dir, args);

  //_pSequencer = std::make_shared<Sequencer>();
  //SDLUtils::Base::checkErrors()();
  //ct->chkErrRt();
}
void parsearg(const string_t& arg, string_t& __out_ out_key, string_t& __out_ out_value) {
  bool isvalue = false;
  std::string key = "";
  std::string value = "";
  //**TODO Move this to AppRunner

  for (int i = 0; i < arg.length(); ++i) {
    if (arg[i] == '=') {
      isvalue = true;
    }
    else if (isvalue) {
      value += arg[i];
    }
    else {
      key += arg[i];
    }
    //**TODO Move to AppRunner
  }

  out_key = StringUtil::trim(key);
  out_value = StringUtil::trim(value);
}
bool Base::checkArg(const std::vector<string_t>& args, const string_t& inkey, const string_t& invalue) {
  string_t key, value;
  for (std::string arg : args) {
    parsearg(arg, key, value);
    if (StringUtil::equalsi(key, inkey)) {
      if (StringUtil::equalsi(value, invalue)) {
        return true;
      }
      else {
        return false;
      }
      break;
    }
  }
  return false;
}
void Base::createLogger(const string_t& logfile_dir, const std::vector<string_t>& args) {
  //These are essentially the system defaults
  bool log_async = true;
  bool disabled = false;
  if (Base::checkArg(args, "log_async", "true")) {
    log_async = true;
  }
  else if (Base::checkArg(args, "log_async", "false")) {
    log_async = false;
  }
  if (Base::checkArg(args, "log_disable", "true")) {
    disabled = true;
  }
  else if (Base::checkArg(args, "log_disable", "false")) {
    disabled = false;
  }
  Base::_logger = std::make_shared<Logger>(log_async, disabled);
  Base::_logger->init(logfile_dir);
}

float Base::fade(float t) {
  //T = [0,1]
  // Fade function as defined by Ken Perlin.  This eases coordinate values
  // so that they will ease towards integral values.  This ends up smoothing
  // the final output.
  return 6 * powf(t, 5) - 15 * powf(t, 4) + 10 * powf(t, 3);
  //  return 6*(t*t*t*t*t) - 15*(t*t*t*t) + 10*(t*t*t);         // 6t^5 - 15t^4 + 10t^3
}

bool Base::isBigEndian() {
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  return true;
#else
  return false;
#endif
}


}  // namespace BR2

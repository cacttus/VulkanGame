/**
*  @file Logger.h
*  @date December 14, 2013
*  @author MetalMario971
*/
#pragma once
#ifndef __LOGGER_16421047220665463212149_H__
#define __LOGGER_16421047220665463212149_H__

#include "../base/Base.h"

namespace VG {
/**
*  @class Logger
*  @brief Logs to a text file in /log/ and/or the console.
*/
class Logger_Internal;
class Logger : public VirtualMemoryShared<Logger>{
public:
  Logger(bool async, bool disabled = false);
  virtual ~Logger() override;

  void init(const string_t& cacheFolder);

  void enableLogToConsole(bool bLogToConsole);
  void enableLogToFile(bool bLogToFile);

  void logInfo(const string_t& msg);
  void logInfo(const string_t& msg, int line, const char* file, const VG::Exception* e = nullptr, bool hideStackTrace = true);
  void logWarn(const string_t& msg, int line, const char* file, const VG::Exception* e = nullptr, bool hideStackTrace = true);
  void logError(const string_t& msg, int line, const char* file, const VG::Exception* e = nullptr, bool hideStackTrace = false);
  void logScript(const string_t& msg, int line, const char* file);
  void logTODO(const string_t& msg, int line, const char* file);
  void logDebug(const string_t& msg, int line, const char* file, const VG::Exception* e = nullptr, bool hideStackTrace = true);
  void logWarnCycle(const string_t& msg, int line, const char* file, const VG::Exception* e = nullptr, int wait_ms = 1000, bool force = false);
  void logErrorCycle(const string_t& msg, int line, const char* file, const VG::Exception* e = nullptr, int wait_ms = 1000, bool force = false);
  void logDebugCycle(const string_t& msg, int line, const char* file, const VG::Exception* e = nullptr, int wait_ms = 1000, bool force = false);

  string_t getLogPath();

private:
  Logger_Internal* _pint = nullptr;
};



}  // namespace VG



//from Clang: warning: ISO C++11 does not allow conversion from string literal to 'char *' [-Wwritable-strings]
#define BRLogDebug(x) VG::Base::logger()->logDebug(Stz x, BR_SRC_FLINE, (char* const)BR_SRC_FNAME)
#define BRLogInfo(x) VG::Base::logger()->logInfo(Stz x, BR_SRC_FLINE, (char* const)BR_SRC_FNAME)
#define BRLogWarn(x) VG::Base::logger()->logWarn(Stz x, BR_SRC_FLINE, (char* const)BR_SRC_FNAME, nullptr)
#define BRLogError(x) VG::Base::logger()->logError(Stz x, BR_SRC_FLINE, (char* const)BR_SRC_FNAME, nullptr)
#define BRLogScript(x) VG::Base::logger()->logScript(Stz x, BR_SRC_FLINE, (char* const)BR_SRC_FNAME)
#define BRLogErrorNoStack(x) VG::Base::logger()->logError(Stz x, BR_SRC_FLINE, (char* const)BR_SRC_FNAME, nullptr, true)
#define BRLogErrorEx(x, aex) VG::Base::logger()->logError(x, BR_SRC_FLINE, (char* const)BR_SRC_FNAME, aex)

#define BRLogErrorOnce(x)                                                              \
  {                                                                                    \
    static bool ___logged_ = false;                                                    \
    if (___logged_ == false) {                                                         \
      ___logged_ = true;                                                               \
      VG::Base::logger()->logError(Stz x, BR_SRC_FLINE, (char* const)BR_SRC_FNAME, nullptr); \
    }                                                                                  \
  }
#define BRLogWarnOnce(x)                                                     \
  {                                                                          \
    static bool ___logged_ = false;                                          \
    if (___logged_ == false) {                                               \
      ___logged_ = true;                                                     \
      VG::Base::logger()->logWarn(Stz x, BR_SRC_FLINE, (char* const)BR_SRC_FNAME); \
    }                                                                        \
  }
#define BRLogDebugOnce(x)                                                     \
  {                                                                           \
    static bool ___logged_ = false;                                           \
    if (___logged_ == false) {                                                \
      ___logged_ = true;                                                      \
      VG::Base::logger()->logDebug(Stz x, BR_SRC_FLINE, (char* const)BR_SRC_FNAME); \
    }                                                                         \
  }
#define BRLogWarnCycle(x)                                                                                \
  {                                                                                                      \
    static bool ___logged_ = true;                                                                       \
    VG::Base::logger()->logWarnCycle(Stz x, BR_SRC_FLINE, (char* const)BR_SRC_FNAME, nullptr, 1000, ___logged_); \
    if (___logged_ == true) {                                                                            \
      ___logged_ = false;                                                                                \
    }                                                                                                    \
  }
#define BRLogErrorCycle(x)                                                                                \
  {                                                                                                       \
    static bool ___logged_ = true;                                                                        \
    VG::Base::logger()->logErrorCycle(Stz x, BR_SRC_FLINE, (char* const)BR_SRC_FNAME, nullptr, 1000, ___logged_); \
    if (___logged_ == true) {                                                                             \
      ___logged_ = false;                                                                                 \
    }                                                                                                     \
  }
#define BRLogDebugCycle(x)                                                                                \
  {                                                                                                       \
    static bool ___logged_ = true;                                                                        \
    VG::Base::logger()->logDebugCycle(Stz x, BR_SRC_FLINE, (char* const)BR_SRC_FNAME, nullptr, 1000, ___logged_); \
    if (___logged_ == true) {                                                                             \
      ___logged_ = false;                                                                                 \
    }                                                                                                     \
  }
  #define BR_LogImportantStr std::string("*\n*\n*\n*\n*\n")
#define BRLogTODO(x)                                                         \
  {                                                                          \
    static bool ___logged_ = false;                                          \
    if (___logged_ == false) {                                               \
      ___logged_ = true;                                                     \
      VG::Base::logger()->logTODO(Stz BR_LogImportantStr + "* " + x + BR_LogImportantStr, BR_SRC_FLINE, (char* const)BR_SRC_FNAME); \
    }                                                                        \
  }
#endif

#include "../base/Logger.h"
#include "../base/FileSystem.h"
#include "../base/ApplicationPackage.h"
#include "../base/DateTime.h"
#include "../base/Gu.h"
#include "../base/DebugHelper.h"
#include "../base/StringUtil.h"
#include "../base/GraphicsWindow.h"
#include "../base/GLContext.h"
#include "../base/Stopwatch.h"
#include "../base/ColoredConsole.h"
#include "../base/OperatingSystem.h"
#include <mutex>
#include <atomic>
#include <fstream>
#include <thread>
#include <future>
#include <algorithm>
#include <iostream>

namespace BR2 {
#pragma region Logger_Internal
class Logger_Internal {
public:
  Logger_Internal();
  virtual ~Logger_Internal();

  enum class LogLevel { Debug,
                        Info,
                        Warn,
                        Error,
                        TODO,
  };
  string_t _logDir;
  string_t _logFileName;
  std::atomic_bool _bAsync = false;

  std::vector<std::string> _toLog;
  std::vector<LogLevel> _logLevel;

  std::atomic_bool _bEnabled = true;
  std::atomic_bool _bLogToFile = true;
  std::atomic_bool _bLogToConsole = true;

  std::atomic_bool _bSuppressLineFileDisplay = false;
  std::mutex _mtLogStackAccess;
  std::atomic_bool _kill;

  string_t addStackTrace(string_t msg);
  string_t createMessageHead(LogLevel level, const char* file, int line);
  void log(string_t msg, string_t header, Logger_Internal::LogLevel level, const BR2::Exception* const e);
  void processLogs_Async();

  std::unique_ptr<Stopwatch> _cycler;

  void log_cycle_mainThread(std::function<void()> f, bool, int wait_ms);
  void log_wedi_mainThread(string_t msg, int line, const char* file, const BR2::Exception* const e, bool hideStackTrace, Logger_Internal::LogLevel level);
};
Logger_Internal::Logger_Internal(){
  _cycler = std::make_unique<Stopwatch>();
  _cycler->start();
}
Logger_Internal::~Logger_Internal(){
  _cycler->stop();
  _cycler = nullptr;
}
void Logger_Internal::log_wedi_mainThread(string_t msg, int line, const char* file, const BR2::Exception* const e, bool hideStackTrace, Logger_Internal::LogLevel level) {
  if (_bEnabled == false) {
    return;
  }

  if (hideStackTrace == false) {
    msg = addStackTrace(msg);
  }

  log(msg, createMessageHead(level, file, line), level, e);
}
void Logger_Internal::log_cycle_mainThread(std::function<void()> f, bool force, int wait_ms) {
  if (_cycler->pulse((t_timeval)wait_ms)) {
    f();
  }
}
string_t Logger_Internal::addStackTrace(string_t msg) {
  msg += OperatingSystem::newline();
  msg += DebugHelper::getStackTrace();
  return msg;
}
string_t Logger_Internal::createMessageHead(LogLevel level, const char* file, int line) {
  string_t logtype;
  if (level == LogLevel::Debug) {
    logtype = "D";
  }
  else if (level == LogLevel::Info) {
    logtype = "I";
  }
  else if (level == LogLevel::Warn) {
    logtype = "W";
  }
  else if (level == LogLevel::Error) {
    logtype = "E";
  }
  else if (level == LogLevel::TODO) {
    logtype = "T";
  }
  else {
    logtype = "?";
  }
  string_t file_line = "";
  if (_bSuppressLineFileDisplay == false) {
    if (file != nullptr) {
      file_line += file;
    }
    if (line >= 0) {
      file_line += ":" + TypeConv::intToStr(line);
    }
  }

  return Stz "" + DateTime::getDateTime().timeToStr() + "[" + logtype + "]" + "[" + file_line + "]";
}
void Logger_Internal::log(string_t msg, string_t header, Logger_Internal::LogLevel level, const BR2::Exception* const e) {
  string_t m = header + " " + msg;

  if (e != nullptr) {
    m += Stz ", Exception: " + e->what();
  }

  m += Stz OperatingSystem::newline();

  if (_bAsync) {
    _mtLogStackAccess.lock();
    {
      _toLog.push_back(m);
      _logLevel.push_back(level);
    }
    _mtLogStackAccess.unlock();
  }
  else {
    _toLog.push_back(m);
    _logLevel.push_back(level);
    processLogs_Async();
  }
}
void Logger_Internal::processLogs_Async() {
  std::vector<string_t> logs;
  std::vector<Logger_Internal::LogLevel> logLevels;
  if (_bAsync) {
    _mtLogStackAccess.lock();
    {
      logs.swap(_toLog);
      logLevels.swap(_logLevel);
    }
    _mtLogStackAccess.unlock();
  }
  else {
    logs.swap(_toLog);
    logLevels.swap(_logLevel);
  }

  string_t appended = "";
  Logger_Internal::LogLevel level;
  for (size_t iMsg = 0; iMsg < logs.size(); ++iMsg) {
    appended += logs[iMsg];
    level = logLevels[iMsg];
    if (_bLogToConsole) {
      //TODO: we could speed up the logger by removing all these find() methods and using a flag up the call chain.
      if (level == Logger_Internal::LogLevel::Debug) {
        ColoredConsole::print(logs[iMsg], ColoredConsole::Color::FG_CYAN);
      }
      else if (level == Logger_Internal::LogLevel::Error) {
        ColoredConsole::print(logs[iMsg], ColoredConsole::Color::FG_RED);
      }
      else if (level == Logger_Internal::LogLevel::Warn) {
        ColoredConsole::print(logs[iMsg], ColoredConsole::Color::FG_YELLOW);
      }
      else if (level == Logger_Internal::LogLevel::TODO) {
        ColoredConsole::print(logs[iMsg], ColoredConsole::Color::FG_GREEN);
      }
      else {
        ColoredConsole::print(logs[iMsg], ColoredConsole::Color::FG_WHITE);
      }
    }
  }

  if (_bLogToFile) {
    if (!FileSystem::fileExists(_logDir)) {
      FileSystem::createDirectoryRecursive(FileSystem::getDirectoryNameFromPath(_logDir));
      FileSystem::createFile(_logDir, false, false);
    }
    //  OperatingSystem::suppressError(183,"Suppressing windows dumb 'append' error 183",false);
    std::ofstream _fileHandle;
    _fileHandle.open(_logDir.c_str(), std::ofstream::app);
    if (_fileHandle.fail() == false) {
      _fileHandle.write(appended.c_str(), (std::streamsize)appended.length());
      _fileHandle.close();
    }
    else {
      //Ignore log writes.  Not app critical
      //Debug only . This is not necessarily an error.
      //We could wait for the file to become available by write-checking
      //it a couple of times
      Gu::debugBreak();
    }
  }

  logs.clear();
  logLevels.clear();
}

#pragma endregion

#pragma region Logger
Logger::Logger(bool async, bool disabled) {
  _pint = new Logger_Internal();
  _pint->_bAsync = async;
  _pint->_bEnabled = !disabled;
}
Logger::~Logger() {
  //kill thread.
  _pint->_kill = true;
  _pint = nullptr;
}
void Logger::init(string_t cache) {
  _pint->_logFileName = Stz "Log_" + DateTime::getDateTime().dateToStr(".") + "_" + DateTime::getDateTime().timeToStr(".") + ".txt";
  string_t cache_rooted = FileSystem::getRootedPath(cache);
  _pint->_logDir = FileSystem::combinePath(cache_rooted, _pint->_logFileName);

  //Run async, if applicable
  if (_pint->_bAsync) {
    //https://thispointer.com/c11-how-to-stop-or-terminate-a-thread/

    Logger_Internal* li = _pint;

    std::thread th([li]() {
      while (true) {
        li->processLogs_Async();
        if (li->_kill) {
          break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
      }
      //delete in this thread.
      delete li;
    });
    th.detach();
  }

  //*Note: do not call the #define shortcuts here.
  logInfo(Stz(_pint->_bAsync ? "Async " : "Synchronous ") + "Logger Initializing " + DateTime::getDateTime().toString());
}
string_t Logger::getLogPath() {
  return _pint->_logDir;
}
void Logger::logInfo(string_t msg) {
  logInfo(msg, -1, "", nullptr, true);
}
void Logger::logTODO(string_t msg, int line, const char* file) {
  _pint->log_wedi_mainThread(msg, line, file, nullptr, true, Logger_Internal::LogLevel::TODO);
}
void Logger::logInfo(string_t msg, int line, const char* file, const BR2::Exception* const e, bool hideStackTrace) {
  _pint->log_wedi_mainThread(msg, line, file, e, hideStackTrace, Logger_Internal::LogLevel::Info);
}
void Logger::logError(string_t msg, int line, const char* file, const BR2::Exception* const e, bool hideStackTrace) {
  _pint->log_wedi_mainThread(msg, line, file, e, hideStackTrace, Logger_Internal::LogLevel::Error);
}
void Logger::logWarn(string_t msg, int line, const char* file, const BR2::Exception* const e, bool hideStackTrace) {
  _pint->log_wedi_mainThread(msg, line, file, e, hideStackTrace, Logger_Internal::LogLevel::Warn);
}
void Logger::logDebug(string_t msg, int line, const char* file, const BR2::Exception* const e, bool hideStackTrace) {
  _pint->log_wedi_mainThread(msg, line, file, e, hideStackTrace, Logger_Internal::LogLevel::Debug);
}
void Logger::logWarnCycle(string_t msg, int line, const char* file, const BR2::Exception* const e, int iCycle, bool force) {
  _pint->log_cycle_mainThread([&]() {
    logWarn(msg, line, file, e);
  },
                              force, iCycle);
}
void Logger::logErrorCycle(string_t msg, int line, const char* file, const BR2::Exception* const e, int iCycle, bool force) {
  _pint->log_cycle_mainThread([&]() {
    logError(msg, line, file, e);
  },
                              force, iCycle);
}
void Logger::logDebugCycle(string_t msg, int line, const char* file, const BR2::Exception* const e, int iCycle, bool force) {
  _pint->log_cycle_mainThread([&]() {
    logDebug(msg, line, file, e);
  },
                              force, iCycle);
}
void Logger::enableLogToConsole(bool bLogToConsole) {
  _pint->_bLogToConsole = bLogToConsole;
  _pint->_bEnabled = _pint->_bLogToConsole || _pint->_bLogToFile;
}
void Logger::enableLogToFile(bool bLogToFile) {
  _pint->_bLogToFile = bLogToFile;
  _pint->_bEnabled = _pint->_bLogToConsole || _pint->_bLogToFile;
}

#pragma endregion
}  // namespace BR2
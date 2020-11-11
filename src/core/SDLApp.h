#pragma once
#ifndef __57823905782390589020_AppRunner_h_2298357238957238957129348__
#define __57823905782390589020_AppRunner_h_2298357238957238957129348__

#include "../core/CoreHeader.h"
#include "../core/SDLIncludes.h"

namespace VG {

/**
*   @class AppRunner
*   @brief The entry point of the application from main()
*/
class SDLApp : public App {
public:
  SDLApp();
  virtual ~SDLApp() override;
  void runApp(const std::vector<string_t>& args, std::vector<std::function<bool()>> unit_tests);

  virtual void checkErrors(bool bLog, bool bBreak) override;
  virtual std::shared_ptr<ApplicationPackage> package() override { return _package; }

  virtual void freeFile(char*& pOutData) override;
  virtual int readFile(const string_t& fname, char*& pOutData, int64_t& _iOutSizeBytes, bool addNull = false) override;
  virtual int writeFile(const string_t& fname, char* pData, size_t _iDataSizeBytes) override;

private:
  void initSDLAndCreateGraphicsApi();
  void doShowError(const string_t& err, const Exception* const e) const;
  void attachToGameHost();
  void printVideoDiagnostics();
  void updateWindowHandleForGamehost();
  void initAudio();
  void initNet();
  void runGameLoopTryCatch();
  void exitApp(const string_t& error, int rc);
  bool argMatch(const std::vector<string_t>& args, const string_t& arg1, int32_t iCount);
  bool runCommands(const std::vector<string_t>& args);
  void loadAppPackage(const std::vector<string_t>& args);
  void runUnitTests(std::vector<std::function<bool()>>);
  void loadDebugPackage();

  std::shared_ptr<SDLApp> _app = nullptr;
  SDL_AudioSpec _audioSpec;
  uint64_t _tvInitStartTime = 0;
  // accept a connection coming in on server_tcpsock
  TCPsocket _gameHostSocket;  //Send and receive data
  TCPsocket _server_tcpsock;  //Accept connections
  std::shared_ptr<GraphicsDriver> _graphicsDriver = nullptr;
  std::shared_ptr<ApplicationPackage> _package = nullptr;
};

}  // namespace VG
#endif

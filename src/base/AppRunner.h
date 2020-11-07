#pragma once
#ifndef __57823905782390589020_AppRunner_h_2298357238957238957129348__
#define __57823905782390589020_AppRunner_h_2298357238957238957129348__

#include "../base/BaseHeader.h"

namespace BR2 {
class AppRunner_Internal;
/**
*   @class AppRunner
*   @brief The entry point of the application from main()
*/
class AppRunner : public VirtualMemory {
public:
  AppRunner();
  virtual ~AppRunner() override;
  void runApp(const std::vector<string_t>& args, std::vector<std::function<bool()>> unit_tests);

private:
  std::unique_ptr<AppRunner_Internal> _pint;
};

}  // namespace BR2
#endif


#include "../base/BuildConfig.h"
#include "../base/ColoredConsole.h"
#include "../base/OperatingSystem.h"
#include "../base/Base.h"
#include <iostream>

#if defined(BR2_OS_LINUX)
#include <term.h>
#endif

namespace VG {
void ColoredConsole::print(const std::string& str, ColoredConsole::Color cc) {
  //Check colored terminal support.
  static int g_colors_supported = -1;  //-1=unset 0=no 1=yes
#if defined(BR2_OS_LINUX)
  if (g_colors_supported == -1) {
    g_colors_supported = 0;
    try {
      //setupterm fills cur_term with the required info, then it's available through
      //the macros defind in term.h using cur_term. Compile with -lncurses.
      std::cout << "Linux Terminal: " << OperatingSystem::getEnvironmentVariable("TERM") << std::endl;
      int ret = setupterm(NULL, 0, NULL);  //if term is null, the environment variable TERM is used
      TERMINAL* tt2 = cur_term;
      if (cur_term && max_colors >= 256) {
        g_colors_supported = 1;
      }
      else {
        std::cout << "Linux terminal colors unavailable." << std::endl;
      }
    }
    catch (...) {
      //Error. We are in the logger so we can't really log.
      std::cout << "Error with terminfo." << std::endl;
      Base::debugBreak();
    }
  }
#elif defined(BR2_OS_WINDOWS)
  g_colors_supported = 1;
#else
  OS_NOT_SUPPORTED_ERROR
#endif

  if (g_colors_supported == 1) {
#if defined(BR2_OS_WINDOWS)
#define ConsoleColorWhite() SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY)
    if (cc == ColoredConsole::Color::FG_WHITE) {
      ConsoleColorWhite();
    }
    else if (cc == ColoredConsole::Color::FG_GRAY) {
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
    }
    else if (cc == ColoredConsole::Color::FG_RED) {
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
    }
    else if (cc == ColoredConsole::Color::FG_CYAN) {
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    }
    else if (cc == ColoredConsole::Color::FG_YELLOW) {
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    }
    else if (cc == ColoredConsole::Color::FG_GREEN) {
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    }
    else if (cc == ColoredConsole::Color::FG_MAGENTA) {
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY);
    }
    else {
      ConsoleColorWhite();
    }
    std::cout << str;
    ConsoleColorWhite();

#elif defined(BR2_OS_LINUX)
    //https://en.wikipedia.org/wiki/ANSI_escape_code#Colors
    const std::string cc_white("\033[0;97m");
    const std::string cc_gray("\033[0;37m");
    const std::string cc_red("\033[0;91m");
    const std::string cc_cyan("\033[0;96m");
    const std::string cc_yellow("\033[0;93m");
    const std::string cc_reset("\033[0m");
    const std::string cc_green("\033[92m");
    const std::string cc_magenta("\033[95m");

    string_t cc_beg = "";
    string_t cc_end = "";

    if (cc == ColoredConsole::Color::FG_WHITE) {
      cc_beg = cc_white;
    }
    else if (cc == ColoredConsole::Color::FG_GRAY) {
      cc_beg = cc_gray;
    }
    else if (cc == ColoredConsole::Color::FG_RED) {
      cc_beg = cc_red;
    }
    else if (cc == ColoredConsole::Color::FG_CYAN) {
      cc_beg = cc_cyan;
    }
    else if (cc == ColoredConsole::Color::FG_YELLOW) {
      cc_beg = cc_yellow;
    }
    else if (cc == ColoredConsole::Color::FG_GREEN) {
      cc_beg = cc_green;
    }
    else if (cc == ColoredConsole::Color::FG_MAGENTA) {
      cc_beg = cc_magenta;
    }
    else {
      cc_beg = cc_white;
    }
    cc_end = cc_reset;

    std::cout << cc_beg << str << cc_end;
#else
    OS_NOT_SUPPORTED_ERROR
#endif
  }
  else {
    std::cout << str;
  }
}

}  // namespace VG

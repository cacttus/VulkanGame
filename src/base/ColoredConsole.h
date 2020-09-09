#pragma once
#ifndef __COLORED_CONSOLE_0234205894098560235483_H__
#define __COLORED_CONSOLE_0234205894098560235483_H__

#include <string>
namespace BR2 { 

//Linux/Windows support.
class ColoredConsole {
public:
  enum class Color { FG_WHITE,FG_GRAY,FG_RED,FG_CYAN,FG_YELLOW };
  static void print(const std::string& str, ColoredConsole::Color cc = ColoredConsole::Color::FG_WHITE);
};

}//NS BR2
#endif
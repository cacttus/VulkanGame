/**
*  @file CoreHeader.h
*  @date 10/10/2020
*  @author MetalMario971
*/
#pragma once
#ifndef __COREHEADER_16050094106409681455028822908_H__
#define __COREHEADER_16050094106409681455028822908_H__

#include "../base/BaseHeader.h"

namespace VG {

class GraphicsDriver;
class SDLApp;
class Component;
class Script;
class LuaScript;
class GraphicsWindow;


class GraphicsWindowCreateParameters {
public:
  static constexpr int Wintype_Desktop = 0;  //X11 doesn't encourage disabling buttons like win32, so we're going with 'window types' instead of disabling properties.
  static constexpr int Wintype_Utility = 1;
  static constexpr int Wintype_Noborder = 2;

  string_t _title = "My_Window";
  int32_t _x = 100;
  int32_t _y = 100;
  int32_t _width = 800;
  int32_t _height = 600;
  int32_t _type = Wintype_Desktop;
  bool _fullscreen = false;
  bool _show = true;               //Show after creating
  bool _forceAspectRatio = false;  //Forces the window buffer to be the same aspect as the screen.
  std::shared_ptr<GraphicsWindow> _parent = nullptr;
  GraphicsWindowCreateParameters(const string_t& title, int32_t x, int32_t y, int32_t width, int32_t height, int32_t type,
                                 bool fullscreen, bool show, bool forceAspectRatio,
                                 std::shared_ptr<GraphicsWindow> parent = nullptr) {
    _title = title;
    _x = x;
    _y = y;
    _width = width;
    _height = height;
    _fullscreen = fullscreen;
    _show = show;
    _forceAspectRatio = forceAspectRatio;
    _parent = parent;
    _type = type;
  }
};


}//ns Game



#endif

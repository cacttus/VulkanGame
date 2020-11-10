#include "../base/BaseHeader.h"
#include "../base/LuaScript.h"
#include "../base/Logger.h"
#include "../base/Gu.h"
#include "../base/ApplicationPackage.h"
#include "../base/FileSystem.h"
#include "../base/OperatingSystem.h"
#include "../base/Stopwatch.h"
#include "../math/Random.h"
#include "../gfx/GraphicsApi.h"
#include "../gfx/GfxHeader.h"
#include "../base/GraphicsWindow.h"
#include "../base/SoundCache.h"
#include <iostream>

//https://github.com/SteveKChiu/lua-intf
#define LUAINTF_HEADERS_ONLY 1
#define LUAINTF_LINK_LUA_COMPILED_IN_CXX 1
#define LUAINTF_STD_WIDE_STRING 0
#include "../ext/LuaIntf/LuaIntf.h"
namespace LuaIntf {
LUA_USING_SHARED_PTR_TYPE(std::shared_ptr)
LUA_USING_LIST_TYPE(std::vector)
LUA_USING_MAP_TYPE(std::map)
}  // namespace LuaIntf

namespace VG {
static int print(lua_State* L) {
  string_t msg = "";
  int n = lua_gettop(L);
  if (n == 1) {
    double sum = 0;
    int i;
    auto argType = lua_type(L, -1);
    if (argType == LUA_TSTRING) {
      const char* s = lua_tostring(L, -1);
      if (s) {
        msg = string_t(s);
      }
      else {
        int n = 0;
        n++;
      }
    }
    else if (argType == LUA_TBOOLEAN) {
      bool b = lua_toboolean(L, -1);
      msg = std::to_string(b);
    }
    else if (argType == LUA_TNUMBER) {
      lua_Number num = lua_tonumber(L, -1);
      msg = std::to_string(num);
    }
    else {
      msg = Stz "<error: invalid arg type to print() = " + argType + ">";
    }
  }
  else {
    msg = Stz "<error: invalid call to print()>";
  }
  if (msg.length() > 0) {
    BRLogScript(msg);
  }
  return 0;
}
#pragma region LuaScript
LuaScript::LuaScript() {
  //Create state
  _pState = luaL_newstate();
  luaL_openlibs(_pState);

//Shortcuts
#define _START_CLASS(x) LuaIntf::LuaBinding(_pState).beginClass<x>(#x)

//This must be a static class member to work.
#define _CONSTANT(c,x) .addConstant(#x, c::x)
//#define _CONSTANT(c,x) .addStaticVariable(#x, c::x, false)

  //Register Script print
  lua_register(_pState, "print", print);

  //shared_ptr
  //https://github.com/SteveKChiu/lua-intf/issues/109
  //Note: type ambiguity errors with overloaded (static) member functions happen.
      int x1 =GraphicsWindowCreateParameters:: Wintype_Desktop;
      int x2 =GraphicsWindowCreateParameters:: Wintype_Utility;
      int x3 =GraphicsWindowCreateParameters:: Wintype_Noborder;
  // BR2 Bindings
  _START_CLASS(Gu)
      .addStaticFunction("getGraphicsApi", &Gu::getGraphicsApi)
      .addStaticFunction("getEngineConfig", &Core::config)
      .addStaticFunction("playSound", &Gu::playSound, LUA_ARGS(const string_t&, const SoundPlayInfo&))
      .endClass();
  _START_CLASS(SoundPlayInfo)
      //You can only have 1 constructor
      .addConstructor(LUA_ARGS(bool, float, float))
      .endClass();
  _START_CLASS(EngineConfig)
      .endClass();
  _START_CLASS(GraphicsApi)
      .addFunction("createWindow",
                   static_cast<std::shared_ptr<GraphicsWindow> (GraphicsApi::*)(const GraphicsWindowCreateParameters&)>(&GraphicsApi::createWindow),
                   LUA_ARGS(const GraphicsWindowCreateParameters&))
      .endClass();
  _START_CLASS(GraphicsWindowCreateParameters)
      .addConstructor(LUA_ARGS(const string_t&,
                               int32_t, int32_t, int32_t, int32_t, int32_t,
                               bool, bool, bool,
                               LuaIntf::_opt<std::shared_ptr<GraphicsWindow>>))
                               //.addVariable("", &GraphicsWindowCreateParameters::Wintype_Desktop, false)
                               //.addConstant("Wintype_Desktop", x1)
                            //   .addConstant("Wintype_Desktop", GraphicsWindowCreateParameters::Wintype_Desktop)
       _CONSTANT(GraphicsWindowCreateParameters, Wintype_Desktop)
       _CONSTANT(GraphicsWindowCreateParameters, Wintype_Utility)
       _CONSTANT(GraphicsWindowCreateParameters, Wintype_Noborder)
      .endClass();
  _START_CLASS(GraphicsWindow)
      .endClass();
  _START_CLASS(vec3)
      .addConstructor(LUA_ARGS(float, float, float))  // use _opt<float> for optional
      .addVariableRef("x", &vec3::x)
      .addVariableRef("y", &vec3::y)
      .addVariableRef("z", &vec3::z)
      .addFunction("distanceTo", &vec3::distance, LUA_ARGS(vec3))
      .addFunction("toString", &vec3::toString, LUA_ARGS(LuaIntf::_def<int, -1, 1>))
      .addFunction("dot", static_cast<float (vec3::*)(const vec3&) const>(&vec3::dot), LUA_ARGS(vec3))
      .addFunction("cross", static_cast<vec3 (vec3::*)(const vec3&) const>(&vec3::cross), LUA_ARGS(vec3))
      .addFunction("__add", static_cast<vec3 (vec3::*)(const vec3&) const>(&vec3::plus), LUA_ARGS(vec3))
      .addFunction("adds", static_cast<vec3 (vec3::*)(const float&) const>(&vec3::plus), LUA_ARGS(float))
      .addFunction("__sub", static_cast<vec3 (vec3::*)(const vec3&) const>(&vec3::minus), LUA_ARGS(vec3))
      .addFunction("subs", static_cast<vec3 (vec3::*)(const float&) const>(&vec3::minus), LUA_ARGS(float))
      .addFunction("__mul", static_cast<vec3 (vec3::*)(const vec3&) const>(&vec3::times), LUA_ARGS(vec3))
      .addFunction("muls", static_cast<vec3 (vec3::*)(const float&) const>(&vec3::times), LUA_ARGS(float))
      .addFunction("__div", static_cast<vec3 (vec3::*)(const vec3&) const>(&vec3::divide), LUA_ARGS(vec3))
      .addFunction("divs", static_cast<vec3 (vec3::*)(const float&) const>(&vec3::divide), LUA_ARGS(float))
      .endClass();
  _START_CLASS(Stopwatch)
      .addConstructor(LUA_ARGS(LuaIntf::_opt<std::string>, LuaIntf::_opt<bool>))  // use _opt<float> for optional
      .addFunction("start", &Stopwatch::start, LUA_ARGS(LuaIntf::_opt<std::string>))
      .addFunction("stop", &Stopwatch::stop, LUA_ARGS(LuaIntf::_opt<bool>, LuaIntf::_opt<bool>))
      .addFunction("deltaMilliseconds", &Stopwatch::deltaMilliseconds)
      .endClass();
  _START_CLASS(Random)
      .addConstructor(LUA_ARGS())
      .addConstructor(LUA_ARGS(int32_t))
      .addFunction("nextFloat", &Random::nextFloat, LUA_ARGS(float, float))
      .addFunction("setSeed", &Random::setSeed, LUA_ARGS(int32_t))
      .endClass();
}
LuaScript::~LuaScript() {
  lua_close(_pState);
}
void LuaScript::runtimeError(const string_t& msg) {
  string_t msg_m = Stz "LUA Runtime Error:" + OperatingSystem::newline() + "  " + _filename + OperatingSystem::newline() + msg + OperatingSystem::newline();
  BRLogError(msg_m);
  Base::debugBreak();
}
void LuaScript::compileError(const string_t& msg) {
  string_t msg_m = Stz "LUA Compile Error:" + OperatingSystem::newline() + "  " + _filename + OperatingSystem::newline() + msg + OperatingSystem::newline();
  BRLogError(msg_m);
  Base::debugBreak();
}

void LuaScript::onStart() {
  //Testing..
  //   while (true) {
  //     string_t file_path = FileSystem::combinePath(Gu::getPackage()->getScriptsFolder(), "/test_math.lua");
  //     compile(file_path);
  //
  //     // std::shared_ptr<LuaFunction> func;
  //     // func = getGlobalFunction("onStart");
  //     // if (func) {
  //     //   func->call(" Data from C++ ");
  //     // }
  //     // func = getGlobalFunction("onUpdate");
  //     // if (func) {
  //     //   func->call(" Data from C++ ");
  //     // }
  //     // func = getGlobalFunction("onExit");
  //     // if (func) {
  //     //   func->call(" Data from C++ ");
  //     // }
  //
  //     // func = getGlobalFunction("SFSDFSDF");
  //     // if (func) {
  //     //   func->call(" Data from C++ ");
  //     // }
  //     std::cin.get();
  //   }
}
void LuaScript::onUpdate(float dt) {
}
void LuaScript::onExit() {
}
void LuaScript::compile(const string_t& filename) {
  _filename = filename;

  string_t file_str = "";
  //Read file lines.
  try {
    file_str += Gu::getPackage()->getFileAsString(filename);
  }
  catch (const Exception& ex) {
    lua_close(_pState);
    return;
  }

  if (luaL_dostring(_pState, file_str.c_str())) {
    string_t msg = string_t(lua_tostring(_pState, -1));
    lua_pop(_pState, 1);
    compileError(msg);
    return;
  }

  return;
}
#pragma endregion

}  // namespace VG

//Parse file.
// for (auto line : lines) {
//   result = luaL_dostring(L, line.c_str());
//   //result = luaL_loadbuffer(L, line.c_str(), line.length(), "line") || lua_pcall(L, 0, 0, 0);
//   if (result) {
//     _pint->compileError(L, filename);
//     return ret;
//   }
// }

// if (!lua_getglobal(L, "onStart")) {
//   _pint->compileError(L, filename);
//   return ret;
// }
// if (!lua_isfunction(L, -1)) {
//   lua_pop(L, 1);
//   return ret;
// }
// class LuaScript;
// class LuaFunction;
// class LuaFunction {
// public:
//   LuaFunction(std::shared_ptr<LuaScript> sc, const string_t& fname);
//   void call(const string_t& data);
//
// private:
//   std::shared_ptr<LuaScript> _pScript = nullptr;
//   string_t _name;
// };
// std::shared_ptr<LuaFunction> LuaScript::getGlobalFunction(const string_t& fname) {
//   std::shared_ptr<LuaFunction> ret = nullptr;
//   if (!lua_getglobal(_pState, fname.c_str())) {
//     runtimeError("Failed to find global function '" + fname + "'");
//     return ret;
//   }
//   if (!lua_isfunction(_pState, -1)) {
//     return ret;
//   }
//   ret = std::make_shared<LuaFunction>(getThis<LuaScript>(), fname);
//
//   //Pop the function name off the stack.
//   lua_pop(_pState, 1);
//
//   return ret;
// }
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
#include "../base/GraphicsWindow.h"
#include <iostream>

//https://github.com/SteveKChiu/lua-intf
#define LUAINTF_HEADERS_ONLY 1
#define LUAINTF_LINK_LUA_COMPILED_IN_CXX 1
#define LUAINTF_STD_WIDE_STRING 0
#include "../ext/LuaIntf/LuaIntf.h"
namespace LuaIntf {
LUA_USING_SHARED_PTR_TYPE(std::shared_ptr)  // LUA_SP
LUA_USING_LIST_TYPE(std::vector)
LUA_USING_MAP_TYPE(std::map)
}  // namespace LuaIntf



namespace BR2 {
static int print(lua_State* L) {
  int n = lua_gettop(L);
  double sum = 0;
  int i;

  string_t msg = "";
  if (n == 1) {
    const char* s = lua_tostring(L, i);
    if(s){
    msg = string_t(s);
    }
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

#define START_CLASS(x) \
  LuaIntf::LuaBinding(_pState).beginClass<x>(#x)

  lua_register(_pState, "print", print);

  // BR2 Bindings
  START_CLASS(Gu)
      .addStaticFunction("getGraphicsApi", &Gu::getGraphicsApi)
      .addStaticFunction("getEngineConfig", &Gu::getEngineConfig)
      .endClass();
  START_CLASS(EngineConfig)
      .endClass();
  START_CLASS(GraphicsApi)
      .addFunction("createWindow", static_cast<std::shared_ptr<GraphicsWindow> (GraphicsApi::*)(const GraphicsWindowCreateParameters&)>(&GraphicsApi::createWindow), LUA_ARGS(const GraphicsWindowCreateParameters&))
      .endClass();
  START_CLASS(GraphicsWindowCreateParameters)
      .addConstructor(LUA_ARGS(const string_t&, int32_t, int32_t, bool, bool, bool, LuaIntf::_opt<std::shared_ptr<GraphicsWindow>>))
      .endClass();
  START_CLASS(GraphicsWindow)
      .endClass();
  START_CLASS(vec3)
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
  START_CLASS(Stopwatch)
      .addConstructor(LUA_ARGS(LuaIntf::_opt<std::string>, LuaIntf::_opt<bool>))  // use _opt<float> for optional
      .addFunction("start", &Stopwatch::start, LUA_ARGS(LuaIntf::_opt<std::string>))
      .addFunction("stop", &Stopwatch::stop, LUA_ARGS(LuaIntf::_opt<bool>, LuaIntf::_opt<bool>))
      .addFunction("deltaMilliseconds", &Stopwatch::deltaMilliseconds)
      .endClass();
  START_CLASS(Random)
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
  Gu::debugBreak();
}
void LuaScript::compileError(const string_t& msg) {
  string_t msg_m = Stz "LUA Compile Error:" + OperatingSystem::newline() + "  " + _filename + OperatingSystem::newline() + msg + OperatingSystem::newline();
  BRLogError(msg_m);
  Gu::debugBreak();
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

}  // namespace BR2

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
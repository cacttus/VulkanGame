#include "../base/BaseHeader.h"
#include "../base/LuaScript.h"
#include "../base/Logger.h"
#include "../base/Gu.h"
#include "../base/ApplicationPackage.h"
#include "../base/FileSystem.h"
#include "../base/OperatingSystem.h"

namespace BR2 {
static int average(lua_State* L) {
  BRLogInfo("Calling 'average' from LUA - exciting!");
  /* get number of arguments */
  int n = lua_gettop(L);
  double sum = 0;
  int i;

  /* loop through each argument */
  for (i = 1; i <= n; i++) {
    /* total the arguments */
    sum += lua_tonumber(L, i);
  }

  /* push the average */
  lua_pushnumber(L, sum / n);

  /* push the sum */
  lua_pushnumber(L, sum);

  /* return the number of results */
  return 2;
}

#pragma region LuaFunction
LuaFunction::LuaFunction(std::shared_ptr<LuaScript> sc, const string_t& fname) {
  _pScript = sc;
  _name = fname;
}
void LuaFunction::call(const string_t& data) {
  if (!lua_getglobal(_pScript->getState(), _name.c_str())) {
    _pScript->runtimeError("Could not find function name " + _name);
    return;
  }
  if (!lua_isfunction(_pScript->getState(), -1)) {
    _pScript->runtimeError("Was not a function: " + _name);
    return;
  }

  lua_pushlstring(_pScript->getState(), data.c_str(), data.length());
  lua_resume(_pScript->getState(), NULL, 1);
}
#pragma endregion
#pragma region LuaScript
LuaScript::LuaScript() {
  //Create state
  _pState = luaL_newstate();
  luaL_openlibs(_pState);

  //Setup C++ <-> Lua interface
  //lua_register(_pState, "average", average);

  // lua_register(_pState, "average", average);

  //Luabind test
  // luabind::module_(_pState) [
  //   luabind::namespace_("MyNamespace")[
  //     luabind::class_<vec2>("vec2")
  //       .def(luabind::constructor<int, int>),
  //       .def("__add", &vec2::operator+),
  //       .def("toString", &vec2::toString)
  //   ]
  // ];
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
std::shared_ptr<LuaFunction> LuaScript::getGlobalFunction(const string_t& fname) {
  std::shared_ptr<LuaFunction> ret = nullptr;
  if (!lua_getglobal(_pState, fname.c_str())) {
    runtimeError("Failed to find global function '" + fname + "'");
    return ret;
  }
  if (!lua_isfunction(_pState, -1)) {
    return ret;
  }
  ret = std::make_shared<LuaFunction>(getThis<LuaScript>(), fname);

  //Pop the function name off the stack.
  lua_pop(_pState, 1);

  return ret;
}
//
// void js_print(CScriptVar *v, void *userdata) {
//     printf("> %s\n", v->getParameter("text")->getString().c_str());
// }
// void js_dump(CScriptVar *v, void *userdata) {
//     CTinyJS *js = (CTinyJS*)userdata;
//     js->root->trace(">  ");
// }
void LuaScript::onStart() {
  //   CTinyJS s;
  //   registerFunctions(&s);
  //   s.addNative("function print(text)", &js_print, 0);
  //   s.addNative("function dump()", &js_dump, &s);
  //
  //   //  registerFunctions(&s);
  //   //  registerMathFunctions(&s);
  //   //  registerStringFunctions(&s);
  //  // s.getRoot()->addChild("result", s.newScriptVar(0));
  //   string_t filename = FileSystem::combinePath(Gu::getPackage()->getScriptsFolder(), "/main.js");
  //   string_t file_str;
  //   try {
  //     file_str = Gu::getPackage()->getFileAsString(filename);
  //   }
  //   catch (const Exception& ex) {
  //     lua_close(_pState);
  //     return;
  //   }
  //
  //   try {
  //     s.execute(file_str);
  //   }
  //   catch (CScriptException* e) {
  //     BRLogError("Error in JS: " + e->text);
  //     delete e;
  //   }
  //   //bool pass = s.getRoot()->findChild("result")->toBoolean();
  //
  //   //TEST TINYHJS
  //
  //   return;

//Testing..
  while (true) {
    string_t path = FileSystem::combinePath(Gu::getPackage()->getScriptsFolder(), "/main.lua");
    compile(path);
    std::shared_ptr<LuaFunction> func = getGlobalFunction("onStart");
    if (func) {
      func->call(" Data from C++ ");
    }
    func = getGlobalFunction("onUpdate");
    if (func) {
      func->call(" Data from C++ ");
    }
    func = getGlobalFunction("onExit");
    if (func) {
      func->call(" Data from C++ ");
    }
    // func = getGlobalFunction("SFSDFSDF");
    // if (func) {
    //   func->call(" Data from C++ ");
    // }
    Gu::debugBreak();
  }
}
void LuaScript::onUpdate(float dt) {
}
void LuaScript::onExit() {
}
void LuaScript::compile(const string_t& filename) {
  _filename = filename;

  //Read file lines.
  string_t file_str;
  try {
    file_str = Gu::getPackage()->getFileAsString(filename);
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

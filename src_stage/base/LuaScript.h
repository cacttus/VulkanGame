/**
*  @file LuaScript.h
*  @date 08/26/2020
*  @author MetalMario971
*/
#pragma once
#ifndef __LUASCRIPT_160116796115863083100017670618_H__
#define __LUASCRIPT_160116796115863083100017670618_H__

#include "../world/Script.h"
#include "../base/LuaIncludes.h"

namespace VG {
class LuaScript : public Script {
public:
  LuaScript();
  virtual ~LuaScript() override;
  virtual void onStart() override;
  virtual void onUpdate(float dt) override;
  virtual void onExit() override;
  lua_State* getState() { return _pState; }
  void compile(const string_t& file);
  void compileError(const string_t& msg);
  void runtimeError(const string_t& str);

private:
  string_t _filename = "";
  lua_State* _pState = nullptr;
  //std::shared_ptr<LuaFunction> getGlobalFunction(const string_t& name);
};

}  // namespace VG

#endif

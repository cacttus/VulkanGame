/**
*  @file EngineConfig.h
*  @date May 8, 2017
*  @author MetalMario971
*/
#pragma once
#ifndef __ENGINECONFIG_14942867052644616215_H__
#define __ENGINECONFIG_14942867052644616215_H__

#include "../base/BaseHeader.h"
#include "../gfx/GfxHeader.h"
#include "../base/XmlFile.h"

namespace BR2 {
class BaseProp : public VirtualMemoryShared<BaseProp> {
public:
  string_t key() { return _key; }

protected:
  string_t _key = "";
};
class BoolProp : BaseProp {
public:
  BoolProp(const string_t& key, bool value) {
    _key = key;
    _value = value;
  }
  bool value() { return _value; }
private:
  bool _value;
};
class Int32Prop : BaseProp {
public:
  Int32Prop(const string_t& key, int32_t value) {
    _key = key;
    _value = value;
  }
  int32_t value() { return _value; }

private:
  int32_t _value;
};
class StringProp : BaseProp {
public:
  StringProp(const string_t& key, string_t value) {
    _key = key;
    _value = value;
  }
  const string_t& value() { return _value; }

private:
  string_t _value;
};

/**
*  @class EngineConfig
*  @brief Stores configuration options for the graphics engine.
*  TODO: this class is getting too big. Create a property system for it.
*/
#define XMLCONFIG_PROP(x) const std::string c_##x = #x;

#define XMLCONFIG_PROP_T(_x, _t)  \
  const std::string c_##_x = #_x; \
  _t get##_x() { return _i##_x; } \
  _t _i##_x;

#define XMLCONFIG_PROP_BOOL_GS(_x, _def) \
public:                                  \
  const std::string c_##_x = #_x;        \
  bool get##_x() { return _b##_x; }      \
  void set##_x(bool v) { _b##_x = v; }   \
                                         \
private:                                 \
  bool _b##_x = _def;

#define XMLCONFIG_PROP_BOOL_G(_x, _def) \
public:                                 \
  const std::string c_##_x = #_x;       \
  bool get##_x() { return _b##_x; }     \
                                        \
private:                                \
  bool _b##_x = _def;

#define XMLCONFIG_PROP_INT32_GS(_x, _def)        \
public:                                          \
  const std::string c_##_x = #_x;                \
  int_fast32_t get##_x() { return _i32##_x; }            \
  void set##_x(int_fast32_t v) { _i32##_x = v; } \
                                                 \
private:                                         \
  int_fast32_t _i32##_x = _def;

#define XMLCONFIG_PROP_INT32_G(_x, _def) \
public:                                  \
  const std::string c_##_x = #_x;        \
  int_fast32_t get##_x() { return _i32##_x; }    \
                                         \
private:                                 \
  int_fast32_t _i32##_x = _def;

class EngineConfig_Internal;
class EngineConfig : public XmlFile {
public:
  EngineConfig();
  virtual ~EngineConfig();
  XMLCONFIG_PROP(MSAASamples);
  XMLCONFIG_PROP(FontBakedCharSize);
  XMLCONFIG_PROP(GpuDeviceName);
  XMLCONFIG_PROP(DefaultFieldOfView);
  XMLCONFIG_PROP(MaxPickDistance);
  XMLCONFIG_PROP(ColorSpace);
  XMLCONFIG_PROP(NumTextQuads);
  XMLCONFIG_PROP(EnableRuntimeErrorChecking);
  XMLCONFIG_PROP(EnableDebugErrorChecking);
  XMLCONFIG_PROP(BakedCharSize);
  XMLCONFIG_PROP(FontBitmapSize);
  XMLCONFIG_PROP(SaveFrameTexture);
  XMLCONFIG_PROP(SaveFrameTextureMod);
  XMLCONFIG_PROP(ShadowMapResolution);
  XMLCONFIG_PROP(ShadowMapMaxInfluences);
  XMLCONFIG_PROP(MaxPointLightShadowDistance);
  XMLCONFIG_PROP(FullscreenWidth);
  XMLCONFIG_PROP(FullscreenHeight);
  XMLCONFIG_PROP(ModelThumbSize);
  XMLCONFIG_PROP(MaxHardwareIncomingBufferSizeBytes);
  XMLCONFIG_PROP(MaxHardwareOutgoingBufferSizeBytes);
  XMLCONFIG_PROP(MaxSoftwareIncomingBufferSizeBytes);
  XMLCONFIG_PROP(MaxSoftwareOutgoingBufferSizeBytes);
  XMLCONFIG_PROP(RenderSystem);
  XMLCONFIG_PROP(WindowedAspectRatio);
  XMLCONFIG_PROP(GamePad);
  XMLCONFIG_PROP(OpenGLProfile);

  XMLCONFIG_PROP_BOOL_G(Linux_ReadableBacktrace, true);
  XMLCONFIG_PROP_BOOL_G(GraphicsErrorLogging_High, true);
  XMLCONFIG_PROP_BOOL_G(GraphicsErrorLogging_Medium, true);
  XMLCONFIG_PROP_BOOL_G(GraphicsErrorLogging_Low, true);
  XMLCONFIG_PROP_BOOL_G(GraphicsErrorLogging_Info, true);
  XMLCONFIG_PROP_BOOL_G(VsyncEnabled, true);
  XMLCONFIG_PROP_BOOL_G(StartFullscreen, false)
  XMLCONFIG_PROP_BOOL_G(EnableFileLog, true)
  XMLCONFIG_PROP_BOOL_G(EnableConsoleLog, true)
  XMLCONFIG_PROP_BOOL_G(ForceAspectRatio, false)
  XMLCONFIG_PROP_BOOL_G(EnableObjectShadows, true)
  XMLCONFIG_PROP_BOOL_G(EnableTerrainShadows, true)
  XMLCONFIG_PROP_BOOL_GS(EnableMSAA, false)
  XMLCONFIG_PROP_BOOL_G(BreakOnSDLError, true)
  XMLCONFIG_PROP_BOOL_G(BreakOnGraphicsError, true)
  XMLCONFIG_PROP_BOOL_GS(ShowConsole, true)

  XMLCONFIG_PROP_INT32_G(MaxPointLights, 32)
  XMLCONFIG_PROP_INT32_G(MaxDirLights, 32)
  XMLCONFIG_PROP_INT32_G(MaxCubeShadowSamples, 4)
  XMLCONFIG_PROP_INT32_G(MaxFrustShadowSamples, 4)
  XMLCONFIG_PROP_INT32_G(DefaultScreenWidth, 800)  //This shouldn't even be a thing. We should use the screen dims to create the window.
  XMLCONFIG_PROP_INT32_G(DefaultScreenHeight, 600)
  XMLCONFIG_PROP_INT32_G(GameHostTimeoutMs, 60000)
  XMLCONFIG_PROP_INT32_G(GameHostPort, 44244)

public:
  void int32Prop(const string_t& a, std::vector<string_t>& toks, int32_t& iind);
  void boolProp(const string_t& a, std::vector<string_t>& toks, int32_t& iind);
  void floatProp(const string_t& a, std::vector<string_t>& toks, int32_t& iind);

  int32_t getMsaaSamples() { return _iMsaaSamples; }
  bool getEnableLogToConsole() { return _bEnableLogToConsole; }
  bool getEnableLogToFile() { return _bEnableLogToFile; }
  string_t getGpuDeviceName() { return _strGpuDeviceName; }
  float getDefaultFieldOfView() { return _fDefaultFieldOfView; }
  float getMaxPickDistance() { return _fMaxPickDistance; }
  ColorSpace::e getColorSpace() { return _eColorSpace; }
  int32_t getNumTextQuads() { return _iNumTextQuads; }
  const bool& getEnableRuntimeErrorChecking() { return _bEnableRuntimeErrorChecking; }
  bool& getEnableDebugErrorChecking() { return _bEnableDebugErrorChecking; }
  int getBakedCharSize() { return _iBakedCharSize; }
  int getFontBitmapSize() { return _iFontBitmapSize; }
  int getSaveFrameTextureMod() { return _iSaveFrameTextureMod; }
  int32_t getShadowMapResolution() { return _iShadowMapResolution; }
  int32_t getShadowMapMaxInfluences() { return _iShadowMapMaxInfluences; }
  float getMaxPointLightShadowDistance() { return _fMaxPointLightShadowDistance; }
  int getFullscreenWidth() { return _iFullscreenWidth; }
  int getFullscreenHeight() { return _iFullscreenHeight; }
  int getModelThumbSize() { return _iModelThumbSize; }
  int getMaxHardwareIncomingBufferSizeBytes() { return _iMaxHardwareIncomingBufferSizeBytes; }
  int getMaxHardwareOutgoingBufferSizeBytes() { return _iMaxHardwareOutgoingBufferSizeBytes; }
  int getMaxSoftwareIncomingBufferSizeBytes() { return _iMaxSoftwareIncomingBufferSizeBytes; }
  int getMaxSoftwareOutgoingBufferSizeBytes() { return _iMaxSoftwareOutgoingBufferSizeBytes; }
  RenderSystem::e getRenderSystem() { return _eRenderSystem; }
  double windowedAspectRatio() { return _windowedAspectRatio; }
  GamePadType gamePadType() { return _eGamePadType; }
  OpenGLProfile openGLProfile() { return _eOpenGLProfile; }
  bool getGameHostAttached() { return _bGameHostAttached; }
  void setGameHostAttached(bool v) { _bGameHostAttached = v; }

protected:
  virtual void pkp(std::vector<string_t>& tokens) override;
  virtual void preLoad() override;
  virtual void postLoad(bool success) override;

private:
  bool _bGameHostAttached = false;
  std::shared_ptr<EngineConfig_Internal> _pint = nullptr;
  OpenGLProfile _eOpenGLProfile = OpenGLProfile::Compatibility;
  GamePadType _eGamePadType = GamePadType::KeyboardAndMouse;
  double _windowedAspectRatio = 1920.0 / 1080.0;
  bool _bEnableLogToFile = true;
  bool _bEnableLogToConsole = true;
  string_t _strGpuDeviceName;
  float _fDefaultFieldOfView = 64.734;
  float _fMaxPickDistance = 2000;
  ColorSpace::e _eColorSpace = ColorSpace::e::SRGB;
  int _iBakedCharSize = 64;
  int _iFontBitmapSize = 2048;
  int _iFullscreenWidth = 1920;
  int _iFullscreenHeight = 1080;
  RenderSystem::e _eRenderSystem = RenderSystem::OpenGL;
  int32_t _iMsaaSamples = 4;
  int _iSaveFrameTextureMod = 10;
  int32_t _iNumTextQuads = 1024;
  int32_t _iShadowMapResolution = 256;
  int32_t _iShadowMapMaxInfluences = 3;
  float _fMaxPointLightShadowDistance = 200.0f;
  int _iModelThumbSize = 256;
  int _iMaxHardwareIncomingBufferSizeBytes = 8192;
  int _iMaxHardwareOutgoingBufferSizeBytes = 8192;
  int _iMaxSoftwareIncomingBufferSizeBytes = 8192;
  int _iMaxSoftwareOutgoingBufferSizeBytes = 8192;

  bool _bEnableRuntimeErrorChecking = true;
#ifdef _DEBUG
  bool _bEnableDebugErrorChecking = true;
#else
  bool _bEnableDebugErrorChecking = false;
#endif
};
}  // namespace BR2

#endif

/**
*  @file ShaderUniform.h
*  @date May 5, 2017
*  @author MetalMario971
*/
#pragma once
#ifndef __SHADERUNIFORM_14940165762940726006_H__
#define __SHADERUNIFORM_14940165762940726006_H__

#include "../gfx/GfxHeader.h"

namespace BR2 {
/**
*  @class ShaderUniform
*  @brief GLSL shader uniform variable (not a block).
*/
class ShaderUniform : public GLFramework {
public:
  ShaderUniform(std::shared_ptr<GLContext> ct, GLenum eType, GLint location, string_t name, size_t iArraySize);
  virtual ~ShaderUniform() override;

  Hash32 getNameHashed() { return _iNameHashed; }
  string_t getName() { return _strName; }
  void copyUniformData(void* pData, GLint count = -1);
  void clearUniformData();
  OpenGLShaderVarType::e getSystemType();
  void setIgnore(bool bIgnore) { _bIgnoreVariable = true; }
  bool getIgnore() { return _bIgnoreVariable; }
  void validate();
  void validateSystemTypeEqualsOpenGlType(OpenGLShaderVarType::e systemNameType, GLenum shaderDefinedUniformType);
  void bindUniformFast();
  bool hasBeenSet() { return _bHasBeenSet; }

  string_t debugGetUniformValueAsString(bool raw);

  void unbind() {
    //Again, not necessary, but it ensures that we set the uniforms
    _bHasBeenSet = false;
    _dbgSetStackInfos.clear();
  }

  template <class Tx>
  static string_t stringifyUniform(std::vector<char>& data, int numComponents, int newlineEle) {
    // @fn stringifyUniform
    // @brief Return a string representation of uniform buffer data.
    // @param data = the data buffer
    // @param numComponents = number of components (vec3 = 3, vec2 = 2)
    // @param newlineEle how many elements until we print a newline.
    string_t ret = "";
    int elesize = sizeof(Tx);
    int eles = data.size() / elesize;
    int nCount = 0;
    int newline = 0;
    string_t delim = "";
    for (size_t iEle = 0; iEle < eles; ++iEle) {
      const char* data_off = ((const char*)(data.data() + iEle));
      Tx ele = *((Tx*)(data_off));
      ret += delim + std::to_string(ele);
      delim = ",";
      nCount++;  //We're printing as much data as we can in case of errors.
      if (nCount == numComponents) {
        ret += " ";  //Separate the data with a space.
        delim = "";
      }
      newline++;
      if (newlineEle > 0 && newline == newlineEle) {
        ret += "\n";
        newline = 0;
      }
    }
    return ret;
  }

protected:
  std::shared_ptr<GLContext> _pContext = nullptr;
  // size_t getArraySize() { return _iArraySize; }

private:
  std::vector<string_t> _dbgSetStackInfos;
  string_t _strName;  //System name
  Hash32 _iNameHashed;
  OpenGLShaderVarType::e _systemType = OpenGLShaderVarType::ut_notype;
  int32_t _iSystemIndex = 0;      // Index of the given variable type, like light1, light2, etc.
  GLint _glLocation = 0;          // locatino of uniform.
  GLenum _glType = GL_ZERO;       //GL enumerated type
  bool _bIgnoreVariable = false;  //Prevents check throw if we didn't set data.
  //Allocator<char>* _pStorage = nullptr; //Temporary holding for uniform data.
  std::vector<char> _pStorage;  //Temporary holding for uniform data.
  GLint _iLastCount = 0;
  size_t _iComponentSize = 0;
  size_t _iComponentCount = 0;
  size_t _iArraySize = 0;  // number of elements statically defined in the shader array []
  bool _bHasBeenSet = false;

  static size_t getByteSizeForType(OpenGLShaderVarType::e etype);
  static size_t getCountForType(OpenGLShaderVarType::e etype);
  static OpenGLShaderVarType::e openglTypeToSystemType(GLenum eType);
};

/**
*  @class ShaderUniformBlock
*  @brief A GLSL uniform block.
*/
class ShaderUniformBlock : public GLFramework {
public:
  ShaderUniformBlock(std::shared_ptr<GLContext> ct, string_t name, GLint iBlockIndex, GLint iBindingIndex, size_t iBufferByteSize);
  virtual ~ShaderUniformBlock() override;
  string_t getName() { return _strName; }
  void copyUniformData(void* value, size_t copySizeBytes);
  bool hasBeenSet() { return _bHasBeenSet; }
  void bindUniformFast();

private:
  std::shared_ptr<GLContext> _pContext = nullptr;
  GLint _iUboId = -2;
  GLint _iBlockIndex = -1;
  GLint _iBindingIndex = -1;
  size_t _iBufferSizeBytes = 0;
  bool _bHasBeenSet = false;
  string_t _strName;
  void* _pValue = nullptr;
};

}  // namespace BR2

#endif

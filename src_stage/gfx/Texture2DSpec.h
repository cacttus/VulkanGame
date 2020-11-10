/**
*  @file Texture2D.h
*  @date August 4, 2011
*  @author MetalMario971
*/
#pragma once
#ifndef __TEXTURE2D_202381107519012904417182_H__
#define __TEXTURE2D_202381107519012904417182_H__

#include "../gfx/GfxHeader.h"
#include "../base/BaseHeader.h"

namespace VG {
enum class TextureFormat { Image4ub,     //png image from disk
                           DepthShadow,  //shadow map - Resolution & BPP, can be changed in the config.xml
                           CubeShadow    //cube shadow map. -  Resolution & BPP, can be changed in the config.xml
};

/**
*  @class Texture2D
*  @brief Stores information about a texture map.
*/
class Texture2DSpec : public GLFramework {
public:
  Texture2DSpec(const string_t& name, TextureFormat fmt, std::shared_ptr<GLContext> ct);
  Texture2DSpec(const string_t& name, string_t loc, std::shared_ptr<GLContext> ctx, bool bRepeatU, bool bRepeatV);
  Texture2DSpec(const string_t& name, TextureFormat fmt, const std::shared_ptr<Img32>, std::shared_ptr<GLContext> ctx, TexFilter::e eFilter = TexFilter::e::Nearest);
  Texture2DSpec(const string_t& name, TextureFormat fmt, std::shared_ptr<GLContext> ctx, unsigned char* texData, int iWidth, int iHeight, bool mipmaps);
  virtual ~Texture2DSpec() override;

  void create(TextureFormat fmt, unsigned char* imageData, uint32_t w, uint32_t h, bool genMipmaps, bool bRepeatU, bool bRepeatV);
  virtual bool bind(TextureChannel::e eChannel, std::shared_ptr<ShaderBase> pShader, bool bIgnoreIfNotFound = false);
  void unbind(TextureChannel::e eChannel);

  string_t getName() { return _strName; }
  string_t getLocation() { return _strLocation; }
  GLuint getGlId() { return _glId; }
  bool getTransparent() { return _bTransparent; }
  void setTransparent(bool b) { _bTransparent = b; }
  uint32_t getWidth() { return _iWidth; }
  uint32_t getHeight() { return _iHeight; }
  float getSizeRatio() { return _fSizeRatio; }
  void setFilter(TexFilter::e filter);           // false = nearest
  TexFilter::e getFilter() { return _eFilter; }  // false = nearest
  void setWrapU(TexWrap::e wrap);
  void setWrapV(TexWrap::e wrap);
  bool getTextureDataFromGpu(std::shared_ptr<Img32> __out_ image);

  void serialize(std::shared_ptr<BinaryFile> fb);
  void deserialize(std::shared_ptr<BinaryFile> fb);

  static int32_t generateMipmapLevels(uint32_t width, uint32_t height);  //return a computed set of mipmap levels for the texture.

private:
  TextureFormat _eFormat;
  GLenum _eGLTextureBinding = GL_TEXTURE_2D;
  GLuint _glId = 0;  // - Texture ID assigned by OpenGL
  GLenum _eGLTextureFormat = 0;
  GLenum _eGLTextureMipmapFormat = 0;
  GLenum _eGLTextureInternalFormat = 0;
  uint32_t _iWidth = 0;
  uint32_t _iHeight = 0;
  float _fSizeRatio = 0.0f;
  string_t _strLocation = "";
  TexFilter::e _eFilter;
  string_t _strName = "";  //Note:this maps to glObjectLabel
  bool _bHasMipmaps = false;
  bool _bRepeatU = false;
  bool _bRepeatV = false;
  bool _bLoadFailed = false;
  bool _bTransparent = false;

  void loadPng(const string_t& loc, bool bRepeatU, bool bRepeatV);
  void calculateGLTextureFormat(TextureFormat type);
};

}  // namespace VG

#endif

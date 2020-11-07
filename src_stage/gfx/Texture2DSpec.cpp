#include "../base/GlobalIncludes.h"
#include "../base/Logger.h"
#include "../base/GLContext.h"
#include "../base/Img32.h"
#include "../base/Gu.h"
#include "../base/Allocator.h"
#include "../base/BinaryFile.h"
#include "../gfx/TexCache.h"
#include "../gfx/ShaderBase.h"
#include "../gfx/Texture2DSpec.h"
#include "../gfx/RenderUtils.h"
#include "../gfx/OpenGLUtils.h"

namespace BR2 {
Texture2DSpec::Texture2DSpec(const string_t& name, TextureFormat fmt, std::shared_ptr<GLContext> ct) : GLFramework(ct) {
  _strName = name;
  _eFormat = fmt;
}
Texture2DSpec::Texture2DSpec(const string_t& name, string_t loc, std::shared_ptr<GLContext> ctx, bool bRepeatU, bool bRepeatV) : GLFramework(ctx) {
  _strName = name;
  _eFormat = TextureFormat::Image4ub;
  loadPng(loc, bRepeatU, bRepeatV);
}
Texture2DSpec::Texture2DSpec(const string_t& name, TextureFormat fmt, const std::shared_ptr<Img32> sp, std::shared_ptr<GLContext> ctx, TexFilter::e eFilter) : GLFramework(ctx) {
  _strName = name;
  create(fmt, (unsigned char*)sp->getData()->ptr(), sp->getWidth(), sp->getHeight(), false, false, false);
  setFilter(eFilter);
}
Texture2DSpec::Texture2DSpec(const string_t& name, TextureFormat fmt, std::shared_ptr<GLContext> ctx, unsigned char* texData, int iWidth, int iHeight, bool mipmaps) : GLFramework(ctx) {
  _strName = name;
  create(fmt, texData, iWidth, iHeight, mipmaps, false, false);
}
Texture2DSpec::~Texture2DSpec() {
  getContext()->glDeleteTextures(1, &_glId);
}
void Texture2DSpec::loadPng(const string_t& imgLoc, bool bRepeatU, bool bRepeatV) {
  _strLocation = imgLoc;
  std::shared_ptr<Img32> sp = Gu::loadImage(imgLoc);
  if (sp != nullptr) {
    create(TextureFormat::Image4ub, (unsigned char*)(sp->getData()->ptr()), sp->getWidth(), sp->getHeight(), false, bRepeatU, bRepeatV);
    Gu::freeImage(sp);
  }
  else {
    _bLoadFailed = true;
  }
}
void Texture2DSpec::calculateGLTextureFormat(TextureFormat fmt) {
  //All textures are 32 bit
  //AssertOrThrow2(getBytesPerPixel() == 4);
  if (fmt == TextureFormat::Image4ub) {
    //We only support one color type.
    _eGLTextureFormat = (GL_RGBA);
    _eGLTextureMipmapFormat = GL_RGBA8;
    _eGLTextureInternalFormat = GL_UNSIGNED_BYTE;
    _eGLTextureBinding = GL_TEXTURE_2D;
  }
  else if (fmt == TextureFormat::DepthShadow) {
    // #define SHADOW_CUBE_MAP_TEX_INTERNAL_FORMAT GL_RGBA16F
    // #define SHADOW_CUBE_MAP_TEX_FORMAT GL_RGBA
    // #define SHADOW_CUBE_MAP_TEX_DATATYPE GL_FLOAT
    //
    //     _eTextureFormat = (GL_RGBA);
    //     _eTextureMipmapFormat = GL_RGBA8;
  }
  else if (fmt == TextureFormat::CubeShadow) {
    // #define SHADOW_CUBE_MAP_TEX_INTERNAL_FORMAT GL_RGBA16F
    // #define SHADOW_CUBE_MAP_TEX_FORMAT GL_RGBA
    // #define SHADOW_CUBE_MAP_TEX_DATATYPE GL_FLOAT
    //
    //     _eTextureFormat = (GL_RGBA);
    //     _eTextureMipmapFormat = GL_RGBA8;
  }
  else {
    BRLogError("Invalid engine texture format " + (int)fmt);
    Gu::debugBreak();
  }
}
bool Texture2DSpec::bind(TextureChannel::e eChannel, std::shared_ptr<ShaderBase> pShader, bool bIgnoreIfNotFound) {
  if (getGlId() == 0) {
    //https://stackoverflow.com/questions/1108589/is-0-a-valid-opengl-texture-id
    BRLogErrorCycle("Texture was not created on the GPU.");
    Gu::debugBreak();
    return false;
  }
  else {
    getContext()->glActiveTexture(GL_TEXTURE0 + eChannel);
    getContext()->glBindTexture(_eGLTextureBinding, getGlId());

    getContext()->chkErrDbg();

    if (pShader != nullptr) {
      //Some cases, we just need to call glBindTexture..
      //But this should almost always be set.
      pShader->setTextureUf(eChannel, bIgnoreIfNotFound);
    }
  }
  return true;
}
void Texture2DSpec::unbind(TextureChannel::e eChannel) {
  getContext()->glActiveTexture(GL_TEXTURE0 + eChannel);
  getContext()->glBindTexture(_eGLTextureBinding, 0);
}
int32_t Texture2DSpec::generateMipmapLevels(uint32_t width, uint32_t height) {
  // - Create log2 mipmaps
  int numMipMaps = 0;
  int x = MathUtils::brMax(width, height);
  for (; x; x = x >> 1) {
    numMipMaps++;
  }
  return numMipMaps;
}
// - Make the texture known to OpenGL
void Texture2DSpec::create(TextureFormat format, unsigned char* imageData, uint32_t w, uint32_t h, bool genMipmaps, bool bRepeatU, bool bRepeatV) {
  _iWidth = w;
  _iHeight = h;
  if (h != 0.0f) {
    _fSizeRatio = (float)w / (float)h;
  }

  calculateGLTextureFormat(format);

  // Bind texture
  getContext()->glActiveTexture(GL_TEXTURE0);
  getContext()->chkErrRt();
  getContext()->glGenTextures(1, &_glId);
  getContext()->chkErrRt();
  getContext()->glBindTexture(_eGLTextureBinding, _glId);
  getContext()->chkErrRt();

  //Calc format
  getContext()->chkErrRt();

  _bHasMipmaps = genMipmaps;
  _bRepeatU = bRepeatU;
  _bRepeatV = bRepeatV;

  //Specify storage mode
  if (genMipmaps) {
    getContext()->chkErrRt();

    int numMipMaps = generateMipmapLevels(getWidth(), getHeight());
    getContext()->glTexStorage2D(_eGLTextureBinding, numMipMaps, _eGLTextureMipmapFormat, getWidth(), getHeight());
    getContext()->chkErrRt();
  }
  else {
    getContext()->glTexStorage2D(_eGLTextureBinding, 1, _eGLTextureMipmapFormat, getWidth(), getHeight());
    getContext()->chkErrRt();
  }

  // Copy texture data
  glTexSubImage2D(
      _eGLTextureBinding,
      0, 0, 0,  //level, x, y
      w,
      h,
      _eGLTextureFormat,          //format
      _eGLTextureInternalFormat,  //type
      (void*)imageData            //pixels
  );

  getContext()->chkErrRt();

  // Specify Parameters
  if (bRepeatU) {
    glTexParameteri(_eGLTextureBinding, GL_TEXTURE_WRAP_S, GL_REPEAT);
  }
  else {
    glTexParameteri(_eGLTextureBinding, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  }
  if (bRepeatV) {
    glTexParameteri(_eGLTextureBinding, GL_TEXTURE_WRAP_T, GL_REPEAT);
  }
  else {
    glTexParameteri(_eGLTextureBinding, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  }

  if (genMipmaps) {
    glTexParameteri(_eGLTextureBinding, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(_eGLTextureBinding, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  }
  else {
    //Default to nearest filtering * this is needed because the
    //texture atlas shows edges with linear filtering
    glTexParameteri(_eGLTextureBinding, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(_eGLTextureBinding, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  }

  //if(Gu::GetEngineDisplayParams()->getEnableAnisotropicFiltering())
  //{
  //    glTexParameterf(_eFormat, GL_TEXTURE_MAX_ANISOTROPY_EXT, Gu::GetEngineDisplayParams()->getTextureAnisotropyLevel());
  //    Gu::checkErrors();
  //}

  // - Generate the mipmaps
  if (genMipmaps) {
    //GL 3.0 mipmaps
    getContext()->glGenerateMipmap(_eGLTextureBinding);
    getContext()->chkErrRt();
  }

  getContext()->setObjectLabel(GL_TEXTURE, getGlId(), _strName);

  //Unbind so we don't modify it
  getContext()->glBindTexture(_eGLTextureBinding, 0);
}
void Texture2DSpec::setWrapU(TexWrap::e wrap) {
  bind(TextureChannel::e::Channel0, nullptr);
  if (wrap == TexWrap::e::Clamp) {
    glTexParameteri(_eGLTextureBinding, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  }
  else if (wrap == TexWrap::e::Repeat) {
    glTexParameteri(_eGLTextureBinding, GL_TEXTURE_WRAP_S, GL_REPEAT);
  }
  else {
    BRThrowNotImplementedException();
  }
  unbind(TextureChannel::e::Channel0);
}
void Texture2DSpec::setWrapV(TexWrap::e wrap) {
  bind(TextureChannel::e::Channel0, nullptr);
  if (wrap == TexWrap::e::Clamp) {
    glTexParameteri(_eGLTextureBinding, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  }
  else if (wrap == TexWrap::e::Repeat) {
    glTexParameteri(_eGLTextureBinding, GL_TEXTURE_WRAP_T, GL_REPEAT);
  }
  else {
    BRThrowNotImplementedException();
  }
  unbind(TextureChannel::e::Channel0);
}
void Texture2DSpec::setFilter(TexFilter::e filter) {
  _eFilter = filter;
  bind(TextureChannel::e::Channel0, nullptr);
  getContext()->chkErrDbg();

  if (filter == TexFilter::e::Linear) {
    if (_bHasMipmaps) {
      getContext()->glGenerateMipmap(_eGLTextureBinding);
      glTexParameteri(_eGLTextureBinding, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(_eGLTextureBinding, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      //Note: GL_GENERATE_MIPMAP is deprecated.
    }
    else {
      glTexParameteri(_eGLTextureBinding, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(_eGLTextureBinding, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      //GL_GENERATE_MIPMAP is deprecated.
    }
  }
  else if (filter == TexFilter::e::Nearest) {
    if (_bHasMipmaps) {
      getContext()->glGenerateMipmap(_eGLTextureBinding);
      glTexParameteri(_eGLTextureBinding, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(_eGLTextureBinding, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    }
    else {
      glTexParameteri(_eGLTextureBinding, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(_eGLTextureBinding, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }
  }
  else {
    BRThrowNotImplementedException();
  }
  getContext()->chkErrRt();

  unbind(TextureChannel::e::Channel0);
}
bool Texture2DSpec::getTextureDataFromGpu(std::shared_ptr<Img32> __out_ image) {
  return getContext()->getTextureDataFromGpu(image, _glId, _eGLTextureBinding);
}
void Texture2DSpec::serialize(std::shared_ptr<BinaryFile> fb) {
  std::shared_ptr<Img32> img = std::make_shared<Img32>();
  getTextureDataFromGpu(img);

  fb->writeVersion();
  fb->writeString(std::move(_strName));
  fb->writeString(std::move(_strLocation));
  fb->writeUint32((uint32_t)_eFormat);
  fb->writeUint32(getWidth());
  fb->writeUint32(getHeight());
  fb->writeBool(std::move(_bHasMipmaps));
  fb->writeBool(std::move(_bRepeatU));
  fb->writeBool(std::move(_bRepeatV));

  fb->writeUint32(std::move((uint32_t)img->getData()->byteSize()));
  fb->write((const char*)img->getData()->ptr(), img->getData()->byteSize());
}
void Texture2DSpec::deserialize(std::shared_ptr<BinaryFile> fb) {
  fb->readVersion();
  fb->readString(_strName);
  fb->readString(_strLocation);

  TextureFormat format;
  fb->readUint32((uint32_t&)format);
  uint32_t iWidth;
  fb->readUint32(iWidth);
  uint32_t iHeight;
  fb->readUint32(iHeight);
  bool bHasMipmaps;
  fb->readBool(bHasMipmaps);
  bool bRepeatU;
  fb->readBool(bRepeatU);
  bool bRepeatV;
  fb->readBool(bRepeatV);

  uint32_t dataSize;
  fb->readUint32(dataSize);

  unsigned char* data = new unsigned char[dataSize];
  fb->read((const char*)data, dataSize);

  create(format, data, iWidth, iHeight, bHasMipmaps, bRepeatU, bRepeatV);
}
}  // namespace BR2
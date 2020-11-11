#include "../base/Logger.h"
#include "../core/opengl/GLContext.h"
#include "../base/Gu.h"
#include "../base/Exception.h"
#include "../base/EngineConfig.h"
#include "../math/MathAll.h"
#include "../gfx/FramebufferBase.h"
#include "../gfx/CameraNode.h"
#include "../gfx/ShaderBase.h"
#include "../gfx/RenderUtils.h"
#include "../core/opengl/OpenGLUtils.h"
#include "../gfx/BufferRenderTarget.h"
#include "../model/ModelHeader.h"
#include "../model/VertexFormat.h"
#include "../model/MeshNode.h"
#include "../model/VboData.h"
#include "../model/VaoDataGeneric.h"

namespace VG {
FramebufferBase::FramebufferBase(const string_t& label, std::shared_ptr<GLContext> pc, bool bMsaa, int nMsaa, vec4& vClear) : GLFramework(pc) {
  _vClear = vClear;
  _bMsaaEnabled = bMsaa;
  _nMsaaSamples = nMsaa;
  _label = label;
}
FramebufferBase::~FramebufferBase() {
  deleteTargets();
}
std::shared_ptr<BufferRenderTarget> FramebufferBase::getTargetByName(const string_t& name) {
  auto it = _mapTargets.find(name);
  if (it != _mapTargets.end()) {
    return it->second;
  }
  return nullptr;
}
void FramebufferBase::checkFramebufferComplete() {
  getContext()->chkErrRt();
  getContext()->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _uiGlFramebufferId);
  getContext()->chkErrRt();

  attachAllTargets();
  setDrawAllTargets();

  GLenum status = getContext()->glCheckFramebufferStatus(GL_FRAMEBUFFER);
  getContext()->chkErrRt();

  if (status != GL_FRAMEBUFFER_COMPLETE) {
    if (status == GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE) {
      BRLogError("Framebuffer is not complete.  Multisampling error.  Make sure that you enable " +
                 "multisampling on ALL textures, additionally make sure all textures have the same setting for FIXED_SAMPLE_LOCATIONS");
    }
    getContext()->chkErrRt();
    BRThrowException("Failed to create framebuffer.");
  }
}
void FramebufferBase::attachAllTargets() {
  for (std::shared_ptr<BufferRenderTarget> inf : _vecTargets) {
    inf->bind();
    //getContext()->glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, inf->getAttachment(), inf->getTextureTarget(), inf->getTexId(), 0);
    //getContext()->chkErrRt();
  }
}
void FramebufferBase::setDrawAllTargets() {
  const int c_iMaxAttachments = 64;
  GLenum attachments[c_iMaxAttachments];
  int iCount = 0;
  for (int i = 0; i < c_iMaxAttachments; ++i) {
    if (i < (int)_vecTargets.size()) {
      if (_vecTargets[i]->getTargetType() == RenderTargetType::e::Color ||
          _vecTargets[i]->getTargetType() == RenderTargetType::e::Pick ||
          _vecTargets[i]->getTargetType() == RenderTargetType::e::Shadow) {
        attachments[i] = _vecTargets[i]->getAttachment();
        iCount++;
      }
    }
    else {
      attachments[i] = 0;
    }
  }

  getContext()->glDrawBuffers(iCount, attachments);
  getContext()->chkErrDbg();
}
void FramebufferBase::addTarget(const string_t& strName, GLenum internalFormat, GLenum texFormat,
                                GLenum dataType, int32_t w, int32_t h, RenderTargetType::e eTargetType) {
  int iIndex = (int)_vecTargets.size();

  std::shared_ptr<BufferRenderTarget> inf = createTarget(getContext(), strName, internalFormat, texFormat, dataType, w, h,
                                                         eTargetType, iIndex, _bMsaaEnabled, _nMsaaSamples);
  _vecTargets.push_back(inf);
  _mapTargets.insert(std::make_pair(strName, inf));
}
void FramebufferBase::addTarget(std::shared_ptr<BufferRenderTarget> other) {
  int iIndex = (int)_vecTargets.size();

  std::shared_ptr<BufferRenderTarget> inf = std::make_shared<BufferRenderTarget>(other->_strName, getContext(), true);
  inf->_iLayoutIndex = iIndex;
  inf->_eTextureTarget = other->_eTextureTarget;
  inf->_eAttachment = GL_COLOR_ATTACHMENT0 + iIndex;
  inf->_eTextureChannel = GL_TEXTURE0 + iIndex;
  inf->_eBlitBit = GL_COLOR_BUFFER_BIT;
  inf->_eTargetType = other->_eTargetType;
  inf->_iGlTexId = other->_iGlTexId;
  inf->_iWidth = other->_iWidth;
  inf->_iHeight = other->_iHeight;

  _vecTargets.push_back(inf);
  _mapTargets.insert(std::make_pair(other->_strName, inf));
}
std::shared_ptr<BufferRenderTarget> FramebufferBase::createTarget(std::shared_ptr<GLContext> ctx, const string_t& strName, GLenum internalFormat, GLenum texFormat,
                                                                  GLenum dataType, int32_t w, int32_t h, RenderTargetType::e eTargetType, int32_t iIndex, bool bMsaaEnabled, int32_t nMsaaSamples) {
  std::shared_ptr<BufferRenderTarget> inf = std::make_shared<BufferRenderTarget>(strName, ctx, false);
  inf->_iLayoutIndex = iIndex;
  inf->_eTextureTarget = GL_TEXTURE_2D;
  inf->_eAttachment = GL_COLOR_ATTACHMENT0 + iIndex;
  inf->_eTextureChannel = GL_TEXTURE0 + iIndex;
  inf->_eBlitBit = GL_COLOR_BUFFER_BIT;
  inf->_eTargetType = eTargetType;
  inf->_iWidth = w;
  inf->_iHeight = h;

  GLint maxAttach = 0;
  glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxAttach);
  if (iIndex > maxAttach) {
    BRThrowException("GPU Does not support enough color attachments, wanted: " + iIndex + " max supported: " + maxAttach);
  }

  makeRenderTexture(ctx, &inf->_iGlTexId,
                    inf->_eAttachment, internalFormat, texFormat, dataType, w, h,
                    &inf->_eTextureTarget, bMsaaEnabled, nMsaaSamples);
  ctx->setObjectLabel(GL_TEXTURE, inf->_iGlTexId, inf->getName());

  return inf;
}
std::shared_ptr<BufferRenderTarget> FramebufferBase::createDepthTarget(std::shared_ptr<GLContext> ctx, const string_t& strName, int32_t w, int32_t h, int iIndex, bool bMsaaEnabled, int32_t nMsaaSamples) {
  std::shared_ptr<BufferRenderTarget> inf = std::make_shared<BufferRenderTarget>(strName, ctx, true);
  //**Note: index doesn't matter for depth target since we simply bind it to GL_Depth_attachment.
  inf->_iLayoutIndex = iIndex;
  if (bMsaaEnabled) {
    inf->_eTextureTarget = GL_TEXTURE_2D_MULTISAMPLE;
  }
  else {
    inf->_eTextureTarget = GL_TEXTURE_2D;
  }
  inf->_eAttachment = GL_DEPTH_ATTACHMENT;  //GL_COLOR_ATTACHMENT0 + iLayoutIndex;
  inf->_eTextureChannel = GL_TEXTURE0 + iIndex;
  inf->_eBlitBit = GL_DEPTH_BUFFER_BIT;
  inf->_eTargetType = RenderTargetType::e::Depth;
  inf->_iWidth = w;
  inf->_iHeight = h;

  //This will cycle through depth formats and choose the most precise.
  ctx->createDepthTexture(strName, &inf->_iGlTexId, w, h, bMsaaEnabled, nMsaaSamples, GL_DEPTH_COMPONENT32F);
  ctx->setObjectLabel(GL_TEXTURE, inf->_iGlTexId, inf->getName());

  return inf;
}
void FramebufferBase::deleteTargets() {
  getContext()->glBindFramebuffer(GL_FRAMEBUFFER, (GLuint)0);

  if (_uiGlFramebufferId > 0) {
    getContext()->glDeleteFramebuffers(1, &_uiGlFramebufferId);
  }

  for (size_t i = 0; i < _vecTargets.size(); ++i) {
    _vecTargets[i] = nullptr;
  }
  _vecTargets.resize(0);
  _mapTargets.clear();
}
//void FramebufferBase::resizeScreenBuffers(int32_t w, int32_t h, std::shared_ptr<RenderTarget> pSharedDepthTarget)
//{
//    deleteTargets();
//    init(w, h, pSharedDepthTarget);
//}
void FramebufferBase::makeRenderTexture(std::shared_ptr<GLContext> ctx, GLuint* iTexId, GLenum eAttachment, GLenum eInternalFormat, GLenum eTextureFormat, GLenum eDataType, int32_t iWidth, int32_t iHeight,
                                        GLenum* eOutTarget, bool bMultisample, int32_t nSamples) {
  ctx->glGenTextures(1, iTexId);
  ctx->chkErrRt();

  if (bMultisample) {
    ctx->glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, *iTexId);
    ctx->chkErrRt();

    //if (Gu::GetEngineDisplayParams()->getEnableAnisotropicFiltering())
    //{
    //    //CHANGED FROM GL_TEXTURE_2D **MIGHT BE WRONG
    //    glTexParameterf(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAX_ANISOTROPY_EXT, Gu::GetEngineDisplayParams()->getTextureAnisotropyLevel());
    //    CheckGpuErrorsDbg();
    //}
    ctx->glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, nSamples, eInternalFormat, iWidth, iHeight, GL_TRUE);
    ctx->chkErrRt();

    if (eOutTarget != NULL){
      *eOutTarget = GL_TEXTURE_2D_MULTISAMPLE;
    }
  }
  else {
    ctx->glBindTexture(GL_TEXTURE_2D, *iTexId);
    ctx->chkErrRt();
    //if (Gu::GetEngineDisplayParams()->getEnableAnisotropicFiltering())
    //{
    //    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, Gu::GetEngineDisplayParams()->getTextureAnisotropyLevel());
    //    Gu::getGraphicsContext()->chkErrRt();
    //}
    glTexImage2D(GL_TEXTURE_2D, 0, eInternalFormat, iWidth, iHeight, 0, eTextureFormat, eDataType, nullptr);
    ctx->chkErrRt();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    ctx->chkErrRt();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    ctx->chkErrRt();

    if (eOutTarget != nullptr) {
      *eOutTarget = GL_TEXTURE_2D;
    }
  }
  glDisable(GL_DITHER);  //Dithering gets enabled for some reason

  ctx->chkErrRt();
}

}  // namespace VG

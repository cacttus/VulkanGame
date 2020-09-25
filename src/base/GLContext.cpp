#include "../base/GlobalIncludes.h"
#include "../base/BaseHeader.h"
#include "../base/TypeConv.h"
#include "../base/StringUtil.h"
#include "../base/Logger.h"
#include "../base/GLContext.h"
#include "../base/Gu.h"
#include "../base/InputManager.h"
#include "../base/Sequencer.h"
#include "../base/GraphicsWindow.h"
#include "../base/Logger.h"
#include "../base/FrameSync.h"
#include "../base/SoundCache.h"
#include "../base/Logger.h"
#include "../base/SDLUtils.h"
#include "../base/OglErr.h"
#include "../base/Allocator.h"
#include "../base/GraphicsWindow.h"
#include "../base/OperatingSystem.h"
#include "../base/EngineConfig.h"
#include "../base/FileSystem.h"
#include "../base/Img32.h"
#include "../math/MathAll.h"
#include "../gfx/RenderUtils.h"
#include "../gfx/OpenGLUtils.h"
#include "../gfx/ShaderBase.h"
#include "../gfx/ParticleManager.h"
#include "../gfx/ShaderUniform.h"
#include "../gfx/TexCache.h"
#include "../gfx/ShaderMaker.h"
#include "../gfx/ShaderMaker.h"
#include "../gfx/LightManager.h"
#include "../gfx/Picker.h"
#include "../gfx/UiControls.h"
#include "../gfx/CameraNode.h"
#include "../gfx/RenderSettings.h"
#include "../gfx/GraphicsContext.h"
#include "../model/VertexFormat.h"
#include "../model/ModelCache.h"
#include "../world/Scene.h"
#include <iostream>
namespace BR2 {
GLContext::GLContext(std::shared_ptr<GraphicsApi> api, std::shared_ptr<GLProfile> profile, SDL_Window* sdl_win) : GraphicsContext(api) {
  _profile = profile;
  _pSDLWindow = sdl_win;

}
GLContext::~GLContext() {
  if (_context) {
    _pOglErr = nullptr;
    /* SDL_GL_MakeCurrent(0, NULL); */ /* doesn't do anything */
    SDL_GL_DeleteContext(_context);
  }
}
bool GLContext::init() {
  bool bRet = false;

  _context = SDL_GL_CreateContext(_pSDLWindow);
  if (!_context) {
    //Eat the "context failed" error.  It's not informative.
    SDLUtils::checkSDLErr(false, false);
    bRet = false;
  }
  else {
    int ver, subver, shad_ver, shad_subver;
    getOpenGLVersion(ver, subver, shad_ver, shad_subver);

    //Make sure we have a good depth value.
    int iDepth = 0;
    SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &iDepth);
    _iSupportedDepthSize = iDepth;
    if (iDepth < _profile->_iDepthBits) {
      BRLogWarn("Depth buffer size " + _profile->_iDepthBits = " not supported in profile.");
      bRet = false;
    }
    else if (checkOpenGlMinimumVersionInfo(_profile->_iMinVersion, _profile->_iMinSubVersion)) {
      printHelpfulDebug();

      if (_profile->_iMSAABuffers == 0) {
        Gu::getEngineConfig()->setEnableMSAA(false);
        BRLogWarn("This configuration did not support MSAA.");
      }

      if (!loadOpenGLFunctions()) {
        BRLogError("Failed to load context.");
        SDL_GL_DeleteContext(_context);
        bRet = false;
      }
      else {
        //Quick GL test.
        glUseProgram(0);

        //Check that OpenGL initialized successfully
        loadCheckProc();

        //Create opengl error handler
        _pOglErr = std::make_unique<OglErr>();
        _pRenderUtils = std::make_unique<RenderUtils>(getThis<GLContext>());

        bRet = true;
      }
    }
  }

  return bRet;
}
void GLContext::setPolygonMode(PolygonMode p) {
  if (isForwardCompatible() == false) {
    GLenum mode = GL_FILL;
    if (p == PolygonMode::Line) {
      mode = GL_LINE;
    }
    else if (p == PolygonMode::Point) {
      mode = GL_POINT;
    }
    //These must be #ifdef out because glPolygonMOde is not present in gl330 core
    glPolygonMode(GL_FRONT, mode);
    glPolygonMode(GL_BACK, mode);
    chkErrDbg();
  }
  else {
    BRLogWarnOnce("glPolygonMode not supported in compatibility mode.");
  }
}
void GLContext::clearGPULog() {
  return _pOglErr->clearGPULog(shared_from_this());
}
bool GLContext::chkErrRt(bool bDoNotBreak, bool doNotLog, const string_t& shaderName, bool clearOnly) {
  //Enable runtime errors.
  return _pOglErr->chkErrRt(shared_from_this(), bDoNotBreak, doNotLog, shaderName, clearOnly);
}
bool GLContext::chkErrDbg(bool bDoNotBreak, bool doNotLog, const string_t& shaderName, bool clearOnly) {
  return _pOglErr->chkErrDbg(shared_from_this(), bDoNotBreak, doNotLog, shaderName, clearOnly);
}
bool GLContext::loadOpenGLFunctions() {
  bool bValid = true;

#define SDLGLP2(x, y)                                                                                       \
  do {                                                                                                      \
    if (!(y = (x)SDL_GL_GetProcAddress(#y))) {                                                              \
      BRLogError("GL Method " + #y + " not supported by your GPU, or there was another error somewhere.."); \
      bValid = false;                                                                                       \
      Gu::debugBreak();                                                                                     \
    }                                                                                                       \
  } while (0)
  //Quick
  //(PFN[A-Z0-9_]*)\s+([a-zA-Z0-9_]*)\s*=\s*nullptr;
  //SDLGLP2($1, $2)
  SDLGLP2(PFNGLUSEPROGRAMPROC, glUseProgram);
  SDLGLP2(PFNGLBINDBUFFERARBPROC, glBindBuffer);
  SDLGLP2(PFNGLGENBUFFERSPROC, glGenBuffers);
  SDLGLP2(PFNGLBUFFERDATAPROC, glBufferData);
  SDLGLP2(PFNGLBUFFERSUBDATAPROC, glBufferSubData);
  SDLGLP2(PFNGLSHADERSOURCEPROC, glShaderSource);
  SDLGLP2(PFNGLCOMPILESHADERPROC, glCompileShader);
  SDLGLP2(PFNGLGETSHADERIVPROC, glGetShaderiv);
  SDLGLP2(PFNGLGETSHADERINFOLOGPROC, glGetShaderInfoLog);
  SDLGLP2(PFNGLCREATEPROGRAMPROC, glCreateProgram);
  SDLGLP2(PFNGLATTACHSHADERPROC, glAttachShader);
  SDLGLP2(PFNGLLINKPROGRAMPROC, glLinkProgram);
  SDLGLP2(PFNGLDETACHSHADERPROC, glDetachShader);
  SDLGLP2(PFNGLDELETESHADERPROC, glDeleteShader);
  SDLGLP2(PFNGLCREATESHADERPROC, glCreateShader);
  SDLGLP2(PFNGLDELETEPROGRAMPROC, glDeleteProgram);
  SDLGLP2(PFNGLGETPROGRAMIVPROC, glGetProgramiv);
  SDLGLP2(PFNGLGETPROGRAMINFOLOGPROC, glGetProgramInfoLog);
  SDLGLP2(PFNGLENABLEVERTEXATTRIBARRAYPROC, glEnableVertexAttribArray);
  SDLGLP2(PFNGLVERTEXATTRIBPOINTERPROC, glVertexAttribPointer);
  SDLGLP2(PFNGLVERTEXATTRIBIPOINTERPROC, glVertexAttribIPointer);
  SDLGLP2(PFNGLDISABLEVERTEXATTRIBARRAYPROC, glDisableVertexAttribArray);
  SDLGLP2(PFNGLACTIVETEXTUREPROC, glActiveTexture);
  SDLGLP2(PFNGLGETUNIFORMLOCATIONPROC, glGetUniformLocation);
  SDLGLP2(PFNGLGETDEBUGMESSAGELOGPROC, glGetDebugMessageLog);
  SDLGLP2(PFNGLGETOBJECTLABELPROC, glGetObjectLabel);
  SDLGLP2(PFNGLOBJECTLABELPROC, glObjectLabel);
  SDLGLP2(PFNGLOBJECTPTRLABELPROC, glObjectPtrLabel);
  SDLGLP2(PFNGLGETOBJECTPTRLABELPROC, glGetObjectPtrLabel);
  SDLGLP2(PFNGLGENVERTEXARRAYSPROC, glGenVertexArrays);
  SDLGLP2(PFNGLBINDVERTEXARRAYPROC, glBindVertexArray);
  SDLGLP2(PFNGLDELETEBUFFERSPROC, glDeleteBuffers);
  SDLGLP2(PFNGLISBUFFERPROC, glIsBuffer);
  SDLGLP2(PFNGLMAPBUFFERPROC, glMapBuffer);
  SDLGLP2(PFNGLUNMAPBUFFERPROC, glUnmapBuffer);
  SDLGLP2(PFNGLGETBUFFERPARAMETERIVPROC, glGetBufferParameteriv);
  SDLGLP2(PFNGLDELETEVERTEXARRAYSPROC, glDeleteVertexArrays);
  SDLGLP2(PFNGLBINDFRAMEBUFFERPROC, glBindFramebuffer);
  SDLGLP2(PFNGLGENFRAMEBUFFERSPROC, glGenFramebuffers);
  SDLGLP2(PFNGLFRAMEBUFFERPARAMETERIPROC, glFramebufferParameteri);
  SDLGLP2(PFNGLDRAWBUFFERSPROC, glDrawBuffers);
  SDLGLP2(PFNGLFRAMEBUFFERTEXTURE2DPROC, glFramebufferTexture2D);
  SDLGLP2(PFNGLCHECKFRAMEBUFFERSTATUSPROC, glCheckFramebufferStatus);
  SDLGLP2(PFNGLDELETEFRAMEBUFFERSPROC, glDeleteFramebuffers);
  SDLGLP2(PFNGLGETACTIVEUNIFORMPROC, glGetActiveUniform);
  SDLGLP2(PFNGLTEXSTORAGE2DPROC, glTexStorage2D);
  SDLGLP2(PFNGLGENERATEMIPMAPPROC, glGenerateMipmap);
  SDLGLP2(PFNGLBINDRENDERBUFFERPROC, glBindRenderbuffer);
  SDLGLP2(PFNGLGENRENDERBUFFERSPROC, glGenRenderbuffers);
  SDLGLP2(PFNGLRENDERBUFFERSTORAGEPROC, glRenderbufferStorage);
  SDLGLP2(PFNGLFRAMEBUFFERRENDERBUFFERPROC, glFramebufferRenderbuffer);
  SDLGLP2(PFNGLGETUNIFORMBLOCKINDEXPROC, glGetUniformBlockIndex);
  SDLGLP2(PFNGLUNIFORMBLOCKBINDINGPROC, glUniformBlockBinding);
  SDLGLP2(PFNGLBINDBUFFERBASEPROC, glBindBufferBase);
  SDLGLP2(PFNGLUNIFORM1IPROC, glUniform1i);
  SDLGLP2(PFNGLUNIFORM1IVPROC, glUniform1iv);
  SDLGLP2(PFNGLUNIFORM2IVPROC, glUniform2iv);
  SDLGLP2(PFNGLUNIFORM3IVPROC, glUniform3iv);
  SDLGLP2(PFNGLUNIFORM4IVPROC, glUniform4iv);
  SDLGLP2(PFNGLUNIFORM1FVPROC, glUniform1fv);
  SDLGLP2(PFNGLUNIFORM2FVPROC, glUniform2fv);
  SDLGLP2(PFNGLUNIFORM3FVPROC, glUniform3fv);
  SDLGLP2(PFNGLUNIFORM4FVPROC, glUniform4fv);
  SDLGLP2(PFNGLUNIFORMMATRIX2FVPROC, glUniformMatrix2fv);
  SDLGLP2(PFNGLUNIFORMMATRIX3FVPROC, glUniformMatrix3fv);
  SDLGLP2(PFNGLUNIFORMMATRIX4FVPROC, glUniformMatrix4fv);
  SDLGLP2(PFNGLUNIFORM1UIVPROC, glUniform1uiv);
  SDLGLP2(PFNGLUNIFORM2UIVPROC, glUniform2uiv);
  SDLGLP2(PFNGLUNIFORM3UIVPROC, glUniform3uiv);
  SDLGLP2(PFNGLUNIFORM4UIVPROC, glUniform4uiv);
  SDLGLP2(PFNGLGETVERTEXATTRIBIUIVPROC, glGetVertexAttribIuiv);
  SDLGLP2(PFNGLGETVERTEXATTRIBFVPROC, glGetVertexAttribfv);
  SDLGLP2(PFNGLGETVERTEXATTRIBIVPROC, glGetVertexAttribiv);
  SDLGLP2(PFNGLGETVERTEXATTRIBIIVPROC, glGetVertexAttribIiv);
  SDLGLP2(PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC, glGetFramebufferAttachmentParameteriv);
  SDLGLP2(PFNGLGETACTIVEUNIFORMBLOCKIVPROC, glGetActiveUniformBlockiv);
  SDLGLP2(PFNGLTEXIMAGE2DMULTISAMPLEPROC, glTexImage2DMultisample);
  SDLGLP2(PFNGLBLITFRAMEBUFFERPROC, glBlitFramebuffer);
  SDLGLP2(PFNGLGETPROGRAMBINARYPROC, glGetProgramBinary);
  SDLGLP2(PFNGLISPROGRAMPROC, glIsProgram);
  SDLGLP2(PFNGLPROGRAMBINARYPROC, glProgramBinary);
  SDLGLP2(PFNGLVALIDATEPROGRAMPROC, glValidateProgram);
  SDLGLP2(PFNGLGETACTIVEATTRIBPROC, glGetActiveAttrib);
  SDLGLP2(PFNGLGETATTRIBLOCATIONPROC, glGetAttribLocation);
  SDLGLP2(PFNGLMEMORYBARRIERPROC, glMemoryBarrier);
  SDLGLP2(PFNGLMAPBUFFERRANGEPROC, glMapBufferRange);
  SDLGLP2(PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC, glGetActiveUniformBlockName);
  SDLGLP2(PFNGLFENCESYNCPROC, glFenceSync);
  SDLGLP2(PFNGLISSYNCPROC, glIsSync);
  SDLGLP2(PFNGLGETSYNCIVPROC, glGetSynciv);
  SDLGLP2(PFNGLDELETESYNCPROC, glDeleteSync);
  SDLGLP2(PFNGLDISPATCHCOMPUTEPROC, glDispatchCompute);
  SDLGLP2(PFNGLGETINTEGERI_VPROC, glGetIntegeri_v);
  SDLGLP2(PFNGLSHADERSTORAGEBLOCKBINDINGPROC, glShaderStorageBlockBinding);
  SDLGLP2(PFNGLGETPROGRAMRESOURCEINDEXPROC, glGetProgramResourceIndex);
  //SDLGLP2(PFNGLGETSAMPLERPARAMETERFVPROC, glGetSamplerParameterfv);
  //SDLGLP2(PFNGLGETSAMPLERPARAMETERIIVPROC, glGetSamplerParameterIiv);
  //SDLGLP2(PFNGLGETSAMPLERPARAMETERIUIVPROC, glGetSamplerParameterIuiv);
  //SDLGLP2(PFNGLGETSAMPLERPARAMETERIUIVPROC, glGetSamplerParameteruiv);
  //SDLGLP2(PFNGLGETSAMPLERPARAMETERIVPROC, glGetSamplerParameteriv);
  //SDLGLP2(PFNGLGETSAMPLERPARAMETERIVPROC, glGetSamplerParameteriv);
  SDLGLP2(PFNGLCOPYIMAGESUBDATAPROC, glCopyImageSubData);
  //SDLGLP2(PFNGLDELETETEXTURESEXTPROC, glDeleteTextures);
  //SDLGLP2(PFNGLGETTEXPARAMETERIIVPROC, glGetTexParameteriv);
  //SDLGLP2(PFNGLGETTEXPARAMETERIUIVPROC, glGetTexParameteruiv);
  //SDLGLP2(PFNGLGETTEXTUREPARAMETERIVEXTPROC, glGetTextureParameteriv);

  return bValid;
}
void GLContext::setLineWidth(float w) {
  if (isForwardCompatible() == false) {
    glLineWidth(w);
    chkErrDbg();
  }
  else {
    BRLogWarnOnce("glLineWidth not supported in compatibility mode.");
  }
}

void GLContext::pushCullFace() {
  GLint cull;
  glGetIntegerv(GL_CULL_FACE, &cull);

  if (_eLastCullFaceStack.size() > MaxStackSize) {
    BRLogWarn("Stack count has been exceeded.  Stack invalid somewhere");
  }
  else {
    _eLastCullFaceStack.push(cull);
  }
}
void GLContext::popCullFace() {
  GLint cull = _eLastCullFaceStack.top();

  if (_eLastCullFaceStack.size() > 0) {
    _eLastCullFaceStack.pop();
  }
  if (_eLastCullFaceStack.size() == 0) {
    _eLastCullFaceStack.push(1);
  }

  if (cull) {
    glEnable(GL_CULL_FACE);
  }
  else {
    glDisable(GL_CULL_FACE);
  }
}
void GLContext::pushBlend() {
  GLint cull;
  glGetIntegerv(GL_BLEND, &cull);

  if (_eLastBlendStack.size() > MaxStackSize) {
    BRLogWarn("Stack count has been exceeded.  Stack invalid somewhere");
  }
  else {
    _eLastBlendStack.push(cull);
  }
}
void GLContext::popBlend() {
  GLint cull = _eLastBlendStack.top();

  if (_eLastBlendStack.size() > 0) {
    _eLastBlendStack.pop();
  }
  if (_eLastBlendStack.size() == 0) {
    _eLastBlendStack.push(1);
  }

  if (cull) {
    glEnable(GL_BLEND);
  }
  else {
    glDisable(GL_BLEND);
  }
}
void GLContext::pushDepthTest() {
  GLint cull;
  glGetIntegerv(GL_DEPTH_TEST, &cull);

  if (_eLastDepthTestStack.size() > MaxStackSize) {
    BRLogWarn("Stack count has been exceeded.  Stack invalid somewhere");
  }
  else {
    _eLastDepthTestStack.push(cull);
  }
}
void GLContext::popDepthTest() {
  GLint cull = _eLastDepthTestStack.top();

  if (_eLastDepthTestStack.size() > 0) {
    _eLastDepthTestStack.pop();
  }
  if (_eLastDepthTestStack.size() == 0) {
    _eLastDepthTestStack.push(1);
  }

  if (cull) {
    glEnable(GL_DEPTH_TEST);
  }
  else {
    glDisable(GL_DEPTH_TEST);
  }
}
//
//void GLContext::beginWin32InlineDebug() {
//#ifdef _WIN32
//#ifdef _DEBUG
//    //**INLINE MODE -- REMOVE FOR RELEASE
//    Gu::getShaderMaker()->shaderBound(nullptr);
//    glBindBuffer(GL_ARRAY_BUFFER, 0);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//    glBindVertexArray(0);
//    chkErrDbg();
//
//    mat4 mProj = Gu::getCamera()->getProj();
//    // mProj.transpose();
//    mat4 mView = Gu::getCamera()->getView();
//    // mView.transpose();
//    glMatrixMode(GL_PROJECTION);
//    glLoadMatrixf((GLfloat*)& mProj);
//    glMatrixMode(GL_MODELVIEW);
//    glLoadMatrixf((GLfloat*)& mView);
//#endif
//#endif
//}

void GLContext::enableCullFace(bool enable) {
  if (enable)
    glEnable(GL_CULL_FACE);
  else
    glDisable(GL_CULL_FACE);
}
void GLContext::enableBlend(bool enable) {
  if (enable)
    glEnable(GL_BLEND);
  else
    glDisable(GL_BLEND);
}

void GLContext::enableDepthTest(bool enable) {
  if (enable)
    glEnable(GL_DEPTH_TEST);
  else
    glDisable(GL_DEPTH_TEST);
}

void GLContext::getOpenGLVersion(int& ver, int& subver, int& shad_ver, int& shad_subver) {
  char* tmp;
  string_t glver;
  string_t glslver;
  ver = subver = shad_ver = shad_subver = 0;

  tmp = (char*)glGetString(GL_VERSION);
  if (tmp != nullptr) {
    glver = tmp;
  }
  else {
    glver = "";
  }

  std::vector<string_t> sv;

  if (glver.length() > 0) {
    sv = StringUtil::split(glver, '.');
    if (sv.size() < 2) {
      BRThrowException("Failed to get OpenGL version. Got '" + glver + "'.  Check that you have OpenGL installed on your machine. You may have to update your 'graphics driver'.");
    }
    ver = TypeConv::strToInt(sv[0]);
    subver = TypeConv::strToInt(sv[1]);
  }
  else {
    BRLogError("Failed to get OpenGL version.");
  }
  if (ver > 3) {
    //This will fail if we try to get an OpenGL version greater than what is supported, returning GL 1.1.  Shade is only supported on GL > 2.1.
    tmp = (char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
    if (tmp != nullptr) {
      glslver = tmp;
    }
    else {
      glslver = "";
    }
    if (glslver.length() > 0) {
      sv = StringUtil::split(glslver, '.');
      if (sv.size() < 2) {
        BRThrowException("Failed to get OpenGL Shade version. Got '" + glslver + "'.  Check that you have OpenGL installed on your machine. You may have to update your 'graphics driver'.");
      }
      shad_ver = TypeConv::strToInt(sv[0]);
      shad_subver = TypeConv::strToInt(sv[1]);
    }
    else {
      BRLogWarn("Failed to get OpenGL Shade version.");
    }
  }
}
bool GLContext::checkOpenGlMinimumVersionInfo(int required_version, int required_subversion) {
  string_t rver = Stz "" + required_version + "." + required_subversion;
  int ver, subver, shad_ver, shad_subver;
  getOpenGLVersion(ver, subver, shad_ver, shad_subver);

  if (ver < required_version || (ver >= required_subversion && subver < required_subversion)) {
    return false;
  }
  else {
    string_t vendor = string_t((char*)glGetString(GL_VENDOR));
    string_t renderer = string_t((char*)glGetString(GL_RENDERER));

    BRLogInfo("\n" + "   OpenGL version " + ver + "." + subver + ".\n" +
              "   GLSL version:          " + shad_ver + "." + shad_subver + "\n" +
              "   GPU:         " + renderer + "\n" +
              "   GPU Vendor:  " + vendor + "\n");
  }

  return true;
}

void GLContext::loadCheckProc() {
  //Check that OpenGL initialized successfully by checking a library pointer.
  PFNGLUSEPROGRAMPROC proc = (PFNGLUSEPROGRAMPROC)SDL_GL_GetProcAddress("glUseProgram");
  if (proc == nullptr) {
    string_t exep;
    exep += "glUseProgram was not found.\n";
    exep += ("  OpenGL not installed, or bad graphics driver.\n");
    BRThrowException(exep);
  }
}
void GLContext::printHelpfulDebug() {
  int tmp = 0;
  SDL_GL_GetAttribute(SDL_GL_DOUBLEBUFFER, &tmp);
  BRLogInfo("SDL_GL_DOUBLEBUFFER: " + tmp);
  SDL_GL_GetAttribute(SDL_GL_BUFFER_SIZE, &tmp);
  BRLogInfo("SDL_GL_BUFFER_SIZE: " + tmp);
  SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &tmp);

  BRLogInfo("SDL_GL_DEPTH_SIZE: " + tmp);
  SDL_GL_GetAttribute(SDL_GL_STENCIL_SIZE, &tmp);
  BRLogInfo("SDL_GL_STENCIL_SIZE: " + tmp);
  SDL_GL_GetAttribute(SDL_GL_ACCELERATED_VISUAL, &tmp);
  BRLogInfo("SDL_GL_ACCELERATED_VISUAL: " + tmp);

  SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &tmp);
  BRLogInfo("SDL_GL_CONTEXT_PROFILE_MASK: " + tmp);
  SDL_GL_GetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, &tmp);
  BRLogInfo("SDL_GL_SHARE_WITH_CURRENT_CONTEXT: " + tmp);
  SDL_GL_GetAttribute(SDL_GL_CONTEXT_FLAGS, &tmp);
  BRLogInfo("SDL_GL_CONTEXT_FLAGS: " + tmp);

  SDL_GL_GetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, &tmp);
  BRLogInfo("SDL_GL_FRAMEBUFFER_SRGB_CAPABLE: " + tmp);
  SDL_GL_GetAttribute(SDL_GL_MULTISAMPLESAMPLES, &tmp);
  BRLogInfo("SDL_GL_MULTISAMPLESAMPLES: " + tmp);
  SDL_GL_GetAttribute(SDL_GL_MULTISAMPLEBUFFERS, &tmp);
  BRLogInfo("SDL_GL_MULTISAMPLEBUFFERS: " + tmp);

  SDL_GL_GetAttribute(SDL_GL_RED_SIZE, &tmp);
  BRLogInfo("SDL_GL_RED_SIZE: " + tmp);
  SDL_GL_GetAttribute(SDL_GL_GREEN_SIZE, &tmp);
  BRLogInfo("SDL_GL_GREEN_SIZE: " + tmp);
  SDL_GL_GetAttribute(SDL_GL_BLUE_SIZE, &tmp);
  BRLogInfo("SDL_GL_BLUE_SIZE: " + tmp);
  SDL_GL_GetAttribute(SDL_GL_ALPHA_SIZE, &tmp);
  BRLogInfo("SDL_GL_ALPHA_SIZE: " + tmp);
}
bool GLContext::isForwardCompatible() {
  return (_profile->_iProfile == SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
}
void GLContext::setWindowAndOpenGLFlags(std::shared_ptr<GLProfile> prof) {
  //Attribs
  SDL_GL_ResetAttributes();
  SDLUtils::checkSDLErr();

  //We want SRGB in the final render, so this should be requested.
  SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, prof->_bSRGB);
  SDLUtils::checkSDLErr();

  //Context sharing will be necessary with multiple-window rendering (we are required to create 1 context per window)
  SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
  SDLUtils::checkSDLErr();

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDLUtils::checkSDLErr();
  SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
  SDLUtils::checkSDLErr();
  SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

  //Not sure, imagine we'd use our own buffer blending to create a VR scene.
  //SDL_GL_SetAttribute(SDL_GL_STEREO, _pGlState->gl_stereo);

  if (prof->_iMinVersion > 0 && prof->_iMinSubVersion > 0) {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, prof->_iMinVersion);
    SDLUtils::checkSDLErr();
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, prof->_iMinSubVersion);
    SDLUtils::checkSDLErr();
  }

  //Desktop Debug = Compatibility, Runtime = Core, Phone = ES
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, prof->_iProfile);
  SDLUtils::checkSDLErr();

  int debug_flag = 0;
#ifdef _DEBUG
  debug_flag = SDL_GL_CONTEXT_DEBUG_FLAG;
#endif

  //Only in GL 3.0 (apparently)
  //Forward compatible deprecates old funcionality for a gain in speed (apparently)
  //https://wiki.libsdl.org/SDL_GLcontextFlag#SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG
  int forward_compat = (prof->_iProfile == SDL_GL_CONTEXT_PROFILE_CORE) ? (0) : (SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, debug_flag | forward_compat);
  SDLUtils::checkSDLErr();

  //Depth size is finicky. Sometimes it will only give us 16 bits. Trying to set stencil to zero MIGHT help
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);
  SDLUtils::checkSDLErr();
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, prof->_iDepthBits);
  SDLUtils::checkSDLErr();
  //Attempt to zero out the stencil buffer to request a 32b depth.

  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDLUtils::checkSDLErr();
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDLUtils::checkSDLErr();
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDLUtils::checkSDLErr();
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
  SDLUtils::checkSDLErr();

  //Multisampling
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, prof->_iMSAASamples);
  SDLUtils::checkSDLErr();
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, prof->_iMSAABuffers);
  SDLUtils::checkSDLErr();
}
bool GLContext::isActive() {
  bool r = Gu::getActiveContext() == getThis<GLContext>();
  return r;
}
string_t GLContext::getObjectLabel(GLenum type, GLuint objectId) {
  string_t res = "";
  if (objectId > 0) {
    const int buflen = 256;
    char lenbuf[buflen];
    GLsizei length = 0;

    chkErrDbg();
    this->glGetObjectLabel(type, objectId, buflen, &length, lenbuf);
    chkErrRt();

    res.assign(lenbuf, length);
    res += Stz "(" + objectId + ")";
  }
  else {
    res = Stz "No Label " + "(" + objectId + ")";
  }
  return res;
}
void GLContext::setObjectLabel(GLenum type, GLuint objectId, const string_t& label) {
  if (StringUtil::isEmpty(label)) {
    //Label was empty.
    BRLogDebug("Warning label was empty.");
    Gu::debugBreak();
  }
  if (objectId > 0) {
    static int _maxlenLabel = -1;
    if (_maxlenLabel == -1) {
      glGetIntegerv(GL_MAX_LABEL_LENGTH, &_maxlenLabel);
    }
    if (label.length() >= _maxlenLabel) {
      //This shouldn't happen.
      Gu::debugBreak();
    }
    else {
      chkErrDbg();
      this->glObjectLabel(type, objectId, label.length(), label.c_str());
      chkErrRt();
    }
  }
}
int32_t GLContext::maxGLTextureUnits() {
  if (_iMaxTexs == -1) {
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &_iMaxTexs);
  }
  return _iMaxTexs;
}

GLenum GLContext::getTextureTarget(GLuint textureId) {
  //The alternative way of using glBindTexture==glGetError() wasn't working. This is more intuitive.
  auto f = _texBindings.find(textureId);
  if (f == _texBindings.end()) {
    return (GLenum)0;
  }
  return f->second;
}
void GLContext::glGenTextures(GLsizei count, GLuint* ids) {
  ::glGenTextures(1, ids);
}
void GLContext::glBindTexture(GLenum target, GLuint texid) {
  auto f = _texBindings.find(texid);
  if (f == _texBindings.end()) {
    _texBindings.insert(std::make_pair(texid, target));
  }
  else {
    f->second = target;
  }

  ::glBindTexture(target, texid);
}
void GLContext::glDeleteTextures(GLsizei n, const GLuint* textures) {
  ::glDeleteTextures(n, textures);
}

void GLContext::resetRenderState() {
  GLContext::debugGetRenderState();
  {
    Gu::getShaderMaker()->shaderBound(nullptr);

    //glUseProgram(NULL);//DO NOT CALL - we must maintain consistency on the gpu driver
    this->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (GLuint)0);
    this->glBindBuffer(GL_ARRAY_BUFFER, (GLuint)0);

    int iMaxTextures = 0;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, (GLint*)&iMaxTextures);
    for (int iTex = 0; iTex < iMaxTextures; iTex++) {
      this->glActiveTexture(GL_TEXTURE0 + iTex);
      this->glBindTexture(GL_TEXTURE_2D, 0);
    }

    //  VaoData::debugDisableAllAttribArrays();
    this->glBindVertexArray(0);
    this->glBindBuffer(GL_ARRAY_BUFFER, 0);
    this->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    this->glActiveTexture(GL_TEXTURE0);

    //Note: Client textures are deprecated
    //  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    //  glDisableClientState(GL_COLOR_ARRAY);
    //  glDisableClientState(GL_NORMAL_ARRAY);
    //  glDisableClientState(GL_VERTEX_ARRAY);
    //  glDisableClientState(GL_INDEX_ARRAY);
    //  glClientActiveTexture(GL_TEXTURE0);
    glDisable(GL_LIGHTING);
  }
  GLContext::debugGetRenderState();
}

//void GLContext::createDepthTexture(GLuint& __out_ texId, int w, int h, GLenum depthSize){
//    //This will query the device to make sure the depth format is supported.
//    Gu::getGraphicsContext()->chkErrRt();
//
//    glGenTextures(1, &texId);
//    glBindTexture(GL_TEXTURE_2D, texId);
//    //THe following parameters are for depth textures only
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);//GL_NONE
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
//    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//    Gu::getGraphicsContext()->chkErrRt();
//
//    //**This might fail on phones - change to depth component 24
//    glTexImage2D(GL_TEXTURE_2D, 0, depthSize, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
//}
void GLContext::createDepthTexture(const string_t& owner, GLuint* __out_ texId, int32_t w, int32_t h, bool bMsaaEnabled, int32_t nMsaaSamples, GLenum eRequestedDepth) {
  //Creates a depth texture, or multisample depth texture on texture channel 0
  //This will query the device to make sure the depth format is supported.
  this->chkErrRt();
  GLenum texTarget;

  string_t label = owner + "_DepthTexture";

  if (bMsaaEnabled) {
    texTarget = GL_TEXTURE_2D_MULTISAMPLE;
    label += "_Multisample";
  }
  else {
    texTarget = GL_TEXTURE_2D;
  }
  this->glActiveTexture(GL_TEXTURE0);
  this->glGenTextures(1, texId);
  this->chkErrRt();
  this->glBindTexture(texTarget, *texId);
  //THe following parameters are for depth textures only
  this->chkErrRt();

  if (bMsaaEnabled == false) {
    //For some reason you can't use this with multisample.
    glTexParameteri(texTarget, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);  //GL_NONE
    this->chkErrRt();
    glTexParameteri(texTarget, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    this->chkErrRt();
    glTexParameterf(texTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    this->chkErrRt();
    glTexParameterf(texTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    this->chkErrRt();
    glTexParameteri(texTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    this->chkErrRt();
    glTexParameteri(texTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    this->chkErrRt();
  }

  this->setObjectLabel(GL_TEXTURE, *texId, label);

  getCompatibleDepthComponent(eRequestedDepth, [&](GLenum eDepth) {
    if (bMsaaEnabled) {
      this->glTexImage2DMultisample(texTarget, nMsaaSamples, eDepth, w, h, GL_TRUE);
      chkErrRt();
    }
    else {
      glTexImage2D(texTarget, 0, eDepth, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
      chkErrRt();
    }
  });

  chkErrRt();
  this->glBindTexture(texTarget, 0);
  chkErrRt();
}

GLenum GLContext::getSupportedDepthSize() {
  int32_t depth = _iSupportedDepthSize;
  //If we don't support requested depth, change it.
  if (depth == 16) {
    return GL_DEPTH_COMPONENT16;
  }
  else if (depth == 24) {
    return GL_DEPTH_COMPONENT24;
  }
  else if (depth == 32) {
    return GL_DEPTH_COMPONENT32F;
    //There is also a 32F component, but .. What/s the difference?
    //eDepthSize = GL_DEPTH_COMPONENT32F;
  }

  BRThrowException("[222039] Unsupported depth component size " + depth);
}
void GLContext::getCompatibleDepthComponent(GLenum eRequestedDepth, std::function<void(GLenum)> func) {
  int32_t depth = this->getSupportedDepthSize();
  //Shortcut lambda to create something that asks for a GL_DEPTH_COMPONENT enum.
  GLenum eDepthSize;
  if (eRequestedDepth == GL_DEPTH_COMPONENT32F && depth < 32) {
    eDepthSize = this->getSupportedDepthSize();
  }
  else if (eRequestedDepth == GL_DEPTH_COMPONENT32 && depth < 32) {
    eDepthSize = this->getSupportedDepthSize();
  }
  else if (eRequestedDepth == GL_DEPTH_COMPONENT24 && depth < 24) {
    eDepthSize = this->getSupportedDepthSize();
  }
  else {
    eDepthSize = eRequestedDepth;
  }

  this->chkErrRt();

  //**This might fail on phones - change to depth component 24
  func(eDepthSize);
  while (true) {
    if (glGetError() != GL_NO_ERROR) {
      if (eDepthSize == GL_DEPTH_COMPONENT32F) {
        BRLogWarn("32 bit floating point depth buffer not supported. Attempting another format.");
        eDepthSize = GL_DEPTH_COMPONENT32;
      }
      else if (eDepthSize == GL_DEPTH_COMPONENT32) {
        BRLogWarn("32 bit depth buffer not supported. Attempting another format.");
        eDepthSize = GL_DEPTH_COMPONENT24;
      }
      else if (eDepthSize == GL_DEPTH_COMPONENT24) {
        BRLogWarn("24 bit floating point depth buffer not supported. Attempting another format.");
        eDepthSize = GL_DEPTH_COMPONENT16;
      }
      else {
        BRThrowException("Creating Depth Texture, No valid depth buffer format could be obtained.");
      }
      //Attempt to use a 24 bit depth buffer
      func(eDepthSize);
    }
    else {
      break;
    }
  }
}
bool GLContext::getTextureDataFromGpu(std::shared_ptr<Img32> __out_ image, GLuint iGLTexId, GLenum eTexTargetBase, int iCubeMapSide) {
  AssertOrThrow2(image != nullptr);
  GLint iSavedTextureBinding;
  GLenum eTexBinding = texTargetToTexBindingQuery(eTexTargetBase);

  GLenum eTexTargetSide = eTexTargetBase;
  if (eTexTargetBase == GL_TEXTURE_CUBE_MAP) {
    AssertOrThrow2(iCubeMapSide >= 0 && iCubeMapSide < 6);
    eTexTargetSide = GL_TEXTURE_CUBE_MAP_POSITIVE_X + iCubeMapSide;
  }

  glGetIntegerv(eTexBinding, &iSavedTextureBinding);
  chkErrRt();

  this->glActiveTexture(GL_TEXTURE0);
  glBindTexture(eTexTargetBase, iGLTexId);
  chkErrRt();
  {
    GLint w, h;
    int32_t iMipLevel = 0;
    glGetTexLevelParameteriv(eTexTargetSide, iMipLevel, GL_TEXTURE_WIDTH, &w);
    chkErrRt();

    glGetTexLevelParameteriv(eTexTargetSide, iMipLevel, GL_TEXTURE_HEIGHT, &h);
    chkErrRt();

    //Adding for the pick texture test.
    GLenum internalFormat = 0;
    glGetTexLevelParameteriv(eTexTargetSide, iMipLevel, GL_TEXTURE_INTERNAL_FORMAT, (GLint*)&internalFormat);
    chkErrRt();

    GLenum calculatedFmt = GL_RGBA;
    GLenum calculatedType = GL_UNSIGNED_BYTE;
    size_t bufsiz_bytes = 0;

    if (internalFormat == GL_RGBA) {
      calculatedFmt = GL_RGBA;
      calculatedType = GL_UNSIGNED_BYTE;
      bufsiz_bytes = w * h * 4;
    }
    else if (internalFormat == GL_RGBA8) {
      calculatedFmt = GL_RGBA;
      calculatedType = GL_UNSIGNED_BYTE;
      bufsiz_bytes = w * h * 4;
    }
    else if (internalFormat == GL_RGBA32F) {  //All color buffers
      calculatedFmt = GL_RGBA;
      calculatedType = GL_UNSIGNED_BYTE;
      bufsiz_bytes = w * h * 4;
    }
    else if (internalFormat == GL_R32UI) {  //Pick buffer
      calculatedType = GL_UNSIGNED_INT;
      calculatedFmt = GL_RED_INTEGER;
      bufsiz_bytes = w * h * 4;
    }
    else if (internalFormat == GL_RGBA16F) {
      calculatedFmt = GL_RGBA;
      calculatedType = GL_UNSIGNED_BYTE;
      bufsiz_bytes = w * h * 4;
    }
    else if (internalFormat == GL_R32F) {
      /*
      https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glGetTexImage.xhtml
      If the selected texture image does not contain four components, the following mappings are applied.
      Single-component textures are treated as RGBA buffers with red set to the single-component value,
      green set to 0, blue set to 0, and alpha set to 1.
      */
      calculatedFmt = GL_RGBA;
      calculatedType = GL_UNSIGNED_BYTE;
      bufsiz_bytes = w * h * 4;
    }
    else if (internalFormat == GL_R16F) {
      calculatedFmt = GL_RGBA;
      calculatedType = GL_UNSIGNED_BYTE;
      bufsiz_bytes = w * h * 4;
    }
    else {
      BRLogError("Invalid or Unsupported texture internal format when reading from GPU" +
                 (int)internalFormat);
      Gu::debugBreak();
    }

    if (false) {
      //Print all image values as floats (tests to see if buffer was written to
      uint8_t* ts = new uint8_t[w * h * 4];
      size_t iNonZero = 0;
      uint8_t lastr, lastg, lastb, lasta;
      glGetTexImage(eTexTargetSide, iMipLevel, calculatedFmt, calculatedType, (GLvoid*)ts);
      for (GLint ih = 0; ih < h; ++ih) {
        for (GLint iw = 0; iw < w; ++iw) {
          float r = ts[ih * (w * 4) + iw * 4 + 0];
          float g = ts[ih * (w * 4) + iw * 4 + 1];
          float b = ts[ih * (w * 4) + iw * 4 + 2];
          float a = ts[ih * (w * 4) + iw * 4 + 3];
          if (lastr != r || lastg != g || lastb != b || lasta != a) {
            std::cout << " ,(" << r << "," << g << "," << b << "," << a << ")";
            iNonZero++;
          }
          lastr = r;
          lastg = g;
          lastb = b;
          lasta = a;
        }
      }
      int nnn = 0;
      nnn++;
    }

    // char* buf = new char[bufsiz_bytes];
    // glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)bi.getData()->ptr());
    //glGetTexImage(GL_TEXTURE_2D, iMipLevel, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)bi.getData()->ptr());
    image->init(w, h, nullptr);
    glGetTexImage(eTexTargetSide, iMipLevel, calculatedFmt, calculatedType, (GLvoid*)image->getData()->ptr());
    chkErrRt();
  }
  glBindTexture(eTexTargetBase, iSavedTextureBinding);
  chkErrRt();

  return true;
}

void GLContext::saveFramebufferAsPng(string_t&& strLoc, GLuint iFBOId) {
  GLint iFbBindingLast;
  GLint iFbWidth;
  GLint iFbHeight;

  //Save current FBO
  glGetIntegerv(GL_FRAMEBUFFER_BINDING, &iFbBindingLast);

  if (iFBOId == 0) {
    iFBOId = iFbBindingLast;
  }

  this->glBindFramebuffer(GL_FRAMEBUFFER, iFBOId);
  {
    //glGetIntegerv(GL_FRAMEBUFFER_DEFAULT_WIDTH, &iFbWidth);
    //glGetIntegerv(GL_FRAMEBUFFER_DEFAULT_HEIGHT, &iFbHeight);
    GLint iViewportBox[4];
    glGetIntegerv(GL_VIEWPORT, (GLint*)iViewportBox);
    iFbWidth = iViewportBox[2];
    iFbHeight = iViewportBox[3];

    //Sanity Values
    AssertOrThrow2(iFbWidth >= 0 && iFbWidth < MemSize::MEMSZ_GIG2);
    AssertOrThrow2(iFbHeight >= 0 && iFbHeight < MemSize::MEMSZ_GIG2);

    std::shared_ptr<Img32> bi = std::make_shared<Img32>(iFbWidth, iFbHeight);
    glReadPixels(0, 0, iFbWidth, iFbHeight, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)bi->getData()->ptr());
    bi->flipV();  //the GL tex image must be flipped to show uprih
    Gu::saveImage(strLoc, bi);
  }
  this->glBindFramebuffer(GL_FRAMEBUFFER, iFbBindingLast);
}
#pragma region Debug Output

void appendLine(string_t& st, const char* c) {
  st += c;
  st += OperatingSystem::newline();
}
void appendLine(string_t& st, const string_t& c) {
  st += c;
  st += OperatingSystem::newline();
}

string_t GLContext::debugGetRenderState(bool bForceRun, bool bPrintToStdout, bool bSaveFramebufferTexture) {
  //This method is called in frames to drag down the debug arrow
  // and we skip it unless we force it to run.
  //Do not comment
  if (!bForceRun) {
    return "";  //Do not comment
  }
  string_t strState = "";

  static bool _bGettingRenderState = false;

  if (_bGettingRenderState == true) {
    return "Render State tried to be called recursively.";  // Prevent recursive calls.
  }
  _bGettingRenderState = true;

  // Gd::verifyRenderThread();//We must be in render thread

  appendLine(strState, "===================================================================");
  appendLine(strState, "  RENDERING SYSTEM STATE                                            ");
  appendLine(strState, "===================================================================");
  this->chkErrRt();

  GLint iBlend;
  glGetIntegerv(GL_BLEND, (GLint*)&iBlend);
  appendLine(strState, Stz " Blending: " + (iBlend ? "ENABLED" : "DISABLED"));
  this->chkErrRt();

  GLint iCullFace;
  glGetIntegerv(GL_CULL_FACE, (GLint*)&iCullFace);
  appendLine(strState, Stz " Culling: " + (iCullFace ? "ENABLED" : "DISABLED"));
  this->chkErrRt();

  GLint iDepthTest;
  glGetIntegerv(GL_DEPTH_TEST, (GLint*)&iDepthTest);
  appendLine(strState, Stz " Depth Test: " + (iDepthTest ? "ENABLED" : "DISABLED"));
  this->chkErrRt();

  GLContext::debugGetLegacyViewAndMatrixStack(strState);
  this->chkErrRt();
  GLContext::debugGetBufferState(strState);
  this->chkErrRt();
  //GLContext::debugGetAttribState(); // This is redundant with vertexarraystate
  //    CheckGpuErrorsDbg();
  GLContext::debugGetTextureState(strState);
  this->chkErrRt();

  GLContext::debugGetVertexArrayState(strState);
  this->chkErrRt();
  GLContext::debugGetFramebufferAttachmentState(strState);
  this->chkErrRt();

  if (bPrintToStdout) {
    Gu::print(strState);
  }
  if (bSaveFramebufferTexture) {
    string_t fname = FileSystem::getScreenshotFilename();
    saveFramebufferAsPng(std::move(fname));
  }

  _bGettingRenderState = false;

  return strState;
}
void GLContext::debugGetLegacyViewAndMatrixStack(string_t& strState) {
  GLint iScissorBox[4];
  GLint iViewportBox[4];
  appendLine(strState, "**Scissor and Legcay info**");
  //View Bounds (Legacy)
  glGetIntegerv(GL_SCISSOR_BOX, (GLint*)iScissorBox);
  glGetIntegerv(GL_VIEWPORT, (GLint*)iViewportBox);
  appendLine(strState, Stz "Scissor: " + iScissorBox[0] + "," + iScissorBox[1] + "," + iScissorBox[2] + "," + iScissorBox[3]);
  appendLine(strState, Stz "Viewport: " + iViewportBox[0] + "," + iViewportBox[1] + "," + iViewportBox[2] + "," + iViewportBox[3]);
  //TODO: legacy matrix array state.
  this->chkErrRt();
}
void GLContext::debugGetBufferState(string_t& strState) {
  appendLine(strState, "--------------------------------------");
  appendLine(strState, "--SHADER STATE");

  GLint iBoundBuffer;
  GLint iCurrentProgram;
  GLint iElementArrayBufferBinding;
  GLint iSsboBinding;  //shader storage
  GLint iUniformBufferBinding;
  GLint iVertexArrayBinding;

  glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &iBoundBuffer);
  glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &iElementArrayBufferBinding);
  glGetIntegerv(GL_SHADER_STORAGE_BUFFER_BINDING, &iSsboBinding);
  glGetIntegerv(GL_UNIFORM_BUFFER_BINDING, &iUniformBufferBinding);
  glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &iVertexArrayBinding);
  glGetIntegerv(GL_CURRENT_PROGRAM, &iCurrentProgram);
  this->chkErrRt();

  appendLine(strState, Stz "Bound Vertex Array Buffer (VBO): " + this->getObjectLabel(GL_BUFFER, iBoundBuffer));
  appendLine(strState, Stz "Bound Element Array Buffer (IBO): " + this->getObjectLabel(GL_BUFFER, iElementArrayBufferBinding));
  appendLine(strState, Stz "Bound Shader Storage Buffer (SSBO): " + this->getObjectLabel(GL_BUFFER, iSsboBinding));
  appendLine(strState, Stz "Bound Uniform Buffer Object: " + this->getObjectLabel(GL_BUFFER, iUniformBufferBinding));
  appendLine(strState, Stz "Bound Vertex Array Object: " + this->getObjectLabel(GL_VERTEX_ARRAY, iVertexArrayBinding));
  appendLine(strState, Stz "Bound Shader Program: " + this->getObjectLabel(GL_PROGRAM, iCurrentProgram));
  this->chkErrRt();

  if (iCurrentProgram > 0) {
    GLContext::debugPrintActiveUniforms(iCurrentProgram, strState);
  }
  else {
    appendLine(strState, "Bound Shader Name: None");
  }
}

void GLContext::debugPrintActiveUniforms(int iGlProgramId, string_t& strState) {
  GLint nUniforms;
  string_t uniformName;
  char name[256];
  GLint name_len = -1;
  GLint iArraySize = -1;
  GLenum uniformType = GL_ZERO;
  GLint nActiveUniformBlocks;
  GLint nMaxUniformLocations;
  GLint nMaxComponentsVert;
  GLint nMaxComponentsFrag;
  GLint nMaxComponentsComp;

  // - Get the number of uniforms
  this->glGetProgramiv(iGlProgramId, GL_ACTIVE_UNIFORMS, &nUniforms);
  this->glGetProgramiv(iGlProgramId, GL_ACTIVE_UNIFORM_BLOCKS, (GLint*)&nActiveUniformBlocks);
  glGetIntegerv(GL_MAX_UNIFORM_LOCATIONS, (GLint*)&nMaxUniformLocations);
  glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, (GLint*)&nMaxComponentsVert);
  glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, (GLint*)&nMaxComponentsFrag);
  glGetIntegerv(GL_MAX_COMPUTE_UNIFORM_COMPONENTS, (GLint*)&nMaxComponentsComp);
  this->chkErrRt();

  appendLine(strState, Stz "# Active Uniforms: " + nUniforms);
  appendLine(strState, Stz "# Active Uniform Blocks: " + nActiveUniformBlocks);
  appendLine(strState, Stz "# Max Uniform Locations: " + nMaxUniformLocations);
  appendLine(strState, Stz "# Max Uniform Components Vertex: " + nMaxComponentsVert);
  appendLine(strState, Stz "# Max Uniform Components Fragment: " + nMaxComponentsFrag);
  appendLine(strState, Stz "# Max Uniform Components Compute: " + nMaxComponentsComp);

  appendLine(strState, "--------------------------------------");
  appendLine(strState, "-- Active Uniform Data: ");

  //Get all uniform names and types into a list.
  for (int32_t i = 0; i < nUniforms; ++i) {
    //Get name an d type
    this->glGetActiveUniform(iGlProgramId, (GLuint)i, 256, &name_len, &iArraySize, &uniformType, (char*)name);
    name[name_len] = 0;
    uniformName = string_t(name);

    //get location
    GLint glLocation = this->glGetUniformLocation((GLuint)iGlProgramId, (GLchar*)uniformName.c_str());

    appendLine(strState, Stz "  Name: " + uniformName);
    appendLine(strState, Stz "  Type: " + GLContext::openGlTypeToString(uniformType));
    appendLine(strState, Stz "  Location: " + glLocation);
    appendLine(strState, Stz "  Array Size: " + iArraySize);

    // Uniform Block Data.
    this->chkErrRt();

    GLuint iCurrentBlockIdx;
    iCurrentBlockIdx = this->glGetUniformBlockIndex(iGlProgramId, uniformName.c_str());

    if (iCurrentBlockIdx != GL_INVALID_INDEX) {
      int iBlockBinding;
      int iBlockDataSize;
      int iBlockNameLength;
      int iBlockActiveUniforms;
      int iBlockActiveUniformIndices;

      this->chkErrRt();
      this->glGetActiveUniformBlockiv(iGlProgramId, iCurrentBlockIdx, GL_UNIFORM_BLOCK_BINDING, (GLint*)&iBlockBinding);
      this->chkErrRt();
      this->glGetActiveUniformBlockiv(iGlProgramId, iCurrentBlockIdx, GL_UNIFORM_BLOCK_DATA_SIZE, (GLint*)&iBlockDataSize);
      this->chkErrRt();
      this->glGetActiveUniformBlockiv(iGlProgramId, iCurrentBlockIdx, GL_UNIFORM_BLOCK_NAME_LENGTH, (GLint*)&iBlockNameLength);
      this->chkErrRt();
      this->glGetActiveUniformBlockiv(iGlProgramId, iCurrentBlockIdx, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, (GLint*)&iBlockActiveUniforms);
      this->chkErrRt();
      this->glGetActiveUniformBlockiv(iGlProgramId, iCurrentBlockIdx, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, (GLint*)&iBlockActiveUniformIndices);
      this->chkErrRt();

      appendLine(strState, Stz "  Block Index: " + iCurrentBlockIdx);
      appendLine(strState, Stz "  Block Binding: " + iBlockBinding);
      appendLine(strState, Stz "  Block Data Size: " + iBlockDataSize);
      appendLine(strState, Stz "  Block Name Length: " + iBlockNameLength);
      appendLine(strState, Stz "  Block Active Uniforms: " + iBlockActiveUniforms);
      appendLine(strState, Stz "  Block Active Uniform Indices: " + iBlockActiveUniformIndices);
    }

    //Data
    if (Gu::isManagerConstructed(ManagerType::ShaderMaker)) {
      //We can call this anywhere. SM is lazy initialized, so this may not be available.
      if (Gu::getShaderMaker()->getBound() != nullptr) {
        std::shared_ptr<ShaderUniform> uf = Gu::getShaderMaker()->getBound()->getUniformByName(uniformName);
        if (uf != nullptr) {
          appendLine(strState, ("  Buffer Data:"));
          if (uf->hasBeenSet() == false) {
            appendLine(strState, ("  not set."));
          }
          else {
            appendLine(strState, Stz "  Text:" + (uf->debugGetUniformValueAsString(false)));
            appendLine(strState, Stz "   Raw:" + (uf->debugGetUniformValueAsString(true)));
          }
        }
        else {
          appendLine(strState, Stz "Uniform " + uniformName + " was not found.  It may be a uniform buffer.");
        }
      }
    }
    else {
      appendLine(strState, Stz " Bound uniform Data not available. Shader manager has not been constructed yet.");
    }
    appendLine(strState, (""));
  }
}
string_t GLContext::openGlTypeToString(GLenum type) {
  switch (type) {
    case GL_UNSIGNED_INT:
      return ("GL_UNSIGNED_INT        ");
      break;
    case GL_UNSIGNED_INT_VEC2:
      return ("GL_UNSIGNED_INT_VEC2 ");
      break;
    case GL_UNSIGNED_INT_VEC3:
      return ("GL_UNSIGNED_INT_VEC3 ");
      break;
    case GL_UNSIGNED_INT_VEC4:
      return ("GL_UNSIGNED_INT_VEC4 ");
      break;
    case GL_FLOAT:
      return ("GL_FLOAT                ");
      break;
    case GL_FLOAT_VEC2:
      return ("GL_FLOAT_VEC2         ");
      break;
    case GL_FLOAT_VEC3:
      return ("GL_FLOAT_VEC3         ");
      break;
    case GL_FLOAT_VEC4:
      return ("GL_FLOAT_VEC4         ");
      break;
    case GL_INT:
      return ("GL_INT                ");
      break;
    case GL_INT_VEC2:
      return ("GL_INT_VEC2             ");
      break;
    case GL_INT_VEC3:
      return ("GL_INT_VEC3             ");
      break;
    case GL_INT_VEC4:
      return ("GL_INT_VEC4             ");
      break;
    case GL_BOOL:
      return ("GL_BOOL                 ");
      break;
    case GL_BOOL_VEC2:
      return ("GL_BOOL_VEC2         ");
      break;
    case GL_BOOL_VEC3:
      return ("GL_BOOL_VEC3         ");
      break;
    case GL_BOOL_VEC4:
      return ("GL_BOOL_VEC4         ");
      break;
    case GL_FLOAT_MAT2:
      return ("GL_FLOAT_MAT2         ");
      break;
    case GL_FLOAT_MAT3:
      return ("GL_FLOAT_MAT3         ");
      break;
    case GL_FLOAT_MAT4:
      return ("GL_FLOAT_MAT4         ");
      break;
    case GL_SAMPLER_1D:
      return ("GL_SAMPLER_1D         ");
      break;
    case GL_SAMPLER_2D:
      return ("GL_SAMPLER_2D         ");
      break;
    case GL_SAMPLER_3D:
      return ("GL_SAMPLER_3D         ");
      break;
    case GL_SAMPLER_CUBE:
      return ("GL_SAMPLER_CUBE         ");
      break;
    case GL_SAMPLER_1D_SHADOW:
      return ("GL_SAMPLER_1D_SHADOW ");
      break;
    case GL_SAMPLER_2D_SHADOW:
      return ("GL_SAMPLER_2D_SHADOW ");
      break;
    case GL_2_BYTES:
      return ("GL_2_BYTES            ");
      break;
    case GL_3_BYTES:
      return ("GL_3_BYTES            ");
      break;
    case GL_4_BYTES:
      return ("GL_4_BYTES            ");
      break;
    case GL_UNSIGNED_SHORT:
      return ("GL_UNSIGNED_SHORT    ");
      break;
    case GL_SHORT:
      return ("GL_SHORT                ");
      break;
    case GL_DOUBLE:
      return ("GL_DOUBLE            ");
      break;
  }
  return Stz "Undefined " + (int)type;
}
void GLContext::debugGetAttribState(string_t& strState) {
  //// - print bound attributes
  //int iMaxAttribs;
  //int iBoundAttrib;
  //glGetIntegerv(GL_MAX_VERTEX_ATTRIBS,&iMaxAttribs);
  //std::cout<<"Attribs: max count = "<<iMaxAttribs<<std::endl;
  //for(int xx=0; xx<iMaxAttribs; ++xx)
  //{
  //    glGetIntegerv(GL_VERTEX_ATTRIB_ARRAY0_NV+xx,&iBoundAttrib);
  //    std::cout<<"attrib "<<xx<<": "<<iBoundAttrib<<std::endl;
  //}
}
void GLContext::debugGetTextureState(string_t& strState) {
  chkErrRt();
  appendLine(strState, Stz "=========================================");
  appendLine(strState, Stz "             TEXTURE STATE               ");
  appendLine(strState, Stz "=========================================");
  GLint iActiveTexture;
  GLint iTextureBindingBuffer;
  GLint iMaxVertexTextureUnits;
  GLint iMaxTextureUnits;
  GLint iMaxCombinedTextureUnits;

  glGetIntegerv(GL_ACTIVE_TEXTURE, &iActiveTexture);  //0x84C0 is TEXTURE0
  glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &iMaxVertexTextureUnits);
  glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &iMaxTextureUnits);
  glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &iMaxCombinedTextureUnits);
  chkErrRt();
  appendLine(strState, Stz "Active Texture : " + "GL_TEXTURE" + TypeConv::intToStr(iActiveTexture - 0x84c0));
  appendLine(strState, Stz "Current active texture: " + "GL_TEXTURE" + TypeConv::intToStr(iActiveTexture - 0x84C0));
  appendLine(strState, Stz "Max Texture Units: " + iMaxTextureUnits);
  appendLine(strState, Stz "Max Vertex Texture Units: " + iMaxVertexTextureUnits);
  appendLine(strState, Stz "Max Combined Texture Units: " + iMaxCombinedTextureUnits);
  appendLine(strState, Stz "Below are the bound textures Per Texture Channel:");

  appendLine(strState, Stz "----------------All Textures----------------");
  //Get the max id (possibly)
  chkErrRt();
  GLuint maxId = 0;
  this->glGenTextures(1, &maxId);
  this->glDeleteTextures(1, &maxId);
  chkErrRt();
  //Show all registered texture parameters
  for (GLuint iTexId = 0; iTexId < maxId; ++iTexId) {
    GLContext::debugPrintTextureInfo(strState, iTexId);
  }
  // static int dummyid = 1;
  // if (dummyid != 3)
  //   GLContext::debugPrintTextureInfo(strState, dummyid);
  // std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
  //           << dummyid << " bosgo"
  //           << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
  //           << std::endl;
  // dummyid++;

  appendLine(strState, Stz "----------------Bound Textures----------------");

  chkErrRt();

  // - Get bound texture units.
  for (int i = 0; i < iMaxVertexTextureUnits; ++i) {
    GLint iTextureId = 0;  //Texture ID
    this->glActiveTexture(GL_TEXTURE0 + i);
    appendLine(strState, Stz "  Channel " + i);
    glGetIntegerv(GL_TEXTURE_BINDING_1D, &iTextureId);
    if (iTextureId > 0) {
      appendLine(strState, Stz "     1D: " + this->getObjectLabel(GL_TEXTURE, iTextureId));
    }
    iTextureId = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_1D_ARRAY, &iTextureId);
    if (iTextureId > 0) {
      appendLine(strState, Stz "     1D_ARRAY: " + this->getObjectLabel(GL_TEXTURE, iTextureId));
    }
    iTextureId = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &iTextureId);
    if (iTextureId > 0) {
      appendLine(strState, Stz "     2D: " + this->getObjectLabel(GL_TEXTURE, iTextureId));
    }
    iTextureId = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_2D_ARRAY, &iTextureId);
    if (iTextureId > 0) {
      appendLine(strState, Stz "     2D_ARRAY: " + this->getObjectLabel(GL_TEXTURE, iTextureId));
    }
    iTextureId = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_2D_MULTISAMPLE, &iTextureId);
    if (iTextureId > 0) {
      appendLine(strState, Stz "     2D_MULTISAMPLE: " + this->getObjectLabel(GL_TEXTURE, iTextureId));
    }
    iTextureId = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY, &iTextureId);
    if (iTextureId > 0) {
      appendLine(strState, Stz "     2D_MULTISAMPLE_ARRAY: " + this->getObjectLabel(GL_TEXTURE, iTextureId));
    }
    iTextureId = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_3D, &iTextureId);
    if (iTextureId > 0) {
      appendLine(strState, Stz "     3D: " + this->getObjectLabel(GL_TEXTURE, iTextureId));
    }
    iTextureId = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_BUFFER, &iTextureId);
    if (iTextureId > 0) {
      appendLine(strState, Stz "     BUFFER: " + this->getObjectLabel(GL_TEXTURE, iTextureId));
    }
    iTextureId = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, &iTextureId);
    if (iTextureId > 0) {
      appendLine(strState, Stz "     CUBE_MAP: " + this->getObjectLabel(GL_TEXTURE, iTextureId));
    }
    iTextureId = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_RECTANGLE, &iTextureId);
    if (iTextureId > 0) {
      appendLine(strState, Stz "     RECTANGLE: " + this->getObjectLabel(GL_TEXTURE, iTextureId));
    }
    chkErrRt();
  }
}
void GLContext::debugPrintTextureInfo(string_t& strState, GLuint iTexId) {
  if (!glIsTexture(iTexId)) {
    return;
  }
  chkErrRt();

  this->glActiveTexture(GL_TEXTURE0);

  string_t texName = this->getObjectLabel(GL_TEXTURE, iTexId);
  chkErrRt();
  //
  GLenum tex_target = this->getTextureTarget(iTexId);
  chkErrRt();

  if (tex_target == (GLenum)0) {
    appendLine(strState, Stz "  " + texName + " - Texture Target information was not handled.");
  }
  else {
    GLenum get_binding = GLContext::texTargetToTexBindingQuery(tex_target);
    if (get_binding == (GLenum)0) {
      appendLine(strState, Stz "  " + texName + " - Texture Binding information (" + tex_target + ") was invalid.");
    }
    else {
      GLint iSavedTextureId = 0;
      glGetIntegerv(get_binding, &iSavedTextureId);
      chkErrRt();
      glBindTexture(tex_target, iTexId);
      {
        GLContext::debugPrintBoundTextureAttribs(strState, texName, tex_target);
      }
      glBindTexture(tex_target, (GLuint)iSavedTextureId);

      chkErrRt();
    }
  }
}
GLenum GLContext::texTargetToTexBindingQuery(GLenum target) {
  if (target == GL_TEXTURE_1D) {
    return GL_TEXTURE_BINDING_1D;
  }
  else if (target == GL_TEXTURE_2D) {
    return GL_TEXTURE_BINDING_2D;
  }
  else if (target == GL_TEXTURE_3D) {
    return GL_TEXTURE_BINDING_3D;
  }
  else if (target == GL_TEXTURE_RECTANGLE) {
    return GL_TEXTURE_BINDING_RECTANGLE;
  }
  else if (target == GL_TEXTURE_BUFFER) {
    return GL_TEXTURE_BINDING_BUFFER;
  }
  else if (target == GL_TEXTURE_CUBE_MAP) {
    return GL_TEXTURE_BINDING_CUBE_MAP;
  }
  else if (target == GL_TEXTURE_1D_ARRAY) {
    return GL_TEXTURE_BINDING_1D_ARRAY;
  }
  else if (target == GL_TEXTURE_2D_ARRAY) {
    return GL_TEXTURE_BINDING_2D_ARRAY;
  }
  else if (target == GL_TEXTURE_CUBE_MAP_ARRAY) {
    return GL_TEXTURE_BINDING_CUBE_MAP_ARRAY;
  }
  else if (target == GL_TEXTURE_2D_MULTISAMPLE) {
    return GL_TEXTURE_BINDING_2D_MULTISAMPLE;
  }
  else if (target == GL_TEXTURE_2D_MULTISAMPLE_ARRAY) {
    return GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY;
  }
  return (GLenum)0;
}
GLenum GLContext::texBindingToTexTargetQuery(GLenum binding) {
  if (binding == GL_TEXTURE_BINDING_1D) {
    return GL_TEXTURE_1D;
  }
  else if (binding == GL_TEXTURE_BINDING_2D) {
    return GL_TEXTURE_2D;
  }
  else if (binding == GL_TEXTURE_BINDING_3D) {
    return GL_TEXTURE_3D;
  }
  else if (binding == GL_TEXTURE_BINDING_RECTANGLE) {
    return GL_TEXTURE_RECTANGLE;
  }
  else if (binding == GL_TEXTURE_BINDING_BUFFER) {
    return GL_TEXTURE_BUFFER;
  }
  else if (binding == GL_TEXTURE_BINDING_CUBE_MAP) {
    return GL_TEXTURE_CUBE_MAP;
  }
  else if (binding == GL_TEXTURE_BINDING_1D_ARRAY) {
    return GL_TEXTURE_1D_ARRAY;
  }
  else if (binding == GL_TEXTURE_BINDING_2D_ARRAY) {
    return GL_TEXTURE_2D_ARRAY;
  }
  else if (binding == GL_TEXTURE_BINDING_CUBE_MAP_ARRAY) {
    return GL_TEXTURE_CUBE_MAP_ARRAY;
  }
  else if (binding == GL_TEXTURE_BINDING_2D_MULTISAMPLE) {
    return GL_TEXTURE_2D_MULTISAMPLE;
  }
  else if (binding == GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY) {
    return GL_TEXTURE_2D_MULTISAMPLE_ARRAY;
  }
  return (GLenum)0;
}
void GLContext::debugPrintBoundTextureAttribs(string_t& strState, const string_t& texName, GLenum tex_target) {
//**Print texture attributes OpenGL
//Quick stringify openGL parameter names and their values.
#define GL_VVAL std::string("(" + TypeConv::intToStr(value) + ")")
#define GL_VPERR std::string("GL Enum " + (GL_VVAL) + " not found.")
#define GL_VP0(dx, dy) ((value == dx) ? (std::string(#dx)) : (dy))
#define GL_TEXPARM(tt, ss, xx, vp)                \
  glGetTexParameteriv(tt, xx, &value);            \
  ss += "    " + std::string(#xx) + " = " + (vp); \
  chkErrRt();

  int value = (int)tex_target;

  string_t strTi = " " + texName + " - ";
  strTi += "Binding: " + GL_VP0(GL_TEXTURE_1D, GL_VP0(GL_TEXTURE_2D, GL_VP0(GL_TEXTURE_3D, GL_VP0(GL_TEXTURE_RECTANGLE, GL_VP0(GL_TEXTURE_BUFFER, GL_VP0(GL_TEXTURE_CUBE_MAP, GL_VP0(GL_TEXTURE_1D_ARRAY, GL_VP0(GL_TEXTURE_2D_ARRAY, GL_VP0(GL_TEXTURE_CUBE_MAP_ARRAY, GL_VP0(GL_TEXTURE_2D_MULTISAMPLE, GL_VP0(GL_TEXTURE_2D_MULTISAMPLE_ARRAY, GL_VPERR))))))))))) + "\n";
  GL_TEXPARM(tex_target, strTi, GL_TEXTURE_MAG_FILTER, GL_VP0(GL_NEAREST, GL_VP0(GL_LINEAR, GL_VPERR)))
  GL_TEXPARM(tex_target, strTi, GL_TEXTURE_MIN_FILTER, GL_VP0(GL_NEAREST, GL_VP0(GL_LINEAR, GL_VP0(GL_NEAREST_MIPMAP_NEAREST, GL_VP0(GL_LINEAR_MIPMAP_NEAREST, GL_VP0(GL_NEAREST_MIPMAP_LINEAR, GL_VP0(GL_LINEAR_MIPMAP_LINEAR, GL_VPERR)))))))
  strTi += "\n";
  GL_TEXPARM(tex_target, strTi, GL_TEXTURE_MIN_LOD, TypeConv::intToStr(value));
  GL_TEXPARM(tex_target, strTi, GL_TEXTURE_MAX_LOD, TypeConv::intToStr(value));
  GL_TEXPARM(tex_target, strTi, GL_TEXTURE_BASE_LEVEL, TypeConv::intToStr(value));
  GL_TEXPARM(tex_target, strTi, GL_TEXTURE_MAX_LEVEL, TypeConv::intToStr(value));
  strTi += "\n";
  GL_TEXPARM(tex_target, strTi, GL_TEXTURE_WRAP_S, GL_VP0(GL_CLAMP_TO_BORDER, GL_VP0(GL_CLAMP_TO_EDGE, GL_VP0(GL_MIRRORED_REPEAT, GL_VP0(GL_MIRROR_CLAMP_TO_EDGE, GL_VP0(GL_REPEAT, GL_VP0(GL_CLAMP_TO_BORDER, GL_VPERR)))))));
  GL_TEXPARM(tex_target, strTi, GL_TEXTURE_WRAP_T, GL_VP0(GL_CLAMP_TO_BORDER, GL_VP0(GL_CLAMP_TO_EDGE, GL_VP0(GL_MIRRORED_REPEAT, GL_VP0(GL_MIRROR_CLAMP_TO_EDGE, GL_VP0(GL_REPEAT, GL_VP0(GL_CLAMP_TO_BORDER, GL_VPERR)))))));
  GL_TEXPARM(tex_target, strTi, GL_TEXTURE_WRAP_R, GL_VP0(GL_CLAMP_TO_BORDER, GL_VP0(GL_CLAMP_TO_EDGE, GL_VP0(GL_MIRRORED_REPEAT, GL_VP0(GL_MIRROR_CLAMP_TO_EDGE, GL_VP0(GL_REPEAT, GL_VP0(GL_CLAMP_TO_BORDER, GL_VPERR)))))));
  strTi += "\n";
  //GL_TEXPARM(tex_target, strTi, GL_TEXTURE_BORDER_COLOR, TypeConv::intToStr(value));
  GL_TEXPARM(tex_target, strTi, GL_TEXTURE_PRIORITY, TypeConv::intToStr(value));
  GL_TEXPARM(tex_target, strTi, GL_TEXTURE_RESIDENT, GL_VP0(GL_TRUE, GL_VP0(GL_FALSE, GL_VPERR)));
  GL_TEXPARM(tex_target, strTi, GL_TEXTURE_COMPARE_MODE, GL_VP0(GL_COMPARE_R_TO_TEXTURE, GL_VP0(GL_NONE, GL_VPERR)));
  strTi += "\n";
  GL_TEXPARM(tex_target, strTi, GL_TEXTURE_COMPARE_FUNC, GL_VP0(GL_LEQUAL, GL_VP0(GL_GEQUAL, GL_VP0(GL_LESS, GL_VP0(GL_GREATER, GL_VP0(GL_EQUAL, GL_VP0(GL_NOTEQUAL, GL_VP0(GL_ALWAYS, GL_VP0(GL_NEVER, GL_VPERR)))))))));
  GL_TEXPARM(tex_target, strTi, GL_DEPTH_TEXTURE_MODE, GL_VP0(GL_LUMINANCE, GL_VP0(GL_INTENSITY, GL_VP0(GL_ALPHA, GL_VPERR))));
  GL_TEXPARM(tex_target, strTi, GL_GENERATE_MIPMAP, GL_VP0(GL_TRUE, GL_VP0(GL_FALSE, GL_VPERR)));

  appendLine(strState, strTi);
}

void GLContext::debugGetFramebufferAttachmentState(string_t& strState) {
  appendLine(strState, "-----------------------------------");
  appendLine(strState, "--FRAMEBUFFERS");
  GLenum eDrawBuffer;
  GLint iDrawFramebufferBinding;  //name of fb beijmg drawn to
  GLint iReadFramebufferBinding;  //name of fb beijmg drawn to
  GLint iRenderbufferBinding;
  GLenum eReadBuffer;
  GLint iSamplerBinding;  //!Texture sampler (should be 2d??)
  GLint maxColorAttachments = 0;
  GLint boundFramebuffer;
  //Reference enums
  //#define GL_FRONT 0x0404
  //#define GL_BACK 0x0405

  // Framebuffers
  glGetIntegerv(GL_DRAW_BUFFER, (GLint*)&eDrawBuffer);  //0x08CE0 is the COLOR ATTACHMENT 1, 0x0405 is the default BACK buffer.
  glGetIntegerv(GL_READ_BUFFER, (GLint*)&eReadBuffer);  //Default: GL_BACK
  glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &iDrawFramebufferBinding);
  glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &iReadFramebufferBinding);
  glGetIntegerv(GL_RENDERBUFFER_BINDING, &iRenderbufferBinding);
  glGetIntegerv(GL_SAMPLER_BINDING, &iSamplerBinding);
  glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColorAttachments);
  glGetIntegerv(GL_FRAMEBUFFER_BINDING, &boundFramebuffer);

  this->chkErrRt();

  appendLine(strState, Stz " # Max Color Attachments: " + maxColorAttachments);
  appendLine(strState, Stz " Current Bound Framebuffer: " + this->getObjectLabel(GL_FRAMEBUFFER, boundFramebuffer));
  appendLine(strState, Stz " Current Draw Framebuffer (glDrawBuffer): " + this->getObjectLabel(GL_FRAMEBUFFER, iDrawFramebufferBinding));
  appendLine(strState, Stz " Current Read Framebuffer (glReadBuffer): " + this->getObjectLabel(GL_FRAMEBUFFER, iReadFramebufferBinding));
  if (iDrawFramebufferBinding != iReadFramebufferBinding) {
    appendLine(strState, "   NOTE: Draw and Read framebuffers are bound different!");
  }
  appendLine(strState, Stz " Current RenderBuffer Binding: " + iRenderbufferBinding);
  appendLine(strState, Stz " Read Buffer Enum: " + eReadBuffer);
  appendLine(strState, Stz " Current Sampler Binding: " + iSamplerBinding);

  if (boundFramebuffer == 0) {
    return;
  }

  // Print details about hte bound buffer.
  for (int i = 0; i < maxColorAttachments; ++i) {
    GLContext::debugPrintFBOAttachment(strState, GL_COLOR_ATTACHMENT0 + i);
  }
  GLContext::debugPrintFBOAttachment(strState, GL_DEPTH_ATTACHMENT);
  GLContext::debugPrintFBOAttachment(strState, GL_STENCIL_ATTACHMENT);
}
void GLContext::debugPrintFBOAttachment(string_t& strState, GLenum attachment) {
  GLint attachmentName = 0;
  GLint attachmentType = 0;
  GLint mipmapLevel = 0;

  string_t strAttachment;
  if (attachment == GL_DEPTH_ATTACHMENT) {
    strAttachment = ("GL_DEPTH_ATTACHMENT");
  }
  else if (attachment == GL_STENCIL_ATTACHMENT) {
    strAttachment = ("GL_STENCIL_ATTACHMENT");
  }
  else if (attachment >= GL_COLOR_ATTACHMENT0 && attachment <= GL_COLOR_ATTACHMENT15) {
    strAttachment = Stz "GL_COLOR_ATTACHMENT" + (attachment - GL_COLOR_ATTACHMENT0);
  }

  appendLine(strState, Stz "  Attachment: " + strAttachment);

  this->glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, attachment, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &attachmentType);
  this->chkErrRt();
  if (attachmentType == GL_NONE) {
    appendLine(strState, Stz "    Type: " + "GL_NONE");
  }
  else if (attachmentType == GL_RENDERBUFFER) {
    this->glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, attachment, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &attachmentName);
    this->chkErrRt();
    appendLine(strState, Stz "    Type: " + "GL_RENDERBUFFER");
    appendLine(strState, Stz "    Name: " + this->getObjectLabel(GL_RENDERBUFFER, attachmentName));
  }
  else if (attachmentType == GL_TEXTURE) {
    this->glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, attachment, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &attachmentName);
    this->chkErrRt();
    this->glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, attachment, GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL, &mipmapLevel);
    this->chkErrRt();
    appendLine(strState, Stz "    Type: " + "GL_TEXTURE");
    appendLine(strState, Stz "    Name: " + this->getObjectLabel(GL_TEXTURE, attachmentName));
    appendLine(strState, Stz "    Mipmap Level: " + mipmapLevel);
  }
}
void GLContext::debugGetVertexArrayState(string_t& strState) {
  appendLine(strState, ("----------------------------------------"));
  appendLine(strState, ("--VERTEX ARRAY STATE"));
  int nMaxAttribs;
  GLint iVertexArrayBinding;
  glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nMaxAttribs);
  glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &iVertexArrayBinding);

  appendLine(strState, Stz "# Bound Vertex Array Id (VAO): " + this->getObjectLabel(GL_VERTEX_ARRAY, iVertexArrayBinding));
  appendLine(strState, Stz "# Max Allowed Atribs: " + nMaxAttribs);
  appendLine(strState, Stz("---------------------------------------"));
  appendLine(strState, Stz("--Active Vertex Attribs: "));

  this->chkErrRt();

  // - Disable all arrays by default.
  for (int iAttrib = 0; iAttrib < nMaxAttribs; ++iAttrib) {
    //TODO:
    size_t iArrayBufferBinding = 0;
    GLint iArrayEnabled = 0;
    GLint iAttribArraySize = 0;
    GLenum iAttribArrayType = 0;
    GLint iAttribArrayStride = 0;
    GLint iAttribArrayInteger = 0;
    GLfloat fCurAttrib[4];
    GLint iCurAttrib[4];
    GLuint uiCurAttrib[4];
    GLint iAttribArrayNormalized;
    //GLint iAttribArrayDivisor;
    memset(fCurAttrib, 0, sizeof(GLfloat) * 4);
    memset(iCurAttrib, 0, sizeof(GLint) * 4);
    memset(uiCurAttrib, 0, sizeof(GLuint) * 4);

    this->glGetVertexAttribiv(iAttrib, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, (GLint*)&iArrayBufferBinding);
    this->chkErrRt();
    this->glGetVertexAttribiv(iAttrib, GL_VERTEX_ATTRIB_ARRAY_ENABLED, (GLint*)&iArrayEnabled);
    this->chkErrRt();
    this->glGetVertexAttribiv(iAttrib, GL_VERTEX_ATTRIB_ARRAY_SIZE, (GLint*)&iAttribArraySize);
    this->chkErrRt();
    this->glGetVertexAttribiv(iAttrib, GL_VERTEX_ATTRIB_ARRAY_TYPE, (GLint*)&iAttribArrayType);
    this->chkErrRt();
    this->glGetVertexAttribiv(iAttrib, GL_VERTEX_ATTRIB_ARRAY_STRIDE, (GLint*)&iAttribArrayStride);
    this->chkErrRt();
    this->glGetVertexAttribiv(iAttrib, GL_VERTEX_ATTRIB_ARRAY_INTEGER, (GLint*)&iAttribArrayInteger);
    this->chkErrRt();

    this->glGetVertexAttribiv(iAttrib, GL_VERTEX_ATTRIB_ARRAY_NORMALIZED, (GLint*)&iAttribArrayNormalized);
    this->chkErrRt();
    //glGetVertexAttribiv(iAttrib, GL_VERTEX_ATTRIB_ARRAY_DIVISOR, (GLint*)&iAttribArrayDivisor);
    //CheckGpuErrorsDbg();

    appendLine(strState, Stz "Attrib " + iAttrib + "  Enabled:" + (iArrayEnabled ? "Y" : "N"));

    if (!iArrayEnabled) {
      continue;
    }

    appendLine(strState, Stz "  ArrayBufferBinding: " + this->getObjectLabel(GL_BUFFER, iArrayBufferBinding));
    appendLine(strState, Stz "  Size: " + iAttribArraySize);
    appendLine(strState, Stz "  Stride: " + iAttribArrayStride);
    appendLine(strState, Stz "  Is Integer: " + (iAttribArrayInteger ? "Y" : "N"));
    appendLine(strState, Stz "  Normalized: " + (iAttribArrayNormalized ? "Y" : "N"));
    appendLine(strState, Stz "  Type: " + GLContext::openGlTypeToString(iAttribArrayType));

    if (iAttrib != 0) {
      //Generic vertex attribute 0 is unique in that it has no current state,
      //so an error will be generated if index is 0. The initial value for all
      //other generic vertex attributes is (0,0,0,1).
      switch (iAttribArrayType) {
        case GL_INT:
          this->glGetVertexAttribIiv(iAttrib, GL_CURRENT_VERTEX_ATTRIB, (GLint*)&iCurAttrib);
          this->chkErrRt();
          appendLine(strState, Stz "  Cur Value: " + iCurAttrib[0] + "," + iCurAttrib[1] + "," + iCurAttrib[2] + "," + iCurAttrib[3]);
          break;
        case GL_UNSIGNED_INT:
          this->glGetVertexAttribIuiv(iAttrib, GL_CURRENT_VERTEX_ATTRIB, (GLuint*)&uiCurAttrib);
          this->chkErrRt();
          appendLine(strState, Stz "  Cur Value: " + uiCurAttrib[0] + "," + uiCurAttrib[1] + "," + uiCurAttrib[2] + "," + uiCurAttrib[3]);
          break;
        case GL_FLOAT:
          this->glGetVertexAttribfv(iAttrib, GL_CURRENT_VERTEX_ATTRIB, (GLfloat*)&iCurAttrib);
          this->chkErrRt();
          appendLine(strState, Stz "  Cur Value: " + fCurAttrib[0] + "," + fCurAttrib[1] + "," + fCurAttrib[2] + "," + fCurAttrib[3]);
          break;
        default:
          appendLine(strState, "  TODO:Cur Value: ");
          break;
      };
    }
    //This reads the attrib values such as float, int etc.
    //GLint iCurrentVertexAttrib;
    //glGetVertexAttribiv(iAttrib, GL_CURRENT_VERTEX_ATTRIB, (GLint*)&iCurrentVertexAttrib);
  }
}
#pragma endregion

}  // namespace BR2

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
#include "../base/GraphicsWindow.h"
#include "../base/EngineConfig.h"
#include "../math/MathAll.h"
#include "../gfx/RenderUtils.h"
#include "../gfx/ParticleManager.h"
#include "../gfx/TexCache.h"
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

namespace BR2 {
GLContext::GLContext(std::shared_ptr<GraphicsApi> api, std::shared_ptr<GLProfile> profile, SDL_Window* sdl_win) : GraphicsContext(api) {
  _profile = profile;
  _pSDLWindow = sdl_win;

  _context = SDL_GL_CreateContext(sdl_win);
  if (!_context) {
    //Eat the "context failed" error.  It's not informative.
    SDLUtils::checkSDLErr(false, false);
    _bValid = false;
  }
  else {
    int ver, subver, shad_ver, shad_subver;
    getOpenGLVersion(ver, subver, shad_ver, shad_subver);

    //Make sure we have a good depth value.
    int iDepth = 0;
    SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &iDepth);
    _iSupportedDepthSize = iDepth;
    if (iDepth < profile->_iDepthBits) {
      BRLogWarn("Depth buffer size " + profile->_iDepthBits = " not supported in profile.");
      _bValid = false;
    }
    else if (checkOpenGlMinimumVersionInfo(profile->_iMinVersion, profile->_iMinSubVersion)) {
      printHelpfulDebug();

      if (profile->_iMSAABuffers == 0) {
        Gu::getEngineConfig()->setEnableMSAA(false);
        BRLogWarn("This configuration did not support MSAA.");
      }

      if (!loadOpenGLFunctions()) {
        BRLogError("Failed to load context.");
        SDL_GL_DeleteContext(_context);
        _bValid = false;
      }
      else {
        //Quick GL test.
        glUseProgram(0);

        //Check that OpenGL initialized successfully
        loadCheckProc();

        //Create opengl error handler
        _pOglErr = std::make_unique<OglErr>();

        _bValid = true;
      }
    }
  }
}
GLContext::~GLContext() {
  if (_context) {
    _pOglErr = nullptr;
    /* SDL_GL_MakeCurrent(0, NULL); */ /* doesn't do anything */
    SDL_GL_DeleteContext(_context);
  }
}
void GLContext::setPolygonMode(PolygonMode p) {
  if (Gu::getCoreContext()->isForwardCompatible() == false) {
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
    Gu::getCoreContext()->chkErrDbg();
  }
  else {
    BRLogWarnOnce("glPolygonMode not supported in compatibility mode.");
  }
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
  SDLGLP2(PFNGLGETSAMPLERPARAMETERFVPROC, glGetSamplerParameterfv);
  SDLGLP2(PFNGLGETSAMPLERPARAMETERIIVPROC, glGetSamplerParameterIiv);
  SDLGLP2(PFNGLGETSAMPLERPARAMETERIUIVPROC, glGetSamplerParameterIuiv);
  SDLGLP2(PFNGLGETSAMPLERPARAMETERIUIVPROC, glGetSamplerParameteruiv);
  SDLGLP2(PFNGLGETSAMPLERPARAMETERIVPROC, glGetSamplerParameteriv);
  SDLGLP2(PFNGLGETSAMPLERPARAMETERIVPROC, glGetSamplerParameteriv);
  SDLGLP2(PFNGLCOPYIMAGESUBDATAPROC, glCopyImageSubData);
  SDLGLP2(PFNGLDELETETEXTURESEXTPROC, glDeleteTextures);
  SDLGLP2(PFNGLGETTEXPARAMETERIIVPROC, glGetTexParameteriv);
  SDLGLP2(PFNGLGETTEXPARAMETERIUIVPROC, glGetTexParameteruiv);

  return bValid;
}
void GLContext::setLineWidth(float w) {
  if (isForwardCompatible() == false) {
    glLineWidth(w);
    Gu::getCoreContext()->chkErrDbg();
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
string_t GLContext::getObjectLabel(GLenum type, GLuint objectId) {
  string_t res = "";
  if (objectId > 0) {
    const int buflen = 256;
    char lenbuf[buflen];
    GLsizei length = 0;

    Gu::checkErrorsDbg();
    this->glGetObjectLabel(type, objectId, buflen, &length, lenbuf);
    Gu::checkErrorsRt();

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
      Gu::checkErrorsDbg();
      this->glObjectLabel(type, objectId, label.length(), label.c_str());
      Gu::checkErrorsRt();
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
  //Returns the binding for a texture, there IS glGetTextureParameteriv with GL_TEXTURE_TARGET which is an extension apparently.

  //Check/handle any errors that may exist.
  Gu::checkErrorsRt();
  GLenum ret = (GLenum)0;

  std::function<GLenum(GLuint, GLenum)> TEX_CHECKTARGET_RT = [](GLuint texid, GLenum ttx) {
    for (int i = 0; i < 100; ++i) {
      if (glGetError() == GL_NO_ERROR) {
        break;
      }
    }
    glBindTexture(ttx, texid);
    if (glGetError() == GL_NO_ERROR) {
      //We call this in order to also ignore potential GPU logs.
      Gu::checkErrorsRt(true);
      return ttx;
    }
    return (GLenum)0;
  };
  //from https://www.khronos.org/opengl/wiki/Texture
  if ((ret = TEX_CHECKTARGET_RT(textureId, GL_TEXTURE_1D)) != (GLenum)0) {
    return ret;
  }
  else if ((ret = TEX_CHECKTARGET_RT(textureId, GL_TEXTURE_2D)) != (GLenum)0) {
    return ret;
  }
  else if ((ret = TEX_CHECKTARGET_RT(textureId, GL_TEXTURE_3D)) != (GLenum)0) {
    return ret;
  }
  else if ((ret = TEX_CHECKTARGET_RT(textureId, GL_TEXTURE_RECTANGLE)) != (GLenum)0) {
    return ret;
  }
  else if ((ret = TEX_CHECKTARGET_RT(textureId, GL_TEXTURE_BUFFER)) != (GLenum)0) {
    return ret;
  }
  else if ((ret = TEX_CHECKTARGET_RT(textureId, GL_TEXTURE_CUBE_MAP)) != (GLenum)0) {
    return ret;
  }
  else if ((ret = TEX_CHECKTARGET_RT(textureId, GL_TEXTURE_1D_ARRAY)) != (GLenum)0) {
    return ret;
  }
  else if ((ret = TEX_CHECKTARGET_RT(textureId, GL_TEXTURE_2D_ARRAY)) != (GLenum)0) {
    return ret;
  }
  else if ((ret = TEX_CHECKTARGET_RT(textureId, GL_TEXTURE_CUBE_MAP_ARRAY)) != (GLenum)0) {
    return ret;
  }
  else if ((ret = TEX_CHECKTARGET_RT(textureId, GL_TEXTURE_2D_MULTISAMPLE)) != (GLenum)0) {
    return ret;
  }
  else if ((ret = TEX_CHECKTARGET_RT(textureId, GL_TEXTURE_2D_MULTISAMPLE_ARRAY)) != (GLenum)0) {
    return ret;
  }

  return ret;
}
void GLContext::glGenTextures(GLsizei count, GLuint* ids) {
  ::glGenTextures(1, ids);
}

}  // namespace BR2

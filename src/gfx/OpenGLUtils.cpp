#include "../base/BaseHeader.h"
#include "../base/FileSystem.h"
#include "../base/OperatingSystem.h"
#include "../base/Gu.h"
#include "../base/Img32.h"
#include "../base/GLContext.h"
#include "../base/Logger.h"
#include "../base/BaseHeader.h"
#include "../base/Allocator.h"
#include "../gfx/OpenGLUtils.h"
#include "../gfx/ShaderMaker.h"
#include "../gfx/ShaderBase.h"
#include "../gfx/ShaderUniform.h"
#include <iostream>

namespace BR2 {
void OpenGLUtils::resetRenderState() {
  OpenGLUtils::debugGetRenderState();
  {
    Gu::getShaderMaker()->shaderBound(nullptr);

    //glUseProgram(NULL);//DO NOT CALL - we must maintain consistency on the gpu driver
    Gu::getCoreContext()->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (GLuint)0);
    Gu::getCoreContext()->glBindBuffer(GL_ARRAY_BUFFER, (GLuint)0);

    int iMaxTextures = 0;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, (GLint*)&iMaxTextures);
    for (int iTex = 0; iTex < iMaxTextures; iTex++) {
      Gu::getCoreContext()->glActiveTexture(GL_TEXTURE0 + iTex);
      Gu::getCoreContext()->glBindTexture(GL_TEXTURE_2D, 0);
    }

    //  VaoData::debugDisableAllAttribArrays();
    Gu::getCoreContext()->glBindVertexArray(0);
    Gu::getCoreContext()->glBindBuffer(GL_ARRAY_BUFFER, 0);
    Gu::getCoreContext()->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    Gu::getCoreContext()->glActiveTexture(GL_TEXTURE0);

    //Note: Client textures are deprecated
    //  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    //  glDisableClientState(GL_COLOR_ARRAY);
    //  glDisableClientState(GL_NORMAL_ARRAY);
    //  glDisableClientState(GL_VERTEX_ARRAY);
    //  glDisableClientState(GL_INDEX_ARRAY);
    //  glClientActiveTexture(GL_TEXTURE0);
    glDisable(GL_LIGHTING);
  }
  OpenGLUtils::debugGetRenderState();
}

//void OpenGLUtils::createDepthTexture(GLuint& __out_ texId, int w, int h, GLenum depthSize){
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
void OpenGLUtils::createDepthTexture(const string_t& owner, GLuint* __out_ texId, int32_t w, int32_t h, bool bMsaaEnabled, int32_t nMsaaSamples, GLenum eRequestedDepth) {
  //Creates a depth texture, or multisample depth texture on texture channel 0
  //This will query the device to make sure the depth format is supported.
  Gu::getCoreContext()->chkErrRt();
  GLenum texTarget;

  string_t label = owner + "_DepthTexture";

  if (bMsaaEnabled) {
    texTarget = GL_TEXTURE_2D_MULTISAMPLE;
    label += "_Multisample";
  }
  else {
    texTarget = GL_TEXTURE_2D;
  }
  Gu::getCoreContext()->glActiveTexture(GL_TEXTURE0);
  Gu::getCoreContext()->glGenTextures(1, texId);
  Gu::getCoreContext()->chkErrRt();
  Gu::getCoreContext()->glBindTexture(texTarget, *texId);
  //THe following parameters are for depth textures only
  Gu::getCoreContext()->chkErrRt();

  if (bMsaaEnabled == false) {
    //For some reason you can't use this with multisample.
    glTexParameteri(texTarget, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);  //GL_NONE
    Gu::getCoreContext()->chkErrRt();
    glTexParameteri(texTarget, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    Gu::getCoreContext()->chkErrRt();
    glTexParameterf(texTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    Gu::getCoreContext()->chkErrRt();
    glTexParameterf(texTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    Gu::getCoreContext()->chkErrRt();
    glTexParameteri(texTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    Gu::getCoreContext()->chkErrRt();
    glTexParameteri(texTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    Gu::getCoreContext()->chkErrRt();
  }

  Gu::getCoreContext()->setObjectLabel(GL_TEXTURE, *texId, label);

  getCompatibleDepthComponent(eRequestedDepth, [&](GLenum eDepth) {
    if (bMsaaEnabled) {
      Gu::getCoreContext()->glTexImage2DMultisample(texTarget, nMsaaSamples, eDepth, w, h, GL_TRUE);
      Gu::checkErrorsRt();
    }
    else {
      glTexImage2D(texTarget, 0, eDepth, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
      Gu::checkErrorsRt();
    }
  });

  Gu::checkErrorsRt();
  Gu::getCoreContext()->glBindTexture(texTarget, 0);
  Gu::checkErrorsRt();
}

GLenum OpenGLUtils::getSupportedDepthSize() {
  int32_t depth = Gu::getCoreContext()->getSupportedDepthSize();
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
  else {
    BRThrowException("[222039] Unsupported depth component size " + depth);
  }
}
void OpenGLUtils::getCompatibleDepthComponent(GLenum eRequestedDepth, std::function<void(GLenum)> func) {
  int32_t depth = Gu::getCoreContext()->getSupportedDepthSize();
  //Shortcut lambda to create something that asks for a GL_DEPTH_COMPONENT enum.
  GLenum eDepthSize;
  if (eRequestedDepth == GL_DEPTH_COMPONENT32F && depth < 32) {
    eDepthSize = getSupportedDepthSize();
  }
  else if (eRequestedDepth == GL_DEPTH_COMPONENT32 && depth < 32) {
    eDepthSize = getSupportedDepthSize();
  }
  else if (eRequestedDepth == GL_DEPTH_COMPONENT24 && depth < 24) {
    eDepthSize = getSupportedDepthSize();
  }
  else {
    eDepthSize = eRequestedDepth;
  }

  Gu::getCoreContext()->chkErrRt();

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
bool OpenGLUtils::getTextureDataFromGpu(std::shared_ptr<Img32> __out_ image, GLuint iGLTexId, GLenum eTexTargetBase, int iCubeMapSide) {
  AssertOrThrow2(image != nullptr);
  GLint iSavedTextureBinding;
  GLenum eTexBinding = texTargetToTexBindingQuery(eTexTargetBase);

  GLenum eTexTargetSide = eTexTargetBase;
  if (eTexTargetBase == GL_TEXTURE_CUBE_MAP) {
    AssertOrThrow2(iCubeMapSide >= 0 && iCubeMapSide < 6);
    eTexTargetSide = GL_TEXTURE_CUBE_MAP_POSITIVE_X + iCubeMapSide;
  }

  glGetIntegerv(eTexBinding, &iSavedTextureBinding);
  Gu::checkErrorsRt();

  Gu::getCoreContext()->glActiveTexture(GL_TEXTURE0);
  glBindTexture(eTexTargetBase, iGLTexId);
  Gu::checkErrorsRt();
  {
    GLint w, h;
    int32_t iMipLevel = 0;
    glGetTexLevelParameteriv(eTexTargetSide, iMipLevel, GL_TEXTURE_WIDTH, &w);
    Gu::checkErrorsRt();

    glGetTexLevelParameteriv(eTexTargetSide, iMipLevel, GL_TEXTURE_HEIGHT, &h);
    Gu::checkErrorsRt();

    //Adding for the pick texture test.
    GLenum internalFormat = 0;
    glGetTexLevelParameteriv(eTexTargetSide, iMipLevel, GL_TEXTURE_INTERNAL_FORMAT, (GLint*)&internalFormat);
    Gu::checkErrorsRt();

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
    Gu::checkErrorsRt();
  }
  glBindTexture(eTexTargetBase, iSavedTextureBinding);
  Gu::checkErrorsRt();

  return true;
}

void OpenGLUtils::saveFramebufferAsPng(string_t&& strLoc, GLuint iFBOId) {
  GLint iFbBindingLast;
  GLint iFbWidth;
  GLint iFbHeight;

  //Save current FBO
  glGetIntegerv(GL_FRAMEBUFFER_BINDING, &iFbBindingLast);

  if (iFBOId == 0) {
    iFBOId = iFbBindingLast;
  }

  Gu::getCoreContext()->glBindFramebuffer(GL_FRAMEBUFFER, iFBOId);
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
  Gu::getCoreContext()->glBindFramebuffer(GL_FRAMEBUFFER, iFbBindingLast);
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

string_t OpenGLUtils::debugGetRenderState(bool bForceRun, bool bPrintToStdout, bool bSaveFramebufferTexture) {
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
  Gu::getCoreContext()->chkErrRt();

  GLint iBlend;
  glGetIntegerv(GL_BLEND, (GLint*)&iBlend);
  appendLine(strState, Stz " Blending: " + (iBlend ? "ENABLED" : "DISABLED"));
  Gu::getCoreContext()->chkErrRt();

  GLint iCullFace;
  glGetIntegerv(GL_CULL_FACE, (GLint*)&iCullFace);
  appendLine(strState, Stz " Culling: " + (iCullFace ? "ENABLED" : "DISABLED"));
  Gu::getCoreContext()->chkErrRt();

  GLint iDepthTest;
  glGetIntegerv(GL_DEPTH_TEST, (GLint*)&iDepthTest);
  appendLine(strState, Stz " Depth Test: " + (iDepthTest ? "ENABLED" : "DISABLED"));
  Gu::getCoreContext()->chkErrRt();

  OpenGLUtils::debugGetLegacyViewAndMatrixStack(strState);
  Gu::getCoreContext()->chkErrRt();
  OpenGLUtils::debugGetBufferState(strState);
  Gu::getCoreContext()->chkErrRt();
  //OpenGLUtils::debugGetAttribState(); // This is redundant with vertexarraystate
  //    CheckGpuErrorsDbg();
  OpenGLUtils::debugGetTextureState(strState);
  Gu::getCoreContext()->chkErrRt();

  OpenGLUtils::debugGetVertexArrayState(strState);
  Gu::getCoreContext()->chkErrRt();
  OpenGLUtils::debugGetFramebufferAttachmentState(strState);
  Gu::getCoreContext()->chkErrRt();

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
void OpenGLUtils::debugGetLegacyViewAndMatrixStack(string_t& strState) {
  GLint iScissorBox[4];
  GLint iViewportBox[4];
  appendLine(strState, "**Scissor and Legcay info**");
  //View Bounds (Legacy)
  glGetIntegerv(GL_SCISSOR_BOX, (GLint*)iScissorBox);
  glGetIntegerv(GL_VIEWPORT, (GLint*)iViewportBox);
  appendLine(strState, Stz "Scissor: " + iScissorBox[0] + "," + iScissorBox[1] + "," + iScissorBox[2] + "," + iScissorBox[3]);
  appendLine(strState, Stz "Viewport: " + iViewportBox[0] + "," + iViewportBox[1] + "," + iViewportBox[2] + "," + iViewportBox[3]);
  //TODO: legacy matrix array state.
  Gu::getCoreContext()->chkErrRt();
}
void OpenGLUtils::debugGetBufferState(string_t& strState) {
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
  Gu::getCoreContext()->chkErrRt();

  appendLine(strState, Stz "Bound Vertex Array Buffer (VBO): " + Gu::getCoreContext()->getObjectLabel(GL_BUFFER, iBoundBuffer));
  appendLine(strState, Stz "Bound Element Array Buffer (IBO): " + Gu::getCoreContext()->getObjectLabel(GL_BUFFER, iElementArrayBufferBinding));
  appendLine(strState, Stz "Bound Shader Storage Buffer (SSBO): " + Gu::getCoreContext()->getObjectLabel(GL_BUFFER, iSsboBinding));
  appendLine(strState, Stz "Bound Uniform Buffer Object: " + Gu::getCoreContext()->getObjectLabel(GL_BUFFER, iUniformBufferBinding));
  appendLine(strState, Stz "Bound Vertex Array Object: " + Gu::getCoreContext()->getObjectLabel(GL_VERTEX_ARRAY, iVertexArrayBinding));
  appendLine(strState, Stz "Bound Shader Program: " + Gu::getCoreContext()->getObjectLabel(GL_PROGRAM, iCurrentProgram));
  Gu::getCoreContext()->chkErrRt();

  if (iCurrentProgram > 0) {
    OpenGLUtils::debugPrintActiveUniforms(iCurrentProgram, strState);
  }
  else {
    appendLine(strState, "Bound Shader Name: None");
  }
}

void OpenGLUtils::debugPrintActiveUniforms(int iGlProgramId, string_t& strState) {
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
  Gu::getCoreContext()->glGetProgramiv(iGlProgramId, GL_ACTIVE_UNIFORMS, &nUniforms);
  Gu::getCoreContext()->glGetProgramiv(iGlProgramId, GL_ACTIVE_UNIFORM_BLOCKS, (GLint*)&nActiveUniformBlocks);
  glGetIntegerv(GL_MAX_UNIFORM_LOCATIONS, (GLint*)&nMaxUniformLocations);
  glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, (GLint*)&nMaxComponentsVert);
  glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, (GLint*)&nMaxComponentsFrag);
  glGetIntegerv(GL_MAX_COMPUTE_UNIFORM_COMPONENTS, (GLint*)&nMaxComponentsComp);
  Gu::getCoreContext()->chkErrRt();

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
    Gu::getCoreContext()->glGetActiveUniform(iGlProgramId, (GLuint)i, 256, &name_len, &iArraySize, &uniformType, (char*)name);
    name[name_len] = 0;
    uniformName = string_t(name);

    //get location
    GLint glLocation = Gu::getCoreContext()->glGetUniformLocation((GLuint)iGlProgramId, (GLchar*)uniformName.c_str());

    appendLine(strState, Stz "  Name: " + uniformName);
    appendLine(strState, Stz "  Type: " + OpenGLUtils::openGlTypeToString(uniformType));
    appendLine(strState, Stz "  Location: " + glLocation);
    appendLine(strState, Stz "  Array Size: " + iArraySize);

    // Uniform Block Data.
    Gu::getCoreContext()->chkErrRt();

    GLuint iCurrentBlockIdx;
    iCurrentBlockIdx = Gu::getCoreContext()->glGetUniformBlockIndex(iGlProgramId, uniformName.c_str());

    if (iCurrentBlockIdx != GL_INVALID_INDEX) {
      int iBlockBinding;
      int iBlockDataSize;
      int iBlockNameLength;
      int iBlockActiveUniforms;
      int iBlockActiveUniformIndices;

      Gu::getCoreContext()->chkErrRt();
      Gu::getCoreContext()->glGetActiveUniformBlockiv(iGlProgramId, iCurrentBlockIdx, GL_UNIFORM_BLOCK_BINDING, (GLint*)&iBlockBinding);
      Gu::getCoreContext()->chkErrRt();
      Gu::getCoreContext()->glGetActiveUniformBlockiv(iGlProgramId, iCurrentBlockIdx, GL_UNIFORM_BLOCK_DATA_SIZE, (GLint*)&iBlockDataSize);
      Gu::getCoreContext()->chkErrRt();
      Gu::getCoreContext()->glGetActiveUniformBlockiv(iGlProgramId, iCurrentBlockIdx, GL_UNIFORM_BLOCK_NAME_LENGTH, (GLint*)&iBlockNameLength);
      Gu::getCoreContext()->chkErrRt();
      Gu::getCoreContext()->glGetActiveUniformBlockiv(iGlProgramId, iCurrentBlockIdx, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, (GLint*)&iBlockActiveUniforms);
      Gu::getCoreContext()->chkErrRt();
      Gu::getCoreContext()->glGetActiveUniformBlockiv(iGlProgramId, iCurrentBlockIdx, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, (GLint*)&iBlockActiveUniformIndices);
      Gu::getCoreContext()->chkErrRt();

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
string_t OpenGLUtils::openGlTypeToString(GLenum type) {
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
void OpenGLUtils::debugGetAttribState(string_t& strState) {
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
void OpenGLUtils::debugGetTextureState(string_t& strState) {
  Gu::checkErrorsRt();
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
  Gu::checkErrorsRt();
  appendLine(strState, Stz "Active Texture : " + "GL_TEXTURE" + TypeConv::intToStr(iActiveTexture - 0x84c0));
  appendLine(strState, Stz "Current active texture: " + "GL_TEXTURE" + TypeConv::intToStr(iActiveTexture - 0x84C0));
  appendLine(strState, Stz "Max Texture Units: " + iMaxTextureUnits);
  appendLine(strState, Stz "Max Vertex Texture Units: " + iMaxVertexTextureUnits);
  appendLine(strState, Stz "Max Combined Texture Units: " + iMaxCombinedTextureUnits);
  appendLine(strState, Stz "Below are the bound textures Per Texture Channel:");

  appendLine(strState, Stz "----------------All Textures----------------");
  //Get the max id (possibly)
  Gu::checkErrorsRt();
  GLuint maxId = 0;
  Gu::getCoreContext()->glGenTextures(1, &maxId);
  Gu::getCoreContext()->glDeleteTextures(1, &maxId);
  Gu::checkErrorsRt();
  //Show all registered texture parameters
   for (GLuint iTexId = 0; iTexId < maxId; ++iTexId) {
     OpenGLUtils::debugPrintTextureInfo(strState, iTexId);
  }
  // static int dummyid = 1;
  // if (dummyid != 3)
  //   OpenGLUtils::debugPrintTextureInfo(strState, dummyid);
  // std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
  //           << dummyid << " bosgo"
  //           << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
  //           << std::endl;
  // dummyid++;

  appendLine(strState, Stz "----------------Bound Textures----------------");

  Gu::checkErrorsRt();

  // - Get bound texture units.
  for (int i = 0; i < iMaxVertexTextureUnits; ++i) {
    GLint iTextureId = 0;  //Texture ID
    Gu::getCoreContext()->glActiveTexture(GL_TEXTURE0 + i);
    appendLine(strState, Stz "  Channel " + i);
    glGetIntegerv(GL_TEXTURE_BINDING_1D, &iTextureId);
    if (iTextureId > 0) {
      appendLine(strState, Stz "     1D: " + Gu::getCoreContext()->getObjectLabel(GL_TEXTURE, iTextureId));
    }
    iTextureId = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_1D_ARRAY, &iTextureId);
    if (iTextureId > 0) {
      appendLine(strState, Stz "     1D_ARRAY: " + Gu::getCoreContext()->getObjectLabel(GL_TEXTURE, iTextureId));
    }
    iTextureId = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &iTextureId);
    if (iTextureId > 0) {
      appendLine(strState, Stz "     2D: " + Gu::getCoreContext()->getObjectLabel(GL_TEXTURE, iTextureId));
    }
    iTextureId = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_2D_ARRAY, &iTextureId);
    if (iTextureId > 0) {
      appendLine(strState, Stz "     2D_ARRAY: " + Gu::getCoreContext()->getObjectLabel(GL_TEXTURE, iTextureId));
    }
    iTextureId = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_2D_MULTISAMPLE, &iTextureId);
    if (iTextureId > 0) {
      appendLine(strState, Stz "     2D_MULTISAMPLE: " + Gu::getCoreContext()->getObjectLabel(GL_TEXTURE, iTextureId));
    }
    iTextureId = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY, &iTextureId);
    if (iTextureId > 0) {
      appendLine(strState, Stz "     2D_MULTISAMPLE_ARRAY: " + Gu::getCoreContext()->getObjectLabel(GL_TEXTURE, iTextureId));
    }
    iTextureId = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_3D, &iTextureId);
    if (iTextureId > 0) {
      appendLine(strState, Stz "     3D: " + Gu::getCoreContext()->getObjectLabel(GL_TEXTURE, iTextureId));
    }
    iTextureId = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_BUFFER, &iTextureId);
    if (iTextureId > 0) {
      appendLine(strState, Stz "     BUFFER: " + Gu::getCoreContext()->getObjectLabel(GL_TEXTURE, iTextureId));
    }
    iTextureId = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, &iTextureId);
    if (iTextureId > 0) {
      appendLine(strState, Stz "     CUBE_MAP: " + Gu::getCoreContext()->getObjectLabel(GL_TEXTURE, iTextureId));
    }
    iTextureId = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_RECTANGLE, &iTextureId);
    if (iTextureId > 0) {
      appendLine(strState, Stz "     RECTANGLE: " + Gu::getCoreContext()->getObjectLabel(GL_TEXTURE, iTextureId));
    }
    Gu::checkErrorsRt();
  }
}
void OpenGLUtils::debugPrintTextureInfo(string_t& strState, GLuint iTexId) {
  if (!glIsTexture(iTexId)) {
    return;
  }
  Gu::checkErrorsRt();

  Gu::getCoreContext()->glActiveTexture(GL_TEXTURE0);

  string_t texName = Gu::getCoreContext()->getObjectLabel(GL_TEXTURE, iTexId);
  Gu::checkErrorsRt();
  //
  GLenum tex_target = Gu::getCoreContext()->getTextureTarget(iTexId);
  Gu::checkErrorsRt();

  if (tex_target == (GLenum)0) {
    appendLine(strState, Stz "  " + texName + " - Texture Target information was not handled.");
  }
  else {
    GLenum get_binding = OpenGLUtils::texTargetToTexBindingQuery(tex_target);
    if (get_binding == (GLenum)0) {
      appendLine(strState, Stz "  " + texName + " - Texture Binding information (" + tex_target + ") was invalid.");
    }
    else {
      GLint iSavedTextureId = 0;
      glGetIntegerv(get_binding, &iSavedTextureId);
      Gu::checkErrorsRt();
      glBindTexture(tex_target, iTexId);
      {
          OpenGLUtils::debugPrintBoundTextureAttribs(strState, texName, tex_target);
      }
      glBindTexture(tex_target, (GLuint)iSavedTextureId);

      Gu::checkErrorsRt();
    }
  }
}
GLenum OpenGLUtils::texTargetToTexBindingQuery(GLenum target) {
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
GLenum OpenGLUtils::texBindingToTexTargetQuery(GLenum binding) {
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
void OpenGLUtils::debugPrintBoundTextureAttribs(string_t& strState, const string_t& texName, GLenum tex_target) {
//**Print texture attributes OpenGL
//Quick stringify openGL parameter names and their values.
#define GL_VVAL std::string("(" + TypeConv::intToStr(value) + ")")
#define GL_VPERR std::string("GL Enum " + (GL_VVAL) + " not found.")
#define GL_VP0(dx, dy) ((value == dx) ? (std::string(#dx)) : (dy))
#define GL_TEXPARM(tt, ss, xx, vp)                     \
  glGetTexParameteriv(tt, xx, &value);                 \
  ss += "    " + std::string(#xx) + " = " + (vp); \
  Gu::checkErrorsRt();

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

void OpenGLUtils::debugGetFramebufferAttachmentState(string_t& strState) {
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

  Gu::getCoreContext()->chkErrRt();

  appendLine(strState, Stz " # Max Color Attachments: " + maxColorAttachments);
  appendLine(strState, Stz " Current Bound Framebuffer: " + Gu::getCoreContext()->getObjectLabel(GL_FRAMEBUFFER, boundFramebuffer));
  appendLine(strState, Stz " Current Draw Framebuffer (glDrawBuffer): " + Gu::getCoreContext()->getObjectLabel(GL_FRAMEBUFFER, iDrawFramebufferBinding));
  appendLine(strState, Stz " Current Read Framebuffer (glReadBuffer): " + Gu::getCoreContext()->getObjectLabel(GL_FRAMEBUFFER, iReadFramebufferBinding));
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
    OpenGLUtils::debugPrintFBOAttachment(strState, GL_COLOR_ATTACHMENT0 + i);
  }
  OpenGLUtils::debugPrintFBOAttachment(strState, GL_DEPTH_ATTACHMENT);
  OpenGLUtils::debugPrintFBOAttachment(strState, GL_STENCIL_ATTACHMENT);
}
void OpenGLUtils::debugPrintFBOAttachment(string_t& strState, GLenum attachment) {
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

  Gu::getCoreContext()->glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, attachment, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &attachmentType);
  Gu::getCoreContext()->chkErrRt();
  if (attachmentType == GL_NONE) {
    appendLine(strState, Stz "    Type: " + "GL_NONE");
  }
  else if (attachmentType == GL_RENDERBUFFER) {
    Gu::getCoreContext()->glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, attachment, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &attachmentName);
    Gu::getCoreContext()->chkErrRt();
    appendLine(strState, Stz "    Type: " + "GL_RENDERBUFFER");
    appendLine(strState, Stz "    Name: " + Gu::getCoreContext()->getObjectLabel(GL_RENDERBUFFER, attachmentName));
  }
  else if (attachmentType == GL_TEXTURE) {
    Gu::getCoreContext()->glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, attachment, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &attachmentName);
    Gu::getCoreContext()->chkErrRt();
    Gu::getCoreContext()->glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, attachment, GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL, &mipmapLevel);
    Gu::getCoreContext()->chkErrRt();
    appendLine(strState, Stz "    Type: " + "GL_TEXTURE");
    appendLine(strState, Stz "    Name: " + Gu::getCoreContext()->getObjectLabel(GL_TEXTURE, attachmentName));
    appendLine(strState, Stz "    Mipmap Level: " + mipmapLevel);
  }
}
void OpenGLUtils::debugGetVertexArrayState(string_t& strState) {
  appendLine(strState, ("----------------------------------------"));
  appendLine(strState, ("--VERTEX ARRAY STATE"));
  int nMaxAttribs;
  GLint iVertexArrayBinding;
  glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nMaxAttribs);
  glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &iVertexArrayBinding);

  appendLine(strState, Stz "# Bound Vertex Array Id (VAO): " + Gu::getCoreContext()->getObjectLabel(GL_VERTEX_ARRAY, iVertexArrayBinding));
  appendLine(strState, Stz "# Max Allowed Atribs: " + nMaxAttribs);
  appendLine(strState, Stz("---------------------------------------"));
  appendLine(strState, Stz("--Active Vertex Attribs: "));

  Gu::getCoreContext()->chkErrRt();

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

    Gu::getCoreContext()->glGetVertexAttribiv(iAttrib, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, (GLint*)&iArrayBufferBinding);
    Gu::getCoreContext()->chkErrRt();
    Gu::getCoreContext()->glGetVertexAttribiv(iAttrib, GL_VERTEX_ATTRIB_ARRAY_ENABLED, (GLint*)&iArrayEnabled);
    Gu::getCoreContext()->chkErrRt();
    Gu::getCoreContext()->glGetVertexAttribiv(iAttrib, GL_VERTEX_ATTRIB_ARRAY_SIZE, (GLint*)&iAttribArraySize);
    Gu::getCoreContext()->chkErrRt();
    Gu::getCoreContext()->glGetVertexAttribiv(iAttrib, GL_VERTEX_ATTRIB_ARRAY_TYPE, (GLint*)&iAttribArrayType);
    Gu::getCoreContext()->chkErrRt();
    Gu::getCoreContext()->glGetVertexAttribiv(iAttrib, GL_VERTEX_ATTRIB_ARRAY_STRIDE, (GLint*)&iAttribArrayStride);
    Gu::getCoreContext()->chkErrRt();
    Gu::getCoreContext()->glGetVertexAttribiv(iAttrib, GL_VERTEX_ATTRIB_ARRAY_INTEGER, (GLint*)&iAttribArrayInteger);
    Gu::getCoreContext()->chkErrRt();

    Gu::getCoreContext()->glGetVertexAttribiv(iAttrib, GL_VERTEX_ATTRIB_ARRAY_NORMALIZED, (GLint*)&iAttribArrayNormalized);
    Gu::getCoreContext()->chkErrRt();
    //glGetVertexAttribiv(iAttrib, GL_VERTEX_ATTRIB_ARRAY_DIVISOR, (GLint*)&iAttribArrayDivisor);
    //CheckGpuErrorsDbg();

    appendLine(strState, Stz "Attrib " + iAttrib + "  Enabled:" + (iArrayEnabled ? "Y" : "N"));

    if (!iArrayEnabled) {
      continue;
    }

    appendLine(strState, Stz "  ArrayBufferBinding: " + Gu::getCoreContext()->getObjectLabel(GL_BUFFER, iArrayBufferBinding));
    appendLine(strState, Stz "  Size: " + iAttribArraySize);
    appendLine(strState, Stz "  Stride: " + iAttribArrayStride);
    appendLine(strState, Stz "  Is Integer: " + (iAttribArrayInteger ? "Y" : "N"));
    appendLine(strState, Stz "  Normalized: " + (iAttribArrayNormalized ? "Y" : "N"));
    appendLine(strState, Stz "  Type: " + OpenGLUtils::openGlTypeToString(iAttribArrayType));

    if (iAttrib != 0) {
      //Generic vertex attribute 0 is unique in that it has no current state,
      //so an error will be generated if index is 0. The initial value for all
      //other generic vertex attributes is (0,0,0,1).
      switch (iAttribArrayType) {
        case GL_INT:
          Gu::getCoreContext()->glGetVertexAttribIiv(iAttrib, GL_CURRENT_VERTEX_ATTRIB, (GLint*)&iCurAttrib);
          Gu::getCoreContext()->chkErrRt();
          appendLine(strState, Stz "  Cur Value: " + iCurAttrib[0] + "," + iCurAttrib[1] + "," + iCurAttrib[2] + "," + iCurAttrib[3]);
          break;
        case GL_UNSIGNED_INT:
          Gu::getCoreContext()->glGetVertexAttribIuiv(iAttrib, GL_CURRENT_VERTEX_ATTRIB, (GLuint*)&uiCurAttrib);
          Gu::getCoreContext()->chkErrRt();
          appendLine(strState, Stz "  Cur Value: " + uiCurAttrib[0] + "," + uiCurAttrib[1] + "," + uiCurAttrib[2] + "," + uiCurAttrib[3]);
          break;
        case GL_FLOAT:
          Gu::getCoreContext()->glGetVertexAttribfv(iAttrib, GL_CURRENT_VERTEX_ATTRIB, (GLfloat*)&iCurAttrib);
          Gu::getCoreContext()->chkErrRt();
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

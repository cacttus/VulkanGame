/**
*  @file OpenGLUtils.h
*  @date 08/16/2020
*  @author MetalMario971
*/
#pragma once
#ifndef __OPENGLUTILS_16002411688204214146226369830_H__
#define __OPENGLUTILS_16002411688204214146226369830_H__

#include "../gfx/GfxHeader.h"

namespace BR2 {
/**
*  @class OpenGLUtils
*  @brief Utilities for OpenGL.
*/
class OpenGLUtils : public VirtualMemoryShared<OpenGLUtils> {
public:
  OpenGLUtils();
  virtual ~OpenGLUtils() override;

  /// Rendering State
  static void resetRenderState();
  static string_t debugGetRenderState(bool bForceRun = false, bool bPrintToStdout = true, bool bSaveFramebufferTexture = true);  //breakpoint it

  // Textures
  static void createDepthTexture(const string_t& owner, GLuint* __out_ texId, int32_t w, int32_t h, bool bMsaaEnabled, int32_t nMsaaSamples, GLenum eRequestedDepth);
  static GLenum getSupportedDepthSize();
  static bool getTextureDataFromGpu(std::shared_ptr<Img32> __out_ image, GLuint iGLTexId, GLenum eTarget, int iCubeMapSide = -1);

  //Conversion
  static string_t openGlTypeToString(GLenum type);  //TODO: move this to the GL hardware or Gd::..

  // Shaders
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

  static GLenum texTargetToTexBindingQuery(GLenum target);
  static GLenum texBindingToTexTargetQuery(GLenum binding);

private:
  // Debug
  static void debugGetVertexArrayState(string_t& strOut);  //breakpoint it
  static void debugGetFramebufferAttachmentState(string_t& strOut);
  static void debugGetTextureState(string_t& strOut);
  static void debugGetAttribState(string_t& strOut);
  static void debugGetBufferState(string_t& strOut);
  static void debugGetLegacyViewAndMatrixStack(string_t& strOut);
  static void debugPrintActiveUniforms(int iGlProgramId, string_t& strOut);
  static void debugPrintFBOAttachment(string_t& strState, GLenum);
  // Texture
  static void saveFramebufferAsPng(string_t&& strLoc, GLuint iFBOId = 0);  // Saves a GL texture by ID to the file path.
  static void getCompatibleDepthComponent(GLenum eRequestedDepth, std::function<void(GLenum)> func);
  static void debugPrintTextureInfo(string_t& state, GLuint iTexId);
  static void debugPrintBoundTextureAttribs(string_t& strState, const string_t& texName, GLenum tex_target);
};

}  // namespace BR2

#endif

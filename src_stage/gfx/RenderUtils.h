/**
*  @file RenderUtils.h
*  @date May 16, 2012
*  @author MetalMario971
*/
#pragma once
#ifndef __RENDERUTILS_215792320710911295042735_H__
#define __RENDERUTILS_215792320710911295042735_H__

#include "../gfx/GfxHeader.h"
#include "../math/MathAll.h"

namespace BR2 {
/**
*  @class RenderUtils
*  @brief Provides utility functions for creating basic shapes, and rendering to the screen.
*/
class RenderUtils : public VirtualMemory {
public:
  RenderUtils(std::shared_ptr<GLContext> ctx);
  void renderTexturedQuadAttrib(std::shared_ptr<CameraNode> cam, float size);  //UNSAFE
                                                                               //static void drawWireSphereShader(float fRadius, vec3& vOffset, vec4& vColor, int32_t nSlices=5, int32_t nStacks=5);
  void drawWireSphereShader(std::shared_ptr<CameraNode> cam, float fRadius, const vec4& vColor, int32_t nSlices = 5, int32_t nStacks = 5, mat4* pMatrix = nullptr);
  void drawWireBoxShader(std::shared_ptr<CameraNode> cam, const Box3f* box, const vec3& vOffset, const vec4& vColor);
  void drawSolidBoxShaded(std::shared_ptr<CameraNode> cam, const Box3f* box, const vec3& vOffset, const vec4& vColor);
  //static void drawPickBox(Box3f* box, uint32_t uiColorId);
  void drawAxisShader(std::shared_ptr<CameraNode> cam, float scale = 10.0f, float lineWidth = 2.0f, const mat4& transform = mat4::identity());  // Renders an Axis at the origin.
  void drawFrustumShader(std::shared_ptr<CameraNode> cam, std::shared_ptr<FrustumBase> pf, const vec4& avColor);                                //idk why there are 2
  void drawGridShader(std::shared_ptr<CameraNode> cam, float r = 1.0f, float g = 1.0f, float b = 1.0f, int32_t nSlices = 60,
                      float fSliceWidth = 20.0f, const vec3& center = vec3(0, 0, 0), std::shared_ptr<ShaderBase> pShader = NULL);  // Renders a grid.

  void saveTexture(string_t&& strLoc, GLuint iGLTexId, GLenum eTexTarget, int iCubeMapSide = -1);  // Saves a GL texture by ID to the file path.

  static void makeVertexFormats();

private:
  std::shared_ptr<GLContext> _pContext = nullptr;
  std::shared_ptr<GLContext> getContext() { return _pContext; }

};

}  // namespace BR2

#endif
